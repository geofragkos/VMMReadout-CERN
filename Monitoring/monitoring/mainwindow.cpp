//
//  mainwindow.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "frame.h"
#include "canvas.h"
#include "DetBase.h"
#include "DetDetector.h"
#include "daqclient.h"
//#include "pedestalsfileloader.h"

#include "QTabWidget"
#include "QTimer"
#include <QLayout>
#include <QDebug>
#include <QAbstractButton>
#include <QRadioButton>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QLineEdit>
#include <QIcon>

#include "TRandom.h"

using namespace online::display;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    panelLayout(0), pedestalsFile(0), loadFile(0), browseFile(0),
    buttonLayout(0), startButton(0), stopButton(0), pauseButton(0), configButton(0), settingsGeneral(0),
    mainTabs(), rawTabs(), statisticsTabs(), rawChambers(), rawElectronics(), statisticsChambers(), statisticsElectronics(),
    EventDisplay(0), Statistics(0), StatisticsAdvanced(0), ApvRaw(0), CrossTalks(0), DaqStatistics(0),
    mainTreeWidget(0), SrsTree(0), chamberTree(0),
    rawFrame(), eventDisplayFrame_rawChambers(), chipsFrame_rawElectronics(),
    beamProfileFrame_statisticsChambers(), chargeFrame_statisticsChambers(), timeFrame_statisticsChambers(),chipsFrame_statisticsElectronics()
{
    //GUI setup for the main window
    ui->setupUi(this);
    createConfigurationPanel();
    setUpStatusBar();


    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );

}

MainWindow::~MainWindow()
{
    delete ui;
}

//++++++++++++++++++++++++++++++++signals' transmitters++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




void MainWindow::transmitStartDaqClickedSignal()
{
    emit startDaqClicked();
}

void MainWindow::transmitStopDaqClickedSignal()
{
    emit stopDaqClicked();
}

void MainWindow::transmitPauseMonitoringClickedSignal()
{
    emit pauseDaqMonitoringClicked();
}

void MainWindow::transmitConfigDaqClickedSignal()
{
    emit configDaqClicked();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void MainWindow::updateTabCanvas(int index)
{

    if(frame* tempFrame = mainTabs->widget(index)->layout()->findChild<frame*>())
        tempFrame->getFrameCanvas()->update();
}


QString* MainWindow::getSelectedItem_slot(QAbstractButton* button)
{
    QString* selectedItem = new QString(button->text());
    qDebug()<<"Selected item : "<< button->text();
    return selectedItem;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//functions for setup of the main window GUI, constant
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//to be prepared for run control (CURRENTLY NOT WORKING)
QStatusBar* MainWindow::setUpStatusBar()
{
    ui->statusbar->setFixedHeight(30);

    QFont labelFont;
    labelFont.setPointSize(10);
    labelFont.setBold(true);

    runNumberLabel = new QLabel(QString(" Run #  :  "));
    runNumberLabel->setAlignment(Qt::AlignCenter);
    runNumberLabel->setMinimumSize(runNumberLabel->sizeHint());
    runNumberLabel->setStyleSheet("border: 0px solid grey");
    runNumberLabel->setFont(labelFont);

    runNumberLabel_update = new QLabel(QString("..."));
    runNumberLabel_update->setAlignment(Qt::AlignCenter);
    runNumberLabel_update->setMinimumSize(runNumberLabel->sizeHint());
    runNumberLabel_update->setStyleSheet("border: 0px solid grey");
    runNumberLabel_update->setFont(labelFont);

    eventCounterLabel = new QLabel(QString(" Event #  :  "));
    eventCounterLabel->setAlignment(Qt::AlignCenter);
    eventCounterLabel->setMinimumSize(runNumberLabel->sizeHint());
    eventCounterLabel->setStyleSheet("border: 0px solid grey");
    eventCounterLabel->setFont(labelFont);

    eventCounterLabel_update = new QLabel(QString("..."));
    eventCounterLabel_update->setAlignment(Qt::AlignCenter);
    eventCounterLabel_update->setMinimumSize(runNumberLabel->sizeHint());
    eventCounterLabel_update->setStyleSheet("border: 0px solid grey");
    eventCounterLabel_update->setFont(labelFont);

    mmDaqStatusLabel = new QLabel(QString(" Daq Status  :  "));
    mmDaqStatusLabel->setAlignment(Qt::AlignCenter);
    mmDaqStatusLabel->setMinimumSize(runNumberLabel->sizeHint());
    mmDaqStatusLabel->setStyleSheet("border: 0px solid grey");
    mmDaqStatusLabel->setFont(labelFont);

    mmDaqStatusLabel_update = new QLabel(QString("..."));
    mmDaqStatusLabel_update->setAlignment(Qt::AlignCenter);
    mmDaqStatusLabel_update->setMinimumSize(runNumberLabel->sizeHint());
    mmDaqStatusLabel_update->setStyleSheet("border: 0px solid grey");
    mmDaqStatusLabel_update->setFont(labelFont);

    statisticsLabel = new QLabel(QString(" Advanced  :  "));
    statisticsLabel->setAlignment(Qt::AlignCenter);
    statisticsLabel->setMinimumSize(runNumberLabel->sizeHint());
    statisticsLabel->setStyleSheet("border: 0px solid grey");
    statisticsLabel->setFont(labelFont);

    statisticsLabel_update = new QLabel(QString("..."));
    statisticsLabel_update->setAlignment(Qt::AlignCenter);
    statisticsLabel_update->setMinimumSize(runNumberLabel->sizeHint());
    statisticsLabel_update->setStyleSheet("border: 0px solid grey");
    statisticsLabel_update->setFont(labelFont);

    //    runNumberLabel = new QLabel(QString("  Run #  :  "));
    //    runNumberLabel_update = new QLabel();
    //    eventCounterLabel = new QLabel(QString("               Event #  :        "));
    //    eventCounterLabel_update = new QLabel();
    //    mmDaqStatusLabel = new QLabel(QString("                Daq Status  :        "));
    //    mmDaqStatusLabel_update = new QLabel();
    //    statisticsLabel = new QLabel(QString("                 Advanced  :       "));
    //    statisticsLabel_update = new QLabel(QString());

    //    QHBoxLayout *statusBarLayout = new QHBoxLayout;
    //    statusBarLayout->addWidget(runNumberLabel);
    //    statusBarLayout->addWidget(runNumberLabel_update);
    //    statusBarLayout->addWidget(eventCounterLabel);
    //    statusBarLayout->addWidget(eventCounterLabel_update);
    //    statusBarLayout->addWidget(mmDaqStatusLabel);
    //    statusBarLayout->addWidget(mmDaqStatusLabel_update);
    //    statusBarLayout->addWidget(statisticsLabel);
    //    statusBarLayout->addWidget(statisticsLabel_update);

    //    QHBoxLayout *runNumberLayout = new QHBoxLayout;
    //    QGroupBox* runNumberBox = new QGroupBox();
    //    runNumberLayout->addWidget(runNumberLabel);
    //    runNumberLayout->addWidget(runNumberLabel_update);
    //    //runNumberLayout->setStretch(1,5);
    //    //runNumberLayout->setStretch(2,5);
    //    runNumberBox->setLayout(runNumberLayout);

    //    QHBoxLayout *eventCounterLayout = new QHBoxLayout;
    //    QGroupBox* eventCounterBox = new QGroupBox();
    //    eventCounterLayout->addWidget(eventCounterLabel);
    //    eventCounterLayout->addWidget(eventCounterLabel_update);
    //    eventCounterLayout->setStretch(1,5);
    //    eventCounterLayout->setStretch(2,5);
    //    eventCounterBox->setLayout(eventCounterLayout);

    //    QHBoxLayout *mmDaqStatusLayout = new QHBoxLayout;
    //    QGroupBox* mmDaqStatusBox = new QGroupBox();
    //    mmDaqStatusLayout->addWidget(mmDaqStatusLabel);
    //    mmDaqStatusLayout->addWidget(mmDaqStatusLabel_update);
    //    mmDaqStatusLayout->setStretch(1,5);
    //    mmDaqStatusLayout->setStretch(2,5);
    //    mmDaqStatusBox->setLayout(mmDaqStatusLayout);

    //    QHBoxLayout *statisticsLayout = new QHBoxLayout;
    //    QGroupBox* statisticsBox = new QGroupBox();
    //    statisticsLayout->addWidget(statisticsLabel);
    //    statisticsLayout->addWidget(statisticsLabel_update);
    //    statisticsLayout->setStretch(1,5);
    //    statisticsLayout->setStretch(2,5);
    //    statisticsBox->setLayout(statisticsLayout);

    //    ui->statusbar->addWidget(runNumberBox);
    //    ui->statusbar->addWidget(eventCounterBox);
    //    ui->statusbar->addWidget(mmDaqStatusBox);
    //    ui->statusbar->addWidget(statisticsBox);

    //    ui->statusbar->setLayout(statusBarLayout);
    //    ui->statusbar->update();

    ui->statusbar->addPermanentWidget(runNumberLabel,1);
    ui->statusbar->addPermanentWidget(runNumberLabel_update,1);
    ui->statusbar->addPermanentWidget(eventCounterLabel,1);
    ui->statusbar->addPermanentWidget(eventCounterLabel_update,1);
    ui->statusbar->addPermanentWidget(mmDaqStatusLabel,2);
    ui->statusbar->addPermanentWidget(mmDaqStatusLabel_update,3);
    ui->statusbar->addPermanentWidget(statisticsLabel,2);
    ui->statusbar->addPermanentWidget(statisticsLabel_update,3);

    ui->statusbar->setStyleSheet("border: 1px solid grey");

    return onlineDisplayStatusBar;
}

void MainWindow::setUpTabEnvironment()
{
    mainTabs = new QTabWidget();
    mainTabs->clear();

    tabLayout = new QVBoxLayout();
    commentLine = new QLineEdit(0);

    mainTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    EventDisplay = new QTabWidget();
    Statistics = new QTabWidget();
    StatisticsAdvanced = new QTabWidget();
    ApvRaw = new QTabWidget();
    CrossTalks = new QTabWidget();
    DaqStatistics = new QTabWidget();
    StatisticsChips = new QTabWidget();

    Statistics->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    StatisticsAdvanced->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ApvRaw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    CrossTalks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    DaqStatistics->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    StatisticsChips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainTabs->setCurrentIndex(0);

    commentDaqLabel = new QLabel(tr("Comments for Daq"));
    commentDaqLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *commentDaqLayout = new QHBoxLayout();
    commentDaqLayout->addWidget(commentDaqLabel);
    commentDaqLayout->addWidget(commentLine);
    commentDaqLayout->setStretch(0,1);
    commentDaqLayout->setStretch(1,9);
    tabLayout->addWidget(mainTabs);
    tabLayout->addLayout(commentDaqLayout);
    //tabLayout->addWidget(commentLine);

    setUpFrameEnvironment();

}

void MainWindow::setUpTreeEnvironment()
{

    mainTreeWidget = new QTreeWidget();
    mainTreeWidget->setColumnCount(1);
    mainTreeWidget->setUpdatesEnabled(true);

    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("Element Selection"));
    mainTreeWidget->setHeaderItem(headerItem);

    chamberTree = new QTreeWidgetItem(mainTreeWidget);
    //chamberTree->setFlags(chamberTree->flags()|Qt::ItemIsUserCheckable);
    chamberTree->setText(0,"Chamber Elements");
    //chamberTree->setCheckState(0, Qt::Checked);
    chamberTree->setExpanded(1);

}

QVBoxLayout* MainWindow::setUpPedestalsFileSelection()
{
    QVBoxLayout* pedestalsLayout = new QVBoxLayout();
    pedestalSectionLabel = new QLabel(QString("Pedestal File Selection")) ;

    pedestalsFile = new QLineEdit("");
    pedestalsFile->setReadOnly(true);

    QHBoxLayout* pedestalsButtonsLayout = new QHBoxLayout();
    loadFile = new QPushButton("Load");
    loadFile->setEnabled(0);
    browseFile = new QPushButton("Select");
    pedestalsButtonsLayout->addWidget(browseFile);
    pedestalsButtonsLayout->addWidget(loadFile);

    pedestalsLayout->addWidget(pedestalsFile);
    pedestalsLayout->addLayout(pedestalsButtonsLayout);

    connect(this->browseFile,SIGNAL(clicked()),this,SLOT(transmitBrowsePedestalsClickedSignal()));
    connect(this->loadFile,SIGNAL(clicked()),this,SLOT(transmitLoadPedestalsClickedSignal()));

    return pedestalsLayout;

}

void MainWindow::setUpFrameEnvironment()
{


    QWidget* eventDisplayFrameTab = new QWidget();
    eventDisplayFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* statisticsFrameTab = new QWidget();
    statisticsFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* statisticsAdvancedFrameTab = new QWidget();
    statisticsAdvancedFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* apvFrameTab = new QWidget();
    apvFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* crossTalksFrameTab = new QWidget();
    crossTalksFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* daqStatisticsFrameTab = new QWidget();
    daqStatisticsFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* pedestalsFrameTab = new QWidget();
    pedestalsFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* statisticsChipsFrameTab = new QWidget();
    statisticsChipsFrameTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    eventDisplayFrame = new frame(eventDisplayFrameTab);
    eventDisplayFrame->frameType ="Event Display";
    addFrameToTabWidget(mainTabs,eventDisplayFrameTab,eventDisplayFrame,"Event Display");
    statisticsFrame = new frame(statisticsFrameTab);
    statisticsFrame->frameType ="Statistics";
    addFrameToTabWidget(mainTabs,statisticsFrameTab,statisticsFrame,"Statistics");
    statisticsChipsFrame = new frame(statisticsChipsFrameTab);
    statisticsChipsFrame->frameType ="Chips Statistics";
    addFrameToTabWidget(mainTabs,statisticsChipsFrameTab,statisticsChipsFrame,"Statistics Chips");
    statisticsAdvancedFrame = new frame(statisticsAdvancedFrameTab);
    statisticsAdvancedFrame->frameType ="Statistics Advanced";
    addFrameToTabWidget(mainTabs,statisticsAdvancedFrameTab,statisticsAdvancedFrame,"Statistics Advanced");
    daqStatisticsFrame = new frame(daqStatisticsFrameTab);
    daqStatisticsFrame->frameType ="DaqStatistics";
    addFrameToTabWidget(mainTabs,daqStatisticsFrameTab,daqStatisticsFrame,"DAQ Statistics");
    apvRawFrame = new frame(apvFrameTab);
    apvRawFrame->frameType ="Apv Raw";
    addFrameToTabWidget(mainTabs,apvFrameTab,apvRawFrame,"Apv Raw");
    crossTalksFrame = new frame(crossTalksFrameTab);
    crossTalksFrame->frameType ="CrossTalks";
    addFrameToTabWidget(mainTabs,crossTalksFrameTab,crossTalksFrame,"CrossTalks");
    pedestalsFrame = new frame(pedestalsFrameTab);
    pedestalsFrame->frameType ="Pedestals";
    addFrameToTabWidget(mainTabs,pedestalsFrameTab,pedestalsFrame,"Pedestals");


}

QVBoxLayout* MainWindow::setUpRunControl()
{
    runSettingsLayout = new QHBoxLayout();
    runControlLayout = new QVBoxLayout();
    buttonLayout = new QHBoxLayout();
    startButton = new QPushButton(QIcon(QString("button_play_icon.png")),"",0);
    startButton->setIconSize(QSize(25,25));
    startPedestalsButton = new QPushButton(QIcon(QString("button_pedestals_icon.jpg")),"",0);
    startPedestalsButton->setIconSize(QSize(25,25));
    stopButton = new QPushButton(QIcon(QString("button_stop_icon.png")),"",0);
    stopButton->setIconSize(QSize(25,25));
    pauseButton = new QPushButton(QIcon(QString("button_pause_icon.png")),"",0);
    configButton = new QPushButton(QIcon(QString("button_config_icon.png")),"",0);
    configButton->setIconSize(QSize(25,25));
    settingsGeneral = new QPushButton("Run Settings",0);
    saveCheckBox = new QCheckBox("write", this);
    saveCheckBox->setVisible(0);

    startButton->setEnabled(1);
    stopButton->setEnabled(0);
    //startPedestalsButton->setEnabled(0);
    //configButton->setEnabled(1);
    settingsGeneral->setEnabled(1);

    //    buttonLayout->addWidget(saveCheckBox);
    buttonLayout->addWidget(startButton);
    //buttonLayout->addWidget(startPedestalsButton);
    //buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(stopButton);
    //buttonLayout->addWidget(configButton);
    //buttonLayout->addWidget(setUpVariousSettingsSelection());
    runControlLabel = new QLabel(tr("mmDaq Run Control Panel"));
    QFont runControlLabelFont;
    runControlLabelFont.setBold(true);
    runControlLabel->setFont(runControlLabelFont);
    runControlLabel->setAlignment(Qt::AlignCenter);
    runControlLayout->addWidget(runControlLabel);
    runControlLayout->addWidget(setUpVariousSettingsSelection());
    runControlLayout->addLayout(buttonLayout);
    runControlLayout->addWidget(settingsGeneral);
    //runControlLayout->
    //runControlLayout->setStretch(1,4);
    //runControlLayout->setStretch(2,3);
    //runControlLayout->setStretch(3,3);
    //runSettingsLayout->addLayout(runControlLayout);
    //runSettingsLayout->addWidget(setUpVariousSettingsSelection());

    return runControlLayout;
}

QGroupBox * MainWindow::setUpVariousSettingsSelection()
{
    runTypeGroupBox = new QGroupBox();

    radioPhysics = new QRadioButton(tr("Physics"));
    radioPedestals = new QRadioButton(tr("Pedestals"));

    radioPhysics->setChecked(true);

    QHBoxLayout *vboxRunType = new QHBoxLayout;
    vboxRunType->addWidget(radioPhysics);
    vboxRunType->addWidget(radioPedestals);
    vboxRunType->addStretch(0.5);
    runTypeGroupBox->setLayout(vboxRunType);
    runTypeGroupBox->setStyleSheet("border: 0px solid grey");
    return runTypeGroupBox;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//functions for modification and control of the main window GUI and its elements, dynamic
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void MainWindow::addFrameToTabWidget(QTabWidget *parentTab, QWidget *targetWidget, frame *frameForTab, const char* tabTitle)
{
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(frameForTab->getFrameCanvas());   //add frame->canvas to tab (not frame) to be resizable
    targetWidget->setLayout(l);
    addTab(parentTab, targetWidget, tabTitle);
    frameForTab->show();

}

void MainWindow::addTab(QTabWidget *parentTab, QWidget *childWidget, const char* tabTitle)  {

    childWidget->setWindowTitle(tabTitle);
    parentTab->addTab(childWidget, tabTitle);
    childWidget->setUpdatesEnabled(true);
    childWidget->show();

}


void MainWindow::createConfigurationPanel()
{
    panelLayout = new QWidget();
    QVBoxLayout *configPanelLayout = new QVBoxLayout(panelLayout);
    QTextEdit *insertCommand = new QTextEdit(panelLayout);
    insertCommand->setGeometry(2,3,2,3);
    QPushButton *sendCommand = new QPushButton(panelLayout);
    sendCommand->setText("SEND");
    configPanelLayout->addWidget(insertCommand);
    configPanelLayout->addWidget(sendCommand);
}

void MainWindow::openConfigurationPanel_slot()
{
    //if(timer->isActive()) reset_slot();
    panelLayout->show();

}

//checking the number of chambers that are selected for displaying


int MainWindow::getElementsNumberOfCheckedChildren(QTreeWidgetItem* parentTreeWidget)
{
    int numberOfElementsDisplayed=0;
    for(int i=0; i<parentTreeWidget->childCount(); i++)
    {
        if((parentTreeWidget->child(i))->checkState(0) == 2)
            numberOfElementsDisplayed++;
    }
    return numberOfElementsDisplayed;
}

//checking the number of chips that are selected for displaying
int MainWindow::getElementsNumberOfCheckedChambersApvChips(QTreeWidgetItem* parentTreeWidget)
{
    int numberOfElementsDisplayed=0;
    for(int i=0; i<parentTreeWidget->parent()->childCount(); i++)
    {   for(int iChip=0; iChip<(parentTreeWidget->parent()->child(i))->childCount();iChip++)    {
            if((parentTreeWidget->parent()->child(i))->child(iChip)->text(0).contains("APV", Qt::CaseInsensitive)
                    && (parentTreeWidget->parent()->child(i))->child(iChip)->checkState(0)==2)
            {
                numberOfElementsDisplayed++;
//                qDebug("CHECK STATE for %s ===  ===  2 ",(parentTreeWidget->parent()->child(i))->child(iChip)->text(0));
//                mem_stripData.push_back(QString::fromUtf8(stripData.at(i).c_str()));

                //numberOfElementsDisplayed+=getElementsNumberOfCheckedChildren(parentTreeWidget->parent()->child(i));
            }
        }
    }
    qDebug("numberOfElementsDisplayed = %i",numberOfElementsDisplayed);
    return numberOfElementsDisplayed;
}

int MainWindow::getElementsNumberOfCheckedChambersVmm1Chips(QTreeWidgetItem* parentTreeWidget)
{
    int numberOfElementsDisplayed=0;
    for(int i=0; i<parentTreeWidget->parent()->childCount(); i++)
    {   for(int iChip=0; iChip<(parentTreeWidget->parent()->child(i))->childCount();iChip++)    {
            if((parentTreeWidget->parent()->child(i))->child(iChip)->text(0).contains("VMM", Qt::CaseInsensitive)
                    && (parentTreeWidget->parent()->child(i))->child(iChip)->checkState(0)==2)
                numberOfElementsDisplayed++;
            //numberOfElementsDisplayed+=getElementsNumberOfCheckedChildren(parentTreeWidget->parent()->child(i));
        }
    }
    return numberOfElementsDisplayed;
}
