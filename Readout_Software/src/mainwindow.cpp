#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "data_handler.h"

// qt
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QFileDialog>
#include <QTime>
#include <QScrollBar>

// std/stl
#include <iostream>
#include <sstream>
using namespace std;


// ------------------------------------------------------------------------- //
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    FECPORT(6007),
    DAQPORT(6006),
    VMMASICPORT(6603),
    VMMAPPPORT(6600),
    S6PORT(6602),
    vmmConfigHandler(0),
    vmmSocketHandler(0),
    vmmConfigModule(0),
    vmmRunModule(0),
    vmmMessageHandler(0),
    //thread
    daqThread(0),
    vmmDataHandler(0),
    vmmCalibModule(0),
    m_commOK(false),
    m_febSetOK(false),
    m_configOK(false),
    m_tdaqOK(false),
    m_runModeOK(false),
    m_acqMode(""),
    m_daqInProgress(false),
    m_hdmiMaskON(false),
    m_runDirectoryOK(true),
    m_readyToRead(false),
    m_inCalibrationLoop(false)
{


    //thread
    daqThread = new QThread();

    vmmMessageHandler = new MessageHandler();
    vmmMessageHandler->setMessageSize(75);
    vmmMessageHandler->setGUI(true);
    connect(vmmMessageHandler, SIGNAL(logReady()), this, SLOT(readLog()));

    ui->setupUi(this);
    this->setFixedSize(1200*1.03,720); // 1400, 725
    //this->setFixedSize(1200,720); // 1400, 725
    //this->setMinimumHeight(720);
    //this->setMinimumWidth(1200);
    Font.setFamily("Arial");
    ui->loggingScreen->setReadOnly(true);
    ui->debugScreen->setReadOnly(true);
    ui->runStatusField->setReadOnly(true);

    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 18px; width: 100px; }");
    ui->tabWidget->setTabText(0,"Channel Registers");
    ui->tabWidget->setTabText(1,"Calibration");
    ui->tabWidget->setTabText(2,"Response");

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // VMM handles
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////
    vmmConfigHandler = new ConfigHandler();
    vmmSocketHandler = new SocketHandler();
    vmmConfigModule  = new Configuration();
    vmmRunModule     = new RunModule();
    vmmDataHandler   = new DataHandler();
    vmmCalibModule = new CalibModule();

    vmmConfigHandler->LoadMessageHandler(msg());
    vmmSocketHandler->LoadMessageHandler(msg());
    vmmConfigModule ->LoadMessageHandler(msg());
    vmmDataHandler  ->LoadMessageHandler(msg());
    vmmCalibModule  ->LoadMessageHandler(msg());
    vmmRunModule    ->LoadMessageHandler(msg());


    m_dbg = false;
    vmmConfigHandler ->setDebug(false);
    vmmSocketHandler ->setDebug(false);
    vmmConfigModule  ->setDebug(false);
    vmmRunModule     ->setDebug(false);
    vmmDataHandler   ->setDebug(false);
    vmmCalibModule   ->setDebug(false);

    // set dry run for testing
    //msg()(" !! SOCKETHANDLER SET FOR DRY RUN !! ");
    //vmmSocketHandler->setDryRun();

    // propagate command counter
    connect(vmmSocketHandler, SIGNAL(commandCounterUpdated()),
                                            this, SLOT(updateCounter()));

    // load the things
    vmmConfigModule->LoadConfig(*vmmConfigHandler);
    vmmConfigModule->LoadSocket(*vmmSocketHandler);

    vmmRunModule->LoadConfig(*vmmConfigHandler);
    vmmRunModule->LoadSocket(*vmmSocketHandler);

    // testing shared memory
    // make it direct connection (which means the daq thread will not be the thread in which it is processed
    // but this will not be needed when actually processing shared memory)
    //connect(ui->testSharedMemory, SIGNAL(pressed()), vmmDataHandler, SLOT(testSharedMem()), Qt::DirectConnection);


    //////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------//
    // connect DataHandler functions
    //------------------------------------------------------------------//
    //////////////////////////////////////////////////////////////////////

    connect(this, SIGNAL(monitorDataSignal(bool)), vmmDataHandler, SLOT(set_monitorData(bool)), Qt::DirectConnection);
    connect(this, SIGNAL(clearSharedMemory(int)), vmmDataHandler, SLOT(clearSharedMemory(int)), Qt::DirectConnection);
    connect(ui->doMonitoring, SIGNAL(stateChanged(int)), this, SLOT(setupMonitoring(int)));
    connect(this, SIGNAL(setUseChannelMap(bool)), vmmDataHandler, SLOT(setUseChannelMap(bool)));
    connect(this, SIGNAL(loadELxChannelMapping(QString)), vmmDataHandler, SLOT(loadELxChannelMapping(QString)));
    connect(this, SIGNAL(setWriteNtuple(bool)), vmmDataHandler, SLOT(setWriteNtuple(bool)));
    connect(this, SIGNAL(setIgnore16(bool)), vmmDataHandler, SLOT(setIgnore16(bool)));
    connect(this, SIGNAL(setCalibrationRun(bool)), vmmDataHandler, SLOT(setCalibrationRun(bool)));
    connect(this, SIGNAL(setupOutputFiles(QString,QString)),
                    vmmDataHandler, SLOT(setupOutputFiles(QString,QString)));
    connect(vmmDataHandler, SIGNAL(setRunDirOK(bool)), this, SLOT(setRunDirOK(bool)));

    connect(this, SIGNAL(setupOutputTrees()), vmmDataHandler, SLOT(setupOutputTrees()));
    connect(this, SIGNAL(startDAQSocket()), vmmDataHandler,
                                SLOT(connectDAQSocket()));
    connect(this, SIGNAL(closeDAQSocket()), vmmDataHandler,
                                SLOT(closeDAQSocket()));

    connect(this, SIGNAL(EndRun()), vmmDataHandler, SLOT(writeAndCloseDataFile()));
    connect(vmmDataHandler, SIGNAL(eventCountStopReached()), this,
                                SLOT(eventCountReached()));

    channelGridLayout = new QGridLayout(this);
    dummy = new QWidget(this);
    CreateChannelsFields();

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // configure some of the widgets' styles and attributes
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////

    //trigger mode buttons
    ui->trgPulser->setCheckable(true);
    ui->trgExternal->setCheckable(true);

    ////acquisition mode buttons
    ui->onACQ->setCheckable(true);
    ui->offACQ->setCheckable(true);

    //hdmi mask
    ui->setMask->setCheckable(true);

    //enable art
    ui->enableART->setCheckable(true);

    // DAQ XML
    ui->loadDAQXMLFile->setEnabled(false);
    ui->writeDAQXMLFile->setEnabled(false);

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // make widget connections
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////

    // ~FSM preliminary
    connect(this, SIGNAL(checkFSM()), this, SLOT(updateFSM()));

    // ------------------------------------------------------ //
    // ------- ~roughly in order of typical operation ------- //
    // ------------------------------------------------------ //

    // set number of FECs
    connect(ui->numberOfFecs, SIGNAL(valueChanged(int)),
                                            this, SLOT(setNumberOfFecs(int)));

    // ping the boards and connect sockets
    connect(ui->openConnection, SIGNAL(pressed()),
                                            this, SLOT(Connect()));

    //addmmfe8
    // set whether running a mmfe8 configuration or mini2
    connect(ui->mini2CheckBox, SIGNAL(pressed()),
                                            this, SLOT(setFEB()));
    connect(ui->mmfe8CheckBox, SIGNAL(pressed()),
                                            this, SLOT(setFEB()));

    //addxadc
    //set whether running a xadc configuration
    connect(ui->xadc,SIGNAL(stateChanged(int)),this,SLOT(setFEB()));



    // prepare the board configuration and send
    connect(ui->SendConfiguration, SIGNAL(pressed()),
                                    this, SLOT(prepareAndSendBoardConfig()));

    // prepare the T/DAQ configuration and send
    connect(ui->setTrgAcqConst, SIGNAL(pressed()),
                                    this, SLOT(prepareAndSendTDAQConfig()));

    // set the run mode and send
    connect(ui->trgPulser, SIGNAL(clicked()),
                                    this, SLOT(setRunMode()));

    connect(ui->trgExternal, SIGNAL(clicked()),
                                    this, SLOT(setRunMode()));

    // set data acquisition on/off
    connect(ui->onACQ, SIGNAL(clicked()),
                                    this, SLOT(setACQMode()));
    connect(ui->offACQ, SIGNAL(clicked()),
                                    this, SLOT(setACQMode()));

    // start run
    connect(ui->checkTriggers, SIGNAL(clicked()),
                                    this, SLOT(triggerHandler()));
    // stop run
    connect(ui->stopTriggerCnt, SIGNAL(clicked()),
                                    this, SLOT(triggerHandler()));
    // clear trigger count
    connect(ui->clearTriggerCnt, SIGNAL(clicked()),
                                    this, SLOT(triggerHandler()));

    // update trigger count
    connect(vmmDataHandler, SIGNAL(checkDAQCount(bool)),
                                    this, SLOT(updateTriggerCount(bool)));

    // ------------------------------------------------------ //
    // ------------- remaining buttons/widgets -------------- //
    // ------------------------------------------------------ //

    // select the output directory for files
    connect(ui->selectDir, SIGNAL(clicked()),
                                    this, SLOT(selectOutputDirectory()));

    // load the board/global configuration from the XML
    connect(ui->loadConfigXMLFileButton, SIGNAL(clicked()),
                                    this, SLOT(loadConfigurationFromFile()));
    // write the total configuration to an XML file
    connect(ui->writeConfigXMLFileButton, SIGNAL(clicked()),
                                    this, SLOT(writeConfigurationToFile()));

    // set event headers
    connect(ui->setEvbld, SIGNAL(clicked()),
                                    this, SLOT(setAndSendEventHeaders()));

    // reset ASICs
    connect(ui->asic_reset, SIGNAL(clicked()),
                                    this, SLOT(resetASICs()));
    // warm init or reboot FEC
    connect(ui->fec_WarmInit, SIGNAL(clicked()),
                                    this, SLOT(resetFEC()));
    connect(ui->fec_reset, SIGNAL(clicked()),
                                    this, SLOT(resetFEC()));

    // set HDMI channel mask
    connect(ui->setMask, SIGNAL(clicked()),
                                    this, SLOT(setHDMIMask()));

    // include the ART bits in the channel mask
    connect(ui->enableART, SIGNAL(clicked()),
                                    this, SLOT(setART()));

    // link status
    connect(ui->linkPB, SIGNAL(clicked()),
                                    this, SLOT(checkLinkStatus()));

    // reset links
    connect(ui->resetLinks, SIGNAL(clicked()),
                                    this, SLOT(resetLinks()));

    // s6 reset configuration
    connect(ui->setck_s6_resets, SIGNAL(clicked()),
                                    this, SLOT(setS6Resets()));
    connect(vmmRunModule, SIGNAL(s6resetStatus(bool)),
                                    this, SLOT(set_s6resetStatus(bool)));

    // daq ~conversion to mV
    connect(ui->sdt, SIGNAL(valueChanged(int)),
                                    this, SLOT(changeDACtoMVs(int)));
    connect(ui->sdp_2, SIGNAL(valueChanged(int)),
                                    this, SLOT(changeDACtoMVs(int)));

    // s6 clocks
    connect(ui->setck_s6, SIGNAL(clicked()),
                                    this, SLOT(setS6clocks()));

    // test pulse config
    connect(ui->setTp_s6, SIGNAL(clicked()),
                                    this, SLOT(configureTP()));

    // move to bottom of log screen
    connect(ui->logBottom, SIGNAL(clicked()),
                                    this, SLOT(updateLogScreen()));

    // set debug and propagate to the tools
    connect(ui->enableDebugPB, SIGNAL(stateChanged(int)),
                                    this, SLOT(toggleDebug()));

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    //  calibration
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////
    connect(vmmCalibModule, SIGNAL(calibrationLoopState(bool)),
                        this, SLOT(calibrationLoopState(bool)));
    connect(this, SIGNAL(stopCalibrationLoop()),
                        vmmCalibModule, SLOT(stopCalibrationLoop()));
    connect(vmmCalibModule, SIGNAL(setPDOCalibrationState(int,int,int)),
                        this, SLOT(setPDOCalibrationState(int,int,int)));
    connect(vmmCalibModule, SIGNAL(setTDOCalibrationState(int,int,int,int)),
                        this, SLOT(setTDOCalibrationState(int,int,int,int)));
    connect(this, SIGNAL(updateCalibrationState(int,int,int,int,int)),
                        vmmDataHandler, SLOT(updateCalibrationState(int,int,int,int,int)));
    connect(vmmCalibModule, SIGNAL(setChannels(int)),
                        this, SLOT(setChannelsForCalib(int)));
    connect(vmmCalibModule, SIGNAL(setChannels(int)),
                        vmmDataHandler, SLOT(setCalibrationChannel(int)));
    connect(vmmCalibModule, SIGNAL(setupCalibrationConfig()),
                        this, SLOT(setupCalibrationConfig()));
    connect(vmmCalibModule, SIGNAL(setCalibrationACQon(int)),
                        this, SLOT(setCalibrationACQon(int)));
    connect(vmmCalibModule, SIGNAL(setCalibrationACQoff()),
                        this, SLOT(setCalibrationACQoff()));
    connect(vmmCalibModule, SIGNAL(endCalibrationRun()),
                        this, SLOT(endCalibrationRun()));
    connect(ui->delaySteps, SIGNAL(currentIndexChanged(int)),
                        this, SLOT(changeDelayLabels(int)));

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // initial state
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////
    SetInitialState();

    //thread
    //msg()("NOT MOVING TO DAQ THREAD");
    vmmDataHandler->moveToThread(daqThread);

    // for now disable the ability to set the online monitoring ON
    // will wait until this is robus
    msg()(" *** Disabling online monitoring *** ");
    ui->doMonitoring->setEnabled(false);
}

// ------------------------------------------------------------------------- //
void MainWindow::keepButtonState(bool)
{
    if(QObject::sender() == ui->trgPulser) {

        if(ui->trgPulser->isChecked()) {
            ui->trgPulser->setDown(true);
        }
    }
}
// ------------------------------------------------------------------------- //
MainWindow::~MainWindow()
{
    delete channelGridLayout;



    delete ui;
}
// ------------------------------------------------------------------------- //
void MainWindow::selectOutputDirectory()
{
    stringstream sx;

    QFileDialog getdir;
    QString dirStr = QFileDialog::getExistingDirectory(this,
                        tr("Select Output Directory"), ".",
                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks); 
    if(dirStr=="") return;

    msg()("Setting output directory to:\n  " + dirStr.toStdString());
    ui->runDirectoryField->setText(dirStr);
    QString check = dirStr;
    if(!ui->runDirectoryField->text().endsWith("/")) check = check + "/";

    int run_number = DataHandler::checkForExistingFiles(dirStr.toStdString(),
                                            ui->runNumber->value());
    if( !(run_number == ui->runNumber->value()) ) {
        sx << "Re-setting run number to: " << run_number;    
        msg()(sx); sx.str("");
    }
    ui->runNumber->setValue(run_number);
}
// ------------------------------------------------------------------------- //
void MainWindow::toggleDebug()
{
    if(ui->enableDebugPB->isChecked()) {
        m_dbg = true;
        vmmConfigHandler ->setDebug(true);
        vmmSocketHandler ->setDebug(true);
        vmmConfigModule  ->setDebug(true);
        vmmRunModule     ->setDebug(true);
        vmmDataHandler   ->setDebug(true);
        vmmCalibModule   ->setDebug(true);
    }
    else {
        m_dbg = false;
        vmmConfigHandler ->setDebug(false);
        vmmSocketHandler ->setDebug(false);
        vmmConfigModule  ->setDebug(false);
        vmmRunModule     ->setDebug(false);
        vmmDataHandler   ->setDebug(false);
        vmmCalibModule   ->setDebug(false);
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::changeDACtoMVs(int)
{
    QString tmp;
    if(QObject::sender() == ui->sdt)
        ui->dacmvLabel->setText(tmp.number((0.6862*ui->sdt->value()+63.478), 'f', 2) + " mV");    
    else if(QObject::sender() == ui->sdp_2)
        ui->dacmvLabel_TP->setText(tmp.number((0.6862*ui->sdp_2->value()+63.478), 'f', 2) + " mV");    
}
// ------------------------------------------------------------------------- //
void MainWindow::readLog()
{
    string buff = msg().buffer();
    ui->loggingScreen->append(QString::fromStdString(buff));
    //ui->loggingScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    msg().clear(); 
}
// ------------------------------------------------------------------------- //
void MainWindow::updateLogScreen()
{
    if(QObject::sender() == ui->logBottom) {
        ui->loggingScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->loggingScreen->verticalScrollBar()->setValue(ui->loggingScreen->verticalScrollBar()->maximum());
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::updateFSM()
{
    //addmmfe8
    if(m_febSetOK && !m_commOK && !m_configOK && !m_tdaqOK && !m_runModeOK && m_acqMode==""){
        ui->openConnection->setEnabled(true);
        msg()("Waiting for open communication with FEC...");
    }
    else if(m_febSetOK && m_commOK && !m_configOK && !m_tdaqOK && !m_runModeOK && m_acqMode==""){
    //    ui->cmdLabel->setText("No\nConfig");
        

    //if(m_commOK && !m_febSetOK && !m_configOK && !m_tdaqOK && !m_runModeOK && m_acqMode=="") {
    //    ui->mini2CheckBox->setEnabled(true);
    //    ui->mmfe8CheckBox->setEnabled(true);
    //    ui->cmdlabel->setText("No FEB\nSet");
    //    msg()("Waiting to set FEB configuration type");
    //}
    ////addmmfe8
    //else if(m_commOK && m_febSetOK && !m_configOK && !m_tdaqOK && !m_runModeOK && m_acqMode=="") { 
        ui->SendConfiguration->setEnabled(true);
        ui->cmdlabel->setText("No\nConfig");

        ui->fec_WarmInit->setEnabled(true);
        ui->fec_reset->setEnabled(true);

        ui->setck_s6->setEnabled(true);
        ui->setTp_s6->setEnabled(true);

        ui->asic_reset->setEnabled(true);

        ui->setMask->setEnabled(true);
        ui->enableART->setEnabled(true);
        ui->linkPB->setEnabled(true);
        ui->resetLinks->setEnabled(true);

 //       msg()("Waiting for configuration to be sent...");
 //   }
 //   else if(m_commOK && m_configOK && !m_tdaqOK && !m_runModeOK && m_acqMode=="") {
        ui->setTrgAcqConst->setEnabled(true);
        ui->setEvbld->setEnabled(true);

 //       msg()("Waiting for T/DAQ constants to be sent...");
 //   }
 //   else if(m_commOK && m_configOK && m_tdaqOK && !m_runModeOK && m_acqMode=="") {
        ui->trgPulser->setEnabled(true);
        ui->trgExternal->setEnabled(true);

 //       msg()("Waiting for trigger mode to be established...");
 //   }
 //   else if(m_commOK && m_configOK && m_tdaqOK && m_runModeOK && m_acqMode=="") {
        ui->onACQ->setEnabled(true);
        ui->clearTriggerCnt->setEnabled(true);

 //       msg()("Ready for DAQ...");
 //   }
 //   else if(m_commOK && m_configOK && m_tdaqOK && m_runModeOK &&
 //               (m_acqMode=="ON")) {
        ui->offACQ->setEnabled(true);
        ui->checkTriggers->setEnabled(true);
        ui->clearTriggerCnt->setEnabled(true);
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::updateCounter()
{
    ui->cmdlabel->setText(QString::number(socketHandle().commandCounter()));
}
// ------------------------------------------------------------------------- //
void MainWindow::Connect()
{
    if(ui->numberOfFecs->value() == 0) {
        ui->connectionLabel->setText("Select number of FECs");
        ui->connectionLabel->setStyleSheet("background-color: light");
        return;
    }

    // ------------------------------------------------- //
    //  "comm info"
    // ------------------------------------------------- //
    CommInfo commInfo;

    commInfo.fec_port       = FECPORT;
    commInfo.daq_port       = DAQPORT;
    commInfo.vmmasic_port   = VMMASICPORT;
    commInfo.vmmapp_port    = VMMAPPPORT;
    commInfo.s6_port        = S6PORT;

    QString iplist = "";
    QString separator = "";
    for(int i = 0; i < ui->numberOfFecs->value(); i++) {
        if(i==(ui->numberOfFecs->value()-1)) {
            separator = "";
        } else {
            separator = ",";
        }
        iplist += ips[i] + separator;
    }
    commInfo.ip_list = iplist;

    //comment
    commInfo.comment = ui->userComments->text();

    configHandle().LoadCommInfo(commInfo);
    bool pingOK = socketHandle().loadIPList(iplist).ping();

    if(pingOK) {
        msg()("Ping successful");
        ui->connectionLabel->setText("all alive");
        ui->connectionLabel->setStyleSheet("background-color: green");

        ///////////////////////////////////////////////////////
        // now that we know we are connected OK, add and bind
        // the sockets
        ///////////////////////////////////////////////////////
        socketHandle().addSocket("FEC", FECPORT, QUdpSocket::ShareAddress);
        connect(vmmRunModule, SIGNAL(checkLinks()), this, SLOT(writeFECStatus()));
        //socketHandle().addSocket("DAQ", DAQPORT); 

        

    } else {
        ui->connectionLabel->setText("ping failed"); 
        ui->connectionLabel->setStyleSheet("background-color: lightGray");
    }

    m_commOK = pingOK;

    emit checkFSM();

}
// ------------------------------------------------------------------------- //
//addmmfe8
void MainWindow::setFEB()
{
    if(QObject::sender() == ui->mini2CheckBox) {
        ui->xadc->setChecked(false);
        msg()("Setting up to configure and readout MINI2");
        propagateFEBSettings(false);
    }

    //addxadc
    //if xadc is checked (StateChanged) then use PropagateFEBSettings1
    //ensure that only one of three buttons is clicked (mmfe8,mini2,xadc)
    else if(QObject::sender() == ui->xadc) {
        if(ui->xadc->isChecked() ){
        ui->mini2CheckBox->setAutoExclusive(false);
        ui->mini2CheckBox->setChecked(false);
        ui->mini2CheckBox->setAutoExclusive(true);

        ui->mmfe8CheckBox->setAutoExclusive(false);
        ui->mmfe8CheckBox->setChecked(false);
        ui->mmfe8CheckBox->setAutoExclusive(true);

        msg()("Setting up to configure and readout XADC");
        propagateFEBSettings1(true);}
        else{
            propagateFEBSettings1(false);
        }
    }

    else if(QObject::sender() == ui->mmfe8CheckBox) {
        ui->xadc->setChecked(false);
        msg()("Setting up to configure and readout MMFE8");
        propagateFEBSettings(true);

        //change the IP default
        msg()("Setting IP to 192.168.0.2","MainWindow::setFEB");
        ui->ip1->setText("192");
        ui->ip2->setText("168");
        ui->ip3->setText("0");
        ui->ip4->setText("2");

        //for now fix the vmm id
        ui->hdmi1->setChecked(false);
        ui->hdmi1_1->setChecked(false);
        ui->hdmi1_2->setChecked(false);
        ui->hdmi2->setChecked(false);
        ui->hdmi2_1->setChecked(false);
        ui->hdmi2_2->setChecked(false);
        ui->hdmi3->setChecked(false);
        ui->hdmi3_1->setChecked(false);
        ui->hdmi3_2->setChecked(false);
        ui->hdmi4->setChecked(false);
        ui->hdmi4_1->setChecked(false);
        ui->hdmi4_2->setChecked(false);
        ui->hdmi5->setChecked(true);
        ui->hdmi5_1->setChecked(true);
        ui->hdmi5_2->setChecked(false);
        ui->hdmi6->setChecked(false);
        ui->hdmi6_1->setChecked(false);
        ui->hdmi6_2->setChecked(false);
        ui->hdmi7->setChecked(false);
        ui->hdmi7_1->setChecked(false);
        ui->hdmi7_2->setChecked(false);
        ui->hdmi8->setChecked(false);
        ui->hdmi8_1->setChecked(false);
        ui->hdmi8_2->setChecked(false);

        ui->hdmi1_art->setChecked(false);
        ui->hdmi2_art->setChecked(false);
        ui->hdmi3_art->setChecked(false);
        ui->hdmi4_art->setChecked(false);
        ui->hdmi5_art->setChecked(false);
        ui->hdmi6_art->setChecked(false);
        ui->hdmi7_art->setChecked(false);
        ui->hdmi8_art->setChecked(false);
    }
    m_febSetOK = true;
    emit checkFSM(); 
}
// ------------------------------------------------------------------------- //
void MainWindow::propagateFEBSettings(bool doMMFE8)
{
    msg()("Propagating FEB settings...");
    vmmConfigModule->setMMFE8(doMMFE8);
    vmmConfigHandler->setMMFE8(doMMFE8);
    vmmRunModule->setMMFE8(doMMFE8);
    vmmDataHandler->setMMFE8(doMMFE8);
    vmmSocketHandler->setMMFE8(doMMFE8);
}

//-------------------XADC---------------------------------------------
void MainWindow::propagateFEBSettings1(bool doXADC)
{
    msg()("Propagating FEB-XADC settings...");
    vmmDataHandler->setXADC(doXADC);
}

//-------------------XADC---------------------------------------------
// ------------------------------------------------------------------------- //
void MainWindow::prepareAndSendBoardConfig()
{
    msg()("Sending configuration...");

    // ------------------------------------------------- //
    //  global settings
    // ------------------------------------------------- //
    GlobalSetting global;

    global.polarity            = ui->spg->currentIndex();
    global.leakage_current     = ui->slg->currentIndex();
    global.analog_tristates    = ui->sdrv->currentIndex();
    global.double_leakage      = ui->sfm->currentIndex();
    global.gain                = ui->sg->currentIndex();
    global.peak_time           = ui->st->currentIndex();
    global.neighbor_trigger    = ui->sng->currentIndex();
    global.tac_slope           = ui->stc->currentIndex();
    global.disable_at_peak     = ui->sdp->currentIndex();
    global.art                 = ui->sfa->currentIndex();
    global.art_mode            = ui->sfam->currentIndex();
    global.dual_clock_art      = ui->sdcka->currentIndex();
    global.out_buffer_mo       = ui->sbfm->currentIndex();
    global.out_buffer_pdo      = ui->sbfp->currentIndex();
    global.out_buffer_tdo      = ui->sbft->currentIndex();
    global.channel_monitor     = ui->sm5_sm0->currentIndex();
    global.monitoring_control  = ui->scmx->currentIndex();
    global.monitor_pdo_out     = ui->sbmx->currentIndex();
    global.adcs                = ui->spdc->currentIndex();
    global.sub_hysteresis      = ui->ssh->currentIndex();
    global.direct_time         = ui->sttt->currentIndex();


    global.direct_time_mode0   = ui->stpp->currentIndex();
    global.direct_time_mode1   = ui->stot->currentIndex();

    // build up the 2-bit word from the mode0 and mode1
    bool ok;
    QString tmp;
    tmp.append(QString::number(global.direct_time_mode0));
    tmp.append(QString::number(global.direct_time_mode1));
    global.direct_time_mode = tmp.toUInt(&ok,2);

    global.conv_mode_8bit      = ui->s8b->currentIndex();
    global.enable_6bit         = ui->s6b->currentIndex();
    global.adc_10bit           = ui->sc010b->currentIndex();
    global.adc_8bit            = ui->sc08b->currentIndex();
    global.adc_6bit            = ui->sc06b->currentIndex();
    global.dual_clock_data     = ui->sdcks->currentIndex();
    global.dual_clock_6bit     = ui->sdck6b->currentIndex();
    global.threshold_dac       = ui->sdt->value();
    global.test_pulse_dac      = ui->sdp_2->value();
    

    // ------------------------------------------------- //
    //  HDMI channel map
    // ------------------------------------------------- //
    vector<ChannelMap> chMap;
    // do this by hand
    for(int i = 0; i < 8; i++) {
        ChannelMap chm;
        chm.hdmi_no = i;
        if       (i==0) {
            chm.on     = (ui->hdmi1->isChecked() ? true : false);
            chm.first  = (ui->hdmi1_1->isChecked() ? true : false);
            chm.second = (ui->hdmi1_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi1_art->isChecked() ? true : false);
        } else if(i==1) {
            chm.on     = (ui->hdmi2->isChecked() ? true : false);
            chm.first  = (ui->hdmi2_1->isChecked() ? true : false);
            chm.second = (ui->hdmi2_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi2_art->isChecked() ? true : false);
        } else if(i==2) {
            chm.on     = (ui->hdmi3->isChecked() ? true : false);
            chm.first  = (ui->hdmi3_1->isChecked() ? true : false);
            chm.second = (ui->hdmi3_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi3_art->isChecked() ? true : false);
        } else if(i==3) {
            chm.on     = (ui->hdmi4->isChecked() ? true : false);
            chm.first  = (ui->hdmi4_1->isChecked() ? true : false);
            chm.second = (ui->hdmi4_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi4_art->isChecked() ? true : false);
        } else if(i==4) {
            chm.on     = (ui->hdmi5->isChecked() ? true : false);
            chm.first  = (ui->hdmi5_1->isChecked() ? true : false);
            chm.second = (ui->hdmi5_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi5_art->isChecked() ? true : false);
        } else if(i==5) {
            chm.on     = (ui->hdmi6->isChecked() ? true : false);
            chm.first  = (ui->hdmi6_1->isChecked() ? true : false);
            chm.second = (ui->hdmi6_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi6_art->isChecked() ? true : false);
        } else if(i==6) {
            chm.on     = (ui->hdmi7->isChecked() ? true : false);
            chm.first  = (ui->hdmi7_1->isChecked() ? true : false);
            chm.second = (ui->hdmi7_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi7_art->isChecked() ? true : false);
        } else if(i==7) {
            chm.on     = (ui->hdmi8->isChecked() ? true : false);
            chm.first  = (ui->hdmi8_1->isChecked() ? true : false);
            chm.second = (ui->hdmi8_2->isChecked() ? true : false);
            chm.art    = (ui->hdmi8_art->isChecked() ? true : false);
        }
        chMap.push_back(chm);
    } //i


    // ------------------------------------------------- //
    //  configuration of indiviual VMM channels
    // ------------------------------------------------- //
    vector<Channel> channels;
    for(int i = 0; i < 64; i++) {
        Channel ch;
        ch.number           = i;
        ch.polarity         = VMMSPBool[i];
        ch.capacitance      = VMMSCBool[i];
        ch.leakage_current  = VMMSLBool[i];
        ch.test_pulse       = VMMSTBool[i];
        ch.hidden_mode      = VMMSMBool[i];
        ch.trim             = VMMSDValue[i];
        ch.monitor          = VMMSMXBool[i];
        ch.s10bitADC        = VMMSZ010bValue[i];
        ch.s8bitADC         = VMMSZ08bValue[i];
        ch.s6bitADC         = VMMSZ06bValue[i];
        channels.push_back(ch);
    } // i

    // global, chMap, channels
    configHandle().LoadBoardConfiguration(global, chMap, channels);

    // send the configuration to the FEC/boards
    //msg()(" !! NOT SENDING SPI CONFIG !! ");
    configModule().SendConfig();

    // toggle/status update
    m_configOK = true;
    emit checkFSM();
}
// ------------------------------------------------------------------------- //
void MainWindow::prepareAndSendTDAQConfig()
{

    TriggerDAQ daq;

    daq.tp_delay        = ui->pulserDelay->value();
    daq.trigger_period  = ui->trgPeriod->text();
    daq.acq_sync        = ui->acqSync->value();
    daq.acq_window      = ui->acqWindow->value();
    if(ui->trgPulser->isChecked()) daq.run_mode = "pulser";
    else if(ui->trgExternal->isChecked()) daq.run_mode = "external";
    else{daq.run_mode = "pulser";}
    //daq.run_mode        = "pulser"; // dummy here
    daq.run_count       = 20; // dummy here
    daq.ignore16        = ui->ignore16->isChecked();
    daq.output_path     = ui->runDirectoryField->text();
    daq.bcid_reset      = ui->bcid_reset->value();

    configHandle().LoadTDAQConfiguration(daq);

    // send the word
    runModule().setTriggerAcqConstants(); 

    // toggle/status update
    m_tdaqOK = true;
    emit checkFSM();
    
}
// ------------------------------------------------------------------------- //
void MainWindow::setRunMode()
{
    QString rmode = "";

    // run mode = pulser
    if(QObject::sender() == ui->trgPulser) {

        // don't re-configure the board for a setting which it is
        // already in
        if(m_runModeOK && configHandle().daqSettings().run_mode=="pulser") {
            ui->trgPulser->setDown(true);
            return;
        }
        ui->trgPulser->setDown(true);
        ui->trgExternal->setDown(false);
        ui->trgExternal->setChecked(false);
        rmode = "pulser";
    }
    // run mode = external
    else if(QObject::sender() == ui->trgExternal) {

        // don't re-configure the board for a setting which it is
        // already in
        if(m_runModeOK && configHandle().daqSettings().run_mode=="external") {
            ui->trgExternal->setDown(true);
            return;
        }
        ui->trgPulser->setDown(false);
        ui->trgPulser->setChecked(false);
        ui->trgExternal->setDown(true);
        rmode = "external";
    }

    TriggerDAQ daq;
    daq.tp_delay        = ui->pulserDelay->value();
    daq.trigger_period  = ui->trgPeriod->text();
    daq.acq_sync        = ui->acqSync->value();
    daq.acq_window      = ui->acqWindow->value();
    daq.run_mode        = rmode;
    daq.run_count       = 20; // dummy here
    daq.ignore16        = ui->ignore16->isChecked();
    daq.output_path     = ui->runDirectoryField->text();

    // update the config handle with the new DAQ object
    configHandle().LoadTDAQConfiguration(daq);

    // send the words
    runModule().setTriggerMode();

    // update status
    m_runModeOK = true;
    emit checkFSM();

}
// ------------------------------------------------------------------------- //
void MainWindow::setACQMode()
{
    // acquisition ON
    if(QObject::sender() == ui->onACQ) {
        
        // don't re-configure the board for a setting which it is
        // already in
        if(m_acqMode=="ON") {
            ui->onACQ->setDown(true);
            return;
        }
        ui->onACQ->setDown(true);
        ui->offACQ->setChecked(false);
        ui->offACQ->setDown(false);
        runModule().ACQon();
        m_acqMode = "ON";
    }
    // acquisition OFF
    else if(QObject::sender() == ui->offACQ) {
        if(m_acqMode=="OFF") {
            ui->offACQ->setDown(true);
            return;
        }
        ui->offACQ->setDown(true);
        ui->onACQ->setChecked(false);
        ui->onACQ->setDown(false);
        runModule().ACQoff();
        m_acqMode = "OFF";
    }

    emit checkFSM();
}
// ------------------------------------------------------------------------- //
void MainWindow::setNumberOfFecs(int)
{
    ui->setVMMs->clear();
    QString counter;
    for(int i = 0; i < ui->numberOfFecs->value(); i++){
        ips[i] = ui->ip1->text() + "." + ui->ip2->text() + "." + ui->ip3->text()
                    + "." + counter.setNum(ui->ip4->text().toInt()+i);
        ui->setVMMs->addItem(counter.setNum(i+1));
        ui->setVMMs->setCurrentIndex(i);
    }
    ui->setVMMs->addItem("All");
    ui->setVMMs->setCurrentIndex(ui->setVMMs->currentIndex()+1);

}
// ------------------------------------------------------------------------- //
void MainWindow::SetInitialState()
{

     msg()("Waiting to set FEB configuration type");
    //addmmfe8
    ui->mini2CheckBox->setEnabled(true);
    ui->mmfe8CheckBox->setEnabled(true);

    //wait for mmfe8 config before connection since MMFE8 doen't need a ping
    ui->openConnection->setEnabled(false);

    // write and SPI configuration
    ui->spiRB->setChecked(true);
    ui->writeRB->setChecked(true);

    // don't yet prepare the config
    //ui->prepareConfigButton->setEnabled(false);

    // disable ability to send config
    ui->SendConfiguration->setEnabled(true);
    ui->cmdlabel->setText("No\nComm.");

    // disable reset of FEC
    ui->fec_WarmInit->setEnabled(false);
    ui->fec_reset->setEnabled(false);

    // disable sending of T/DAQ settings and constants
    ui->setTrgAcqConst->setEnabled(false);
    ui->trgPulser->setEnabled(false);
    ui->trgExternal->setEnabled(false);
    ui->onACQ->setEnabled(false);
    ui->offACQ->setEnabled(false);

    // disable clock settings
    ui->setck_s6->setEnabled(false);
    ui->setTp_s6->setEnabled(false);

    // disable start/stop run
    //ui->clearTriggerCnt->setEnabled(false);
    ui->checkTriggers->setEnabled(true);       //True wste na emfanizete
    ui->stopTriggerCnt->setEnabled(false);

    // misc DAQ buttons
    ui->setEvbld->setEnabled(false);
    ui->asic_reset->setEnabled(false);

    // hdmi mask
    ui->setMask->setChecked(false);
    ui->setMask->setEnabled(false);
    ui->enableART->setEnabled(false);
    ui->linkPB->setEnabled(false);
    ui->resetLinks->setEnabled(false);

    // dac to mV
    QString tmp;
    #warning is this conversion still valid?
    ui->dacmvLabel->setText(tmp.number((0.6862*ui->sdt->value()+63.478), 'f', 2) + " mV");
    ui->dacmvLabel_TP->setText(tmp.number((0.6862*ui->sdp_2->value()+63.478), 'f', 2) + " mV");

    // disable calibration
    //ui->calibration->setChecked(false);
    ui->doCalib->setChecked(false);
    ui->doPDOCalib->setChecked(false);
    ui->doTDOCalib->setChecked(false);

}
// ------------------------------------------------------------------------- //
void MainWindow::CreateChannelsFields()
{
    Font.setPointSize(8);
    int margin = 8;
    channelGridLayout->setContentsMargins(margin*0.75, margin, margin*2*10, margin);
    channelGridLayout->setHorizontalSpacing(1);
    channelGridLayout->setVerticalSpacing(1);

    ui->gridLayout_channels->setContentsMargins(margin*0.75, margin, margin*4.4*5, margin);
    ui->gridLayout_channels->setHorizontalSpacing(1);
    ui->gridLayout_channels->setVerticalSpacing(1);
    ui->gridLayout_channels->setGeometry(QRect(620,10,380, 12));
//    ui->gridLayout_channels->setSpacing(1);

    QString initialValueRadio = "";
    QString counter;

    SPLabel = new QPushButton("SP");
    SCLabel = new QPushButton("SC");
    SLLabel = new QPushButton("SL");
    STLabel = new QPushButton("ST");
    SMLabel = new QPushButton("SM");
    SDLabel = new QComboBox();
    SZ010bLabel = new QComboBox();
    SZ08bLabel = new QComboBox();
    SZ06bLabel = new QComboBox();
    SMXLabel = new QPushButton("SMX");
    
    SPLabel2 = new QPushButton("SP");
    SCLabel2 = new QPushButton("SC");
    SLLabel2 = new QPushButton("SL");
    STLabel2 = new QPushButton("ST");
    SMLabel2 = new QPushButton("SM");
    SDLabel2 = new QComboBox();
    SZ010bLabel2 = new QComboBox();
    SZ08bLabel2 = new QComboBox();
    SZ06bLabel2 = new QComboBox();
    SMXLabel2 = new QPushButton("SMX");

    for(int i = 0; i < 16; i++) {
        SDLabel->addItem(counter.setNum(i)+" mV");
        SDLabel2->addItem(counter.setNum(i)+" mV");
    }
    for(int i = 0; i < 32; i++) {
        SZ010bLabel->addItem(counter.setNum(i)+" ns");
        SZ010bLabel2->addItem(counter.setNum(i)+" ns");
    }
    for(int i = 0; i < 16; i++) {
        SZ08bLabel->addItem(counter.setNum(i)+" ns");
        SZ08bLabel2->addItem(counter.setNum(i)+" ns");
    }
    for(int i = 0; i < 8; i++) {
        SZ06bLabel->addItem(counter.setNum(i)+" ns");
        SZ06bLabel2->addItem(counter.setNum(i)+" ns");
    }

    VMMSPBoolAll=0;
    VMMSCBoolAll=0;
    VMMSLBoolAll=0;
    VMMSTBoolAll=0;
    VMMSMBoolAll=0;
    VMMSMXBoolAll=0;
    VMMSZ010bBoolAll=0;
    VMMSZ08bBoolAll=0;
    VMMSZ06bBoolAll=0;

    VMMSPBoolAll2=0;
    VMMSCBoolAll2=0;
    VMMSLBoolAll2=0;
    VMMSTBoolAll2=0;
    VMMSMBoolAll2=0;
    VMMSMXBoolAll2=0;
    VMMSZ010bBoolAll2=0;
    VMMSZ08bBoolAll2=0;
    VMMSZ06bBoolAll2=0;

    SPLabel->setFixedSize(20,15);
    SCLabel->setFixedSize(20,15);
    SLLabel->setFixedSize(20,15);
    STLabel->setFixedSize(20,15);
    SMLabel->setFixedSize(20,15);
    SDLabel->setFixedSize    (50,19);
    SZ010bLabel->setFixedSize(50,19);
    SZ08bLabel->setFixedSize (50,19);
    SZ06bLabel->setFixedSize (50,19);
    SMXLabel->setFixedSize(30,15);
    SPLabel2->setFixedSize(20,15);
    SCLabel2->setFixedSize(20,15);
    SLLabel2->setFixedSize(20,15);
    STLabel2->setFixedSize(20,15);
    SMLabel2->setFixedSize(20,15);
    SDLabel2->setFixedSize(50,20);
    SZ010bLabel2->setFixedSize(50,20);
    SZ08bLabel2->setFixedSize(50,20);
    SZ06bLabel2->setFixedSize(50,20);
    SMXLabel2->setFixedSize(30,15);

    SDLabel     ->installEventFilter(this);
    SZ010bLabel ->installEventFilter(this);
    SZ08bLabel  ->installEventFilter(this);
    SZ06bLabel  ->installEventFilter(this);

    SPLabel->setFont(Font);
    SCLabel->setFont(Font);
    SLLabel->setFont(Font);
    STLabel->setFont(Font);
    SMLabel->setFont(Font);
    SDLabel->setFont(Font);
    SZ010bLabel->setFont(Font);
    SZ08bLabel->setFont(Font);
    SZ06bLabel->setFont(Font);
    SMXLabel->setFont(Font);
    SPLabel2->setFont(Font);
    SCLabel2->setFont(Font);
    SLLabel2->setFont(Font);
    STLabel2->setFont(Font);
    SMLabel2->setFont(Font);
    SDLabel2->setFont(Font);
    SZ010bLabel2->setFont(Font);
    SZ08bLabel2->setFont(Font);
    SZ06bLabel2->setFont(Font);
    SMXLabel2->setFont(Font);

    int col = 5;
        QLabel *spacer = new QLabel("");
    ui->gridLayout_channels->addWidget(spacer);
    ui->gridLayout_channels->addWidget(SPLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SCLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SLLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(STLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SMLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SDLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SMXLabel,    0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ010bLabel, 0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ08bLabel,  0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ06bLabel,  0, col, Qt::AlignCenter);

    for (int i = 0; i<64; i++){
        VMMChannel[i] = new QLineEdit(counter.setNum(i+1),ui->tab_3);
        VMMChannel[i]->setAlignment(Qt::AlignHCenter);
        
        VMMSC[i] = new QPushButton(initialValueRadio,ui->tab_3);
        VMMSL[i] = new QPushButton(initialValueRadio,ui->tab_3);
        VMMST[i] = new QPushButton(initialValueRadio,ui->tab_3);
        VMMSM[i] = new QPushButton(initialValueRadio,ui->tab_3);
        VMMSMX[i] = new QPushButton(initialValueRadio,ui->tab_3);
        
        
        VMMSC[i]->setFixedSize (15,15);
        VMMST[i]->setFixedSize (15,15);
        VMMSL[i]->setFixedSize (15,15);
        VMMSM[i]->setFixedSize (15,15);
        VMMSMX[i]->setFixedSize(15,15);

        VMMSC[i]->setStyleSheet("background-color: lightGray");
        VMMSM[i]->setStyleSheet("background-color: lightGray");
        VMMST[i]->setStyleSheet("background-color: lightGray");
        VMMSL[i]->setStyleSheet("background-color: lightGray");
        VMMSMX[i]->setStyleSheet("background-color: lightGray");
   
   
        VMMSPBool[i]=0;
        VMMSCBool[i]=0;
        VMMSMBool[i]=0;
        VMMSTBool[i]=0;
        VMMSLBool[i]=0;
        VMMSMXBool[i]=0;

        VMMChannel[i]->setEnabled(0);
        VMMSDVoltage[i] = new QComboBox(ui->tab_3);
        VMMSDVoltage[i]->setFixedSize(50,20);
        VMMSDVoltage[i]->setFont(Font);
 
        for(int j=0;j<16;j++){
            VMMSDVoltage[i]->addItem(counter.setNum(j)+" mV");
        }

        VMMSZ010bCBox[i] = new QComboBox(ui->tab_3);
        VMMSZ010bCBox[i]->setFixedSize(50,20);
        VMMSZ010bCBox[i]->setFont(Font);

        VMMSZ08bCBox[i] = new QComboBox(ui->tab_3);
        VMMSZ08bCBox[i]->setFixedSize(50,20);
        VMMSZ08bCBox[i]->setFont(Font);

        VMMSZ06bCBox[i] = new QComboBox(ui->tab_3);
        VMMSZ06bCBox[i]->setFixedSize(50,20);
        VMMSZ06bCBox[i]->setFont(Font);

     
        for(int j=0;j<32;j++){
            VMMSZ010bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<16;j++){
            VMMSZ08bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<8;j++){
            VMMSZ06bCBox[i]->addItem(counter.setNum(j)+" ns");
        }

       // set initial ADC values
        VMMSZ010bCBox[i]->setCurrentIndex(0);
        VMMSZ010bValue[i]=0;
        VMMSZ08bCBox[i]->setCurrentIndex(0);
        VMMSZ08bValue[i]=0;
        VMMSZ06bCBox[i]->setCurrentIndex(0);
        VMMSZ06bValue[i]=0;

        VMMNegativeButton[i] = new QPushButton(ui->tab_3);
        VMMNegativeButton[i]->setText("negative");

        VMMChannel[i]->setFixedSize(20,18);
        VMMNegativeButton[i]->setFixedSize(40,18);
        VMMNegativeButton[i]->setFont(Font);
        QLabel *spacer = new QLabel("");

        if(i==0) {
      //          ui->gridLayout_channels->addWidget(SPLabel,     i,2, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SCLabel,     i,3, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SLLabel,     i,4, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(STLabel,     i,5, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SMLabel,     i,6, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SDLabel,     i,7, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SMXLabel,    i,8, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SZ010bLabel, i,9, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SZ08bLabel,  i,10, Qt::AlignCenter);
      //          ui->gridLayout_channels->addWidget(SZ06bLabel,  i,11, Qt::AlignCenter);
        }
        channelGridLayout->addWidget(VMMChannel[i],         i+1,1, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMNegativeButton[i],  i+1,2, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSC[i],              i+1,3, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSL[i],              i+1,4, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMST[i],              i+1,5, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSM[i],              i+1,6, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSDVoltage[i],       i+1,7, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSMX[i],             i+1,8, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSZ010bCBox[i],      i+1,9, Qt::AlignCenter);
        VMMSZ010bCBox[i]->setFocusPolicy(Qt::StrongFocus);
        channelGridLayout->addWidget(VMMSZ08bCBox[i],       i+1,10, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSZ06bCBox[i],       i+1,11, Qt::AlignCenter);

        VMMSDVoltage[i]         ->installEventFilter(this);
        VMMSDVoltage[i]->setFocusPolicy(Qt::ClickFocus);
        VMMSZ010bCBox[i]        ->installEventFilter(this);
        VMMSZ08bCBox[i]         ->installEventFilter(this);
        VMMSZ06bCBox[i]         ->installEventFilter(this);
        

/*
        if(i<32){
            if(i==0){
                channelGridLayout->addWidget(SPLabel,     i,2, Qt::AlignCenter);
                channelGridLayout->addWidget(SCLabel,     i,3, Qt::AlignCenter);
                channelGridLayout->addWidget(SLLabel,     i,4, Qt::AlignCenter);
                channelGridLayout->addWidget(STLabel,     i,5, Qt::AlignCenter);
                channelGridLayout->addWidget(SMLabel,     i,6, Qt::AlignCenter);
                channelGridLayout->addWidget(SDLabel,     i,7, Qt::AlignCenter);
                channelGridLayout->addWidget(SMXLabel,    i,8, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ010bLabel, i,9, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ08bLabel,  i,10, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ06bLabel,  i,11, Qt::AlignCenter);

                channelGridLayout->addWidget(SPLabel2,     i,14, Qt::AlignCenter);
                channelGridLayout->addWidget(SCLabel2,     i,15, Qt::AlignCenter);
                channelGridLayout->addWidget(SLLabel2,     i,16, Qt::AlignCenter);
                channelGridLayout->addWidget(STLabel2,     i,17, Qt::AlignCenter);
                channelGridLayout->addWidget(SMLabel2,     i,18, Qt::AlignCenter);
                channelGridLayout->addWidget(SDLabel2,     i,19, Qt::AlignCenter);
                channelGridLayout->addWidget(SMXLabel2,    i,20, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ010bLabel2, i,21, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ08bLabel2,  i,22, Qt::AlignCenter);
                channelGridLayout->addWidget(SZ06bLabel2,  i,23, Qt::AlignCenter);
            }
            channelGridLayout->addWidget(VMMChannel[i],         i+1,1, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMNegativeButton[i],  i+1,2, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSC[i],              i+1,3, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSL[i],              i+1,4, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMST[i],              i+1,5, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSM[i],              i+1,6, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSDVoltage[i],       i+1,7, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSMX[i],             i+1,8, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ010bCBox[i],      i+1,9, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ08bCBox[i],       i+1,10, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ06bCBox[i],       i+1,11, Qt::AlignCenter);
            channelGridLayout->addWidget(spacer,                 i+1,12, Qt::AlignLeft);
        }
        else{
            channelGridLayout->addWidget(VMMChannel[i],         i-32+1,13, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMNegativeButton[i],  i-32+1,14, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSC[i],              i-32+1,15, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSL[i],              i-32+1,16, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMST[i],              i-32+1,17, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSM[i],              i-32+1,18, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSDVoltage[i],       i-32+1,19, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSMX[i],             i-32+1,20, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ010bCBox[i],      i-32+1,21, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ08bCBox[i],       i-32+1,22, Qt::AlignCenter);
            channelGridLayout->addWidget(VMMSZ06bCBox[i],       i-32+1,23, Qt::AlignCenter);
        }

*/
    } // i

    //channelGridLayout->setGeometry(QRect(620,12,50,50));
    channelGridLayout->setSpacing(1);
    channelGridLayout->setGeometry(QRect(620,12,380,2000));
    dummy->setLayout(channelGridLayout);
    //ui->scrollArea->setLayout(channelGridLayout);
    ui->scrollArea->setWidget(dummy);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //ui->tab_3->setGeometry(QRect(620,12,50,50));
    //ui->tab_3->setGeometry(QRect(620,12,730,700));
    //ui->tab_3->setLayout(channelGridLayout);

    //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // channel connections
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////

    // -------------------------------------------------------------------- //
    // update channel voltages
    // -------------------------------------------------------------------- //
    connect(SDLabel,  SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelVoltages(int)));
    connect(SDLabel2, SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelVoltages(int)));


    // -------------------------------------------------------------------- //
    // update channel ADC values
    // -------------------------------------------------------------------- //
    connect(SZ010bLabel,  SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ08bLabel,   SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ06bLabel,   SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ010bLabel2, SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ08bLabel2,  SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ06bLabel2,  SIGNAL(currentIndexChanged(int)),
                                        this,SLOT(updateChannelADCs(int)));


    // -------------------------------------------------------------------- //
    // update channel states (green boxes in Channel Fields)
    // -------------------------------------------------------------------- //
    connect(SPLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SCLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SLLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(STLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SMLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SMXLabel,  SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SPLabel2,  SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SCLabel2,  SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SLLabel2,  SIGNAL(pressed()),
                                          this, SLOT(updateChannelState()));
    connect(STLabel2,  SIGNAL(pressed()),
                                          this, SLOT(updateChannelState()));
    connect(SMLabel2,  SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SMXLabel2, SIGNAL(pressed()), this, SLOT(updateChannelState()));

    ///////////////////////////////////////////////////////////////////
    // do updates for individual channels
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < 64; i++) {

        // ---------- channel voltages ----------- //
        connect(VMMSDVoltage[i],SIGNAL(currentIndexChanged(int)),
                                        this,SLOT(updateChannelVoltages(int)));

        // ---------------- ADCs ---------------- //
        connect(VMMSZ010bCBox[i],SIGNAL(currentIndexChanged(int)),
                                        this,SLOT(updateChannelADCs(int)));
        connect(VMMSZ08bCBox[i],SIGNAL(currentIndexChanged(int)),
                                            this,SLOT(updateChannelADCs(int)));
        connect(VMMSZ06bCBox[i],SIGNAL(currentIndexChanged(int)),
                                            this,SLOT(updateChannelADCs(int)));
        // ----------- channel states ----------- //
        //connect(VMMNegativeButton[i],SIGNAL(pressed()),
        connect(VMMNegativeButton[i],SIGNAL(clicked()),
                                            this,SLOT(updateChannelState()));
        connect(VMMSC[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSM[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMST[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSL[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSMX[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
    }

}
// ------------------------------------------------------------------------- //
void MainWindow::updateChannelState()
{
    // ***********************  SC  ********************************* //
    if(SCLabel == QObject::sender()){
        if(VMMSCBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: green");
                VMMSCBool[j]=true;
            }
            VMMSCBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: lightGray");
                VMMSCBool[j]=0;
            }
            VMMSCBoolAll=0;
        }
    }//else if(SCLabel2 == QObject::sender()){
//        if(VMMSCBoolAll2==0){
//            for(int j=32;j<64;j++){
//                VMMSC[j]->setStyleSheet("background-color: green");
//                VMMSCBool[j]=true;
//            }
//            VMMSCBoolAll2=1;
//        }else{
//            for(int j=32;j<64;j++){
//                VMMSC[j]->setStyleSheet("background-color: lightGray");
//                VMMSCBool[j]=0;
//            }
//            VMMSCBoolAll2=0;
//        }
//    }

    // ***********************  SL  ********************************* //
    if(SLLabel == QObject::sender()){
        if(VMMSLBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSL[j]->setStyleSheet("background-color: green");
                VMMSLBool[j]=true;
            }
            VMMSLBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSL[j]->setStyleSheet("background-color: lightGray");
                VMMSLBool[j]=0;
            }
            VMMSLBoolAll=0;
        }
    }//else if(SLLabel2 == QObject::sender()){
 //       if(VMMSLBoolAll2==0){
 //           for(int j=32;j<64;j++){
 //               VMMSL[j]->setStyleSheet("background-color: green");
 //               VMMSLBool[j]=true;
 //           }
 //           VMMSLBoolAll2=1;
 //       }else{
 //           for(int j=32;j<64;j++){
 //               VMMSL[j]->setStyleSheet("background-color: lightGray");
 //               VMMSLBool[j]=0;
 //           }
 //           VMMSLBoolAll2=0;
 //       }
 //   }
    // ***********************  ST  ********************************* //
    if(STLabel == QObject::sender()){
        if(VMMSTBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMST[j]->setStyleSheet("background-color: green");
                VMMSTBool[j]=true;
            }
            VMMSTBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMST[j]->setStyleSheet("background-color: lightGray");
                VMMSTBool[j]=0;
            }
            VMMSTBoolAll=0;
        }
    }//else if(STLabel2 == QObject::sender()){
 //       if(VMMSTBoolAll2==0){
 //           for(int j=32;j<64;j++){
 //               VMMST[j]->setStyleSheet("background-color: green");
 //               VMMSTBool[j]=true;
 //           }
 //           VMMSTBoolAll2=1;
 //       }else{
 //           for(int j=32;j<64;j++){
 //               VMMST[j]->setStyleSheet("background-color: lightGray");
 //               VMMSTBool[j]=0;
 //           }
 //           VMMSTBoolAll2=0;
 //       }
 //   }
    // ***********************  SM  ********************************* //
    if(SMLabel == QObject::sender()){
        if(VMMSMBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSM[j]->setStyleSheet("background-color: green");
                VMMSMBool[j]=true;
            }
            VMMSMBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSM[j]->setStyleSheet("background-color: lightGray");
                VMMSMBool[j]=0;
            }
            VMMSMBoolAll=0;
        }
    }//else if(SMLabel2 == QObject::sender()){
 //       if(VMMSMBoolAll2==0){
 //           for(int j=32;j<64;j++){
 //               VMMSM[j]->setStyleSheet("background-color: green");
 //               VMMSMBool[j]=true;
 //           }
 //           VMMSMBoolAll2=1;
 //       }else{
 //           for(int j=32;j<64;j++){
 //               VMMSM[j]->setStyleSheet("background-color: lightGray");
 //               VMMSMBool[j]=0;
 //           }
 //           VMMSMBoolAll2=0;
 //       }
 //   }
    // ***********************  SMX  ********************************* //
    if(SMXLabel == QObject::sender()){
        if(VMMSMXBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSMX[j]->setStyleSheet("background-color: green");
                VMMSMXBool[j]=true;
            }
            VMMSMXBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSMX[j]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[j]=0;
            }
            VMMSMXBoolAll=0;
        }
    }//else if(SMXLabel2 == QObject::sender()){
 //       if(VMMSMXBoolAll2==0){
 //           for(int j=32;j<64;j++){
 //               VMMSMX[j]->setStyleSheet("background-color: green");
 //               VMMSMXBool[j]=true;
 //           }
 //           VMMSMXBoolAll2=1;
 //       }else{
 //           for(int j=32;j<64;j++){
 //               VMMSMX[j]->setStyleSheet("background-color: lightGray");
 //               VMMSMXBool[j]=0;
 //           }
 //           VMMSMXBoolAll2=0;
 //       }
 //   }

    // ******************  SMP (negative buttons) *********************** //
    if(SPLabel == QObject::sender()){
        if(VMMSPBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMNegativeButton[j]->setStyleSheet("background-color: green");
                VMMSPBool[j]=true;
            }
            VMMSPBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMNegativeButton[j]->setStyleSheet("background-color: light");
                VMMSPBool[j]=0;
            }
            VMMSPBoolAll=0;
        }
    }//else if(SPLabel2 == QObject::sender()){
 //       if(VMMSPBoolAll2==0){
 //           for(int j=32;j<64;j++){
 //               VMMNegativeButton[j]->setStyleSheet("background-color: green");
 //               VMMSPBool[j]=true;
 //           }
 //           VMMSPBoolAll2=1;
 //       }else{
 //           for(int j=32;j<64;j++){
 //               VMMNegativeButton[j]->setStyleSheet("background-color: light");
 //               VMMSPBool[j]=0;
 //           }
 //           VMMSPBoolAll2=0;
 //       }
 //   }
    // *********************  Loop Individually  ********************** //
    for(int i=0;i<64;i++){
        if(VMMSC[i] == QObject::sender()){
            if(VMMSCBool[i]==0){
                VMMSC[i]->setStyleSheet("background-color: green");
                VMMSCBool[i]=true;
            }else if(VMMSCBool[i]==1){
                VMMSC[i]->setStyleSheet("background-color: lightGray");
                VMMSCBool[i]=false;
            }
        }else if(VMMST[i] == QObject::sender()){
            if(VMMSTBool[i]==0){
                VMMST[i]->setStyleSheet("background-color: green");
                VMMSTBool[i]=true;
            }else if(VMMSTBool[i]==1){
                VMMST[i]->setStyleSheet("background-color: lightGray");
                VMMSTBool[i]=false;
            }
        }else if(VMMSL[i] == QObject::sender()){
            if(VMMSLBool[i]==0){
                VMMSL[i]->setStyleSheet("background-color: green");
                VMMSLBool[i]=true;
            }else if(VMMSLBool[i]==1){
                VMMSL[i]->setStyleSheet("background-color: lightGray");
                VMMSLBool[i]=false;
            }
        }else if(VMMSM[i] == QObject::sender()){
            if(VMMSMBool[i]==0){
                VMMSM[i]->setStyleSheet("background-color: green");
                VMMSMBool[i]=true;
            }else if(VMMSMBool[i]==1){
                VMMSM[i]->setStyleSheet("background-color: lightGray");
                VMMSMBool[i]=false;
            }
        }else if(VMMSMX[i] == QObject::sender()){
            if(VMMSMXBool[i]==0){
                VMMSMX[i]->setStyleSheet("background-color: green");
                VMMSMXBool[i]=true;
            }else if(VMMSMXBool[i]==1){
                VMMSMX[i]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[i]=false;
            }
        }else if(VMMNegativeButton[i] == QObject::sender()){
            if(VMMSPBool[i]==0){
                VMMNegativeButton[i]->setStyleSheet("background-color: green");
                VMMSPBool[i]=true;
            }else if(VMMSPBool[i]==1){
                VMMNegativeButton[i]->setStyleSheet("background-color: light");
                VMMSPBool[i]=false;
            }
        }
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::updateChannelVoltages(int index){
    // ***********************  SD  ******************************** //
    if(SDLabel == QObject::sender()){
        for(int j=0;j<64;j++){
        //for(int j=0;j<32;j++){
            VMMSDVoltage[j]->setCurrentIndex(index);
            VMMSDValue[j]=index;
        }
    }
  //  if(SDLabel2 == QObject::sender()){
  //      for(int j=32;j<64;j++){
  //          VMMSDVoltage[j]->setCurrentIndex(index);
  //          VMMSDValue[j]=index;
  //      }
  //  }
    for(int i=0;i<64;i++){
        if(VMMSDVoltage[i] == QObject::sender()){
            VMMSDValue[i]=index;
        }
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::updateChannelADCs(int index)
{
    // ***********************  SD  ******************************* //
    for(int j=0;j<64;j++){
    //for(int j=0;j<32;j++){
        if(SZ010bLabel == QObject::sender()){
            VMMSZ010bCBox[j]->setCurrentIndex(index);
            VMMSZ010bValue[j]=index;
        }
        if(SZ08bLabel == QObject::sender()){
            VMMSZ08bCBox[j]->setCurrentIndex(index);
            VMMSZ08bValue[j]=index;
        }
        if(SZ06bLabel == QObject::sender()){
            VMMSZ06bCBox[j]->setCurrentIndex(index);
            VMMSZ06bValue[j]=index;
        }
    }

  //  for(int j=32;j<64;j++){
  //      if(SZ010bLabel2 == QObject::sender()){
  //          VMMSZ010bCBox[j]->setCurrentIndex(index);
  //          VMMSZ010bValue[j]=index;
  //      }
  //      if(SZ08bLabel2 == QObject::sender()){
  //          VMMSZ08bCBox[j]->setCurrentIndex(index);
  //          VMMSZ08bValue[j]=index;
  //      }
  //      if(SZ06bLabel2 == QObject::sender()){
  //          VMMSZ06bCBox[j]->setCurrentIndex(index);
  //          VMMSZ06bValue[j]=index;
  //      }
  //  }
    for(int i=0;i<64;i++){
        if(VMMSZ010bCBox[i] == QObject::sender()){
            VMMSZ010bValue[i]=index;
        }
        if(VMMSZ08bCBox[i] == QObject::sender()){
            VMMSZ08bValue[i]=index;
        }
        if(VMMSZ06bCBox[i] == QObject::sender()){
            VMMSZ06bValue[i]=index;
        }
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::loadConfigurationFromFile()
{

    // Load the file from the user
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Load Configuration XML File"), "../configs/",
        tr("XML Files (*.xml)"));
    if(filename.isNull()) return;

    stringstream sx;
    sx << "Loading configuration from file: " << filename.toStdString();
    msg()(sx);

    // pass the file to ConfigHandler to parse and load
    configHandle().LoadConfig(filename);
    updateConfigState();
}
// ------------------------------------------------------------------------- //
void MainWindow::updateConfigState()
{

    // ------------------------------------------- //
    //  GlobalSetting
    // ------------------------------------------- //
    GlobalSetting global = configHandle().globalSettings();

    ui->spg->setCurrentIndex(global.polarity);
    ui->slg->setCurrentIndex(global.leakage_current);
    ui->sdrv->setCurrentIndex(global.analog_tristates);
    ui->sfm->setCurrentIndex(global.double_leakage);
    ui->sg->setCurrentIndex(global.gain);
    ui->st->setCurrentIndex(global.peak_time);
    ui->sng->setCurrentIndex(global.neighbor_trigger);
    ui->stc->setCurrentIndex(global.tac_slope);
    ui->sdp->setCurrentIndex(global.disable_at_peak);
    ui->sfa->setCurrentIndex(global.art);
    ui->sfam->setCurrentIndex(global.art_mode);
    ui->sdcka->setCurrentIndex(global.dual_clock_art);
    ui->sbfm->setCurrentIndex(global.out_buffer_mo);
    ui->sbfp->setCurrentIndex(global.out_buffer_pdo);
    ui->sbft->setCurrentIndex(global.out_buffer_tdo);
    ui->sm5_sm0->setCurrentIndex(global.channel_monitor);
    ui->scmx->setCurrentIndex(global.monitoring_control);
    ui->sbmx->setCurrentIndex(global.monitor_pdo_out);
    ui->spdc->setCurrentIndex(global.adcs);
    ui->ssh->setCurrentIndex(global.sub_hysteresis);
    ui->sttt->setCurrentIndex(global.direct_time);
    ui->stpp->setCurrentIndex(global.direct_time_mode0);
    ui->stot->setCurrentIndex(global.direct_time_mode1);
    ui->s8b->setCurrentIndex(global.conv_mode_8bit);
    ui->s6b->setCurrentIndex(global.enable_6bit);
    ui->sc010b->setCurrentIndex(global.adc_10bit);
    ui->sc08b->setCurrentIndex(global.adc_8bit);
    ui->sc06b->setCurrentIndex(global.adc_6bit);
    ui->sdcks->setCurrentIndex(global.dual_clock_data);
    ui->sdck6b->setCurrentIndex(global.dual_clock_6bit);
    ui->sdt->setValue(global.threshold_dac);
    ui->sdp_2->setValue(global.test_pulse_dac);
 

/*
    // ------------------------------------------- //
    //  HDMI Channel Map
    // ------------------------------------------- //
    vector<ChannelMap> chMap;
    for(int i = 0; i < 8; i++) {
        chMap.push_back(configHandle().hdmiChannelSettings(i));
    }
    int ch = 0;
    ui->hdmi1->setChecked(  chMap[ch].on);
    ui->hdmi1_1->setChecked(chMap[ch].first);
    ui->hdmi1_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi2->setChecked(  chMap[ch].on);
    ui->hdmi2_1->setChecked(chMap[ch].first);
    ui->hdmi2_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi3->setChecked(  chMap[ch].on);
    ui->hdmi3_1->setChecked(chMap[ch].first);
    ui->hdmi3_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi4->setChecked(  chMap[ch].on);
    ui->hdmi4_1->setChecked(chMap[ch].first);
    ui->hdmi4_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi5->setChecked(  chMap[ch].on);
    ui->hdmi5_1->setChecked(chMap[ch].first);
    ui->hdmi5_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi6->setChecked(  chMap[ch].on);
    ui->hdmi6_1->setChecked(chMap[ch].first);
    ui->hdmi6_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi7->setChecked(  chMap[ch].on);
    ui->hdmi7_1->setChecked(chMap[ch].first);
    ui->hdmi7_2->setChecked(chMap[ch].second);
    ch++;
    ui->hdmi8->setChecked(  chMap[ch].on);
    ui->hdmi8_1->setChecked(chMap[ch].first);
    ui->hdmi8_2->setChecked(chMap[ch].second);
*/

    // ------------------------------------------------- //
    //  individual channels
    // ------------------------------------------------- //
    for(int i = 0; i < 64; i++) {
        Channel chan = configHandle().channelSettings(i);
        if(!((int)chan.number == i)) {
            stringstream ss;
            ss << "Channel config from ConfigHandler is out of sync!"
               << " Attempting to access state of channel number " << i
               << " but corresponding index in ConfigHandler channel map is "
               << chan.number << "!";
            msg()(ss, "MainWindow::updateConfigState", true);
            exit(1);
        }

        // for some reason in the original code
        // booleans of 0 are 'true' and booleans
        // of 1 are 'false'. go figure?
        VMMSPBool[i] = !chan.polarity;
        VMMSCBool[i] = !chan.capacitance;
        VMMSLBool[i] = !chan.leakage_current;
        VMMSTBool[i] = !chan.test_pulse;
        VMMSMBool[i] = !chan.hidden_mode;
        VMMSMXBool[i] = !chan.monitor;
        VMMSDValue[i] = chan.trim;
        VMMSZ010bValue[i] = chan.s10bitADC;
        VMMSZ08bValue[i] = chan.s8bitADC;
        VMMSZ06bValue[i] = chan.s6bitADC;

        // make the gui gui
        VMMNegativeButton[i]->click();
        VMMSMX[i]->click();
        VMMSM[i]->click();
        VMMSL[i]->click();
        VMMST[i]->click();
        VMMSC[i]->click();
        VMMSDVoltage[i]->setCurrentIndex(VMMSDValue[i]);
        VMMSZ010bCBox[i]->setCurrentIndex(VMMSZ010bValue[i]);
        VMMSZ08bCBox[i]->setCurrentIndex(VMMSZ08bValue[i]);
        VMMSZ06bCBox[i]->setCurrentIndex(VMMSZ06bValue[i]);

    }
}
// ------------------------------------------------------------------------- //
void MainWindow::writeConfigurationToFile()
{

    // open a dialog to select the output file name
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save XML Configuration File"), "../configs",
        tr("XML Files (*.xml)"));

    if(filename.isNull()) return;

    // now be sure to load all the configuration
    // ------------------------------------------------- //
    //  global settings
    // ------------------------------------------------- //
    GlobalSetting global;

    global.polarity            = ui->spg->currentIndex();
    global.leakage_current     = ui->slg->currentIndex();
    global.analog_tristates    = ui->sdrv->currentIndex();
    global.double_leakage      = ui->sfm->currentIndex();
    global.gain                = ui->sg->currentIndex();
    global.peak_time           = ui->st->currentIndex();
    global.neighbor_trigger    = ui->sng->currentIndex();
    global.tac_slope           = ui->stc->currentIndex();
    global.disable_at_peak     = ui->sdp->currentIndex();
    global.art                 = ui->sfa->currentIndex();
    global.art_mode            = ui->sfam->currentIndex();
    global.dual_clock_art      = ui->sdcka->currentIndex();
    global.out_buffer_mo       = ui->sbfm->currentIndex();
    global.out_buffer_pdo      = ui->sbfp->currentIndex();
    global.out_buffer_tdo      = ui->sbft->currentIndex();
    global.channel_monitor     = ui->sm5_sm0->currentIndex();
    global.monitoring_control  = ui->scmx->currentIndex();
    global.monitor_pdo_out     = ui->sbmx->currentIndex();
    global.adcs                = ui->spdc->currentIndex();
    global.sub_hysteresis      = ui->ssh->currentIndex();
    global.direct_time         = ui->sttt->currentIndex();


    global.direct_time_mode0   = ui->stpp->currentIndex();
    global.direct_time_mode1   = ui->stot->currentIndex();

    // build up the 2-bit word from the mode0 and mode1
    bool ok;
    QString tmp;
    tmp.append(QString::number(global.direct_time_mode0));
    tmp.append(QString::number(global.direct_time_mode1));
    global.direct_time_mode = tmp.toUInt(&ok,2);

    global.conv_mode_8bit      = ui->s8b->currentIndex();
    global.enable_6bit         = ui->s6b->currentIndex();
    global.adc_10bit           = ui->sc010b->currentIndex();
    global.adc_8bit            = ui->sc08b->currentIndex();
    global.adc_6bit            = ui->sc06b->currentIndex();
    global.dual_clock_data     = ui->sdcks->currentIndex();
    global.dual_clock_6bit     = ui->sdck6b->currentIndex();
    global.threshold_dac       = ui->sdt->value();
    global.test_pulse_dac      = ui->sdp_2->value();
    

    // ------------------------------------------------- //
    //  HDMI channel map
    // ------------------------------------------------- //
    vector<ChannelMap> chMap;
    ChannelMap chm;
    // do this by hand
    for(int i = 0; i < 8; i++) {
        chm.hdmi_no = i;
        if       (i==0) {
            chm.on     = (ui->hdmi1->isChecked() ? true : false);
            chm.first  = (ui->hdmi1_1->isChecked() ? true : false);
            chm.second = (ui->hdmi1_2->isChecked() ? true : false);
        } else if(i==1) {
            chm.on     = (ui->hdmi2->isChecked() ? true : false);
            chm.first  = (ui->hdmi2_1->isChecked() ? true : false);
            chm.second = (ui->hdmi2_2->isChecked() ? true : false);
        } else if(i==2) {
            chm.on     = (ui->hdmi3->isChecked() ? true : false);
            chm.first  = (ui->hdmi3_1->isChecked() ? true : false);
            chm.second = (ui->hdmi3_2->isChecked() ? true : false);
        } else if(i==3) {
            chm.on     = (ui->hdmi4->isChecked() ? true : false);
            chm.first  = (ui->hdmi4_1->isChecked() ? true : false);
            chm.second = (ui->hdmi4_2->isChecked() ? true : false);
        } else if(i==4) {
            chm.on     = (ui->hdmi5->isChecked() ? true : false);
            chm.first  = (ui->hdmi5_1->isChecked() ? true : false);
            chm.second = (ui->hdmi5_2->isChecked() ? true : false);
        } else if(i==5) {
            chm.on     = (ui->hdmi6->isChecked() ? true : false);
            chm.first  = (ui->hdmi6_1->isChecked() ? true : false);
            chm.second = (ui->hdmi6_2->isChecked() ? true : false);
        } else if(i==6) {
            chm.on     = (ui->hdmi7->isChecked() ? true : false);
            chm.first  = (ui->hdmi7_1->isChecked() ? true : false);
            chm.second = (ui->hdmi7_2->isChecked() ? true : false);
        } else if(i==7) {
            chm.on     = (ui->hdmi8->isChecked() ? true : false);
            chm.first  = (ui->hdmi8_1->isChecked() ? true : false);
            chm.second = (ui->hdmi8_2->isChecked() ? true : false);
        }
        chMap.push_back(chm);
    } //i


    // ------------------------------------------------- //
    //  configuration of indiviual VMM channels
    // ------------------------------------------------- //
    vector<Channel> channels;
    for(int i = 0; i < 64; i++) {
        Channel ch;
        ch.number           = i;
        ch.polarity         = VMMSPBool[i];
        ch.capacitance      = VMMSCBool[i];
        ch.leakage_current  = VMMSLBool[i];
        ch.test_pulse       = VMMSTBool[i];
        ch.hidden_mode      = VMMSMBool[i];
        ch.trim             = VMMSDValue[i];
        ch.monitor          = VMMSMXBool[i];
        ch.s10bitADC        = VMMSZ010bValue[i];
        ch.s8bitADC         = VMMSZ08bValue[i];
        ch.s6bitADC         = VMMSZ06bValue[i];
        channels.push_back(ch);
    } // i

    // global, chMap, channels
    configHandle().LoadBoardConfiguration(global, chMap, channels);

    // ------------------------------------------------- //
    //  T/DAQ settings
    // ------------------------------------------------- //
    TriggerDAQ daq;

    daq.tp_delay        = ui->pulserDelay->value();
    daq.trigger_period  = ui->trgPeriod->text();
    daq.acq_sync        = ui->acqSync->value();
    daq.acq_window      = ui->acqWindow->value();
    daq.run_mode        = "pulser"; // dummy here
    daq.run_count       = 20; // dummy here
    daq.ignore16        = ui->ignore16->isChecked();
    daq.output_path     = ui->runDirectoryField->text();

    configHandle().LoadTDAQConfiguration(daq);

    configHandle().WriteConfig(filename);
}
// ------------------------------------------------------------------------- //
void MainWindow::setAndSendEventHeaders()
{
    runModule().setEventHeaders(ui->evbld_infodata->currentIndex(),
                                ui->evbld_mode->currentIndex(),
                                ui->timeStampResCheckBox->isChecked());
    ui->appRB->setChecked(1);
}
// ------------------------------------------------------------------------- //
void MainWindow::resetASICs()
{
    runModule().resetASICs();
}
// ------------------------------------------------------------------------- //
void MainWindow::resetFEC()
{
    bool do_reset = (ui->fec_reset == QObject::sender() ? true : false);
    runModule().resetFEC(do_reset);
    ui->fecRB->setChecked(1);

    ui->trgExternal->setChecked(false);
    ui->trgPulser->setChecked(false);
    ui->setMask->setChecked(false);
    ui->onACQ->setChecked(false);
    ui->offACQ->setChecked(false);
    ui->setTrgAcqConst->setChecked(false);

    SetInitialState();
    m_commOK = true;
    m_configOK = false;
    m_tdaqOK = false;
    m_runModeOK = false;
    m_acqMode = "";
    emit checkFSM();
}
// ------------------------------------------------------------------------- //
void MainWindow::setHDMIMask()
{
    runModule().setMask();
    ui->appRB->setChecked(true);

    if(m_hdmiMaskON) {
        ui->setMask->setDown(true);
    }
    ui->setMask->setDown(true);
    m_hdmiMaskON = true;

}
// ------------------------------------------------------------------------- //
void MainWindow::setART()
{
    bool enabling = true;
    bool enableHoldOff = ui->holdOffCheckBox->isChecked();
    if(!(ui->enableART->isChecked())) enabling = false;
    runModule().enableART(enabling, enableHoldOff);
    ui->s6RB->setChecked(true);
}
// ------------------------------------------------------------------------- //
void MainWindow::checkLinkStatus()
{
    ui->appRB->setChecked(true);
    runModule().checkLinkStatus();
}
// ------------------------------------------------------------------------- //
void MainWindow::writeFECStatus()
{

    QByteArray buff;
    buff.clear();
    buff.resize(socketHandle().fecSocket().pendingDatagramSize());
    socketHandle().fecSocket().readDatagram(buff.data(), buff.size());
    if(buff.size()==0) return;

    bool ok;
    QString sizeOfPackageReceived, datagramCheck;
    datagramCheck = buff.mid(0,4).toHex();
    quint32 check = datagramCheck.toUInt(&ok,16);

    sizeOfPackageReceived = sizeOfPackageReceived.number(buff.size(),10);

    #warning What is this check?
    if(check<1000000) {
        stringstream ss;
        ss << " ****** NEW PACKET RECEIVED ****** " << endl;
        ss << " Data received size: " << sizeOfPackageReceived.toStdString()
           << " bytes" << endl;
        QString bin, hex;
        for(int i = 0; i < buff.size()/4; i++) {
            hex = buff.mid(i*4, 4).toHex();
            quint32 tmp32 = hex.toUInt(&ok,16);
            if(i==0) ss << " Rec'd ID: " << bin.number(tmp32,10).toStdString() << endl;
            else {
                ss << " Data, " << i << ": " << bin.number(tmp32,16).toStdString() << endl;
            }
        } // i
        ui->debugScreen->append(QString::fromStdString(ss.str()));
        ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::resetLinks()
{
    ui->appRB->setChecked(true);
    runModule().resetLinks();
}
// ------------------------------------------------------------------------- //
void MainWindow::setS6Resets()
{
    ui->s6RB->setChecked(true);
    runModule().setS6Resets(ui->s6_tkPulses->value(),
                            ui->s6_autoReset->isChecked(),
                            ui->s6_FECReset->isChecked(),
                            ui->fecPeriodReset->value());
}
// ------------------------------------------------------------------------- //
void MainWindow::set_s6resetStatus(bool ok)
{
    if(ok) {
        ui->setck_s6_resets->setStyleSheet("background-color: green");
    }
    else {
        ui->setck_s6_resets->setStyleSheet("background-color: red");
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::setupMonitoring(int /*doit*/)
{
    if(ui->doMonitoring->isChecked())
        dataHandle().setupMonitoring();
}
// ------------------------------------------------------------------------- //
void MainWindow::eventCountReached()
{
    ui->stopTriggerCnt->click();
}
// ------------------------------------------------------------------------- //
void MainWindow::triggerHandler()

{
    stringstream sx;

  //  if(ui->useMapping->isChecked()) {
  //      emit setUseChannelMap(true);
  //      QString mapFileName = configHandle().daqSettings().mapping_file;
  //      emit loadELxChannelMapping(mapFileName);
  //  }
  //  else {
  //      emit setUseChannelMap(false);
  //  }

    if(QObject::sender() == ui->checkTriggers) {
        ui->clearTriggerCnt->setEnabled(false);

        dataHandle().setEventCountStop(ui->eventCountStop->text().toInt());//toInt());

        //spin up the DAQ
        daqThread->start();
        delay();

        //shared memory
        //ui->doMonitoring->setEnabled(false);
        //dataHandle().set_monitorData(ui->doMonitoring->isChecked());

        //mapping
        if(ui->useMapping->isChecked()) {
            emit setUseChannelMap(true);
            QString mapFileName = configHandle().daqSettings().mapping_file; // default to mini2 map
            emit loadELxChannelMapping(mapFileName);
        } else {
            emit setUseChannelMap(false);
        }

        ui->useMapping->setEnabled(false);

//------------------------------------XADC**MMFE8**MINI2 BUTTONS-----------------------------------------//

        if(ui->xadc->isChecked() || ui->mmfe8CheckBox->isChecked() || ui->mini2CheckBox->isChecked()) {
               ui->mmfe8CheckBox->setEnabled(false);
               ui->mini2CheckBox->setEnabled(false);
               ui->xadc->setEnabled(false);
        }


//------------------------------------END XADC**MMFE8**MINI2 BUTTONS-----------------------------------------//




        emit setWriteNtuple(ui->writeData->isChecked());
        emit setIgnore16(ui->ignore16->isChecked());
        //emit setCalibrationRun(ui->calibration->isChecked());
        //emit setCalibrationRun( (ui->doPDOCalib->isChecked() || ui->doTDOCalib->isChecked()) );
        emit setCalibrationRun( ui->doCalib->isChecked() &&
                (ui->doPDOCalib->isChecked() || ui->doTDOCalib->isChecked()) );
        bool ok;
        if(ui->writeData->isChecked()) {

            // setup the output files
            QString rootFileDir = ui->runDirectoryField->text();
            qDebug()<<rootFileDir;
            if(!rootFileDir.endsWith("/")) rootFileDir = rootFileDir + "/";
            QString filename_init = "run_%04d.root";

            // check for existing files, provide new tag if necessary
            int run_number = DataHandler::checkForExistingFiles(rootFileDir.toStdString(),
                                                    ui->runNumber->value());
            if( !(run_number==ui->runNumber->value()) ) {
                sx << "Re-setting run number to: " << run_number; 
                msg()(sx);sx.str("");
            }
            ui->runNumber->setValue(run_number);
                
            const char* filename_formed = Form(filename_init.toStdString().c_str(),
                            run_number);
            bool filesOK = dataHandle().setupOutputFiles(rootFileDir, filename_formed);
            if(!filesOK) { emit badRunDirectory(); return; }
        
            // set this here so that we know the run will start
            // (i.e. we have passed the check for the output location, etc...)
            ui->eventCountStop->setEnabled(false);

            // not all things need to be signal/slot
            dataHandle().setupOutputTrees();

 //           dataHandle().dataFileHeader(configHandle().commSettings(),
 //                                       configHandle().globalSettings(),
 //                                       configHandle().daqSettings());


            //fill the run properties
            QString tmpTPSkew = ui->tpSkew->currentText().remove("ns");
            double tp_skew = tmpTPSkew.toDouble(&ok);
            dataHandle().getRunProperties(configHandle().globalSettings(),
                         ui->runNumber->value(), ui->angle->value(),
                         tp_skew);

            // things should be OK by now
            sx << " -------------------------------- \n";
            sx << " *** Starting DAQ run " << ui->runNumber->value() << " *** \n";
            sx << " -------------------------------- ";
            msg()(sx); sx.str("");

            //reset DAQ count for this run
            dataHandle().resetDAQCount();
            QString cnt;
            ui->triggerCntLabel->setText(cnt.number(dataHandle().getDAQCount(), 10));

            //thread
            delay(); delay();
            emit startDAQSocket();
            m_daqInProgress = true;

            // gui stuff
            ui->runStatusField->setText("Run:"+ui->runNumber->text()+" ongoing");
            ui->runStatusField->setStyleSheet("background-color:green");
            ui->checkTriggers->setEnabled(false);
            ui->stopTriggerCnt->setEnabled(true);

            /////////////////////////////////////////////////
            // calibration loop
            /////////////////////////////////////////////////
            //if(ui->calibration->isChecked()) {
            if(ui->doCalib->isChecked() && ui->doPDOCalib->isChecked()) {

                if(ui->autoCalib->isChecked()) {

                    /////////////////////////////////////////
                    // load the calibration recipe
                    /////////////////////////////////////////
                    pdoCalibration calib;

                    calib.gain_start = ui->sg_cal_min->currentIndex();
                    calib.gain_end = ui->sg_cal_max->currentIndex();

                    calib.threshold_start = int(ui->sdt_cal_min->value());
                    calib.threshold_end   = int(ui->sdt_cal_max->value());
                    calib.threshold_step  = int(ui->sdt_cal_step->value());

                    calib.pulser_start = int(ui->sdp_cal_min->value());
                    calib.pulser_end   = int(ui->sdp_cal_max->value());
                    calib.pulser_step  = int(ui->sdp_cal_step->value());

                    calibModule().loadPDOCalibrationRecipe(calib);

                    calibModule().setNEvents(int(ui->evCalib->text().toUInt(&ok,10)));
                    calibModule().setChannelRange(int(ui->chRange_calib_min->currentIndex()),
                                                  int(ui->chRange_calib_max->currentIndex()));

                    calibModule().beginPDOCalibration(); 
                }

              //  if(ui->autoCalib->isChecked())
              //      startCalibration();
              //  else if(ui->manCalib->isChecked())
              //      msg()("Manual Calibration");
            } // PDO calibration

            else if(ui->doCalib->isChecked() && ui->doTDOCalib->isChecked()) {
                if(ui->autoCalib->isChecked()) {

                    /////////////////////////////////////////
                    // load the calibration recipe
                    /////////////////////////////////////////
                    tdoCalibration calib;

                    calib.n_steps = ui->delaySteps->currentIndex();
                    calib.gain = int(ui->sg_tdoCal->currentIndex());
                    calib.amplitude = int(ui->sdp_tdoCal->value());
                    calib.threshold = int(ui->sdt_tdoCal->value());

                    calibModule().loadTDOCalibrationRecipe(calib);

                    calibModule().setNEvents(int(ui->evCalib->text().toUInt(&ok,10)));
                    calibModule().setChannelRange(int(ui->chRange_calib_min->currentIndex()),
                                                    int(ui->chRange_calib_max->currentIndex()));

                    calibModule().beginTDOCalibration();
                } // auto


            } // TDO calibration

        } // writeData
        else {
            ui->clearTriggerCnt->setEnabled(false);
            ui->eventCountStop->setEnabled(false);

            sx << " -------------------------------- \n";
            sx << " *** Starting DAQ run " << ui->runNumber->value() << " ***\n";
            sx << " -------------------------------- ";
            msg()(sx); sx.str("");
            m_daqInProgress = true;

            //msg()("startDAQSocket");
            emit startDAQSocket();

            ui->checkTriggers->setEnabled(false);
            ui->stopTriggerCnt->setEnabled(true);

            //ui->doMonitoring->setEnabled(false);
        } // not writing data

        ui->runNumber->setEnabled(false);

    } // checkTriggers

    if(QObject::sender() == ui->clearTriggerCnt) {
        dataHandle().resetDAQCount();
        QString cnt;
        ui->triggerCntLabel->setText(cnt.number(dataHandle().getDAQCount(), 10));
    } // clearTriggerCnt

    if(QObject::sender() == ui->stopTriggerCnt) {
        if(!m_daqInProgress) return;
        ui->clearTriggerCnt->setEnabled(true);
        sx << " -------------------------------- \n";
        sx << " *** Ending DAQ run " << ui->runNumber->value() << " ***\n";
        sx << " -------------------------------- ";
        msg()(sx); sx.str("");
        ui->useMapping->setEnabled(true);
        ui->eventCountStop->setEnabled(true);

        // daq socket exists in another thread so must call it with signal
        emit closeDAQSocket();

        m_daqInProgress = false;

        if(m_inCalibrationLoop) {
            msg()(sx);
            emit stopCalibrationLoop();
            delay();
        }
        delay();
        delay();
        delay();
        delay();
        //shared
        //ui->doMonitoring->setEnabled(true);
        //dataHandle().clearSharedMemory();

        ui->runStatusField->setText("Run:"+ui->runNumber->text()+" finished");
        ui->runStatusField->setStyleSheet("background-color: lightGray");
        if(ui->writeData->isChecked()) {
            qDebug()<<"I pass from write and close data file";
            dataHandle().writeAndCloseDataFile();
            //emit EndRun();
        } // writeData
        daqThread->quit();

        ui->runNumber->setValue(ui->runNumber->value()+1);
        ui->checkTriggers->setEnabled(true);
        ui->stopTriggerCnt->setEnabled(false);
        ui->runNumber->setEnabled(true);
        ui->xadc->setEnabled(true);
        ui->mini2CheckBox->setEnabled(true);
        ui->mmfe8CheckBox->setEnabled(true);
    } // stopTriggerCnt

}
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
void MainWindow::calibrationLoopState(bool calib_on)
{
    m_inCalibrationLoop = calib_on;
}
// ------------------------------------------------------------------------- //
void MainWindow::updateTriggerCount(bool fine)
{
    QString cnt_;
    int count = dataHandle().getDAQCount();

    // as the DAQ is on a different thread, the 
    // call here can be behind the DAQ count at the
    // time at which the counter was incremented
    // just round it down to the nearest mult. of 100
    // (be sure to adjust this if you begin changing
    // the increment of the counter update)

    if(fine) {
        ui->triggerCntLabel->setText(cnt_.number(count,10));
    }
    else {
        count = (count + 50) / 100 * 100;
        ui->triggerCntLabel->setText(cnt_.number(count,10));
    }
}
// ------------------------------------------------------------------------- //
void MainWindow::setPDOCalibrationState(int gain, int threshold_daq, int pulser_daq)
{
    ui->sg->setCurrentIndex(gain);
    ui->sdt->setValue(threshold_daq);
    ui->sdp_2->setValue(pulser_daq);

    // send overall calibration state
    emit updateCalibrationState(ui->sg->currentIndex(),
                                ui->sdt->value(),
                                ui->sdp_2->value(),
                                ui->tpSkew->currentIndex(),
                                ui->st->currentIndex());
}
// ------------------------------------------------------------------------- //
void MainWindow::changeDelayLabels(int delayIdx)
{

    QString tmp;
    #warning current TDO loop always starts at zero delay

    ui->tdoCalib_skewLabLow->setText(tmp.number(0.000) + " ns");
    ui->tdoCalib_skewLabHigh->setText(tmp.number((3.125*delayIdx), 'f', 3) + " ns");
    //ui->tdoCalib_skewLabHigh->setText(tmp.number((3.125*ui->delaySteps->currentIndex()), 'f', 2) + " ns");
}
// ------------------------------------------------------------------------- //
void MainWindow::setTDOCalibrationState(int delay, int gain, int amplitude, int threshold)
{
    ui->sg->setCurrentIndex(gain);
    ui->sdt->setValue(threshold);
    ui->sdp_2->setValue(amplitude);

    ui->tpSkew->setCurrentIndex(delay);

    emit ui->setTp_s6->click();

    emit updateCalibrationState(ui->sg->currentIndex(),
                                ui->sdt->value(),
                                ui->sdp_2->value(),
                                ui->tpSkew->currentIndex(),
                                ui->st->currentIndex());

}
// ------------------------------------------------------------------------- //
void MainWindow::setChannelsForCalib(int calib_channel)
{
    QString style_off = "background-color: lightGray";

    for(int ch = 0; ch < 64; ch++) {
        VMMSM[ch]->setStyleSheet(style_off);
        VMMSMBool[ch] = 0;
        VMMSMBoolAll = 0;
        VMMST[ch]->setStyleSheet(style_off);
        VMMSTBool[ch] = 0;
        VMMSTBoolAll = 0;
    } // ch

    if(ui->masking_calib->isChecked()) {
        emit SMLabel->click();
        VMMSM[calib_channel]->click();
        if(calib_channel>0)  VMMSM[calib_channel-1]->click();
        if(calib_channel<63) VMMSM[calib_channel+1]->click();
    } // masking

    VMMST[calib_channel]->click();
}
// ------------------------------------------------------------------------- //
void MainWindow::setupCalibrationConfig()
{
    if(dbg()) msg()("   --> Setting mask...");
    emit ui->setMask->click();
    delay();

    if(dbg()) msg()("   --> Setting headers...");
    emit ui->setEvbld->click();
    delay();

    if(dbg()) msg()("   --> Initializing electronics...");
    emit ui->SendConfiguration->click();
    delay();
    emit ui->SendConfiguration->click();
    delay();

    if(dbg()) msg()("   --> Setting T/DAQ configuration...");
    emit ui->setTrgAcqConst->click();
    delay();

    if(dbg()) msg()("   --> Setting pulser DAC...");
    emit ui->trgPulser->click();

}
// ------------------------------------------------------------------------- //
void MainWindow::setCalibrationACQon(int events_for_loop)
{
    bool ok;

    int initial_number_of_events = ui->triggerCntLabel->text().toUInt(&ok,10);

    emit ui->onACQ->click();

    stringstream sx;

    //int x = 0;
    while((int(ui->triggerCntLabel->text().toUInt(&ok,10) - initial_number_of_events)
                    <= events_for_loop) && calibModule().continueLoop()) {
        //msg()(" * Waiting for event collecting * ");
        //sx.str("");
        //sx << ui->triggerCntLabel->text().toUInt(&ok,10);
        //msg()(sx);
        delay();
        delay();
        delay();
        delay();
        //if(x>10) { x = 0; break; }
        //x++;
    } // while loop
    
    emit calibModule().calibACQcomplete();

}
// ------------------------------------------------------------------------- //
void MainWindow::setCalibrationACQoff()
{
    emit ui->offACQ->click();
    delay();
    delay();
    delay();

    emit ui->asic_reset->click();
    delay();
    delay();
}
// ------------------------------------------------------------------------- //
void MainWindow::endCalibrationRun()
{
    emit ui->stopTriggerCnt->click();
}
// ------------------------------------------------------------------------- //
void MainWindow::startCalibration()
{
    stringstream sx;

    delay();
    bool ok;
    int x = 0;
    int eventsMultiplier = 0;

    sx << " ---------------------------------- \n"
       << "  Starting calibration \n"
       << " ---------------------------------- ";
    msg()(sx); sx.str("");

    for(int g = ui->sg_cal_min->currentIndex(); g <= ui->sg_cal_max->currentIndex(); g++) {
        ui->sg->setCurrentIndex(g);
        for(int thr = ui->sdt_cal_min->value(); thr <= ui->sdt_cal_max->value();
                                                            thr += ui->sdt_cal_step->value()) {
            ui->sdt->setValue(thr);
            for(int p = ui->sdp_cal_min->value(); p <= ui->sdp_cal_max->value();
                                                            p += ui->sdp_cal_step->value()) {
                ui->sdp_2->setValue(p);

                sx << " ---------------------------------- \n"
                   << "  Calibration Loop \n"
                   << "   > Gain            : " << ui->sg->currentText().toStdString() << "\n"
                   << "   > Threshold       : " << ui->sdt->value() << "\n"
                   << "   > Pulser DAC      : " << ui->sdp_2->value() << "\n"
                   << " ----------------------------------";
                msg()(sx); sx.str("");


                // send the updated info to run_module here

                for(int ch = ui->chRange_calib_min->currentIndex();
                        ch <= ui->chRange_calib_max->currentIndex(); ch++) {
                    delay();
                    for(int chT = 0; chT < 64; chT++) {
                        VMMSM[chT]->setStyleSheet("background-color: lightGray");
                        VMMSMBool[chT]=0;
                        VMMSMBoolAll=0;
                        VMMSMBoolAll2=0;
                        VMMST[chT]->setStyleSheet("background-color: lightGray");
                        VMMSTBool[chT]=0;
                        VMMSTBoolAll=0;
                        VMMSTBoolAll2=0;
                    } //chT
                    if(ui->masking_calib->isChecked()) {
                        emit SMLabel->click();
                        emit SMLabel2->click();
                        VMMSM[ch]->click();
                        if(ch>0)
                            VMMSM[ch-1]->click();
                        if(ch<63)
                            VMMSM[ch+1]->click();
                    } //masking

                    sx << " > Calibrating channel: " << ch;
                    msg()(sx);sx.str("");

                    VMMST[ch]->click();
                    sx << " --> Set mask..."; msg()(sx); sx.str("");
                    emit ui->setMask->click();
                    delay();
                    sx << " --> Set headers..."; msg()(sx); sx.str("");
                    emit ui->setEvbld->click();
                    delay();

                    sx << " --> Initialize electronics..."; msg()(sx); sx.str("");
                    emit ui->SendConfiguration->click();
                    delay();
                    emit ui->SendConfiguration->click();
                    delay();

                    sx << " --> Set trigger constants..."; msg()(sx); sx.str("");
                    emit ui->setTrgAcqConst->click();
                    delay();
                    sx << " --> Setting pulser DAC..."; msg()(sx); sx.str("");
                    emit ui->trgPulser->click();

                    int initialEvents = ui->triggerCntLabel->text().toUInt(&ok,10);

                    sx << " --> Enabling DAQ..."; msg()(sx); sx.str("");
                    emit ui->onACQ->click();

                    eventsMultiplier++;
                    unsigned int eventsForCalibration = ui->evCalib->text().toUInt(&ok,10);

                    sx << " --> Starting pulser with number of events: " << eventsForCalibration;
                    msg()(sx); sx.str("");
                    delay();

                    #warning don't see how this case can happen
                    if(QObject::sender() == ui->stopTriggerCnt)
                        break;

                    while(ui->triggerCntLabel->text().toUInt(&ok,10) - initialEvents <= eventsForCalibration){
                        sx << " * Waiting for event collecting... * "; msg()(sx); sx.str("");
                        delay();
                        delay();
                        delay();
                        delay();
                        delay();
                        if(x>10) { x = 0; break; }
                        x++;
                        if(!ui->stopTriggerCnt->isEnabled())
                            break;
                    }

                    emit ui->offACQ->click();
                    delay();
                    delay();
                    delay();

                    if(QObject::sender() == ui->stopTriggerCnt)
                        break;

                    sx << " ----------------------------------"; msg()(sx); sx.str("");

                } //ch
            } //p
        } //thr
    } //g
    sx << " ---------------------------------- \n"
       << "  Calibration finished \n"
       << " ---------------------------------- ";
    msg()(sx); sx.str("");
    emit ui->stopTriggerCnt->click();

}
// ------------------------------------------------------------------------- //
void MainWindow::setRunDirOK(bool dirStatus)
{
    m_runDirectoryOK = dirStatus;
}
// ------------------------------------------------------------------------- //
void MainWindow::badRunDirectory()
{
    m_runDirectoryOK = false;

    ui->runDirectoryField->setStyleSheet("QLineEdit { background: rgb(220, 0, 0); selection-background-color: rgb(233, 99, 0); }");
    delay();
    delay();
    ui->runDirectoryField->setStyleSheet("QLineEdit { background: white; selection-background-color: rgb(233, 99, 0); }");
    delay();
    delay();
    ui->runDirectoryField->setStyleSheet("QLineEdit { background: rgb(220, 0, 0); selection-background-color: rgb(233, 99, 0); }");
    delay();
    delay();
    ui->runDirectoryField->setStyleSheet("QLineEdit { background: white; selection-background-color: rgb(233, 99, 0); }");
}
// ------------------------------------------------------------------------- //
void MainWindow::setS6clocks()
{
    ui->s6RB->setChecked(1);
    runModule().s6clocks(ui->cktk_s6->currentIndex(),
                         ui->ckbc_s6->currentIndex(),
                         ui->ckbc_skew_s6->currentIndex());

}
// ------------------------------------------------------------------------- //
void MainWindow::configureTP()
{
    ui->s6RB->setChecked(1);
    runModule().configTP(ui->tpSkew->currentIndex(),
                         ui->tpWidth->currentIndex(),
                         ui->tpPolarity->currentIndex());
}
// ------------------------------------------------------------------------- //
void MainWindow::delay()
{
    QTime dieTime = QTime::currentTime().addMSecs(200);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}
// ------------------------------------------------------------------------- //
void MainWindow::delayMs()
{
    QTime dieTime = QTime::currentTime().addMSecs(10);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Wheel)
    {
        //if(obj->hasFocus()) {
//            ui->scrollArea->setFocus();
        event->ignore();
        //}
    //    qDebug() << "Wheel event blocked";
        return true;
    }
    return false;
}
