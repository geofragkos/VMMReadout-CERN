#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include "sleeper.h"
#include "tcphandler.h"
#include "tcphandlerthread.h"
#include <QDir>
#include <QFileDialog>


//_________________________________________________________________________________________
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

		//Commented out as it is not needed (already loaded) - ARV 30/06/15
    //gSystem->Load("libMylib.so");

  	_config = new Configuration();
    _config->blockSignals(true);
    _runDAQ = new RunDAQ(); 
    _dataProcessor = new DataProcessor();
    m_daqConstantsLoaded = false;

    idForCustomCommands=0;
    multicastingFlag=0;
    FECPort=6007;
    DAQPort=6006;
    VMMASICPort=6603;
    VMMAPPPort=6600;
    S6Port=6602;
    DCARDPort=6601;

    daqFile_fpga = 0;
    FPGAPort=65000;
    headerSet =0;
    pulserDi=0;
    Font.setFamily("Arial");
    Font.setPixelSize(9);
    ui->setupUi(this);
    SetChambersMapping();
    spiSent=0;

    for(int i=0;i<=19;i++){
        boardThreshold[i]=220;
    }
    //ui->sdt->setValue(220);
    //********************** Sub Classing TCP for FPGA Trigger Data *******************************
    tcpHandler tcpData;
    tcpHandlerThread *tcpThread = new tcpHandlerThread();
    tcpData.moveToThread(tcpThread);
    tcpThread->start();

    //*********************************************************************************************
    connect(_config, SIGNAL(checkStatus()), this, SLOT(dataPending()));
    connect(_dataProcessor, SIGNAL(checkDAQCount()), this, SLOT(updateTriggerCount()));

    connect(ui->numbersOfFecs, SIGNAL(valueChanged(int)), this , SLOT(SetNumberOfFecs(int)));
    //    SetNumberOfFecs(ui->numbersOfFecs->value());
    connect(ui->openConnection, SIGNAL(pressed()), this , SLOT(Connect()));
    connect(ui->openConnection_tr, SIGNAL(pressed()), this , SLOT(ConnectTCP()));
    connect(ui->closeConnection_tr, SIGNAL(pressed()), this , SLOT(ConnectTCP()));

    connect(ui->SendConfiguration, SIGNAL(clicked()), this , SLOT(HandleRecipe()));
    connect(ui->loadConfigXMLFile, SIGNAL(clicked()), this , SLOT(LoadConfigurationFromFile()));
    connect(ui->writeConfigXMLFile, SIGNAL(clicked()), this , SLOT(WriteConfigurationFromFile()));
    connect(ui->loadDAQXMLFile, SIGNAL(clicked()), this , SLOT(LoadDAQConfigurationFromFile()));
    connect(ui->writeDAQXMLFile, SIGNAL(clicked()), this, SLOT(WriteDAQConfigurationFile()));
    connect(ui->useMapping, SIGNAL(stateChanged(int)), this , SLOT(setChannelMaps(int)));
    //    connect(ui->SendConfiguration_tr, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->onAcq_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->offAcq_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->triggerEnable_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->triggerDisable_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->triggerCnt_fpga_clear, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->triggerCnt_fpga_enable, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->realData_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->testData_fpga, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->triggerCnt_fpga_read, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->timeWindow_fpgaPB, SIGNAL(clicked()), this , SLOT(MergerFPGA()));
    connect(ui->trigger_fpga_init, SIGNAL(clicked()), this , SLOT(MergerFPGA()));

    connect(ui->hideChannels, SIGNAL(stateChanged(int)), this , SLOT(hideChannels(int)));
    connect(ui->enableDebugPB, SIGNAL(stateChanged(int)), this , SLOT(showDebugScreen(int)));
    connect(ui->clearDebugScreenPB, SIGNAL(clicked()), this , SLOT(clearDebugScreen()));
    connect(ui->clearDebugScreenPB_fpga, SIGNAL(clicked()), this , SLOT(clearDebugScreen()));

    connect(ui->hdmi1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi1_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi1_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi2_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi2_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi3, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi3_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi3_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi4, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi4_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi4_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi5, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi5_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi5_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi6, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi6_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi6_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi7, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi7_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi7_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi8, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi8_1, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    connect(ui->hdmi8_2, SIGNAL(stateChanged(int)), this , SLOT(CreateChannelMap(int)));
    CreateChannelMap(0);

    connect(ui->onACQ, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->offACQ, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    //    connect(ui->applyDetectionMode, SIGNAL(clicked()), this , SLOT(customCommandHandler()));
    connect(ui->applyDetectionMode, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->applyTrigger, SIGNAL(clicked()), this , SLOT(customCommandHandler()));
    connect(ui->asic_reset, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->triggerCounter_reset, SIGNAL(clicked()), this , SLOT(customCommandHandler()));
    connect(ui->fec_reset, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->fec_WarmInit, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->lx9Reset, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setDaqMode, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setHeaderPB, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->sampleSetPB, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->trgExternal, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->trgPulser, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setMask, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->linkPB, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setEvbld, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->resetLinks, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setTrgAcqConst, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setck_s6, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));
    connect(ui->setTp_s6, SIGNAL(clicked()), this , SLOT(customCommandWithoutResponse()));

    connect(ui->setVMMs, SIGNAL(currentIndexChanged(int)), this , SLOT(constructHeader(int)));
    connect(ui->sdt, SIGNAL(valueChanged(int)), this , SLOT(changeDACtoMVs(int)));
    QString tempstring;
    ui->dacmvLabel->setText(tempstring.number((0.6862*ui->sdt->value()+63.478), 'f', 2)+ " mV");

    int intervalDi = 110;//msec
    threadDi = new MyThread(intervalDi, this);
    connect(threadDi, SIGNAL(valueChanged(int)), this, SLOT(sendDis(int)));
    //    connect(ui->startDi, SIGNAL(clicked()), this , SLOT(threadHandler()));
    //    connect(ui->stopDi, SIGNAL(clicked()), this , SLOT(threadHandler()));

    int intervalRefreshHistos = 5000;//5 sec
    threadFill = new MyThread(intervalRefreshHistos,this);
    connect(threadFill, SIGNAL(valueChanged(int)), this, SLOT(updateCanvas(int)));

    connect(ui->checkTriggers, SIGNAL(clicked()), this , SLOT(triggerHandler()));
    //    connect(ui->checkTriggers, SIGNAL(clicked()), this , SLOT(customCommandHandler()));
    connect(ui->clearTriggerCnt, SIGNAL(clicked()), this , SLOT(triggerHandler()));
    connect(ui->stopTriggerCnt, SIGNAL(clicked()), this , SLOT(triggerHandler()));

    //    connect(ui->checkTriggers, SIGNAL(clicked()), this , SLOT(triggerHandler_fpga()));
    connect(ui->clearTriggerCnt, SIGNAL(clicked()), this , SLOT(triggerHandler_fpga()));
    connect(ui->stopTriggerCnt, SIGNAL(clicked()), this , SLOT(triggerHandler_fpga()));

    connect(ui->monitoring, SIGNAL(stateChanged(int)), this , SLOT(openMonitoring(int)));

    connect(ui->loadCalibDataCB, SIGNAL(stateChanged(int)), this , SLOT(loadCalibrationConstants(int)));
    connect(ui->loadThrCB, SIGNAL(stateChanged(int)), this , SLOT(LoadThresholds(int)));

    //tabs
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(0);
    groupModify = new QGroupBox(tr("VMM2 Channels"),this);
    grid = new QGridLayout(this);
    ui->tabWidget->addTab(groupModify, "CHANNELS");
    ui->tabWidget->addTab(ui->groupBox_6, "CALIBRATION");
    ui->tabWidget->addTab(ui->groupBox_18, "FEC RESPONSE");

    CreateChannelsField();
    initializePlots();

    ui->runDirectoryField->setText("");
    runDirectory.setPath(ui->runDirectoryField->text());
    connect(ui->selectDir, SIGNAL(clicked()),this,SLOT(selectWorkingDirectory()));
    //    ui->runGroupbox->setStyleSheet("background-: white; padding: 1px; border-style: outset");

    ui->spiRB->setChecked(true);
    ui->writeRB->setChecked(true);
    //this->setFixedSize(1204,725);
    this->setFixedSize(1400,725);
    ui->hideChannels->setChecked(1);
    commandCounter = 0;
    QString temp;
    ui->cmdlabel->setText(temp.number(commandCounter));
    ui->stopTriggerCnt->setEnabled(0);
    //    ui->startDi->setText("Start\nPulser");
    //    ui->stopDi->setText("Stop\nPulser");



}
//_________________________________________________________________________________________
void MainWindow::SetChambersMapping(){
    T1Chips[1]=16;T1Chips[2]=17;
    T2Chips[1]=18;T2Chips[2]=19;
    T3Chips[1]=20;T3Chips[2]=21;
    T4Chips[1]=22;T4Chips[2]=23;
    T5Chips[1]=24;T5Chips[2]=25;
    T6Chips[1]=26;T6Chips[2]=27;
    T7Chips[1]=28;T7Chips[2]=29;
    T8Chips[1]=30;T8Chips[2]=31;
}

//_________________________________________________________________________________________
MainWindow::~MainWindow()
{
    delete ui;
}
//_________________________________________________________________________________________

void MainWindow::CreateChannelMap(int state){
		qDebug()<< "[MainWindow::CreateChannelMap] Unused arg state= " <<state;
    QString chMapString = "0000000000000000";//16bit
    if (ui->hdmi1->isChecked()){
        if(ui->hdmi1_1->isChecked())
            chMapString.replace(15,1,"1");
        if(ui->hdmi1_2->isChecked())
            chMapString.replace(14,1,"1");
    }else{
        ui->hdmi1_1->setChecked(0);
        chMapString.replace(15,1,"0");
        ui->hdmi1_2->setChecked(0);
        chMapString.replace(14,1,"0");
    }
    if (ui->hdmi2->isChecked()){
        if(ui->hdmi2_1->isChecked())
            chMapString.replace(13,1,"1");
        if(ui->hdmi2_2->isChecked())
            chMapString.replace(12,1,"1");
    }else{
        ui->hdmi2_1->setChecked(0);
        chMapString.replace(13,1,"0");
        ui->hdmi2_2->setChecked(0);
        chMapString.replace(12,1,"0");
    }
    if (ui->hdmi3->isChecked()){
        if(ui->hdmi3_1->isChecked())
            chMapString.replace(11,1,"1");
        if(ui->hdmi3_2->isChecked())
            chMapString.replace(10,1,"1");
    }else{
        ui->hdmi3_1->setChecked(0);
        chMapString.replace(11,1,"0");
        ui->hdmi3_2->setChecked(0);
        chMapString.replace(10,1,"0");
    }
    if (ui->hdmi4->isChecked()){
        if(ui->hdmi4_1->isChecked())
            chMapString.replace(9,1,"1");
        if(ui->hdmi4_2->isChecked())
            chMapString.replace(8,1,"1");
    }else{
        ui->hdmi4_1->setChecked(0);
        chMapString.replace(9,1,"0");
        ui->hdmi4_2->setChecked(0);
        chMapString.replace(8,1,"0");
    }
    if (ui->hdmi5->isChecked()){
        if(ui->hdmi5_1->isChecked())
            chMapString.replace(7,1,"1");
        if(ui->hdmi5_2->isChecked())
            chMapString.replace(6,1,"1");
    }else{
        ui->hdmi5_1->setChecked(0);
        chMapString.replace(7,1,"0");
        ui->hdmi5_2->setChecked(0);
        chMapString.replace(6,1,"0");
    }
    if (ui->hdmi6->isChecked()){
        if(ui->hdmi6_1->isChecked())
            chMapString.replace(5,1,"1");
        if(ui->hdmi6_2->isChecked())
            chMapString.replace(4,1,"1");
    }else{
        ui->hdmi6_1->setChecked(0);
        chMapString.replace(5,1,"0");
        ui->hdmi6_2->setChecked(0);
        chMapString.replace(4,1,"0");
    }
    if (ui->hdmi7->isChecked()){
        if(ui->hdmi7_1->isChecked())
            chMapString.replace(3,1,"1");
        if(ui->hdmi7_2->isChecked())
            chMapString.replace(2,1,"1");
    }else{
        ui->hdmi7_1->setChecked(0);
        chMapString.replace(3,1,"0");
        ui->hdmi7_2->setChecked(0);
        chMapString.replace(2,1,"0");
    }
    if (ui->hdmi8->isChecked()){
        if(ui->hdmi8_1->isChecked())
            chMapString.replace(1,1,"1");
        if(ui->hdmi8_2->isChecked())
            chMapString.replace(0,1,"1");
    }else{
        ui->hdmi8_1->setChecked(0);
        chMapString.replace(1,1,"0");
        ui->hdmi8_2->setChecked(0);
        chMapString.replace(0,1,"0");
    }


    bool ok;
    channelMap=(quint16)chMapString.toInt(&ok,2);
    //qDebug()<<channelMap<<endl;
}

void MainWindow::DecodeChannelMap(int chanmap){

	int index=0;
	bool atleastone=false;
	pair <int,int> doublet;

	QList<QCheckBox*> hdmis = QList<QCheckBox*>() << ui->hdmi1 << ui->hdmi2 << ui->hdmi3 << ui->hdmi4 << ui->hdmi5 << ui->hdmi6 << ui->hdmi7 << ui->hdmi8; 
	QList<QCheckBox*> hdmiVMMs = QList<QCheckBox*>() << ui->hdmi1_1 << ui->hdmi1_2 << ui->hdmi2_1 << ui->hdmi2_2 << ui->hdmi3_1 << ui->hdmi3_2 << ui->hdmi4_1 << ui->hdmi4_2 << ui->hdmi5_1 << ui->hdmi5_2 << ui->hdmi6_1 << ui->hdmi6_2 << ui->hdmi7_1 << ui->hdmi7_2 << ui->hdmi8_1 << ui->hdmi8_2; 

	//Create 16-bit QString from int channelmap
	QString stest = QString("%1").arg(chanmap, 16, 2, QChar('0'));

	//Iterate over characters in string to set channel Map check marks in GUI
	for(QString::const_iterator itr(stest.end()-1); itr != stest.begin()-1; --itr) {

		//First set values in pair
		if (*itr == '1') {
			if (index%2==0) doublet.first = 1;
			else doublet.second = 1;
			atleastone=true; //Set true if at least one of the two VMMs is set at 1
		} else {
			if (index%2==0) doublet.first = 0;
			else doublet.second = 0;
		}

		//Now click
		if (index%2!=0) {
			if (atleastone) {
				if (!hdmis.at(index/2)->isChecked()) hdmis.at(index/2)->animateClick();
				if (doublet.first && !hdmiVMMs.at(index-1)->isChecked()) hdmiVMMs.at(index-1)->animateClick();
				if (doublet.second && !hdmiVMMs.at(index)->isChecked()) hdmiVMMs.at(index)->animateClick();
				if (!doublet.first && hdmiVMMs.at(index-1)->isChecked()) hdmiVMMs.at(index-1)->animateClick();
				if (!doublet.second && hdmiVMMs.at(index)->isChecked()) hdmiVMMs.at(index)->animateClick();
			}
			else { //Clicking on main hdmi button will uncheck the two associated VMMs
				if (hdmis.at(index/2)->isChecked()) hdmis.at(index/2)->animateClick();
			}
			atleastone=false;
		}
		++index;
	}

}

//_________________________________________________________________________________________________
void MainWindow::PingFec(){
/*
    for(int i=0;i<ui->numbersOfFecs->value();i++){
        //int exitCode = QProcess::execute("ping", QStringList() << "-t2" << ips[i]); //Works for MACs
        int exitCode = QProcess::execute("ping", QStringList() << "-c1" << ips[i]); //Works for linux
        //int exitCode =0;
        if (0 == exitCode) {
            communicationAlive[i]=true;
        } else {
            communicationAlive[i]=false;
        }
        Sleeper::msleep(100);
    }
*/
}
//_________________________________________________________________________________________________
void MainWindow::PingFPGA(){
    int exitCode = QProcess::execute("ping", QStringList() << "-t1" << ips_tr);
    if (0 == exitCode) {
        communicationAliveFPGA=true;
    } else {
        communicationAliveFPGA=false;
    }
    Sleeper::msleep(1000);
}
//_________________________________________________________________________________________________
void MainWindow::SetNumberOfFecs(int){
    ui->setVMMs->clear();
    QString counter;
    for(int i=0;i<ui->numbersOfFecs->value();i++){
        ips[i]=ui->ip1->text()+"."+ui->ip2->text()+"."+ui->ip3->text()+"."+counter.setNum(ui->ip4->text().toInt()+i);
        ui->setVMMs->addItem(counter.setNum(i+1));
        ui->setVMMs->setCurrentIndex(i);
    }
    ui->setVMMs->addItem("All");
    ui->setVMMs->setCurrentIndex(ui->setVMMs->currentIndex()+1);
    if(ui->setVMMs->currentText()=="All") ui->header4LE->setText(ui->ip4->text()); //What is that for? - ARV
		/* Remove all notions of socket from the GUI
    if(ui->connectionLabel->text()==" All Alive"){
        bnd=false;
        ui->connectionLabel->setText("Re-Establish Connection");
        socket->close();
        socket->disconnectFromHost();
        ui->connectionLabel->setStyleSheet("background-color: light");
    }
		*/
}
//_________________________________________________________________________________________________
void MainWindow::SetFPGAIP(){
    ips_tr=ui->ip1_tr->text()+"."+ui->ip2_tr->text()+"."+ui->ip3_tr->text()+"."+ui->ip4_tr->text();
}
//_________________________________________________________________________________________________
void MainWindow::Connect(){
    //PingFec();
	SendParamsToConfigModule();//Only needed for IPs
    SendRunParamsToDAQModule();

	if (ui->numbersOfFecs->value() == 0){
		ui->connectionLabel->setText("Select number of FECs");
		ui->connectionLabel->setStyleSheet("background-color: light");
		return;
	}
	int pinged = _config->Ping();
	if (pinged == 0) {
		ui->connectionLabel->setText(" All Alive");
		ui->connectionLabel->setStyleSheet("background-color: green");
	} else {
		ui->connectionLabel->setText(" Ping failed");
		ui->connectionLabel->setStyleSheet("background-color: lightGray");
	}

    socket = new QUdpSocket(this);
		/*
    QString deads="";
    int numFECS=0;
    bnd = socket->bind(FECPort);//, QUdpSocket::ShareAddress);
    if(bnd){
        connect( socket, SIGNAL(readyRead()), this, SLOT(dataPending()) );
        if (ui->numbersOfFecs->value()==0){
            ui->connectionLabel->setText("Select number of FECs");
            ui->connectionLabel->setStyleSheet("background-color: light");
        }

        for(int i=0;i<ui->numbersOfFecs->value();i++){
            //connect( socket, SIGNAL(readyRead()), this, SLOT(dataPending()) );
            if(communicationAlive[i]==1){
                QString temp;
                ui->connectionLabel->setText("  "+temp.number(i+1)+"  is :Alive");
                ui->connectionLabel->setStyleSheet("background-color: green");
                numFECS++;
            }else{
                QString temp;
                deads = deads + temp.number(i+1)+",";
                ui->connectionLabel->setText("  "+deads+"  is/are :Dead");
                ui->connectionLabel->setStyleSheet("background-color: red");
            }
            if(numFECS==ui->numbersOfFecs->value()){
                ui->connectionLabel->setText(" All Alive");
                ui->connectionLabel->setStyleSheet("background-color: green");
            }
        }
    }else{
        ui->connectionLabel->setText(" nocket");
        ui->connectionLabel->setStyleSheet("background-color: lightGray");
        socket->close();
        socket->disconnectFromHost();
    }
		*/
}
//_________________________________________________________________________________________________
void MainWindow::dataPending()
{
    QByteArray ret = _config->getLinkStatusData();
    if(ret.size()) {
        packageHandler(ret);
    } else {
        qDebug() << "[MainWindow::dataPending]    Empty datagram returned by link status";
    }
  //packageHandler(_config->getLinkStatusData());
  //  while(bnd && socket->hasPendingDatagrams())
  //  {
  //      buffer.resize(socket->pendingDatagramSize());
  //      socket->readDatagram( buffer.data(), buffer.size() );
  //      packageHandler(buffer);
  //  }
}
//_________________________________________________________________________________________
int MainWindow::packageHandler(QByteArray package)
{
    if(ui->enableDebugPB->isChecked()){
        bool ok;
        QString sizeOfPackageReceived;
        QString datagramCheck;
        datagramCheck = package.mid(0,4).toHex();
        //        qDebug()<<"CHECK STR: "<<datagramCheck;

        quint32 check = datagramCheck.toUInt(&ok,16);
        //        qDebug()<<"CHECK NU: "<<check;

        if(check<1000000){
            ui->debugScreen->append("****** NEW PACKET RECEIVED ****");
            ui->debugScreen->append("Data Received Size: " + sizeOfPackageReceived.number(package.size(),10)+" bytes");
            QString datagramBin,datagramHex;
            for(int i=0;i<package.size()/4;i++){
                datagramHex = package.mid(i*4,4).toHex();
                quint32 temp32 = datagramHex.toUInt(&ok,16);
                if (i==0) ui->debugScreen->append("Req ID :"+datagramBin.number(temp32,10));
                else ui->debugScreen->append("Data, "+QString::number(i)+": "+datagramBin.number(temp32,16));
            }
            ui->debugScreen->moveCursor(QTextCursor::End ,QTextCursor::MoveAnchor );
        }
    }
    //_config->blockSignals(true);
    //customCommandHandler();

		return 0;
}
//_________________________________________________________________________________________________
void MainWindow::ConnectTCP(){
    if(ui->openConnection_tr == QObject::sender()){
        SetFPGAIP();
        PingFPGA();
        socketTCP = new QTcpSocket();
        socketTCP->connectToHost(ips_tr,FPGAPort, QTcpSocket::ReadWrite);
        connect(socketTCP, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(socketTCP, SIGNAL(readyRead()), this, SLOT(readTCP()));
        if (socketTCP->waitForConnected(2000)){
            //connect( socket, SIGNAL(readyRead()), this, SLOT(dataPending()) );
            ui->connectionLabel_tr->setText("      Alive");
            ui->connectionLabel_tr->setStyleSheet("background-color: green");
        }else{
            ui->connectionLabel_tr->setText("   Problem");
            ui->connectionLabel_tr->setStyleSheet("background-color: lightGray");
        }
    }else if(ui->closeConnection_tr == QObject::sender()){
        socketTCP->close();
        socketTCP->disconnect();
        ui->connectionLabel_tr->setText("      Disconnected");
        ui->connectionLabel_tr->setStyleSheet("background-color: red");
    }
}
//_________________________________________________________________________________________________
void MainWindow::readTCP(){
    if(ui->readTrigger->isChecked()){
        bool ok;
        QString tempString;
        while(socketTCP->bytesAvailable()>0){
            //        ui->debugScreen_fpga->append("************************ NEW DCS PACKET RECEIVED **************************");
            daqCnt_fpga++;
            ui->triggerCntLabel_fpga->setText(tempString.number(daqCnt_fpga,10));
            //        QString inputPacket =  socketTCP->readAll().toHex();
            QByteArray trInfo = socketTCP->readAll();
            //        qDebug()<<"Everything:"<<trInfo.toHex();
            //ui->debugScreen_fpga->append(inputPacket);
            //new thread to write the file
            //writeFPGAFile(inputPacket);
            QString eventString = trInfo.toHex().left(8);
            qint32 eventTrigger = eventString.toInt(&ok,16);
            //        qDebug()<<eventTrigger<<"   "<<eventString;
            triggerEventVariable = eventTrigger;
            triggerTimeVariable.clear(); triggerChannelVariable.clear();triggerBoardVariable.clear();
            for(int i=0;i<8;i++){
                std::vector<double> chTrinfo = bytesTobitsTrigger(trInfo.mid(12+i*4,4));
                triggerChannelVariable.push_back(chTrinfo.at(0));
                triggerChannelVariable.push_back(chTrinfo.at(2));
                triggerTimeVariable.push_back(chTrinfo.at(1));
                triggerTimeVariable.push_back(chTrinfo.at(3));
                triggerBoardVariable.push_back((i*2)+1);
                triggerBoardVariable.push_back((i*2)+2);
            }
            if(fileDaqRoot->IsOpen())
                vmm1Trigger->Fill();
            if(ui->monitoring->isChecked())
                EventNumberTrigger->Fill(eventTrigger);
        }
        ui->debugScreen_fpga->moveCursor(QTextCursor::End ,QTextCursor::MoveAnchor );
    }
}
//_________________________________________________________________________________________________
void MainWindow::ConnectFPGA(){
    SetFPGAIP();
    PingFPGA();
    socketFPGA = new QUdpSocket(this);
    bndFPGA = socketFPGA->bind(FPGAPort, QUdpSocket::ShareAddress);
    if(bndFPGA && communicationAliveFPGA){
        //connect( socketFPGA, SIGNAL(readyRead()), this, SLOT(dataPending()) );
        ui->connectionLabel_tr->setText("      Alive");
        ui->connectionLabel_tr->setStyleSheet("background-color: green");
    }else{
        ui->connectionLabel_tr->setText("   Problem");
        ui->connectionLabel_tr->setStyleSheet("background-color: lightGray");
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socketTCP->errorString()));
    }

}
//_________________________________________________________________________________________________
void MainWindow::Disconnect(){
		/*
    ui->connectionLabel->setText("Disconnected");
    ui->connectionLabel->setStyleSheet("background-color: red");
    socket->disconnectFromHost();
    bnd = false;
		*/
}
//_________________________________________________________________________________________________
void MainWindow::CreateChannelsField(){

    //tabs
    //QGroupBox *groupModify = new QGroupBox(tr("VMM2 Channels"),this);
		//This line creates a runtime message, as "this" already has a layout. Should be corrected. - ARV 30/06/15
    //QGridLayout *grid = new QGridLayout(this);
    //QGridLayout *grid = new QGridLayout(this->centralWidget());

    int margin = 7;
    grid->setContentsMargins(margin, margin, margin, margin);
    grid->setHorizontalSpacing(2);
    grid->setVerticalSpacing(2);
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

    for(int j=0;j<16;j++){
        SDLabel->addItem(counter.setNum(j)+" mV");
        SDLabel2->addItem(counter.setNum(j)+" mV");
    }

    for(int j=0;j<32;j++){
        SZ010bLabel->addItem(counter.setNum(j)+" ns");
        SZ010bLabel2->addItem(counter.setNum(j)+" ns");
    }
    for(int j=0;j<16;j++){
        SZ08bLabel->addItem(counter.setNum(j)+" ns");
        SZ08bLabel2->addItem(counter.setNum(j)+" ns");
    }
    for(int j=0;j<8;j++){
        SZ06bLabel->addItem(counter.setNum(j)+" ns");
        SZ06bLabel2->addItem(counter.setNum(j)+" ns");
    }

    VMM1SPBoolAll=0;
    VMM1SCBoolAll=0;
    VMM1SLBoolAll=0;
    VMM1STBoolAll=0;
    VMM1SMBoolAll=0;
    VMM1SMXBoolAll=0;
    VMM1SZ010bBoolAll=0;
    VMM1SZ08bBoolAll=0;
    VMM1SZ06bBoolAll=0;

    VMM1SPBoolAll2=0;
    VMM1SCBoolAll2=0;
    VMM1SLBoolAll2=0;
    VMM1STBoolAll2=0;
    VMM1SMBoolAll2=0;
    VMM1SMXBoolAll2=0;
    VMM1SZ010bBoolAll2=0;
    VMM1SZ08bBoolAll2=0;
    VMM1SZ06bBoolAll2=0;

    SPLabel->setFixedSize(20,15);
    SCLabel->setFixedSize(20,15);
    SLLabel->setFixedSize(20,15);
    STLabel->setFixedSize(20,15);
    SMLabel->setFixedSize(20,15);
    SDLabel->setFixedSize(50,20);
    SZ010bLabel->setFixedSize(50,20);
    SZ08bLabel->setFixedSize(50,20);
    SZ06bLabel->setFixedSize(50,20);
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

    connect(SPLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SDLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelVoltages(int)));
    connect(SZ010bLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SZ08bLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SZ06bLabel,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SCLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SLLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(STLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SMLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SMXLabel,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SPLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SCLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SDLabel2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelVoltages(int)));
    connect(SZ010bLabel2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SZ08bLabel2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SZ06bLabel2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
    connect(SLLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(STLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SMLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));
    connect(SMXLabel2,SIGNAL(pressed()),this,SLOT(updateChannelState()));

    for (int i = 0; i<64; i++){
        VMM1Channel[i] = new QLineEdit(counter.setNum(i+1),groupModify);

        VMM1SC[i] = new QPushButton(initialValueRadio,groupModify);
        VMM1SL[i] = new QPushButton(initialValueRadio,groupModify);
        VMM1ST[i] = new QPushButton(initialValueRadio,groupModify);
        VMM1SM[i] = new QPushButton(initialValueRadio,groupModify);
        VMM1SMX[i] = new QPushButton(initialValueRadio,groupModify);


        VMM1SC[i]->setFixedSize(15,15);
        VMM1ST[i]->setFixedSize(15,15);
        VMM1SL[i]->setFixedSize(15,15);
        VMM1SM[i]->setFixedSize(15,15);
        VMM1SMX[i]->setFixedSize(15,15);

        VMM1SC[i]->setStyleSheet("background-color: lightGray");
        VMM1SM[i]->setStyleSheet("background-color: lightGray");
        VMM1ST[i]->setStyleSheet("background-color: lightGray");
        VMM1SL[i]->setStyleSheet("background-color: lightGray");
        VMM1SMX[i]->setStyleSheet("background-color: lightGray");

        connect(VMM1SC[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMM1SM[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMM1ST[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMM1SL[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMM1SMX[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));

        VMM1SPBool[i]=0;
        VMM1SCBool[i]=0;
        VMM1SMBool[i]=0;
        VMM1STBool[i]=0;
        VMM1SLBool[i]=0;
        VMM1SMXBool[i]=0;


        VMM1Channel[i]->setEnabled(0);
        VMM1SDVoltage[i] = new QComboBox(groupModify);
        VMM1SDVoltage[i]->setFixedSize(50,20);
        connect(VMM1SDVoltage[i],SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelVoltages(int)));
        VMM1SDVoltage[i]->setFont(Font);

        for(int j=0;j<16;j++){
            VMM1SDVoltage[i]->addItem(counter.setNum(j)+" mV");
        }

        VMM1SZ010bCBox[i] = new QComboBox(groupModify);
        VMM1SZ010bCBox[i]->setFixedSize(50,20);
        connect(VMM1SZ010bCBox[i],SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
        VMM1SZ010bCBox[i]->setFont(Font);

        VMM1SZ08bCBox[i] = new QComboBox(groupModify);
        VMM1SZ08bCBox[i]->setFixedSize(50,20);
        connect(VMM1SZ08bCBox[i],SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
        VMM1SZ08bCBox[i]->setFont(Font);

        VMM1SZ06bCBox[i] = new QComboBox(groupModify);
        VMM1SZ06bCBox[i]->setFixedSize(50,20);
        connect(VMM1SZ06bCBox[i],SIGNAL(currentIndexChanged(int)),this,SLOT(updateChannelADCs(int)));
        VMM1SZ06bCBox[i]->setFont(Font);

        for(int j=0;j<32;j++){
            VMM1SZ010bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<16;j++){
            VMM1SZ08bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<8;j++){
            VMM1SZ06bCBox[i]->addItem(counter.setNum(j)+" ns");
        }

        VMM1NegativeButton[i] = new QPushButton(groupModify);
        VMM1NegativeButton[i]->setText("negative");
        connect(VMM1NegativeButton[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));

        VMM1Channel[i]->setFixedSize(20,18);
        VMM1NegativeButton[i]->setFixedSize(40,18);
        VMM1NegativeButton[i]->setFont(Font);
        QLabel *spacer = new QLabel("     ");

        if(i<32){
            if(i==0){
                grid->addWidget(SPLabel,     i,2, Qt::AlignCenter);
                grid->addWidget(SCLabel,     i,3, Qt::AlignCenter);
                grid->addWidget(SLLabel,     i,4, Qt::AlignCenter);
                grid->addWidget(STLabel,     i,5, Qt::AlignCenter);
                grid->addWidget(SMLabel,     i,6, Qt::AlignCenter);
                grid->addWidget(SDLabel,     i,7, Qt::AlignCenter);
                grid->addWidget(SMXLabel,    i,8, Qt::AlignCenter);
                grid->addWidget(SZ010bLabel, i,9, Qt::AlignCenter);
                grid->addWidget(SZ08bLabel,  i,10, Qt::AlignCenter);
                grid->addWidget(SZ06bLabel,  i,11, Qt::AlignCenter);

                grid->addWidget(SPLabel2,     i,14, Qt::AlignCenter);
                grid->addWidget(SCLabel2,     i,15, Qt::AlignCenter);
                grid->addWidget(SLLabel2,     i,16, Qt::AlignCenter);
                grid->addWidget(STLabel2,     i,17, Qt::AlignCenter);
                grid->addWidget(SMLabel2,     i,18, Qt::AlignCenter);
                grid->addWidget(SDLabel2,     i,19, Qt::AlignCenter);
                grid->addWidget(SMXLabel2,    i,20, Qt::AlignCenter);
                grid->addWidget(SZ010bLabel2, i,21, Qt::AlignCenter);
                grid->addWidget(SZ08bLabel2,  i,22, Qt::AlignCenter);
                grid->addWidget(SZ06bLabel2,  i,23, Qt::AlignCenter);
            }
            grid->addWidget(VMM1Channel[i],         i+1,1, Qt::AlignCenter);
            grid->addWidget(VMM1NegativeButton[i],  i+1,2, Qt::AlignCenter);
            grid->addWidget(VMM1SC[i],              i+1,3, Qt::AlignCenter);
            grid->addWidget(VMM1SL[i],              i+1,4, Qt::AlignCenter);
            grid->addWidget(VMM1ST[i],              i+1,5, Qt::AlignCenter);
            grid->addWidget(VMM1SM[i],              i+1,6, Qt::AlignCenter);
            grid->addWidget(VMM1SDVoltage[i],       i+1,7, Qt::AlignCenter);
            grid->addWidget(VMM1SMX[i],             i+1,8, Qt::AlignCenter);
            grid->addWidget(VMM1SZ010bCBox[i],      i+1,9, Qt::AlignCenter);
            grid->addWidget(VMM1SZ08bCBox[i],       i+1,10, Qt::AlignCenter);
            grid->addWidget(VMM1SZ06bCBox[i],       i+1,11, Qt::AlignCenter);
            grid->addWidget(spacer,                 i+1,12, Qt::AlignCenter);
        }
        else{
            grid->addWidget(VMM1Channel[i],         i-32+1,13, Qt::AlignCenter);
            grid->addWidget(VMM1NegativeButton[i],  i-32+1,14, Qt::AlignCenter);
            grid->addWidget(VMM1SC[i],              i-32+1,15, Qt::AlignCenter);
            grid->addWidget(VMM1SL[i],              i-32+1,16, Qt::AlignCenter);
            grid->addWidget(VMM1ST[i],              i-32+1,17, Qt::AlignCenter);
            grid->addWidget(VMM1SM[i],              i-32+1,18, Qt::AlignCenter);
            grid->addWidget(VMM1SDVoltage[i],       i-32+1,19, Qt::AlignCenter);
            grid->addWidget(VMM1SMX[i],             i-32+1,20, Qt::AlignCenter);
            grid->addWidget(VMM1SZ010bCBox[i],      i-32+1,21, Qt::AlignCenter);
            grid->addWidget(VMM1SZ08bCBox[i],       i-32+1,22, Qt::AlignCenter);
            grid->addWidget(VMM1SZ06bCBox[i],       i-32+1,23, Qt::AlignCenter);
        }
    }
    groupModify->setGeometry(QRect(620, 12, 730, 700));
    groupModify->setLayout(grid);
}
//_________________________________________________________________________________________
void MainWindow::updateChannelADCs(int index){
    // ***********************  SD  *********************************************
    for(int j=0;j<32;j++){
        if(SZ010bLabel == QObject::sender()){
            VMM1SZ010bCBox[j]->setCurrentIndex(index);
            VMM1SZ010bValue[j]=index;
        }
        if(SZ08bLabel == QObject::sender()){
            VMM1SZ08bCBox[j]->setCurrentIndex(index);
            VMM1SZ08bValue[j]=index;
        }
        if(SZ06bLabel == QObject::sender()){
            VMM1SZ06bCBox[j]->setCurrentIndex(index);
            VMM1SZ06bValue[j]=index;
        }
    }

    for(int j=32;j<64;j++){
        if(SZ010bLabel2 == QObject::sender()){
            VMM1SZ010bCBox[j]->setCurrentIndex(index);
            VMM1SZ010bValue[j]=index;
        }
        if(SZ08bLabel2 == QObject::sender()){
            VMM1SZ08bCBox[j]->setCurrentIndex(index);
            VMM1SZ08bValue[j]=index;
        }
        if(SZ06bLabel2 == QObject::sender()){
            VMM1SZ06bCBox[j]->setCurrentIndex(index);
            VMM1SZ06bValue[j]=index;
        }
    }
    for(int i=0;i<64;i++){
        if(VMM1SZ010bCBox[i] == QObject::sender()){
            VMM1SZ010bValue[i]=index;
        }
        if(VMM1SZ08bCBox[i] == QObject::sender()){
            VMM1SZ08bValue[i]=index;
        }
        if(VMM1SZ06bCBox[i] == QObject::sender()){
            VMM1SZ06bValue[i]=index;
        }
    }
}
//_________________________________________________________________________________________
//_________________________________________________________________________________________
void MainWindow::updateChannelVoltages(int index){
    // ***********************  SD  *********************************************
    if(SDLabel == QObject::sender()){
        for(int j=0;j<32;j++){
            VMM1SDVoltage[j]->setCurrentIndex(index);
            VMM1SDValue[j]=index;
        }
    }
    if(SDLabel2 == QObject::sender()){
        for(int j=32;j<64;j++){
            VMM1SDVoltage[j]->setCurrentIndex(index);
            VMM1SDValue[j]=index;
        }
    }
    for(int i=0;i<64;i++){
        if(VMM1SDVoltage[i] == QObject::sender()){
            VMM1SDValue[i]=index;
        }
    }
}
//_________________________________________________________________________________________
void MainWindow::updateChannelState(){
    // ***********************  SC  *********************************************
    if(SCLabel == QObject::sender()){
        if(VMM1SCBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1SC[j]->setStyleSheet("background-color: green");
                VMM1SCBool[j]=true;
            }
            VMM1SCBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1SC[j]->setStyleSheet("background-color: lightGray");
                VMM1SCBool[j]=0;
            }
            VMM1SCBoolAll=0;
        }
    }else if(SCLabel2 == QObject::sender()){
        if(VMM1SCBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1SC[j]->setStyleSheet("background-color: green");
                VMM1SCBool[j]=true;
            }
            VMM1SCBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1SC[j]->setStyleSheet("background-color: lightGray");
                VMM1SCBool[j]=0;
            }
            VMM1SCBoolAll2=0;
        }
    }

    // ***********************  SL  *********************************************
    if(SLLabel == QObject::sender()){
        if(VMM1SLBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1SL[j]->setStyleSheet("background-color: green");
                VMM1SLBool[j]=true;
            }
            VMM1SLBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1SL[j]->setStyleSheet("background-color: lightGray");
                VMM1SLBool[j]=0;
            }
            VMM1SLBoolAll=0;
        }
    }else if(SLLabel2 == QObject::sender()){
        if(VMM1SLBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1SL[j]->setStyleSheet("background-color: green");
                VMM1SLBool[j]=true;
            }
            VMM1SLBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1SL[j]->setStyleSheet("background-color: lightGray");
                VMM1SLBool[j]=0;
            }
            VMM1SLBoolAll2=0;
        }
    }
    // ***********************  ST  *********************************************
    if(STLabel == QObject::sender()){
        if(VMM1STBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1ST[j]->setStyleSheet("background-color: green");
                VMM1STBool[j]=true;
            }
            VMM1STBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1ST[j]->setStyleSheet("background-color: lightGray");
                VMM1STBool[j]=0;
            }
            VMM1STBoolAll=0;
        }
    }else if(STLabel2 == QObject::sender()){
        if(VMM1STBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1ST[j]->setStyleSheet("background-color: green");
                VMM1STBool[j]=true;
            }
            VMM1STBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1ST[j]->setStyleSheet("background-color: lightGray");
                VMM1STBool[j]=0;
            }
            VMM1STBoolAll2=0;
        }
    }
    // ***********************  SM  *********************************************
    if(SMLabel == QObject::sender()){
        if(VMM1SMBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1SM[j]->setStyleSheet("background-color: green");
                VMM1SMBool[j]=true;
            }
            VMM1SMBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1SM[j]->setStyleSheet("background-color: lightGray");
                VMM1SMBool[j]=0;
            }
            VMM1SMBoolAll=0;
        }
    }else if(SMLabel2 == QObject::sender()){
        if(VMM1SMBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1SM[j]->setStyleSheet("background-color: green");
                VMM1SMBool[j]=true;
            }
            VMM1SMBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1SM[j]->setStyleSheet("background-color: lightGray");
                VMM1SMBool[j]=0;
            }
            VMM1SMBoolAll2=0;
        }
    }
    // ***********************  SMX  *********************************************
    if(SMXLabel == QObject::sender()){
        if(VMM1SMXBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1SMX[j]->setStyleSheet("background-color: green");
                VMM1SMXBool[j]=true;
            }
            VMM1SMXBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1SMX[j]->setStyleSheet("background-color: lightGray");
                VMM1SMXBool[j]=0;
            }
            VMM1SMXBoolAll=0;
        }
    }else if(SMXLabel2 == QObject::sender()){
        if(VMM1SMXBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1SMX[j]->setStyleSheet("background-color: green");
                VMM1SMXBool[j]=true;
            }
            VMM1SMXBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1SMX[j]->setStyleSheet("background-color: lightGray");
                VMM1SMXBool[j]=0;
            }
            VMM1SMXBoolAll2=0;
        }
    }

    // ***********************  SP (negative buttons)  *********************************************
    if(SPLabel == QObject::sender()){
        if(VMM1SPBoolAll==0){
            for(int j=0;j<32;j++){
                VMM1NegativeButton[j]->setStyleSheet("background-color: green");
                VMM1SPBool[j]=true;
            }
            VMM1SPBoolAll=1;
        }else{
            for(int j=0;j<32;j++){
                VMM1NegativeButton[j]->setStyleSheet("background-color: light");
                VMM1SPBool[j]=0;
            }
            VMM1SPBoolAll=0;
        }
    }else if(SPLabel2 == QObject::sender()){
        if(VMM1SPBoolAll2==0){
            for(int j=32;j<64;j++){
                VMM1NegativeButton[j]->setStyleSheet("background-color: green");
                VMM1SPBool[j]=true;
            }
            VMM1SPBoolAll2=1;
        }else{
            for(int j=32;j<64;j++){
                VMM1NegativeButton[j]->setStyleSheet("background-color: light");
                VMM1SPBool[j]=0;
            }
            VMM1SPBoolAll2=0;
        }
    }
    // ***********************  Loop Individually  *********************************************
    for(int i=0;i<64;i++){
        if(VMM1SC[i] == QObject::sender()){
            if(VMM1SCBool[i]==0){
                VMM1SC[i]->setStyleSheet("background-color: green");
                VMM1SCBool[i]=true;
            }else if(VMM1SCBool[i]==1){
                VMM1SC[i]->setStyleSheet("background-color: lightGray");
                VMM1SCBool[i]=false;
            }
        }else if(VMM1ST[i] == QObject::sender()){
            if(VMM1STBool[i]==0){
                VMM1ST[i]->setStyleSheet("background-color: green");
                VMM1STBool[i]=true;
            }else if(VMM1STBool[i]==1){
                VMM1ST[i]->setStyleSheet("background-color: lightGray");
                VMM1STBool[i]=false;
            }
        }else if(VMM1SL[i] == QObject::sender()){
            if(VMM1SLBool[i]==0){
                VMM1SL[i]->setStyleSheet("background-color: green");
                VMM1SLBool[i]=true;
            }else if(VMM1SLBool[i]==1){
                VMM1SL[i]->setStyleSheet("background-color: lightGray");
                VMM1SLBool[i]=false;
            }
        }else if(VMM1SM[i] == QObject::sender()){
            if(VMM1SMBool[i]==0){
                VMM1SM[i]->setStyleSheet("background-color: green");
                VMM1SMBool[i]=true;
            }else if(VMM1SMBool[i]==1){
                VMM1SM[i]->setStyleSheet("background-color: lightGray");
                VMM1SMBool[i]=false;
            }
        }else if(VMM1SMX[i] == QObject::sender()){
            if(VMM1SMXBool[i]==0){
                VMM1SMX[i]->setStyleSheet("background-color: green");
                VMM1SMXBool[i]=true;
            }else if(VMM1SMXBool[i]==1){
                VMM1SMX[i]->setStyleSheet("background-color: lightGray");
                VMM1SMXBool[i]=false;
            }
        }else if(VMM1NegativeButton[i] == QObject::sender()){
            if(VMM1SPBool[i]==0){
                VMM1NegativeButton[i]->setStyleSheet("background-color: green");
                VMM1SPBool[i]=true;
            }else if(VMM1SPBool[i]==1){
                VMM1NegativeButton[i]->setStyleSheet("background-color: light");
                VMM1SPBool[i]=false;
            }
        }
    }
}
//_________________________________________________________________________________________
void MainWindow::HandleRecipe(){
    bool ok;

    if(ui->loadCalibDataCB->isChecked() && ui->setVMMs->currentText()=="All"){
        for(int i=0;i<ui->numbersOfFecs->value();i++){
            spiSent=1;
            currentAddressForRecipe = ips[i];
            QStringList list = currentAddressForRecipe.split(".");
            int boardToSendData = list.at(3).toInt(&ok,10);
            for(int l=0;l<calibConstants[boardToSendData-16].size();l++){
                VMM1SDVoltage[l]->setCurrentIndex(calibConstants[boardToSendData-16][l]);
                emit VMM1SDVoltage[l]->setCurrentIndex(calibConstants[boardToSendData-16][l]);
                VMM1SDValue[l]=calibConstants[boardToSendData-16][l];
            }
            ui->sdt->setValue(boardThreshold[boardToSendData-15]);
            Merger();
            Sleeper::msleep(5);
        }
    }else if(ui->loadCalibDataCB->isChecked()){
        spiSent=1;
        QString address = ips[ui->setVMMs->currentIndex()];
        currentAddressForRecipe = address;
        QStringList list = currentAddressForRecipe.split(".");
        int boardToSendData = list.at(3).toInt(&ok,10);
        for(int l=0;l<calibConstants[boardToSendData-16].size();l++){
            VMM1SDVoltage[l]->setCurrentIndex(calibConstants[boardToSendData-16][l]);
            emit VMM1SDVoltage[l]->setCurrentIndex(calibConstants[boardToSendData-16][l]);
            VMM1SDValue[l]=calibConstants[boardToSendData-16][l];
        }
        ui->sdt->setValue(boardThreshold[boardToSendData-15]);
        Merger();
    }else{
        Merger();
    }
}

//_________________________________________________________________________________________
void MainWindow::SendParamsToConfigModule() {

		//cout << ui->ip1->text().toStdString() << "." << ui->ip2->text().toStdString()  << "." << ui->ip3->text().toStdString()  << "." << ui->ip4->text().toStdString()  << endl;

		int nVMMs = -1;
		int ipVMM = -1;
		ipVMM = ui->ip4->text().toInt();

		if (ui->setVMMs->currentText()=="All") {
			nVMMs = ui->numbersOfFecs->value();
		} else if (ui->setVMMs->currentText()!="") {
			nVMMs = 1;
			ipVMM += ui->setVMMs->currentIndex(); 
		} else {
			cout << "Need to set VMM number value first using drop-down list" << endl;
			return;
		}
		//cout << nVMMs << " " << ipVMM << endl;


		int result = _config->LoadConfigFromGUI(ipVMM, nVMMs, channelMap, ui->spg->currentIndex(), ui->slg->currentIndex(), 
																ui->sdrv->currentIndex(), ui->sfm->currentIndex(), ui->sg->currentIndex(), ui->st->currentIndex(),
																ui->stc->currentIndex(), ui->sng->currentIndex(), ui->sdp->currentIndex(), ui->sfa->currentIndex(), ui->sfam->currentIndex(), ui->sdcka->currentIndex(), ui->sbfm->currentIndex(), 
																ui->sbfp->currentIndex(), ui->sbft->currentIndex(), ui->sm5_sm0->currentIndex(), ui->scmx->currentIndex(), ui->sbmx->currentIndex(), ui->spdc->currentIndex(), 
																ui->ssh->currentIndex(), ui->sttt->currentIndex(), ui->stpp->currentIndex(), ui->stot->currentIndex(), ui->s8b->currentIndex(), ui->s6b->currentIndex(), 
																ui->sc010b->currentIndex(), ui->sc08b->currentIndex(), ui->sc06b->currentIndex(), ui->sdcks->currentIndex(), ui->sdck6b->currentIndex(), ui->sdt->value(), ui->sdp_2->value(),
                                VMM1SPBool, VMM1SCBool, VMM1SLBool, VMM1STBool, VMM1SMBool, VMM1SDValue, VMM1SMXBool, VMM1SZ010bValue, VMM1SZ08bValue, VMM1SZ06bValue);
                                //VMM1SPBool, VMM1SCBool, VMM1SLBool, VMM1STBool, VMM1SMBool, VMM1SDValue, VMM1SMXBool, VMM1SZ010bValue, VMM1SZ08bValue, VMM1SZ06bValue, true); //for debug purposes (OFF by default)
		cout << "Result from LoadConfigFromGUI: " << result << endl;

	return;

}
//_________________________________________________________________________________________
void MainWindow::SendRunParamsToDAQModule()
{
    qDebug() << "[MainWindow::SendRunParamsToDAQModule]    Sending trigger acquisition constants to DAQ module.";
    // --- send the DAQ constants --- //
    _runDAQ->LoadDAQConstantsFromGUI(ui->pulserDelay->value(),
                                  ui->trgPeriod->text(),
                                  ui->acqSync->value(),
                                  ui->acqWindow->value());
    // --- send the channel map --- //
    _runDAQ->LoadChannelMap(channelMap);
    // --- send the list of IPs --- //
    _runDAQ->LoadIPList(_config->getIPs());

    m_daqConstantsLoaded = true;
}

//_________________________________________________________________________________________
void MainWindow::Merger(){

		SendParamsToConfigModule();
		_config->DumpConfigParams();
		int result = _config->SendConfig();
		cout << "Result from SendConfig: " << result << endl;
/*
    //qDebug()<<"##################################### GLOBAL BITS <<",  ##################################";

    //QDataStream::BigEndian	QSysInfo::BigEndian
    //out.device()->seek(4);
    int headerLenght = 0;//2*32;
    QBitArray channelRegisters(2144+headerLenght,false);//64*32 for channels + 3*32 for global - 1088 before

    QString GlobalRegistersStrings[3];
    int GlobalRegistersStringsSequence[3];

    //####################### Global SPI 0 ###############################################
    //    qDebug()<<"GL SP1 0";
    GlobalRegistersStrings[0]="00000000000000000000000000000000";//init
    GlobalRegistersStringsSequence[0]=0;

    QString sc010b_2 = "00"; //10bit
    GlobalRegistersStringsSequence[0] = GlobalRegistersStringsSequence[0]+2;
    sc010b_2 = sc010b_2.number(ui->sc010b->currentIndex(),2);
    for(int j=0;j<sc010b_2.size();j++){
        QString bitTmp=sc010b_2.at(sc010b_2.size()-1-j);
        GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0]-j-1,1, bitTmp);
    }

    QString sc08b_2 = "00"; //8bit
    GlobalRegistersStringsSequence[0] = GlobalRegistersStringsSequence[0]+2;
    sc08b_2 = sc08b_2.number(ui->sc08b->currentIndex(),2);
    for(int j=0;j<sc08b_2.size();j++){
        QString bitTmp=sc08b_2.at(sc08b_2.size()-1-j);
        GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0]-j-1,1, bitTmp);
    }

    QString sc06b_2 = "000"; //6bit
    GlobalRegistersStringsSequence[0] = GlobalRegistersStringsSequence[0]+3;
    sc06b_2 = sc06b_2.number(ui->sc06b->currentIndex(),2);
    for(int j=0;j<sc06b_2.size();j++){
        QString bitTmp=sc06b_2.at(sc06b_2.size()-1-j);
        GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0]-j-1,1, bitTmp);
    }

    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->s8b->currentIndex()));//8-bit enable
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->s6b->currentIndex()));//6-bit enable
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->spdc->currentIndex()));//ADC enable
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->sdcks->currentIndex()));//dual clock serialized
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->sdcka->currentIndex()));//dual clock ART
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->sdck6b->currentIndex()));//dual clk 6-bit
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->sdrv->currentIndex()));//tristates analog
    GlobalRegistersStringsSequence[0]++;
    GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence[0],1, QString::number(ui->stpp->currentIndex()));//timing out 2

       qDebug()<<"GL0: " << GlobalRegistersStrings[0];
    //#######################################################################";

    //####################### Global SPI 1 ###############################################
    //qDebug()<<"GL SP1 1";
    GlobalRegistersStrings[1]="00000000000000000000000000000000";//init
    GlobalRegistersStringsSequence[1]=0;

    QString st0_1 = "00"; //peak time
    GlobalRegistersStringsSequence[1] = GlobalRegistersStringsSequence[1]+2;
    st0_1 = st0_1.number(ui->st->currentIndex(),2);
    for(int j=0;j<st0_1.size();j++){
        QString bitTmp=st0_1.at(st0_1.size()-1-j);
        GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1]-j-1,1, bitTmp);
    }

    GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1],1, QString::number(ui->sfm->currentIndex()));//doubles the leakage current
    GlobalRegistersStringsSequence[1]++;

    QString sg0_2 = "000"; //gain
    GlobalRegistersStringsSequence[1] = GlobalRegistersStringsSequence[1]+3;
    sg0_2 = sg0_2.number(ui->sg->currentIndex(),2);
    for(int j=0;j<sg0_2.size();j++){
        QString bitTmp=sg0_2.at(sg0_2.size()-1-j);
        GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1]-j-1,1, bitTmp);
    }

    GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1],1, QString::number(ui->sng->currentIndex()));//neighbor
    GlobalRegistersStringsSequence[1]++;
    GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1],1, QString::number(ui->stot->currentIndex()));//Direct outputs setting
    GlobalRegistersStringsSequence[1]++;
    GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1],1, QString::number(ui->sttt->currentIndex()));//direct timing
    GlobalRegistersStringsSequence[1]++;
    GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1],1, QString::number(ui->ssh->currentIndex()));//hysterisis
    GlobalRegistersStringsSequence[1]++;

    QString stc0_2 = "00"; //TAC slope adjustment
    GlobalRegistersStringsSequence[1] = GlobalRegistersStringsSequence[1]+2;
    stc0_2 = stc0_2.number(ui->stc->currentIndex(),2);
    for(int j=0;j<stc0_2.size();j++){
        QString bitTmp=stc0_2.at(stc0_2.size()-1-j);
        GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1]-j-1,1, bitTmp);
    }

    QString sdt0_9 = "0000000000"; //thr dac
    GlobalRegistersStringsSequence[1] = GlobalRegistersStringsSequence[1]+10;
    sdt0_9 = sdt0_9.number(ui->sdt->value(),2);
    for(int j=0;j<sdt0_9.size();j++){
        QString bitTmp=sdt0_9.at(sdt0_9.size()-1-j);
        GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1]-j-1,1, bitTmp);
    }

    QString sdp0_9 = "0000000000"; //pulse dac
    GlobalRegistersStringsSequence[1] = GlobalRegistersStringsSequence[1]+10;
    sdp0_9 = sdp0_9.number(ui->sdp_2->value(),2);
    for(int j=0;j<sdp0_9.size();j++){
        QString bitTmp=sdp0_9.at(sdp0_9.size()-1-j);
        GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence[1]-j-1,1, bitTmp);
    }

    qDebug()<<"GL1: "<<GlobalRegistersStrings[1];

    //#######################################################################";
    //####################### Global SPI 2 ###############################################
    GlobalRegistersStrings[2]="00000000000000000000000000000000";//init
    GlobalRegistersStringsSequence[2]=16;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->spg->currentIndex()));//polarity
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sdp->currentIndex()));//disable at peak
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sbmx->currentIndex()));//analog monitor to pdo
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sbft->currentIndex()));//tdo buffer
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sbfp->currentIndex()));//pdo buffer
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sbfm->currentIndex()));//mo buffer
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->slg->currentIndex()));//leakage current 0=enabled
    GlobalRegistersStringsSequence[2]++;

    QString sm0_5 = "000000"; //10 bit adc lsb
    GlobalRegistersStringsSequence[2] = GlobalRegistersStringsSequence[2]+6;
    sm0_5 = sm0_5.number(ui->sm5_sm0->currentIndex(),2);
    for(int j=0;j<sm0_5.size();j++){
        QString bitTmp=sm0_5.at(sm0_5.size()-1-j);
        GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2]-j-1,1, bitTmp);
    }

    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->scmx->currentIndex()));//multiplexer
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sfa->currentIndex()));//ART enable
    GlobalRegistersStringsSequence[2]++;
    GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence[2],1, QString::number(ui->sfam->currentIndex()));//ART mode

    qDebug() << "GL2: " << GlobalRegistersStrings[2];

    //#######################################################################";
    QString channelRegistersStrings[64];
    for(int i=0;i<64;i++){
        int bitSequenceCh=8;
        channelRegistersStrings[i]="00000000000000000000000000000000";//init
        //        qDebug()<<"----------------- NEW CHANNEL: "<<i<<",  -------------------------------";
        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1SPBool[i]));bitSequenceCh++;
        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1SCBool[i]));bitSequenceCh++;
        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1SLBool[i]));bitSequenceCh++;
        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1STBool[i]));bitSequenceCh++;
        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1SMBool[i]));bitSequenceCh++;

        QString sd0_3 = "0000"; //pulse dac
        bitSequenceCh = bitSequenceCh+4;
        sd0_3 = sd0_3.number(VMM1SDVoltage[i]->currentIndex(),2);
        //        qDebug()<<"To tetrabito einai :"<<sd0_3;
        for(int j=0;j<sd0_3.size();j++){
            QString bitTmp=sd0_3.at(sd0_3.size()-1-j);
            //            qDebug()<<"To bit stin thesi :"<<j<<", :"<<bitTmp;
            channelRegistersStrings[i].replace(bitSequenceCh+j-4,1, bitTmp);
        }

        channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(VMM1SMXBool[i]));bitSequenceCh++;

        QString sz010b_5 = "00000"; //10 bit adc lsb
        bitSequenceCh = bitSequenceCh+5;
        sz010b_5 = sz010b_5.number(VMM1SZ010bCBox[i]->currentIndex(),2);
        for(int j=0;j<sz010b_5.size();j++){
            QString bitTmp=sz010b_5.at(sz010b_5.size()-1-j);
            channelRegistersStrings[i].replace(bitSequenceCh-j-1,1, bitTmp);
        }

        QString sz08b_4 = "0000"; //8 bit adc lsb
        bitSequenceCh = bitSequenceCh+4;
        sz08b_4 = sz08b_4.number(VMM1SZ08bCBox[i]->currentIndex(),2);
        for(int j=0;j<sz08b_4.size();j++){
            QString bitTmp=sz08b_4.at(sz08b_4.size()-1-j);
            channelRegistersStrings[i].replace(bitSequenceCh-j-1,1, bitTmp);
        }

        QString sz06b_3 = "000"; //8 bit adc lsb
        bitSequenceCh = bitSequenceCh+3;
        sz06b_3 = sz06b_3.number(VMM1SZ06bCBox[i]->currentIndex(),2);
        for(int j=0;j<sz06b_3.size();j++){
            QString bitTmp=sz06b_3.at(sz06b_3.size()-1-j);
            channelRegistersStrings[i].replace(bitSequenceCh-j-1,1, bitTmp);
        }

        //        qDebug()<<channelRegistersStrings[i];
        //        qDebug()<<"-----------------------------------------------------------------------";
    }

    UpdateCounter();
    //__________ Writing _______________
    if(ui->writeRB->isChecked()){
        ui->spiRB->setChecked(1);
        if(ui->s6RB->isChecked()){
            //            QByteArray datagramAll;
            //            QDataStream out (&datagramAll, QIODevice::WriteOnly);
            //            out.device()->seek(0);
            //            QString header = "FEAD";
            //            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)0;
            //            if(ui->SendConfiguration == QObject::sender()){
            //                Sender(datagramAll);
            //                //qDebug()<<"Data:"<<datagramAll.toHex()<<", size:"<<datagramAll.size();
            //            }
        }
        if(ui->appRB->isChecked()){
            //            QByteArray datagramAll;
            //            QDataStream out (&datagramAll, QIODevice::WriteOnly);
            //            out.device()->seek(0);
            //            QString header = "FEAD";
            //            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
            //            if(ui->SendConfiguration == QObject::sender()){
            //                Sender(datagramAll);
            //                //qDebug()<<"Data:"<<datagramAll.toHex()<<", size:"<<datagramAll.size();
            //            }
        }
        if(ui->spiRB->isChecked()){
            QByteArray datagramSPI;
            QDataStream out (&datagramSPI, QIODevice::WriteOnly);
            out.device()->seek(0);
            QString cmd = "AAAAFFFF";
            QString msbCounter = "0x80000000";
            //            qDebug()<<"O COUNTTER: "<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
            out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint32)channelMap;
            out<<(quint32)cmd.toUInt(&ok,16)<<(quint32)0;

            quint32 firstRegisterChannelSPI = 0;
            quint32 lastRegisterChannelSPI = 63;
            quint32 firstRegisterGlobalSPI = 64;
            quint32 lastRegisterGlobalSPI = 66;

            for(unsigned int i=firstRegisterChannelSPI;i<=lastRegisterChannelSPI;i++){
                bool ok;
                out<<(quint32)(i)<<(quint32)channelRegistersStrings[i].toUInt(&ok,2);
                //                qDebug()<<"Address:"<<i<<", value:"<<(quint32)channelRegistersStrings[i].toUInt(&ok,2);
            }
            for(unsigned int i=firstRegisterGlobalSPI;i<=lastRegisterGlobalSPI;i++){
                bool ok;
                out<<(quint32)(i)<<(quint32)GlobalRegistersStrings[i-firstRegisterGlobalSPI].toUInt(&ok,2);
                //                qDebug()<<"Address:"<<i<<", value:"<<(quint32)GlobalRegistersStrings[i].toUInt(&ok,2);
            }
            //Download the SPI
            out<<(quint32)128<<(quint32)1;

            if(ui->SendConfiguration == QObject::sender()){
                Sender(datagramSPI);
                if(ui->loadCalibDataCB->isChecked())spiSent=1;
                //                downloadSPI();
                //qDebug()<<"Data:"<<datagramAll.toHex()<<", size:"<<datagramAll.size()<<", spi size:"<<channelRegisters.size()-headerLenght;
            }
        }
    }else if(ui->readRB->isChecked()){// ^^^^^^^^^^^^^^^^^ Reading ^^^^^^^^^^^^^^^^^^^^^
        ui->spiRB->setChecked(1);
        if(ui->s6RB->isChecked()){
            //            QByteArray datagramAll;
            //            QDataStream out (&datagramAll, QIODevice::WriteOnly);
            //            out.device()->seek(0);
            //            QString header = "FEAD";
            //            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)1;
            //            if(ui->SendConfiguration == QObject::sender()){
            //                Sender(datagramAll);
            //                //qDebug()<<"Data:"<<datagramAll.toHex()<<", size:"<<datagramAll.size();
            //            }
        }
        if(ui->appRB->isChecked()){
            //            QByteArray datagramAll;
            //            QDataStream out (&datagramAll, QIODevice::WriteOnly);
            //            out.device()->seek(0);
            //            QString header = "FEAD";
            //            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)3;
            //            if(ui->SendConfiguration == QObject::sender()){
            //                Sender(datagramAll);
            //                //qDebug()<<"Data:"<<datagramAll.toHex()<<", size:"<<datagramAll.size();
            //            }
        }
        if(ui->spiRB->isChecked()){
            QByteArray datagramSPI;
            QDataStream out (&datagramSPI, QIODevice::WriteOnly);
            out.device()->seek(0);
            QString cmd = "BBAAFFFF";
            QString msbCounter = "0x80000000";
            out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint32)channelMap;
            out<<(quint32)cmd.toUInt(&ok,16);
            out<<(quint32)0;
            quint32 firstRegisterChannelSPI = 0;
            quint32 lastRegisterGlobalSPI = 66;

            for(unsigned int i=firstRegisterChannelSPI;i<=lastRegisterGlobalSPI;i++){
                out<<(quint32)(i);
            }
            Sender(datagramSPI);

        }
    }
*/
}
//_________________________________________________________________________________________
void MainWindow::downloadSPI(){//if to be called from datapending - responce = 1
/*
    bool ok;
    UpdateCounter();
    QByteArray datagramAll;
    QDataStream out (&datagramAll, QIODevice::WriteOnly);
    QString msbCounter = "0x80000000";
    QString header = "AAAAFFFF";
    out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
    out<<(quint32)channelMap;
    out<<(quint32)header.toUInt(&ok,16);
    out<<(quint32)0;
    out<<(quint32)128<<(quint32)1;
    Sender(datagramAll);

    //    UpdateCounter();
    //    out.device()->seek(0);
    //    out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
    //    out<<(quint16)2<<(quint32)9<<(quint32)0;
    //    Sleeper::msleep(10);
    //    if(ui->loadCalibDataCB->isChecked())spiSent=1;
    //    Sender(datagramAll);

    //    // ADC Start Dly
    //    UpdateCounter();
    //    out.device()->seek(0);
    //    out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
    //    out<<(quint16)2<<(quint32)6<<(quint32)15;
    //    Sleeper::msleep(10);
    //    if(ui->loadCalibDataCB->isChecked())spiSent=1;
    //    Sender(datagramAll);

    //    //Start Dly
    //    UpdateCounter();
    //    out.device()->seek(0);
    //    out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
    //    out<<(quint16)2<<(quint32)7<<(quint32)6;
    //    Sleeper::msleep(10);
    //    if(ui->loadCalibDataCB->isChecked())spiSent=1;
    //    Sender(datagramAll);
*/
}
//_________________________________________________________________________________________
void MainWindow::LoadConfigurationFromFile(){

	int result;
	bool SPBool[64], SCBool[64], SLBool[64], STBool[64], SMBool[64], SMXBool[64];
	quint8 SDVoltage[64], SZ010bCBox[64], SZ08bCBox[64], SZ06bCBox[64];
	QStringList ips_list;

	//Opens a file dialog to choose XML config file to use
	QString filename = QFileDialog::getOpenFileName(this,
    tr("Open Configuration XML File"), "../configs/", tr("XML Files (*.xml)"));
	
	if (filename.isNull()) return;

	cout << filename.toStdString() << endl;

	result = _config->ReadCFile(filename);
	cout << "ReadCFile return code: " << result << endl;

	_config->getIPs(ips_list);
	ui->numbersOfFecs->setValue(ips_list.size());
	ui->ip4->setText(ips_list.at(0).split(".").last());
	DecodeChannelMap(_config->getChannelMap());
	//Get config values from configuration module object
	ui->spg->setCurrentIndex(_config->getChPolarity());
	ui->slg->setCurrentIndex(_config->getLeakCurrent());
	ui->sdrv->setCurrentIndex(_config->getAnalogTristates());
	ui->sfm->setCurrentIndex(_config->getDoubleLeak());
	ui->sg->setCurrentIndex(_config->getGainVal());
	ui->st->setCurrentIndex(_config->getPeakt());
	ui->stc->setCurrentIndex(_config->getTACslope());
	ui->sdp->setCurrentIndex(_config->getDisAtPeak());
	ui->sfa->setCurrentIndex(_config->getART());
	ui->sfam->setCurrentIndex(_config->getARTmode());
	ui->sdcka->setCurrentIndex(_config->getDualClock());
	ui->sbfm->setCurrentIndex(_config->getSBFM());
	ui->sbfp->setCurrentIndex(_config->getSBFP());
	ui->sbft->setCurrentIndex(_config->getSBFT());
	ui->sm5_sm0->setCurrentIndex(_config->getChanMon());
	ui->scmx->setCurrentIndex(_config->getSCMX());
	ui->sbmx->setCurrentIndex(_config->getSBMX());
	ui->spdc->setCurrentIndex(_config->getADCs());
	ui->ssh->setCurrentIndex(_config->getHyst());
	ui->sttt->setCurrentIndex(_config->getDirectTime());
	ui->stpp->setCurrentIndex(_config->getDirectTimeModeOne());
	ui->stot->setCurrentIndex(_config->getDirectTimeModeTwo());
	ui->s8b->setCurrentIndex(_config->getEbitConvMode());
	ui->s6b->setCurrentIndex(_config->getSbitEnable());
	ui->sc010b->setCurrentIndex(_config->getADC10b());
	ui->sc08b->setCurrentIndex(_config->getADC8b());
	ui->sc06b->setCurrentIndex(_config->getADC6b());
	ui->sdcks->setCurrentIndex(_config->getDualClockData());
	ui->sdck6b->setCurrentIndex(_config->getDualClocksbit());
	ui->sdt->setValue(_config->getThresholdDAC());
	ui->sdp_2->setValue(_config->getTestPulseDAC());

	_config->getChannelArrays(SPBool, SCBool, SLBool, STBool, SMBool, SDVoltage, SMXBool, SZ010bCBox, SZ08bCBox, SZ06bCBox);
	//Click on QPushButton to trigger the slot response
	//setCurrentIndex on QComboBox will trigger the currentindexchanged signal
	for(int i=0;i<64;i++){
		if (SPBool[i]) VMM1NegativeButton[i]->click(); //false = negative polarity; true = positive polarity (green)
		if (SCBool[i]) VMM1SC[i]->click();
		if (!SLBool[i]) VMM1SL[i]->click(); //enabled = false; disabled = true;
		if (STBool[i]) VMM1ST[i]->click();
		if (SMBool[i]) VMM1SM[i]->click();
		if (SMXBool[i]) VMM1SMX[i]->click();
		VMM1SDVoltage[i]->setCurrentIndex(SDVoltage[i]);
		VMM1SZ010bCBox[i]->setCurrentIndex(SZ010bCBox[i]);
		VMM1SZ08bCBox[i]->setCurrentIndex(SZ08bCBox[i]);
		VMM1SZ06bCBox[i]->setCurrentIndex(SZ06bCBox[i]);
	}

}
//_________________________________________________________________________________________
void MainWindow::LoadDAQConfigurationFromFile(){

    ui->useMapping->blockSignals(true);
    // open the file
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open DAQ Configuration XML File"), "../configs/", tr("XML Files (*.xml)"));
    if(filename.isNull()) return;
    qDebug() << "[MainWindow::LoadDAQConfigurationFromFile]    Loading DAQ config file : " << filename;

    // load the daq configuration
    _runDAQ->getDAQConfig(filename);
   
    // set the output directory 
    ui->runDirectoryField->setText(_runDAQ->getOutputDirectory());
   
    // trigger acquisition constants
    ui->pulserDelay->setValue(_runDAQ->tpDelay());  // TP delay
    ui->trgPeriod->setText(_runDAQ->trigPeriod());
    ui->acqSync->setValue(_runDAQ->acqSync());
    ui->acqWindow->setValue(_runDAQ->acqWindow());

    // general run configuration
    ui->useMapping->setChecked(_runDAQ->useChannelMap());
    ui->ignore16->setChecked(_runDAQ->ignore16());
    ui->useMapping->blockSignals(false);

}
//_________________________________________________________________________________________
void MainWindow::WriteConfigurationFromFile(){
	//bool ok;

	QString runmode;

	//Opens a file dialog to select or create XML config file to save
	QString filename = QFileDialog::getSaveFileName(this, 
		tr("Save XML Configuration File"), "../configs", tr("XML Files (*.xml)"));

	if (filename.isNull()) return;

	//cout << filename.toStdString() << endl;

	if (ui->trgPulser->isChecked()) runmode="pulser";
	else if (ui->trgExternal->isChecked()) runmode="external";

	SendParamsToConfigModule();
	//qDebug() << channelMap;
	_config->LoadRunParamsFromGUI(ui->pulserDelay->value(), ui->trgPeriod->text(), ui->acqSync->value(), ui->acqWindow->value(), runmode);
	_config->WriteCFile(filename);

}
//_________________________________________________________________________________________
void MainWindow::WriteDAQConfigurationFile()
{
    // TODO :: need to implement writing of DAQ configuration file

    QString outfilename = QFileDialog::getSaveFileName(this,
        tr("Save DAQ Configuration XML"), "../configs", tr("XML Files (*.xml)"));
    if(outfilename.isNull()) return;

    _runDAQ->LoadDAQConstantsFromGUI(ui->pulserDelay->value(),
                                        ui->trgPeriod->text(),
                                        ui->acqSync->value(),
                                        ui->acqWindow->value());

    QString runmode_ = "";
    if(ui->trgPulser->isChecked() && !(ui->trgExternal->isChecked())) runmode_ = "pulser";
    else if(!(ui->trgPulser->isChecked()) && ui->trgExternal->isChecked()) runmode_ = "external";
    else {
        // set to default
        runmode_ = "pulser";
    }
    QString mapfile_ = "";
    bool usemap = false;
    if(ui->useMapping->isChecked()) {
        usemap = true;
        // this datamember will always be filled if useMapping has been set
        mapfile_ = _dataProcessor->getMapFileName();
    }
    QString outdir_ = ui->runDirectoryField->text(); 
    bool ignore16_ = false;
    if(ui->ignore16->isChecked()) ignore16_ = true;

    _runDAQ->writeNewDAQConfigFile(outfilename,
                                    usemap,
                                    ignore16_,
                                    runmode_,
                                    mapfile_,
                                    outdir_);

}
//_________________________________________________________________________________________
void MainWindow::setChannelMaps(int)
{
    if(ui->useMapping->isChecked()) {
        QString mapfilename = QFileDialog::getOpenFileName(this,
                tr("Open Peripheral Channel Map File"), "../configs/", tr("TXT Files (*.txt)"));
        if(mapfilename.isNull()) return;
        qDebug() << "[MainWindow::setChannelMaps]    Loading peripherla channel map file : " << mapfilename;
        _dataProcessor->setDataType("MINI2");
        _dataProcessor->setMapFileName(mapfilename);
        _dataProcessor->fillChannelMaps();
    }

}
//_________________________________________________________________________________________
void MainWindow::updateTriggerCount()
{
    QString cnt_;
    ui->triggerCntLabel->setText(cnt_.number(_dataProcessor->getDAQCount(), 10));
}
//_________________________________________________________________________________________
void MainWindow::customCommandHandler(){//if to be called from datapending - responce = 1
    bool ok;

    //idForCustomCommands = 1 onACQ,
    //                      2 offACQ
    //                      3 detection time mode + edge detection
    //                      4 trigger mode
    //                      5 trigger counter
    //                      6 asic
    //                      7 cdaq reset
    //                      8

    if(ui->onACQ == QObject::sender()||ui->offACQ == QObject::sender()||
            ui->asic_reset == QObject::sender()||ui->fec_reset == QObject::sender()||
            ui->triggerCounter_reset == QObject::sender()||ui->checkTriggers== QObject::sender()){
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
        out<<(quint16)3<<(quint32)0<<(quint32)0;
        Sender(datagramAll);
        if(ui->onACQ == QObject::sender())idForCustomCommands = 1;
        else if(ui->offACQ == QObject::sender())idForCustomCommands = 2;
        else if(ui->triggerCounter_reset == QObject::sender()||ui->checkTriggers== QObject::sender())idForCustomCommands = 5;
        else if(ui->asic_reset == QObject::sender()) idForCustomCommands = 6;
        else if(ui->fec_reset == QObject::sender()) idForCustomCommands = 7;
    }if(ui->applyDetectionMode == QObject::sender()){
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
        out<<(quint16)3<<(quint32)0<<(quint32)0<<(quint32)4<<(quint32)0;
        Sender(datagramAll);
        idForCustomCommands = 3;
    }if(ui->applyTrigger == QObject::sender()){
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
        out<<(quint16)3<<(quint32)0<<(quint32)0<<(quint32)3<<(quint32)0;
        Sender(datagramAll);
        idForCustomCommands = 4;
    }else {
        if(idForCustomCommands!=0){
            QString datagramBin,datagramHex;
            datagramHex = buffer.mid(0,4).toHex();
            quint32 reqReceived =datagramHex.toUInt(&ok,16);
            if(reqReceived==commandCounter){
                QDataStream out (&buffer, QIODevice::WriteOnly);
                if(idForCustomCommands == 1){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 0, true);
                    out.device()->seek(12);
                    out<<temp32;
                }else if(idForCustomCommands == 2){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 0, false);
                    out.device()->seek(12);
                    out<<temp32;
                }else if(idForCustomCommands == 3){
                    QString tempString;
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 2, ui->edgeSelection->currentIndex());
                    QString tempStr = tempString.number(temp32,10);
                    temp32 = ValueToReplace(tempStr, 1, ui->detectionMode->currentIndex());

                    //                    datagramHex.replace(6,1,tempString.number(ui->detectionMode->currentIndex()));
                    //                    datagramHex.replace(5,1,tempString.number(ui->edgeSelection->currentIndex()));
                    //                    quint32 temp32 = datagramHex.toUInt(&ok,2);
                    //                    datagramBin.number(temp32,2);
                    out.device()->seek(12);
                    out<<temp32;
                    out.device()->seek(20);
                    temp32 = (ui->detectionTime->value()-50)/25;
                    out<<temp32;
                }else if(idForCustomCommands == 4){
                    QString tempString;
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32=0;
                    if(ui->triggerMode->currentIndex()==1)
                        temp32 = ValueToReplace(datagramHex, 3, true);
                    else if (ui->triggerMode->currentIndex()==0)
                        temp32 = ValueToReplace(datagramHex, 3, false);
                    datagramBin.number(temp32,2);
                    out.device()->seek(12);
                    out<<temp32;
                    out.device()->seek(20);
                    temp32 = (ui->triggerTimeout->value()-25)/25;
                    out<<temp32;
                }else if(idForCustomCommands == 5){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 13, true);
                    out.device()->seek(12);
                    out<<temp32;
                }else if(idForCustomCommands == 6){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 14, true);
                    out.device()->seek(12);
                    out<<temp32;
                }else if(idForCustomCommands == 7){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 15, true);
                    out.device()->seek(12);
                    out<<temp32;
                }

                out.device()->seek(6);
                out<<(quint16)2; //change to write mode

                UpdateCounter();
                Sender(buffer);

                // Clear Bits
                if(idForCustomCommands == 5){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 13, false);
                    out.device()->seek(12);
                    out<<temp32;
                    UpdateCounter();
                    Sender(buffer);
                }else if(idForCustomCommands == 6){
                    datagramHex = buffer.mid(12,4).toHex();
                    quint32 temp32 = ValueToReplace(datagramHex, 14, false);
                    out.device()->seek(12);
                    out<<temp32;
                    UpdateCounter();
                    Sender(buffer);
                }

                idForCustomCommands=0;
            }
        }
    }
}
//_________________________________________________________________________________________
void MainWindow::customCommandWithoutResponse(){
    if(ui->setDaqMode == QObject::sender()){
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
        out<<(quint16)2<<(quint32)12<<(quint32)ui->daqModeOutput->currentIndex();
        Sender(datagramAll);
    }else if(ui->setHeaderPB == QObject::sender()){
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        if(ui->setVMMs->currentText()!="All"){
            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
            out<<(quint32)13<<(quint8)ui->header4LE->text().toUInt(&ok,10)<<(quint8)ui->header3LE->text().toUInt(&ok,10);
            out<<(quint8)ui->header2LE->text().toUInt(&ok,10)<<(quint8)ui->header1LE->text().toUInt(&ok,10);
            Sender(datagramAll);
        }else {
            headerSet = 1;
            for(int i=0;i<ui->setVMMs->count()-1;i++){
                UpdateCounter();
                ui->setVMMs->setCurrentIndex(i);
                QString tempString;
                out.device()->seek(0);
                ui->header4LE->setText(tempString.number(i+ui->ip4->text().toUInt(&ok,10),10));
                out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
                out<<(quint32)13<<(quint8)ui->header4LE->text().toUInt(&ok,10)<<(quint8)ui->header3LE->text().toUInt(&ok,10);
                out<<(quint8)ui->header2LE->text().toUInt(&ok,10)<<(quint8)ui->header1LE->text().toUInt(&ok,10);
                Sender(datagramAll);
            }
            headerSet = 0;
            ui->header4LE->setText(ui->ip4->text()+1);
            ui->setVMMs->setCurrentIndex(ui->setVMMs->currentIndex()+1);
        }
    }else if(ui->lx9Reset == QObject::sender()){
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)0;
        out<<(quint32)0<<(quint32)4;
        Sender(datagramAll);
        out.device()->seek(12);
        out<<(quint32)5;
        Sender(datagramAll);
    }else if(ui->sampleSetPB == QObject::sender()){
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        if(ui->writeRB->isChecked())
            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
        else if(ui->readRB->isChecked())
            out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)3;
        out<<(quint32)11<<(quint32)(ui->sampleValue->currentIndex());
        Sender(datagramAll);
    }else if(ui->applyDetectionMode == QObject::sender()){
        qDebug()<< "Function Without Response in Use ! This will reset everything in register 0x00 and the trigger!!";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        QString header = "FEAD";
        out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
        out<<(quint32)0<<(quint32)7<<(quint32)4<<(quint32)((ui->detectionTime->value()-50)/25);
        Sender(datagramAll);
    }else if(ui->asic_reset==QObject::sender()){
        qDebug()<<"Resetting the ASICs";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)128<<(quint32)2;
        Sender(datagramAll);
    }else if(ui->onACQ==QObject::sender()||ui->offACQ==QObject::sender()){

        ///////////////////////////////// use run_module [begin] dantrim
        if(ui->onACQ==QObject::sender()) {
            // UpdateCounter ?
            _runDAQ->ACQOn();
            ui->onACQ->setStyleSheet("background-color: green");
            ui->offACQ->setStyleSheet("background-color: lightGray");
        }
        else if(ui->offACQ==QObject::sender()) {
            _runDAQ->ACQOff();
            ui->onACQ->setStyleSheet("background-color: lightGray");
            ui->offACQ->setStyleSheet("background-color: green");
        }
        ui->appRB->setChecked(1);
        ///////////////////////////////// use run_module [end] dantrim

/*
        qDebug()<<"Controling the ACQ";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)15;
        if(ui->onACQ==QObject::sender()){
            out<<(quint32)1;
            qDebug()<<"ACQ On";
            ui->onACQ->setStyleSheet("background-color: green");
            ui->offACQ->setStyleSheet("background-color: lightGray");
        }else if(ui->offACQ==QObject::sender()){
            out<<(quint32)0;
            qDebug()<<"ACQ Off";
            ui->onACQ->setStyleSheet("background-color: lightGray");
            ui->offACQ->setStyleSheet("background-color: green");
        }
        ui->appRB->setChecked(1);
        Sender(datagramAll);
*/
    }else if(ui->fec_reset==QObject::sender()||ui->fec_WarmInit==QObject::sender()){
        bool do_reset = (ui->fec_reset==QObject::sender() ? true : false);
        _config->checkFEC(do_reset);
        ui->fecRB->setChecked(1);
        ui->trgExternal->setStyleSheet("background-color: lightGray");
        ui->trgPulser->setStyleSheet("background-color: lightGray");
        ui->setMask->setStyleSheet("background-color: lightGray");
        ui->onACQ->setStyleSheet("background-color: lightGray");
        ui->offACQ->setStyleSheet("background-color: lightGray");
        ui->setTrgAcqConst->setStyleSheet("background-color: lightGray");
    /*
        //original
        QString from;
        if(ui->fec_reset==QObject::sender())
            from = "Reset";
        else if(ui->fec_WarmInit==QObject::sender())
            from = "Warm Init";

        qDebug()<<from<<" FEC";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        QString address = "FFFFFFFF";
        QString value;
        if(ui->fec_reset==QObject::sender())
            value = "FFFF8000";
        else if(ui->fec_WarmInit==QObject::sender())
            value = "FFFF0001";

        out<<(quint32)(address.toUInt(&ok,16));
        out<<(quint32)(value.toUInt(&ok,16));
        ui->fecRB->setChecked(1);
        ui->trgExternal->setStyleSheet("background-color: lightGray");
        ui->trgPulser->setStyleSheet("background-color: lightGray");
        ui->setMask->setStyleSheet("background-color: lightGray");
        ui->onACQ->setStyleSheet("background-color: lightGray");
        ui->offACQ->setStyleSheet("background-color: lightGray");
        ui->setTrgAcqConst->setStyleSheet("background-color: lightGray");
        Sender(datagramAll);
    */
    }else if(ui->setTrgAcqConst==QObject::sender()){

        ///////////////////////////////// use run_module [begin] dantrim
        if(!m_daqConstantsLoaded)
            SendRunParamsToDAQModule();
        _runDAQ->SetTrigAcqConstants();
        ui->setTrgAcqConst->setStyleSheet("background-color: green");
        ui->appRB->setChecked(1);
        ///////////////////////////////// use run_module [end] dantrim

/*
        qDebug()<<"Setting the trigger/acq constants";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        QString trgPeriod = ui->trgPeriod->text();
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)2<<(quint32)trgPeriod.toInt(&ok,16);
        out<<(quint32)4<<(quint32)ui->pulserDelay->value();
        out<<(quint32)5<<(quint32)ui->acqSync->value();
        out<<(quint32)6<<(quint32)ui->acqWindow->value();
        ui->setTrgAcqConst->setStyleSheet("background-color: green");
        ui->appRB->setChecked(1);
        Sender(datagramAll);
*/
    }else if(ui->trgExternal==QObject::sender()||ui->trgPulser==QObject::sender()){

        ///////////////////////////////// use run_module [begin] dantrim
        if(ui->trgExternal==QObject::sender()) {
            _runDAQ->SetTriggerMode(true);
            ui->trgExternal->setStyleSheet("background-color: green");
            ui->trgPulser->setStyleSheet("background-color: lightGray");
        }
        else if(ui->trgPulser==QObject::sender()) {
            _runDAQ->SetTriggerMode(false);
            ui->trgExternal->setStyleSheet("background-color: lightGray");
            ui->trgPulser->setStyleSheet("background-color: green");
        }
        ui->appRB->setChecked(1);
        ///////////////////////////////// use run_module [end] dantrim

/*
        qDebug()<<"Controling the trigger";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        QString trgPeriod = "3FFFE";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)0;
        if(ui->trgExternal==QObject::sender()){
            out<<(quint32)4;
            //                out<<(quint32)2<<(quint32)trgPeriod.toInt(&ok,16);
            //                out<<(quint32)3<<(quint32)80;
            //                out<<(quint32)5<<(quint32)ui->acqSync->value();
            //                out<<(quint32)6<<(quint32)ui->acqWindow->value();
            qDebug()<<"External Trigger";
            ui->trgExternal->setStyleSheet("background-color: green");
            ui->trgPulser->setStyleSheet("background-color: lightGray");
        }else if(ui->trgPulser==QObject::sender()){
            out<<(quint32)7;
            //                out<<(quint32)2<<(quint32)trgPeriod.toInt(&ok,16);
            //                out<<(quint32)3<<(quint32)80;
            //                out<<(quint32)4<<(quint32)ui->pulserDelay->value();
            //                out<<(quint32)5<<(quint32)ui->acqSync->value();
            //                out<<(quint32)6<<(quint32)ui->acqWindow->value();
            qDebug()<<"Pulser enabled";
            ui->trgExternal->setStyleSheet("background-color: lightGray");
            ui->trgPulser->setStyleSheet("background-color: green");
        }
        ui->appRB->setChecked(1);
        Sender(datagramAll);
*/
    }else if(ui->setMask==QObject::sender()){
        _config->setMask();
        ui->appRB->setChecked(1);
        ui->setMask->setStyleSheet("background-color: green");
    /*
        // original
        qDebug()<<"Setting HDMI Mask";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)8;
        out<<(quint32)channelMap;
        ui->appRB->setChecked(1);
        ui->setMask->setStyleSheet("background-color: green");
        Sender(datagramAll);
    */
    }else if(ui->linkPB==QObject::sender() && ui->enableDebugPB->isChecked()){
        _config->blockSignals(false);
        _config->linkPB();
        void delayMs();
        delayMs();
        delayMs();
        _config->blockSignals(true);
        ui->appRB->setChecked(1);

    /*
        // original
        qDebug()<<"Quering the link status";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "BBAAFFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out<<(quint32)channelMap;
        out<<(quint32)cmd.toUInt(&ok,16);//<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)0;
        out<<(quint32)16;
        ui->appRB->setChecked(1);
        Sender(datagramAll);
    */
    }else if(ui->resetLinks==QObject::sender()){
        _config->resetLinks();
        ui->appRB->setChecked(1);

    /*
        //original
        qDebug()<<"Reset the link status";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AA";
        QString cmdType = "AA";
        QString cmdLength = "FFFF";
        QString msbCounter = "0x80000000";
        QString cmdReset = "FF";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)13;
        out<<(quint32)cmdReset.toUInt(&ok,16);
        ui->appRB->setChecked(1);
        Sender(datagramAll);
        datagramAll.clear();
        out.device()->seek(0);
        UpdateCounter();
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint16)0<<(quint16)channelMap;
        out<<(quint8)cmd.toUInt(&ok,16)<<(quint8)cmdType.toUInt(&ok,16)<<(quint16)cmdLength.toUInt(&ok,16);
        out<<(quint32)13;
        out<<(quint32)0;
        ui->appRB->setChecked(1);
        Sender(datagramAll);
    */
    }else if(ui->setEvbld==QObject::sender()){
        _config->setEventHeaders(ui->evebld_infodata->currentIndex(), ui->evbld_mode->currentIndex());
        ui->appRB->setChecked(1);

    /*
        // original
        qDebug()<<"Setting headers";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AAAAFFFF";
        QString msbCounter = "0x80000000";
        quint32 evbldInfo=0;
        if(ui->evebld_infodata->currentIndex()==0) evbldInfo =256;
        else if(ui->evebld_infodata->currentIndex()==1) evbldInfo =512;
        else if(ui->evebld_infodata->currentIndex()==2) evbldInfo =1024;
        else if(ui->evebld_infodata->currentIndex()==3) evbldInfo =2048;
        else if(ui->evebld_infodata->currentIndex()==4) evbldInfo =4096;
        else if(ui->evebld_infodata->currentIndex()==5) evbldInfo =8192;
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out<<(quint32)channelMap;
        out<<(quint32)cmd.toUInt(&ok,16)<<(quint32)0;
        out<<(quint32)10<<(quint32)ui->evbld_mode->currentIndex();
        out<<(quint32)12<<(quint32)1280;//ui->evebld_infodata->currentIndex();//evbldInfo;
        ui->appRB->setChecked(1);
        //        qDebug()<<"evbld_mode: "<<ui->evbld_mode->currentIndex();
                qDebug()<<"evbld_infodata: "<<evbldInfo;
        Sender(datagramAll);
    */
    }else if(ui->setck_s6==QObject::sender()){
        _config->setS6_ck(ui->cktk_s6->currentIndex(), ui->ckbc_s6->currentIndex(), ui->ckbc_skew_s6->currentIndex());
        ui->s6RB->setChecked(1);

    /*
        //original
        qDebug()<<"Setting CKBC & CKTK";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AAAAFFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out<<(quint32)channelMap;
        out<<(quint32)cmd.toUInt(&ok,16)<<(quint32)0;
        out<<(quint32)6<<(quint32)(ui->cktk_s6->currentIndex()*16);
        out<<(quint32)7<<(quint32)(ui->ckbc_s6->currentIndex()+(ui->ckbc_skew_s6->currentIndex()*16));
        ui->s6RB->setChecked(1);
        Sender(datagramAll);
    */

    }else if(ui->setTp_s6==QObject::sender()){
        _config->setS6_Tp(ui->tpScew->currentIndex(), ui->tpWidth->currentIndex(), ui->tpPolarity->currentIndex());
        ui->s6RB->setChecked(1);

    /*
        //original
        qDebug()<<"Setting Test Pulse settings";
        bool ok;
        UpdateCounter();
        QByteArray datagramAll;
        QDataStream out (&datagramAll, QIODevice::WriteOnly);
        out.device()->seek(0);
        QString cmd = "AAAAFFFF";
        QString msbCounter = "0x80000000";
        out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out<<(quint32)channelMap;
        out<<(quint32)cmd.toUInt(&ok,16)<<(quint32)0;
        out<<(quint32)2<<(quint32)(ui->tpScew->currentIndex()+(ui->tpWidth->currentIndex()*16)+(ui->tpPolarity->currentIndex()*128));
        ui->s6RB->setChecked(1);
        Sender(datagramAll);
    */
    }
}
//_________________________________________________________________________________________
quint32 MainWindow::ValueToReplace(QString datagramHex, int bitToChange, bool bitValue){
    bool ok;
    QBitArray commandToSend(32,false);
    QString datagramBin, temp;
    datagramBin = temp.number(datagramHex.toUInt(&ok,16),2);
    for(int i=0;i<datagramBin.size();i++){
        QString bit = datagramBin.at(i);
        commandToSend.setBit(32-datagramBin.size()+i,bit.toUInt(&ok,10));
    }
    commandToSend.setBit(31-bitToChange,bitValue);//set 13th bit
    QByteArray byteArray = bitsToBytes(commandToSend);
    quint32 temp32 = byteArray.toHex().toUInt(&ok,16);
    return temp32;
}
//_________________________________________________________________________________________
quint32 MainWindow::reverse32(QString datagramHex){
    bool ok;
    QBitArray commandToSend(32,false);
    QString datagramBin, temp;
    datagramBin = temp.number(datagramHex.toUInt(&ok,16),2);
    for(int i=0;i<datagramBin.size();i++){
        QString bit = datagramBin.at(i);
        commandToSend.setBit(32-datagramBin.size()+i,bit.toUInt(&ok,10));
    }
    QBitArray commandToSendR(32,false);
    for(int i=0;i<32;i++){
        commandToSendR.setBit(31-i,commandToSend[i]);
    }
    QByteArray byteArray = bitsToBytes(commandToSendR);
    quint32 temp32 = byteArray.toHex().toUInt(&ok,16);
    return temp32;
}
//_________________________________________________________________________________________
void MainWindow::MergerFPGA(){
    QByteArray datagramAll;
    QDataStream out (&datagramAll, QIODevice::WriteOnly);
    if(ui->onAcq_fpga == QObject::sender())
        out<<(quint32)3<<(quint32)0<<(quint32)1<<(quint32)1;
    else if(ui->offAcq_fpga == QObject::sender())
        out<<(quint32)3<<(quint32)0<<(quint32)1<<(quint32)0;

    else if(ui->triggerCnt_fpga_enable == QObject::sender())
        out<<(quint32)2<<(quint32)3<<(quint32)1<<(quint32)0;

    else if(ui->triggerCnt_fpga_clear == QObject::sender()){
        out<<(quint32)2<<(quint32)3<<(quint32)1<<(quint32)1;
        SenderFPGA(datagramAll);
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)3<<(quint32)1<<(quint32)0;
    }
    else if(ui->triggerEnable_fpga == QObject::sender())
        out<<(quint32)2<<(quint32)4<<(quint32)1<<(quint32)1;
    else if(ui->triggerDisable_fpga == QObject::sender())
        out<<(quint32)2<<(quint32)4<<(quint32)1<<(quint32)0;

    else if(ui->realData_fpga == QObject::sender())
        out<<(quint32)2<<(quint32)5<<(quint32)1<<(quint32)0;
    else if(ui->testData_fpga == QObject::sender())
        out<<(quint32)2<<(quint32)5<<(quint32)1<<(quint32)1;

    else if(ui->triggerCnt_fpga_read == QObject::sender())
        out<<(quint32)1<<(quint32)48<<(quint32)1;


    else if(ui->timeWindow_fpgaPB == QObject::sender())
        out<<(quint32)2<<(quint32)6<<(quint32)1<<(quint32)(ui->fa_maxCnt->value());

    SenderFPGA(datagramAll);

    if(ui->trigger_fpga_init== QObject::sender()){
        qDebug()<< "Initialization of FPGA";

        qDebug()<< "Turning DAQ OFF";
        out.device()->seek(0);
        out<<(quint32)3<<(quint32)0<<(quint32)1<<(quint32)0;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Clear Trigger Counter";
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)3<<(quint32)1<<(quint32)1;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Enable Trigger Counter";
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)3<<(quint32)1<<(quint32)0;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Enable Triggers";
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)4<<(quint32)1<<(quint32)1;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Set Datatype to Real";
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)5<<(quint32)1<<(quint32)0;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Setting Time Window";
        out.device()->seek(0);
        out<<(quint32)2<<(quint32)6<<(quint32)1<<(quint32)255;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Turning DAQ ON";
        out.device()->seek(0);
        out<<(quint32)3<<(quint32)0<<(quint32)1<<(quint32)1;
        Sleeper::msleep(500);
        SenderFPGA(datagramAll);

        qDebug()<< "Initialization of FPGA - DONE!";
    }
    // Read Back Directly

    //    QByteArray datagramAllRead;
    //    QDataStream outRead (&datagramAllRead, QIODevice::WriteOnly);
    //    if(ui->onAcq_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)0<<(quint32)1;
    //    else if(ui->offAcq_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)0<<(quint32)1;

    ////    else if(ui->triggerCnt_fpga_clear == QObject::sender())
    ////        outRead<<(quint32)1<<(quint32)3<<(quint32)1;

    //    else if(ui->triggerEnable_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)4<<(quint32)1;
    //    else if(ui->triggerDisable_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)4<<(quint32)1;

    //    else if(ui->realData_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)5<<(quint32)1;
    //    else if(ui->testData_fpga == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)5<<(quint32)1;

    //    else if(ui->timeWindow_fpgaPB == QObject::sender())
    //        outRead<<(quint32)1<<(quint32)6<<(quint32)1;

    //    SenderFPGA(datagramAllRead);


}
//_________________________________________________________________________________________
QByteArray MainWindow::bitsToBytes(QBitArray bits) {
    QByteArray bytes;
    bytes.resize(bits.count()/8);//add more §for a header of 8 bits????
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(7-(b%8))));
    return bytes;
}
//_________________________________________________________________________________________
void MainWindow::Sender(QByteArray blockOfData)
{
    quint16 destPort=0;
    if(ui->spiRB->isChecked())
        destPort=VMMASICPort;
    if(ui->appRB->isChecked())
        destPort=VMMAPPPort;
    if(ui->fecRB->isChecked())
        destPort=FECPort;
    if(ui->s6RB->isChecked())
        destPort=S6Port;

    bnd = socket->bind(FECPort);//, QUdpSocket::ShareAddress);

    if(bnd && ui->connectionLabel->text()==" All Alive"){
        if(ui->setVMMs->currentText()!=""){
            if(ui->setVMMs->currentText()=="All" && headerSet == 0 && multicastingFlag == 0){
                if(!spiSent){
                    for(int i=0;i<ui->numbersOfFecs->value();i++){
                        QString address = ips[i];
                        Sleeper::msleep(5);
                        socket->writeDatagram(blockOfData,QHostAddress(address), destPort);
                        if(!threadDi->isRunning())
                            qDebug()<<"IP:"<<address<<", data:"<<blockOfData.toHex()<<", size:"<<blockOfData.size();
                    }
                }else{
                    socket->writeDatagram(blockOfData,QHostAddress(currentAddressForRecipe), destPort);
                    if(!threadDi->isRunning())
                        qDebug()<<"IP:"<<currentAddressForRecipe<<", data:"<<blockOfData.toHex()<<", size:"<<blockOfData.size();
                    spiSent=0;
                }
            }else if( multicastingFlag == 0){
                QString address = ips[ui->setVMMs->currentIndex()];
                socket->writeDatagram(blockOfData,QHostAddress(address), destPort);
                qDebug()<<"IP:"<<address<<", data:"<<blockOfData.toHex()<<", size:"<<blockOfData.size();
            }else if( multicastingFlag == 1){
                Sleeper::msleep(20);
                socket->writeDatagram(blockOfData,QHostAddress("10.0.0.255"), destPort);
                qDebug()<<"Broadcasting IP:"<<QHostAddress::Broadcast<<", data:"<<blockOfData.toHex()<<", size:"<<blockOfData.size();
                multicastingFlag=0;
            }
        }else{
            qDebug()<<"NO IPs";
        }
    }else{
        qDebug() <<"Error from socket: "<< socket->errorString()<<", Local Port = "<<socket->localPort()<< ", Bind Reply: "<<bnd;
        socket->close();
        socket->disconnectFromHost();
    }

		//Disconnect socket after writing datagrams
		bnd=false;
    socket->close();
    socket->disconnectFromHost();
		delete(socket);

}
//_________________________________________________________________________________________
void MainWindow::SenderFPGA(QByteArray blockOfData)
{
    //    if(bndFPGA && communicationAliveFPGA){
    //QString address = ips_tr;
    //socketFPGA->writeDatagram(blockOfData,QHostAddress(address), FPGAPort);
    socketTCP->write(blockOfData);
    qDebug()<<blockOfData.toHex();
    //    }else{
    //        qDebug()<<"NO IPs";
    //    }
}
//_________________________________________________________________________________________
void MainWindow::hideChannels(int state){

//tabs
//		qDebug()<< "[MainWindow::hideChannels] Unused arg state= " <<state;
//    if(!ui->hideChannels->isChecked()){
//        this->setFixedSize(620,725);
//        ui->enableDebugPB->setChecked(0);
//    }else{
//        this->setFixedSize(1400,725);
//    }
}
//_________________________________________________________________________________________
void MainWindow::showDebugScreen(int state){
//tabs
//		qDebug()<< "[MainWindow::showDebugScreen] Unused arg state= " <<state;
//
//    if(ui->hideChannels->isChecked() && ui->enableDebugPB->isChecked()){
//        this->setFixedSize(1400,874);
//    }else if(!ui->hideChannels->isChecked() && ui->enableDebugPB->isChecked()){
//        this->setFixedSize(620,874);
//    }else if(!ui->hideChannels->isChecked() && !ui->enableDebugPB->isChecked()){
//        this->setFixedSize(620,725);
//    }else if(ui->hideChannels->isChecked() && !ui->enableDebugPB->isChecked()){
//        this->setFixedSize(1400,725);
//    }

    // dantrim -- maybe this isn't the best place for this, but will see if it works
    // when ACQ is on
    // tabs
    _dataProcessor->setDebug(ui->tabWidget->currentIndex()==2);
    //_dataProcessor->setDebug(ui->enableDebugPB->isChecked());
}
//_________________________________________________________________________________________
void MainWindow::clearDebugScreen(){
    if(ui->clearDebugScreenPB == QObject::sender())
        ui->debugScreen->setText("*************** Waiting for Data *****************");
    if(ui->clearDebugScreenPB_fpga == QObject::sender())
        ui->debugScreen_fpga->setText("*************** Waiting for Data *****************");
}
//_________________________________________________________________________________________
void MainWindow::UpdateCounter(){
    commandCounter++;
    QString tmp;
    ui->cmdlabel->setText(tmp.number(commandCounter));
}
//_________________________________________________________________________________________
void MainWindow::constructHeader(int){
    bool ok;
    QString tempString;
    if(ui->setVMMs->currentText()!="All"){
        ui->header4LE->setText(tempString.number(ui->setVMMs->currentIndex()+ui->ip4->text().toUInt(&ok,10),10));
    }
    if(ui->setVMMs->currentText()=="All")ui->header4LE->setText(ui->ip4->text());
}
//_________________________________________________________________________________________
void MainWindow::threadHandler(){
    //    if(ui->startDi == QObject::sender()){
    //        threadDi->start();
    //    }else if(ui->stopDi == QObject::sender()){
    //        threadDi->exit(0);
    //        qDebug()<<"Di Thread Stopped";
    //    }
}
//_________________________________________________________________________________________
void MainWindow::sendDis(int){
/*
    bool ok;
    UpdateCounter();
    QByteArray datagramAll;
    QDataStream out (&datagramAll, QIODevice::WriteOnly);
    QString header = "FEAD";
    out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16)<<(quint16)2;
    out<<(quint32)1<<(quint32)32;
    if(ui->setVMMs->currentText()=="All")
        multicastingFlag=1;
    void delayMs();

    //    Sleeper::msleep(10);
    Sender(datagramAll);
    out.device()->seek(12);
    out<<(quint32)0;
    if(ui->setVMMs->currentText()=="All")
        multicastingFlag=1;
    Sleeper::msleep(10);
    //    delayMs();
    Sender(datagramAll);
*/
}

//_________________________________________________________________________________________________
void MainWindow::triggerHandler(){

    // set the write flag for dataprocessor
    _dataProcessor->setWriteData(static_cast<bool>(ui->writeData->isChecked()));
    _dataProcessor->setUseChannelMap(static_cast<bool>(ui->useMapping->isChecked()));
    _dataProcessor->setIgnore16(static_cast<bool>(ui->ignore16->isChecked()));
    if(ui->calibration->isChecked()) _dataProcessor->setCalibration(true);

    if(ui->checkTriggers == QObject::sender()){
        bool ok;
        if(ui->writeData->isChecked()){
            // setup the output root file
            QString rootFileDir = ui->runDirectoryField->text() + "/";
            QString filename_init = "run_%4d.root";
            const char* filename_formed = Form(filename_init.toStdString().c_str(), ui->runNumber->text().toInt(&ok,10));
            QString rootFileName = QString(filename_formed);
            _dataProcessor->setupOutputFile(rootFileDir, rootFileName);

            // setup the output ttrees

            _dataProcessor->setupOutputTrees();

            // send dataprocessor the run parameters and fill the associated tree if writing the data out

            int run_number, tac_slope, peak_time, dac_counts, pulser_counts, angle_; // dataprocessor doesn't do anly calibration things yet this is a TODO item
            double gain_;
            run_number = ui->runNumber->value();
            tac_slope = ui->stc->currentText().left(4).toInt();
            peak_time = ui->st->currentText().left(3).toInt();
            dac_counts = ui->sdt->value();
            pulser_counts = ui->sdp_2->value();
            angle_ = ui->angle->value();
            gain_ = ui->sg->currentText().left(3).toDouble();

            _dataProcessor->fillRunProperties(run_number, gain_, tac_slope, peak_time, dac_counts, pulser_counts, angle_);

            // DAQ counter
            _dataProcessor->resetDAQCount();
            QString cnt_;
            ui->triggerCntLabel->setText(cnt_.number(_dataProcessor->getDAQCount(), 10));
           
            // setup the DAQ socket for listening 
            socketDAQ = new QUdpSocket(this);
            bool bounding = socketDAQ->bind(DAQPort);
            if(bounding){
                qDebug()<<"************  DAQ PORT READY ******************";
                connect( socketDAQ, SIGNAL(readyRead()), this, SLOT(dataDAQPending()) );
            }
            else{
                qDebug()<<"[MainWindow::triggerHandler]    DAQ socket unable to bind! Disconnecting and closing.";
                socketDAQ->disconnectFromHost();
                socketDAQ->close();
            }

            ui->runStatusField->setText("Run:"+ui->runNumber->text()+" ongoing");
            ui->runStatusField->setStyleSheet("background-color:green");
            ui->checkTriggers->setEnabled(0);
            ui->stopTriggerCnt->setEnabled(1);

            // TODO : ADD IN CALIBRATION LOOPS (ask George/Andree about this)

            calibrationStop = false;
            if(ui->calibration->isChecked()){
                if(ui->autoCalib->isChecked())
                    startCalibration();
                else if(ui->manCalib->isChecked())
                    qDebug()<<"Manual Calibration";
            }

        } else { // if not writing data, skip setting up the trees
            socketDAQ = new QUdpSocket(this);
            bool bounding = socketDAQ->bind(DAQPort);
            if(bounding)
                connect(socketDAQ, SIGNAL(readyRead()), this, SLOT(dataDAQPending()) );
            ui->checkTriggers->setEnabled(0);
            ui->stopTriggerCnt->setEnabled(1);
        }


       /*     /// [below] original ----------------------------------- ///
            QString runNumber;
            QTextStream daqInput(&fileDaq);
            QString rootFile = ui->runDirectoryField->text()+"/run_%4d.root";
            const char* rootFileChar = rootFile.toStdString().c_str();
            fileDaqRoot = new TFile(Form(rootFileChar,ui->runNumber->text().toInt(&ok,10)),"UPDATE");
            run_properties = new TTree("run_properties","run_properties");
            vmm2 = new TTree("vmm2","vmm2");
            //            vmm2Calibration = new TTree("vmm2Calibration","vmm2Calibration");

            int runVariable, tacSlopVariable, peakTimeVariable,dacCountsVariable,pulserCountsVariable,calibrationRunVariable,angleVariable;
            double gainVariable;
            QString tempString;
            TBranch *run = run_properties->Branch("runNumber", &runVariable);
            TBranch *gain = run_properties->Branch("gain", &gainVariable);
            TBranch *tacSlope = run_properties->Branch("tacSlope", &tacSlopVariable);
            TBranch *peakTime = run_properties->Branch("peakTime", &peakTimeVariable);
            TBranch *dacCounts = run_properties->Branch("dacCounts", &dacCountsVariable);
            TBranch *pulserCounts = run_properties->Branch("pulserCounts", &pulserCountsVariable);
            TBranch *angle = run_properties->Branch("angle", &angleVariable);

            TBranch *calibrationRun = run_properties->Branch("calibrationRun", &calibrationRunVariable);
            if(ui->calibration->isChecked()) calibrationRunVariable=1;
            else calibrationRunVariable=0;

            runVariable = ui->runNumber->value();
            tempString = ui->sg->currentText().left(3);
            gainVariable = tempString.toDouble();
            tempString = ui->stc->currentText().left(4);
            tacSlopVariable = tempString.toInt();
            tempString = ui->st->currentText().left(3);
            peakTimeVariable = tempString.toInt();
            dacCountsVariable = ui->sdt->value();
            pulserCountsVariable = ui->sdp_2->value();
            angleVariable = ui->angle->value();

            run->Fill();
            gain->Fill();
            tacSlope->Fill();
            peakTime->Fill();
            dacCounts->Fill();
            pulserCounts->Fill();
            calibrationRun->Fill();
            angle->Fill();

            run_properties->Fill();
            run_properties->Write("", TObject::kOverwrite);
            delete run_properties;

            tdoVariable.clear();pdoVariable.clear();chIdVariable.clear();flgVariable.clear();bcidVariable.clear();thrVariable.clear();
            boardVariable.clear();fecVariable.clear();chipIdVariable.clear();GrayDecodedVariable.clear();evSizeVariable.clear();
            triggerTimestampVariable.clear();triggerCounterVariable.clear();
            currentEvent =0;
            //Event Identifier
            eventNumberBranch = vmm2->Branch("event",&eventNumberVariable);
            eventNumberCounterBranch = vmm2->Branch("eventCnt",&eventNumberCounterVariable);
            eventNumberFAFABranch = vmm2->Branch("eventFAFA", &eventNumberFAFAVariable);
            //Calibration
            if(ui->calibration->isChecked()){
                pulserBranchForCalibration = vmm2->Branch("pulser",&pulserVariableForCalibration);
                gainBranchForCalibration = vmm2->Branch("gain",&gainVariableForCalibration);
                intTimeBranchForCalibration = vmm2->Branch("intTime",&intTimeVariableForCalibration);
                thrBranchForCalibration = vmm2->Branch("thresholdSet",&thrVariableForCalibration);
                neighborBranchForCalibration = vmm2->Branch("neighbor","std::vector<vector<int> >", &neighborVariableForCalibration);
                calibRunBranch = vmm2->Branch("calibrationRun",&calibRunVariable);
            }
            //Mapping & counters
            triggerTimestampBranch = vmm2->Branch("triggerTimestamp", "std::vector<int>", &triggerTimestampVariable);
            triggerCounterBranch = vmm2->Branch("triggerCounter", "std::vector<int>", &triggerCounterVariable);
            fecBranch = vmm2->Branch("fec", "std::vector<int>", &fecVariable);
            boardBranch = vmm2->Branch("board", "std::vector<int>", &boardVariable);
            chipIdBranch = vmm2->Branch("chip", "std::vector<int>", &chipIdVariable);
            evSizeBranch = vmm2->Branch("eventSize", "std::vector<int>", &evSizeVariable);
            //Data
            chIdBranch = vmm2->Branch("channel","std::vector<vector<int> >", &chIdVariable);
            flgBranch = vmm2->Branch("flag","std::vector<vector<int> >", &flgVariable);
            thrBranch = vmm2->Branch("threshold","std::vector<vector<int> >", &thrVariable);
            pdoBranch = vmm2->Branch("pdo","std::vector<vector<int> >", &pdoVariable);
            tdoBranch = vmm2->Branch("tdo","std::vector<vector<int> >", &tdoVariable);
            bcidBranch = vmm2->Branch("bcid","std::vector<vector<int> >", &bcidVariable);
            GrayDecodedBranch = vmm2->Branch("grayDecoded","std::vector<vector<int> >", &GrayDecodedVariable);

            dataNotEmpty=0;




            QString tempStringCnt;
            daqCnt=0;
            daqCntTemp=0;
            ui->triggerCntLabel->setText(tempStringCnt.number(daqCnt,10));

            socketDAQ = new QUdpSocket(this);
            bool bounding = socketDAQ->bind(DAQPort);
            if(bounding){
                qDebug()<<"************  DAQ PORT READY ******************";
                connect( socketDAQ, SIGNAL(readyRead()), this, SLOT(dataDAQPending()) );

            }
            else{
                qDebug()<<"DAQ Port Problem!";
                socketDAQ->disconnectFromHost();
                socketDAQ->close();
            }

            ui->runStatusField->setText("Run:"+ui->runNumber->text()+" ongoing");
            ui->runStatusField->setStyleSheet("background-color:green");
            ui->checkTriggers->setEnabled(0);
            ui->stopTriggerCnt->setEnabled(1);
            calibrationStop =0;
            if(ui->calibration->isChecked()){
             if(ui->autoCalib->isChecked())
                startCalibration();
             else if(ui->manCalib->isChecked())
                 qDebug()<<"Manual Calibration";
            }

        }else{
            socketDAQ = new QUdpSocket(this);
            bool bounding = socketDAQ->bind(DAQPort);
            if(bounding)
                connect(socketDAQ, SIGNAL(readyRead()), this, SLOT(dataDAQPending()) );
            ui->checkTriggers->setEnabled(0);
            ui->stopTriggerCnt->setEnabled(1);
        }
        */
    }
    if(ui->clearTriggerCnt == QObject::sender()){
        _dataProcessor->resetDAQCount();
        QString cnt_;
        ui->triggerCntLabel->setText(cnt_.number(_dataProcessor->getDAQCount(), 10));
        //daqCnt=0;
        //daqCntTemp=0;
        //QString tempString;
        //ui->triggerCntLabel->setText(tempString.number(daqCnt,10));
    }
    if(ui->stopTriggerCnt == QObject::sender()){
        calibrationStop = true;
        socketDAQ->close();
        ui->runStatusField->setText("Run:"+ui->runNumber->text()+" finished");
        ui->runStatusField->setStyleSheet("background-color:lightGray");
        if(ui->writeData->isChecked()) {
            _dataProcessor->writeAndCloseDataFile();
        }
   //     if(ui->writeData->isChecked()){
   //         vmm2->Write("", TObject::kOverwrite);
   //         fileDaqRoot->Write();
   //         fileDaqRoot->Close();
   //     }
        ui->runNumber->setValue(ui->runNumber->value()+1);
        ui->checkTriggers->setEnabled(1);
        ui->stopTriggerCnt->setEnabled(0);
        if(ui->calibration->isChecked())
            threadDi->exit(0);
//        ui->calibration->setChecked(0);
    }
}
//_________________________________________________________________________________________________
void MainWindow::writeFPGAFile(QString inputPacket){
    QTextStream daqInputfpga(&fileDaq_fpga);
    if(daqFile_fpga==true){
        daqInputfpga<<inputPacket<<"\n";
    }
}

//_________________________________________________________________________________________________
void MainWindow::triggerHandler_fpga(){
    if(!ui->calibration->isChecked()){
        if(ui->checkTriggers == QObject::sender()){
            QString runNumber;

            vmm1Trigger = new TTree("vmm1Trigger","vmm1Trigger");
            triggerEventBranch = vmm1Trigger->Branch("event",&triggerEventVariable);
            triggerTimeBranch = vmm1Trigger->Branch("time",&triggerTimeVariable);
            triggerChannelBranch = vmm1Trigger->Branch("channel",&triggerChannelVariable);
            triggerBoardBranch = vmm1Trigger->Branch("board",&triggerBoardVariable);

            daqCnt_fpga=0;
            QString tempString;
            ui->triggerCntLabel_fpga->setText(tempString.number(daqCnt_fpga,10));
            daqFile_fpga = 1;
            if (fileDaq_fpga.exists()){
                QMessageBox::information(this, "ERROR",
                                         "Run File exists in "+runDirectory.path()+
                                         "Please increase the run Number");
            }else{
                fileDaq_fpga.open(QIODevice::ReadWrite);
                QTextStream daqInput_fpga(&fileDaq_fpga);
                daqInput_fpga<<"##################################################################"<<"\n";
                daqInput_fpga<<"#                  Trigger run: "+runNumber.number(ui->runNumber->value(),10)+"                             #"<<"\n";
                daqInput_fpga<<"#    Gain="+ui->sg->currentText()+", Peak Time:"+ui->st->currentText()+", Neighbor Trigger: "+ui->sng->currentText()+"       #"<<"\n";
                daqInput_fpga<<"#    TAC Slope="+ui->stc->currentText()+", DAC Counts:"+ui->sdt->text()+", in mV:"+ui->dacmvLabel->text()+", Time window:"+ui->fa_maxCnt->text()+"(counts in 5ns/count)           #"<<"\n";
                daqInput_fpga<<"#    User Comments="+ui->userComments->text()+"           #"<<"\n";
                daqInput_fpga<<"##################################################################"<<"\n";
            }
        }
        if(ui->clearTriggerCnt == QObject::sender()){
            QString tempString;
            daqCnt_fpga=0;
            ui->triggerCntLabel_fpga->setText(tempString.number(daqCnt_fpga,10));
        }
        if(ui->stopTriggerCnt == QObject::sender()){
            daqFile_fpga = 0;
            Sleeper::msleep(10);
            fileDaq_fpga.close();
        }
    }
}
//_________________________________________________________________________________________________
void MainWindow::dataDAQPending()
{
    //bool ok;
    //bool debug=0;
    //QString tempString;
    QByteArray bufferNow;
    while(socketDAQ->hasPendingDatagrams())
    {
        bufferNow.clear();
        bufferNow.resize(socketDAQ->pendingDatagramSize());
        socketDAQ->readDatagram(bufferNow.data(), bufferNow.size());
        _dataProcessor->parseData(bufferNow);
/*
        QString frmCounter = bufferDAQ.mid(0,4).toHex();

        if(debug){
            qDebug()<< "1)*************** DATA COMING WITH SIZE:  "<<bufferDAQ.size();
            qDebug()<<bufferDAQ.toHex() ;
            qDebug()<<"Frame Counter: "<<frmCounter;
        }
        if(bufferDAQ.size()==12){
            qDebug()<<" Empty Event ";
        }
        if(frmCounter!="fafafafa"){// && bufferDAQ.size()>12){
            dataNotEmpty=1;
            QString chip = bufferDAQ.mid(7,1).toHex();
            QString trigCntString = bufferDAQ.mid(8,2).toHex();
            QString trigtimeStampString = bufferDAQ.mid(10,2).toHex();

            if(ui->writeData->isChecked()){
                chipIdVariable.push_back(chip.toInt(&ok,16));
                triggerCounterVariable.push_back(trigCntString.toInt(&ok,16));
                triggerTimestampVariable.push_back(trigtimeStampString.toInt(&ok,16));
                evSizeVariable.push_back(bufferDAQ.size()-12);
            }

            if(debug){
                QString data = bufferDAQ.mid(12,bufferDAQ.size()).toHex();
                QString header = bufferDAQ.mid(4,4).toHex();
                qDebug()<<"Header: "<<header;
                qDebug()<<"Chip: "<<chip.toInt(&ok,16);
                qDebug()<<"Data: "<<data;
            }

            std::vector<int> tempPdo, tempTdo, tempBCID, tempGrayDecoded, tempCh, tempFlg, tempThr, tempNeighbor;
            tempPdo.clear();tempTdo.clear();tempCh.clear();tempBCID.clear();tempGrayDecoded.clear();tempFlg.clear();tempThr.clear(); tempNeighbor.clear();
            for(int i=12;i<bufferDAQ.size();){
                quint32 bytesInt1Temp=reverse32(bufferDAQ.mid(i,4).toHex());

                QString bytesInt1String=bufferDAQ.mid(i,4).toHex();
                quint32 bytesInt1=bytesInt1String.toUInt(&ok,16);
                quint32 bytesInt2=reverse32(bufferDAQ.mid(i+4,4).toHex());

                if(debug){
                    QString bRev, aRev;
                    qDebug()<<"before:  "<<bRev.number(bytesInt1Temp,2);
                    qDebug()<<"after :  "<<aRev.number(bytesInt1,2);
                }

                uint Flag = (bytesInt2 & 0x1);
                tempFlg.push_back(Flag);

                uint Threshold = (bytesInt2 & 0x2)>>1;
                tempThr.push_back(Threshold);

                uint ChannelFromData = (bytesInt2 & 0xfc)>>2;
                if(ui->calibration->isChecked())
                    tempNeighbor.push_back(!VMM1STBool[ChannelFromData]);

                uint Channel = 0;
                if(ui->useMapping->isChecked()){
                    Channel = TMappingMini2(ChannelFromData,chip.toInt(&ok,16));
                }else{
                    Channel = ChannelFromData;
                }
                tempCh.push_back(Channel);

                // ////////////////////////////////////////////////////////////////////////////////////////////////////
                QString DataString;
                DataString="00000000000000000000000000000000";//init
                QString dString = "00000000000000000000000000000000"; //10bit
                dString = dString.number(bytesInt1,2);
                for(int j=0;j<dString.size();j++){
                    QString bitTmp=dString.at(dString.size()-1-j);
                    DataString.replace(j,1, bitTmp);
                }
                if(debug){
                    qDebug()<<"dStri :  "<<dString;
                    qDebug()<<"afte2 :  "<<DataString;
                }

                // ////////////////////////////////////////////////////////////////////////////////////////////////////
                QString Charge_s1 = DataString.left(8);
                QString Charge_s2 = DataString.mid(14,2);
                QString Charge_final;
                Charge_final.append(Charge_s2);
                Charge_final.append(Charge_s1);
                uint Charge = 0;
                if(Charge_final.right(4)=="0000" && ui->ignore16->isChecked())
                    Charge = 1025;
                else
                    Charge = Charge_final.toUInt(&ok,2);

                //uint Charge = (bytesInt1 & 0xffc00000)>>22;
                tempPdo.push_back(Charge);

                QString TAC_s1 = DataString.mid(8,6);
                QString TAC_s2 = DataString.mid(22,2);
                QString TAC_final;
                TAC_final.append(TAC_s2);
                TAC_final.append(TAC_s1);
                uint TAC = TAC_final.toUInt(&ok,2);

                //uint TAC = (bytesInt1 & 0x3fc000)>>14;
                tempTdo.push_back(TAC);


                QString BCID_s1 = DataString.mid(16,6);
                QString BCID_s2 = DataString.mid(26,6);
                QString BCID_final;
                BCID_final.append(BCID_s2);
                BCID_final.append(BCID_s1);
                uint BCID = BCID_final.toUInt(&ok,2);


                //uint BCID = (bytesInt1 & 0x3ffc)>>2;
                tempBCID.push_back(BCID);
                uint grayBin = grayToBinary(BCID);
                tempGrayDecoded.push_back(grayBin);

                if(debug){
                    qDebug()<<"Before flipping: 1: "<<bufferDAQ.mid(i,4).toHex()<<",    2: "<<bufferDAQ.mid(i+4,4).toHex();
                    qDebug()<<"After bytesInt   1: "<<bytesInt1<<",   2: "<<bytesInt2;
                    qDebug()<<"Flag: "<<Flag;
                    qDebug()<<"Threshold: "<<Threshold;
                    qDebug()<<"Channel: "<<Channel;
                    qDebug()<<"Charge: "<<Charge;
                    qDebug()<<"Bits: "<<DataString;
                    qDebug()<<"CH1: "<<Charge_s1;
                    qDebug()<<"CH2: "<<Charge_s2;
                    qDebug()<<"CH final: "<<Charge_final;
                    //qDebug()<<"CH2 final int: "<<Charge_Final;

                    qDebug()<<"TAC: "<<TAC;
                    qDebug()<<"BCID: "<<BCID<<", to gray code: "<<grayBin;
                }
                i=i+8;
            }
            if(debug){
                qDebug()<<"EVENT with size: "<<tempCh.size();
                for(unsigned int i=0;i<tempCh.size();i++){
                    qDebug()<< "i: "<<i<<", Channel: "<< tempCh[i];
                }
            }
            if(ui->writeData->isChecked()){
                flgVariable.push_back(tempFlg);
                thrVariable.push_back(tempThr);
                chIdVariable.push_back(tempCh);
                pdoVariable.push_back(tempPdo);
                tdoVariable.push_back(tempTdo);
                bcidVariable.push_back(tempBCID);
                GrayDecodedVariable.push_back(tempGrayDecoded);
                if(ui->calibration->isChecked())
                    neighborVariableForCalibration.push_back(tempNeighbor);
            }

        }else if(frmCounter=="fafafafa"){// && dataNotEmpty==1){
            dataNotEmpty=0;
            daqCnt++;
            if(daqCnt-daqCntTemp==100){
                ui->triggerCntLabel->setText(tempString.number(daqCnt,10));
                daqCntTemp=daqCnt;
            }
            eventNumberFAFAVariable=daqCnt-1;

            if(ui->calibration->isChecked()){
                calibRunVariable=1;
                pulserVariableForCalibration = ui->sdp_2->value();
                QString tempGainString = ui->sg->currentText().left(3);
                gainVariableForCalibration = tempGainString.toDouble(&ok);
                QString tempIntTime = ui->st->currentText().left(3);
                intTimeVariableForCalibration = tempIntTime.toInt(&ok,10);
                thrVariableForCalibration = ui->sdt->value();
            }
            if(debug){
                qDebug()<<"Writting EVENT with size: "<<chIdVariable.size();
                for(unsigned int i=0;i<chIdVariable.size();i++){
                    for(unsigned int j=0;j<chIdVariable[i].size();j++){
                        qDebug()<< "i: "<<i<< ", j: "<<j<<", Channel: "<< chIdVariable[i][j];
                    }
                }
            }
            if(ui->writeData->isChecked()){
                vmm2->Fill();

                fecVariable.clear();
                boardVariable.clear();
                chipIdVariable.clear();
                evSizeVariable.clear();
                triggerTimestampVariable.clear();
                triggerCounterVariable.clear();
                pdoVariable.clear();
                tdoVariable.clear();
                bcidVariable.clear();
                chIdVariable.clear();
                thrVariable.clear();
                flgVariable.clear();
                GrayDecodedVariable.clear();
                neighborVariableForCalibration.clear();
            }
        }
//    */
    }
}
//_________________________________________________________________________________________________
uint MainWindow:: grayToBinary(uint num)
{
    uint mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1)
    {
        num = num ^ mask;
    }
    return num;
}
//_________________________________________________________________________________________________
void MainWindow::changeDACtoMVs(int value){
		qDebug()<< "[MainWindow::changeDACtoMVs] Unused arg value= " <<value;
    QString tempstring;
    ui->dacmvLabel->setText(tempstring.number((0.6862*ui->sdt->value()+63.478), 'f', 2)+" mV");
}

//_________________________________________________________________________________________________
std::vector<double> MainWindow::bytesTobits(QByteArray bytes){
    bool ok;
    quint32 bytesInt = bytes.toHex().toUInt(&ok,16);
    uint time = (bytesInt & 0x1fff);
    uint amplitude = (bytesInt & 0x3ffe000) >> 13;
    uint channel = (bytesInt & 0xfc000000) >> 26;
    std::vector<double> chInfo;
    chInfo.clear();
    chInfo.push_back(channel);
    chInfo.push_back(time);//*(3.3/16384.));
    chInfo.push_back(amplitude);//*(3.3/16384.));
    return chInfo;
}
//_________________________________________________________________________________________________
std::vector<double> MainWindow::bytesTobitsTrigger(QByteArray bytes){
    bool ok;
    quint32 bytesInt = bytes.toHex().toUInt(&ok,16);
    uint time2 = (bytesInt & 0xff00)>>8;
    uint channel2 = (bytesInt & 0x0000003f);
    uint time1 = (bytesInt & 0xff000000) >> 24;
    uint channel1 = (bytesInt & 0x3f0000) >> 16;
    //qDebug()<<"Trigger Data Came : "<<bytes.toHex()<<", ch1:"<<channel1<<", time1:"<<time1<<", ch2:"<<channel2<<", time2:"<<time2;
    std::vector<double> chInfo;
    chInfo.clear();
    chInfo.push_back(channel1);
    chInfo.push_back(time1);
    chInfo.push_back(channel2);
    chInfo.push_back(time2);
    return chInfo;
}
//_________________________________________________________________________________________________
void MainWindow::initializePlots(){
    T1Profile = new TH1D("T1Profile","T1 Beam Profile",256,0,256);
    T2Profile = new TH1D("T2Profile","T2 Beam Profile",256,0,256);
    T3Profile = new TH1D("T3Profile","T3 Beam Profile",256,0,256);
    T4Profile = new TH1D("T4Profile","T4 Beam Profile",256,0,256);
    T5Profile = new TH1D("T5Profile","T5 Beam Profile",256,0,256);
    T6Profile = new TH1D("T6Profile","T6 Beam Profile",256,0,256);
    T7Profile = new TH1D("T7Profile","T7 Beam Profile",256,0,256);
    T8Profile = new TH1D("T8Profile","T8 Beam Profile",256,0,256);

    T1Times = new TH1D("T1Times","T1 Times",1000,0,5000);
    T2Times = new TH1D("T2Times","T2 Times",1000,0,5000);
    T3Times = new TH1D("T3Times","T3 Times",1000,0,5000);
    T4Times = new TH1D("T4Times","T4 Times",1000,0,5000);
    T5Times = new TH1D("T5Times","T5 Times",1000,0,5000);
    T6Times = new TH1D("T6Times","T6 Times",1000,0,5000);
    T7Times = new TH1D("T7Times","T7 Times",1000,0,5000);
    T8Times = new TH1D("T8Times","T8 Times",1000,0,5000);

    T1Charges = new TH1D("T1Charges","T1 Charges",1000,0,5000);
    T2Charges = new TH1D("T2Charges","T2 Charges",1000,0,5000);
    T3Charges = new TH1D("T3Charges","T3 Charges",1000,0,5000);
    T4Charges = new TH1D("T4Charges","T4 Charges",1000,0,5000);
    T5Charges = new TH1D("T5Charges","T5 Charges",1000,0,5000);
    T6Charges = new TH1D("T6Charges","T6 Charges",1000,0,5000);
    T7Charges = new TH1D("T7Charges","T7 Charges",1000,0,5000);
    T8Charges = new TH1D("T8Charges","T8 Charges",1000,0,5000);

    BoardsHits = new TH1D("BoardsHits","Boards Hits",35,0,35);
    EventNumber = new TH1D("EventNumber","Tracking Event Number",500000,0,500000);
    EventNumberTrigger = new TH1D("EventNumberTrigger","Trigger Event Number",500000,0,500000);
}
//_________________________________________________________________________________________________
void MainWindow::updateCanvas(int){

    //    Quality->GetCanvas()->Update();
    //    Quality->Refresh();
    //    EventNumber->GetXaxis()->SetRangeUser(EventNumber->FindFirstBinAbove(0),EventNumber->FindLastBinAbove(0));
    //    EventNumberTrigger->GetXaxis()->SetRangeUser(EventNumberTrigger->FindFirstBinAbove(0),EventNumberTrigger->FindLastBinAbove(0));

    //    times->GetCanvas()->Update();
    //    times->Refresh();

    //    charges->GetCanvas()->Update();
    //    charges->Refresh();

    //    profiles->GetCanvas()->Update();
    //    profiles->Refresh();
}

//_________________________________________________________________________________________________
void MainWindow::openMonitoring(int){
    //    if(ui->monitoring->isChecked()){
    //        threadFill->start();
    //        monitoringTabWidget = new QTabWidget();
    //        profiles = new TQtWidget();
    //        times = new TQtWidget();
    //        charges = new TQtWidget();
    //        Quality = new TQtWidget();
    //        monitoringTabWidget->addTab(profiles,"Beam Profiles");
    //        monitoringTabWidget->addTab(times,"Timing Distributions");
    //        monitoringTabWidget->addTab(charges,"Charge Distributions");
    //        monitoringTabWidget->addTab(Quality,"Quality Plots");

    //        T1Profile->Reset(); T1Times->Reset(); T1Charges->Reset();
    //        T2Profile->Reset(); T2Times->Reset(); T2Charges->Reset();
    //        T3Profile->Reset(); T3Times->Reset(); T3Charges->Reset();
    //        T4Profile->Reset(); T4Times->Reset(); T4Charges->Reset();
    //        T5Profile->Reset(); T5Times->Reset(); T5Charges->Reset();
    //        T6Profile->Reset(); T6Times->Reset(); T6Charges->Reset();
    //        T7Profile->Reset(); T7Times->Reset(); T7Charges->Reset();
    //        T8Profile->Reset(); T8Times->Reset(); T8Charges->Reset();
    //        BoardsHits->Reset(); EventNumber->Reset();EventNumberTrigger->Reset();

    //        profiles->GetCanvas()->Divide(2,4);
    //        profiles->GetCanvas()->cd(1);
    //        T1Profile->Draw();
    //        profiles->GetCanvas()->cd(2);
    //        T2Profile->Draw();
    //        profiles->GetCanvas()->cd(3);
    //        T3Profile->Draw();
    //        profiles->GetCanvas()->cd(4);
    //        T4Profile->Draw();
    //        profiles->GetCanvas()->cd(5);
    //        T5Profile->Draw();
    //        profiles->GetCanvas()->cd(6);
    //        T6Profile->Draw();
    //        profiles->GetCanvas()->cd(7);
    //        T7Profile->Draw();
    //        profiles->GetCanvas()->cd(8);
    //        T8Profile->Draw();

    //        times->GetCanvas()->Divide(2,4);
    //        times->GetCanvas()->cd(1);
    //        T1Times->Draw();
    //        times->GetCanvas()->cd(2);
    //        T2Times->Draw();
    //        times->GetCanvas()->cd(3);
    //        T3Times->Draw();
    //        times->GetCanvas()->cd(4);
    //        T4Times->Draw();
    //        times->GetCanvas()->cd(5);
    //        T5Times->Draw();
    //        times->GetCanvas()->cd(6);
    //        T6Times->Draw();
    //        times->GetCanvas()->cd(7);
    //        T7Times->Draw();
    //        times->GetCanvas()->cd(8);
    //        T8Times->Draw();

    //        charges->GetCanvas()->Divide(2,4);
    //        charges->GetCanvas()->cd(1);
    //        T1Charges->Draw();
    //        charges->GetCanvas()->cd(2);
    //        T2Charges->Draw();
    //        charges->GetCanvas()->cd(3);
    //        T3Charges->Draw();
    //        charges->GetCanvas()->cd(4);
    //        T4Charges->Draw();
    //        charges->GetCanvas()->cd(5);
    //        T5Charges->Draw();
    //        charges->GetCanvas()->cd(6);
    //        T6Charges->Draw();
    //        charges->GetCanvas()->cd(7);
    //        T7Charges->Draw();
    //        charges->GetCanvas()->cd(8);
    //        T8Charges->Draw();

    //        Quality->GetCanvas()->Divide(2,2);
    //        Quality->GetCanvas()->cd(1);
    //        BoardsHits->Draw();
    //        Quality->GetCanvas()->cd(2);
    //        EventNumber->Draw();
    //        Quality->GetCanvas()->cd(4);
    //        EventNumberTrigger->Draw();
    //        monitoringTabWidget->show();
    //    }else{
    //        threadFill->exit(0);
    //        monitoringTabWidget->close();
    //        if(threadFill->isRunning()){
    //            qDebug()<<"Den To Ekleise o aisthimatias";
    //            threadFill->quit();
    //            threadFill->exit(1);
    //        }
    //    }
}

void MainWindow::selectWorkingDirectory(){
    QFileDialog dirDialog;
    dirDialog.setFileMode(QFileDialog::DirectoryOnly);
    dirDialog.exec();
    runDirectory = dirDialog.directory();
    QString temp(runDirectory.path() );
    ui->runDirectoryField->setText(temp);
    QStringList filters;
    filters<<"run_*.root";
    runDirectory.setNameFilters(filters);
    QFileInfoList listOfFiles = runDirectory.entryInfoList();
    if(listOfFiles.size()!=0){
        for(int i=0;i<listOfFiles.size();i++){
            QFileInfo fileInfo = listOfFiles.at(i);
            QString temp;
            if("run_"+temp.number(ui->runNumber->value())+".root"==fileInfo.fileName()){
                ui->runNumber->setValue(ui->runNumber->value()+1);
            }
        }
    }
}
//------------------------- Load Thresholds -------------------------------------------
void MainWindow::LoadThresholds(int state){
		qDebug()<< "[MainWindow::LoadThresholds] Unused arg state= " <<state;
    if(ui->loadThrCB->isChecked()){
        if(ui->abovePed->value()==5){
            // 5mV
            boardThreshold[1]=204;
            boardThreshold[2]=179;
            boardThreshold[3]=183;
            boardThreshold[4]=192;
            boardThreshold[5]=189;
            boardThreshold[6]=176;
            boardThreshold[7]=174;
            boardThreshold[8]=187;
            boardThreshold[9]=155;
            boardThreshold[10]=166;
            boardThreshold[11]=167;
            boardThreshold[12]=173;
            boardThreshold[13]=187;
            boardThreshold[14]=171;
            boardThreshold[15]=176;
            boardThreshold[16]=147;
        }else if(ui->abovePed->value()==10){
            // 10 mV
            boardThreshold[1]=212;
            boardThreshold[2]=187;
            boardThreshold[3]=190;
            boardThreshold[4]=200;
            boardThreshold[5]=196;
            boardThreshold[6]=183;
            boardThreshold[7]=181;
            boardThreshold[8]=194;
            boardThreshold[9]=162;
            boardThreshold[10]=174;
            boardThreshold[11]=174;
            boardThreshold[12]=180;
            boardThreshold[13]=196;
            boardThreshold[14]=178;
            boardThreshold[15]=183;
            boardThreshold[16]=155;
        }else if(ui->abovePed->value()==15){
            // 15mV
            boardThreshold[1]=221;
            boardThreshold[2]=194;
            boardThreshold[3]=198;
            boardThreshold[4]=207;
            boardThreshold[5]=203;
            boardThreshold[6]=191;
            boardThreshold[7]=189;
            boardThreshold[8]=201;
            boardThreshold[9]=170;
            boardThreshold[10]=181;
            boardThreshold[11]=181;
            boardThreshold[12]=187;
            boardThreshold[13]=204;
            boardThreshold[14]=185;
            boardThreshold[15]=191;
            boardThreshold[16]=162;
        }else if(ui->abovePed->value()==20){
            // 20mV
            boardThreshold[1]=230;
            boardThreshold[2]=202;
            boardThreshold[3]=205;
            boardThreshold[4]=215;
            boardThreshold[5]=210;
            boardThreshold[6]=198;
            boardThreshold[7]=196;
            boardThreshold[8]=208;
            boardThreshold[9]=177;
            boardThreshold[10]=188;
            boardThreshold[11]=189;
            boardThreshold[12]=195;
            boardThreshold[13]=212;
            boardThreshold[14]=192;
            boardThreshold[15]=198;
            boardThreshold[16]=170;
        }else if(ui->abovePed->value()==25){
            // 25mV
            boardThreshold[1]=237;
            boardThreshold[2]=209;
            boardThreshold[3]=213;
            boardThreshold[4]=223;
            boardThreshold[5]=217;
            boardThreshold[6]=206;
            boardThreshold[7]=204;
            boardThreshold[8]=216;
            boardThreshold[9]=184;
            boardThreshold[10]=195;
            boardThreshold[11]=196;
            boardThreshold[12]=202;
            boardThreshold[13]=220;
            boardThreshold[14]=199;
            boardThreshold[15]=205;
            boardThreshold[16]=177;
        }
    }else{
        for(int i=0;i<=19;i++){
            boardThreshold[i]=220;
        }
        ui->sdt->setValue(220);
    }
}
//------------------------- Calibration Procedure -------------------------------------------
void MainWindow::startCalibration(){
    void delay();
    bool ok;
    int eventsMultiplier=0;
    if(ui->calibration->isChecked()){
        qDebug() << " ----------------------------- ";
        qDebug() << "    * Staring Calibration *    ";
        qDebug() << " ----------------------------- ";
        for (int g=ui->sg_cal_min->currentIndex();g<=ui->sg_cal_max->currentIndex();g++){
            ui->sg->setCurrentIndex(g);
            //qDebug()<<"Gain:"<<ui->sg->currentText();

            for(int thr = ui->sdt_cal_min->value(); thr<=ui->sdt_cal_max->value(); thr+=ui->sdt_cal_step->value()){
                ui->sdt->setValue(thr);
                //qDebug()<<"Threshold:"<<ui->sdt->value();

                for(int p=ui->sdp_cal_min->value();p<=ui->sdp_cal_max->value();p+=ui->sdp_cal_step->value()){
                    ui->sdp_2->setValue(p);
                    //qDebug()<<"Pulser Value:"<<ui->sdp_2->value();

                    qDebug() << " ---------------------------- ";
                    qDebug() << "       Calibration Loop       ";
                    qDebug() << "  > Gain         : " << ui->sg->currentText();
                    qDebug() << "  > Threshold    : " << ui->sdt->value();
                    qDebug() << "  > Pulser Value : " << ui->sdp_2->value();
                    qDebug() << " ---------------------------- ";

                    ////////////////////////////////////////////
                    // send dataprocessor the calib info
                    ////////////////////////////////////////////
                    int _calib_pulse    = ui->sdp_2->value();
                    double _calib_gain  = ui->sg->currentText().left(3).toDouble(&ok);
                    int _calib_peakTime = ui->st->currentText().left(3).toInt(&ok,10); 
                    int _calib_thresh   = ui->sdt->value();
                    _dataProcessor->updateCalibInfo(_calib_pulse,
                                                    _calib_gain,
                                                    _calib_peakTime,
                                                    _calib_thresh);
                    ////////////////////////////////////////////

                    for(int ch=ui->chRange_calib_min->currentIndex();ch<=ui->chRange_calib_max->currentIndex();ch++){
                        delay();

                        for(int chT=0;chT<64;chT++){
                            VMM1SM[chT]->setStyleSheet("background-color: lightGray");
                            VMM1SMBool[chT]=0;
                            VMM1SMBoolAll=0;
                            VMM1SMBoolAll2=0;
                            VMM1ST[chT]->setStyleSheet("background-color: lightGray");
                            VMM1STBool[chT]=0;
                            VMM1STBoolAll=0;
                            VMM1STBoolAll2=0;
                        }
                        if(ui->masking_calib->isChecked()){
                            emit SMLabel->click();
                            emit SMLabel2->click();
                            VMM1SM[ch]->click();
                            if(ch>0) {
                                VMM1SM[ch-1]->click();
                            }
                            if(ch<63) {
                                VMM1SM[ch+1]->click();
                            }
                        }
                        qDebug()<<"  > Calibrating Channel : " << ch;
                        VMM1ST[ch]->click();

                        qDebug()<<"  > Set Mask";
                        emit ui->setMask->click();
                        delay();
                        qDebug()<<"  > Set headers";
                        emit ui->setEvbld->click();
                        delay();

                        qDebug()<<"  > Initialize Electronics";
                        emit ui->SendConfiguration->click();
                        delay();
                        emit ui->SendConfiguration->click();
                        delay();

                        qDebug()<<"  > Trigger Constants set";
                        emit ui->setTrgAcqConst->click();
                        delay();
                        qDebug()<<"  > Setting Pulser DAC";
                        emit ui->trgPulser->click();

                        int initialEvents = ui->triggerCntLabel->text().toUInt(&ok,10);

                        qDebug()<<"  > Enabling DAC";
                        emit ui->onACQ->click();

                        eventsMultiplier++;
                        unsigned int eventsForCalibration = ui->evCalib->text().toUInt(&ok,10);

                        qDebug()<<"  > Starting Pulser with number of Events: "<<eventsForCalibration;
                        delay();


                        if(ui->stopTriggerCnt == QObject::sender())
                            break;

                        while(ui->triggerCntLabel->text().toUInt(&ok,10)-initialEvents<=eventsForCalibration){
                            qDebug()<<" * Waiting for Event Collecting * ";
                            delay();
                            delay();
                            delay();
                            delay();
                            delay();
                            if(!ui->stopTriggerCnt->isEnabled())
                                break;
                        }

                        emit ui->offACQ->click();
                        delay();
                        delay();
                        delay();

                        if(ui->stopTriggerCnt == QObject::sender())
                            break;

                        qDebug() << " ---------------------------- ";

                    }//loop over channels
                }// pulser
            }// thresholds
        }//loop over gains
    }// calib check
    qDebug() << " ---------------------------- ";
    qDebug() << "   * Calibration Finished *   ";
    qDebug() << " ---------------------------- ";
    emit ui->stopTriggerCnt->click();
}

//--------------------------------------------------------------------
void delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(200);//.addSecs(1);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//--------------------------------------------------------------------
void delayMs()
{
    QTime dieTime= QTime::currentTime().addMSecs(10);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//--------------------------------------------------------------------
void MainWindow::loadCalibrationConstants(int){
    bool ok;
    if(ui->loadCalibDataCB->isChecked()){
        QFile file(":/configs/boardCalibConstants_qt.txt") ;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
            QString line = in.readLine();
            QVector<int> temp;
            QStringList list = line.split(",");
            for(int i=1;i<18;i++){
                temp.clear();
                for(int j=0;j<64;j++){
                    temp.push_back(list.at((j*17)+i).toInt(&ok,10));
                }
                calibConstants.push_back(temp);
            }
        }
        //        for(int i=0;i<calibConstants.size();i++){
        //            for(int j=0;j<calibConstants[i].size();j++){
        //                qDebug()<<"board:"<<i<<", channel:"<<j<<", trim:"<<calibConstants[i][j];
        //            }
        //        }
        qDebug()<<"Calibration File Loaded";
    }else{
        qDebug()<<"Reset Trims Done";
        calibConstants.clear();
        emit SDLabel->setCurrentIndex(0);
        emit SDLabel2->setCurrentIndex(0);
        for(int i=0;i<64;i++) emit VMM1SDVoltage[i]->setCurrentIndex(0);
    }
}

int MainWindow::TMapping(int PanasonicPin, int ChipNumber){
    if(ChipNumber==0 || ChipNumber==14)
        PanasonicPin = PanasonicPin+2;
    else if (ChipNumber==1 || ChipNumber==15)
        PanasonicPin = PanasonicPin+2+64;
    int strip=0;
    if(PanasonicPin==2) strip=33;
    if(PanasonicPin==3) strip=32;
    if(PanasonicPin==4) strip=34;
    if(PanasonicPin==5) strip=31;
    if(PanasonicPin==6) strip=35;
    if(PanasonicPin==7) strip=30;
    if(PanasonicPin==8) strip=36;
    if(PanasonicPin==9) strip=29;
    if(PanasonicPin==10) strip=37;
    if(PanasonicPin==11) strip=28;
    if(PanasonicPin==12) strip=38;
    if(PanasonicPin==13) strip=27;
    if(PanasonicPin==14) strip=39;
    if(PanasonicPin==15) strip=26;
    if(PanasonicPin==16) strip=40;
    if(PanasonicPin==17) strip=25;
    if(PanasonicPin==18) strip=41;
    if(PanasonicPin==19) strip=24;
    if(PanasonicPin==20) strip=42;
    if(PanasonicPin==21) strip=23;
    if(PanasonicPin==22) strip=43;
    if(PanasonicPin==23) strip=22;
    if(PanasonicPin==24) strip=44;
    if(PanasonicPin==25) strip=21;
    if(PanasonicPin==26) strip=45;
    if(PanasonicPin==27) strip=20;
    if(PanasonicPin==28) strip=46;
    if(PanasonicPin==29) strip=19;
    if(PanasonicPin==30) strip=47;
    if(PanasonicPin==31) strip=18;
    if(PanasonicPin==32) strip=48;
    if(PanasonicPin==33) strip=17;
    if(PanasonicPin==34) strip=49;
    if(PanasonicPin==35) strip=16;
    if(PanasonicPin==36) strip=50;
    if(PanasonicPin==37) strip=15;
    if(PanasonicPin==38) strip=51;
    if(PanasonicPin==39) strip=14;
    if(PanasonicPin==40) strip=52;
    if(PanasonicPin==41) strip=13;
    if(PanasonicPin==42) strip=53;
    if(PanasonicPin==43) strip=12;
    if(PanasonicPin==44) strip=54;
    if(PanasonicPin==45) strip=11;
    if(PanasonicPin==46) strip=55;
    if(PanasonicPin==47) strip=10;
    if(PanasonicPin==48) strip=56;
    if(PanasonicPin==49) strip=9;
    if(PanasonicPin==50) strip=57;
    if(PanasonicPin==51) strip=8;
    if(PanasonicPin==52) strip=58;
    if(PanasonicPin==53) strip=7;
    if(PanasonicPin==54) strip=59;
    if(PanasonicPin==55) strip=6;
    if(PanasonicPin==56) strip=60;
    if(PanasonicPin==57) strip=5;
    if(PanasonicPin==58) strip=61;
    if(PanasonicPin==59) strip=4;
    if(PanasonicPin==60) strip=62;
    if(PanasonicPin==61) strip=3;
    if(PanasonicPin==62) strip=63;
    if(PanasonicPin==63) strip=2;
    if(PanasonicPin==64) strip=64;
    if(PanasonicPin==65) strip=1;
    if(PanasonicPin==66) strip=65;
    if(PanasonicPin==67) strip=128;
    if(PanasonicPin==68) strip=66;
    if(PanasonicPin==69) strip=127;
    if(PanasonicPin==70) strip=67;
    if(PanasonicPin==71) strip=126;
    if(PanasonicPin==72) strip=68;
    if(PanasonicPin==73) strip=125;
    if(PanasonicPin==74) strip=69;
    if(PanasonicPin==75) strip=124;
    if(PanasonicPin==76) strip=70;
    if(PanasonicPin==77) strip=123;
    if(PanasonicPin==78) strip=71;
    if(PanasonicPin==79) strip=122;
    if(PanasonicPin==80) strip=72;
    if(PanasonicPin==81) strip=121;
    if(PanasonicPin==82) strip=73;
    if(PanasonicPin==83) strip=120;
    if(PanasonicPin==84) strip=74;
    if(PanasonicPin==85) strip=119;
    if(PanasonicPin==86) strip=75;
    if(PanasonicPin==87) strip=118;
    if(PanasonicPin==88) strip=76;
    if(PanasonicPin==89) strip=117;
    if(PanasonicPin==90) strip=77;
    if(PanasonicPin==91) strip=116;
    if(PanasonicPin==92) strip=78;
    if(PanasonicPin==93) strip=115;
    if(PanasonicPin==94) strip=79;
    if(PanasonicPin==95) strip=114;
    if(PanasonicPin==96) strip=80;
    if(PanasonicPin==97) strip=113;
    if(PanasonicPin==98) strip=81;
    if(PanasonicPin==99) strip=112;
    if(PanasonicPin==100) strip=82;
    if(PanasonicPin==101) strip=111;
    if(PanasonicPin==102) strip=83;
    if(PanasonicPin==103) strip=110;
    if(PanasonicPin==104) strip=84;
    if(PanasonicPin==105) strip=109;
    if(PanasonicPin==106) strip=85;
    if(PanasonicPin==107) strip=108;
    if(PanasonicPin==108) strip=86;
    if(PanasonicPin==109) strip=107;
    if(PanasonicPin==110) strip=87;
    if(PanasonicPin==111) strip=106;
    if(PanasonicPin==112) strip=88;
    if(PanasonicPin==113) strip=105;
    if(PanasonicPin==114) strip=89;
    if(PanasonicPin==115) strip=104;
    if(PanasonicPin==116) strip=90;
    if(PanasonicPin==117) strip=103;
    if(PanasonicPin==118) strip=91;
    if(PanasonicPin==119) strip=102;
    if(PanasonicPin==120) strip=92;
    if(PanasonicPin==121) strip=101;
    if(PanasonicPin==122) strip=93;
    if(PanasonicPin==123) strip=100;
    if(PanasonicPin==124) strip=94;
    if(PanasonicPin==125) strip=99;
    if(PanasonicPin==126) strip=95;
    if(PanasonicPin==127) strip=98;
    if(PanasonicPin==128) strip=96;
    if(PanasonicPin==129) strip=97;


    return strip;
}

int MainWindow::TMappingMini2(int VMMChannel, int ChipNumber){
    int strip=0;
    if(ChipNumber==0 || ChipNumber==2 || ChipNumber==4 || ChipNumber==6 || ChipNumber==8 || ChipNumber==10 || ChipNumber==12 || ChipNumber==14){
        if(VMMChannel==0)  strip=32;
        if(VMMChannel==1)  strip=33;
        if(VMMChannel==2)  strip=31;
        if(VMMChannel==3)  strip=34;
        if(VMMChannel==4)  strip=30;
        if(VMMChannel==5)  strip=35;
        if(VMMChannel==6)  strip=29;
        if(VMMChannel==7)  strip=36;
        if(VMMChannel==8)  strip=28;
        if(VMMChannel==9)  strip=37;
        if(VMMChannel==10) strip=27;
        if(VMMChannel==11) strip=38;
        if(VMMChannel==12) strip=26;
        if(VMMChannel==13) strip=39;
        if(VMMChannel==14) strip=25;
        if(VMMChannel==15) strip=40;
        if(VMMChannel==16) strip=24;
        if(VMMChannel==17) strip=41;
        if(VMMChannel==18) strip=23;
        if(VMMChannel==19) strip=42;
        if(VMMChannel==20) strip=22;
        if(VMMChannel==21) strip=43;
        if(VMMChannel==22) strip=21;
        if(VMMChannel==23) strip=44;
        if(VMMChannel==24) strip=20;
        if(VMMChannel==25) strip=45;
        if(VMMChannel==26) strip=19;
        if(VMMChannel==27) strip=46;
        if(VMMChannel==28) strip=18;
        if(VMMChannel==29) strip=47;
        if(VMMChannel==30) strip=17;
        if(VMMChannel==31) strip=48;
        if(VMMChannel==32) strip=16;
        if(VMMChannel==33) strip=49;
        if(VMMChannel==34) strip=15;
        if(VMMChannel==35) strip=50;
        if(VMMChannel==36) strip=14;
        if(VMMChannel==37) strip=51;
        if(VMMChannel==38) strip=13;
        if(VMMChannel==39) strip=52;
        if(VMMChannel==40) strip=12;
        if(VMMChannel==41) strip=53;
        if(VMMChannel==42) strip=11;
        if(VMMChannel==43) strip=54;
        if(VMMChannel==44) strip=10;
        if(VMMChannel==45) strip=55;
        if(VMMChannel==46) strip=9;
        if(VMMChannel==47) strip=56;
        if(VMMChannel==48) strip=8;
        if(VMMChannel==49) strip=57;
        if(VMMChannel==50) strip=7;
        if(VMMChannel==51) strip=58;
        if(VMMChannel==52) strip=6;
        if(VMMChannel==53) strip=59;
        if(VMMChannel==54) strip=5;
        if(VMMChannel==55) strip=60;
        if(VMMChannel==56) strip=4;
        if(VMMChannel==57) strip=61;
        if(VMMChannel==58) strip=3;
        if(VMMChannel==59) strip=62;
        if(VMMChannel==60) strip=2;
        if(VMMChannel==61) strip=63;
        if(VMMChannel==62) strip=1;
        if(VMMChannel==63) strip=64;
    }else if(ChipNumber==1 || ChipNumber==3 || ChipNumber==5 || ChipNumber==7 || ChipNumber==9 || ChipNumber==11 || ChipNumber==13 || ChipNumber==15){
        if(VMMChannel==0)  strip=128;
        if(VMMChannel==1)  strip=65;
        if(VMMChannel==2)  strip=127;
        if(VMMChannel==3)  strip=66;
        if(VMMChannel==4)  strip=126;
        if(VMMChannel==5)  strip=67;
        if(VMMChannel==6)  strip=125;
        if(VMMChannel==7)  strip=68;
        if(VMMChannel==8)  strip=124;
        if(VMMChannel==9)  strip=69;
        if(VMMChannel==10) strip=123;
        if(VMMChannel==11) strip=70;
        if(VMMChannel==12) strip=122;
        if(VMMChannel==13) strip=71;
        if(VMMChannel==14) strip=121;
        if(VMMChannel==15) strip=72;
        if(VMMChannel==16) strip=120;
        if(VMMChannel==17) strip=73;
        if(VMMChannel==18) strip=119;
        if(VMMChannel==19) strip=74;
        if(VMMChannel==20) strip=118;
        if(VMMChannel==21) strip=75;
        if(VMMChannel==22) strip=117;
        if(VMMChannel==23) strip=76;
        if(VMMChannel==24) strip=116;
        if(VMMChannel==25) strip=77;
        if(VMMChannel==26) strip=115;
        if(VMMChannel==27) strip=78;
        if(VMMChannel==28) strip=114;
        if(VMMChannel==29) strip=79;
        if(VMMChannel==30) strip=113;
        if(VMMChannel==31) strip=80;
        if(VMMChannel==32) strip=112;
        if(VMMChannel==33) strip=81;
        if(VMMChannel==34) strip=111;
        if(VMMChannel==35) strip=82;
        if(VMMChannel==36) strip=110;
        if(VMMChannel==37) strip=83;
        if(VMMChannel==38) strip=109;
        if(VMMChannel==39) strip=84;
        if(VMMChannel==40) strip=108;
        if(VMMChannel==41) strip=85;
        if(VMMChannel==42) strip=107;
        if(VMMChannel==43) strip=86;
        if(VMMChannel==44) strip=106;
        if(VMMChannel==45) strip=87;
        if(VMMChannel==46) strip=105;
        if(VMMChannel==47) strip=88;
        if(VMMChannel==48) strip=104;
        if(VMMChannel==49) strip=89;
        if(VMMChannel==50) strip=103;
        if(VMMChannel==51) strip=90;
        if(VMMChannel==52) strip=102;
        if(VMMChannel==53) strip=91;
        if(VMMChannel==54) strip=101;
        if(VMMChannel==55) strip=92;
        if(VMMChannel==56) strip=100;
        if(VMMChannel==57) strip=93;
        if(VMMChannel==58) strip=99;
        if(VMMChannel==59) strip=94;
        if(VMMChannel==60) strip=98;
        if(VMMChannel==61) strip=95;
        if(VMMChannel==62) strip=97;
        if(VMMChannel==63) strip=96;
    }

    return strip;
}
