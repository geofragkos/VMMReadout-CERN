//
//  mainwindow.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "frame.h"
//#include "testframe.h"
//#include "daqclient.h"
#include <QTabWidget>
#include <QTreeWidget>
#include <QTimer>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLayout>
#include <QLabel>
#include <QObject>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#ifndef Q_MOC_RUN
//#include "pedestalsfileloader.h"
#endif

//#include "DaqServerConfig.h"

#include "TRandom.h"



namespace Ui {
class MainWindow;
class pedestalsFileLoader;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow *ui;
    ~MainWindow();

    TRandom *rndm;

    void addTab(QTabWidget *parentTab, QWidget *childWidget, const char* tabTitle);
    QStatusBar* setUpStatusBar();
    //void setUpRadioBoxSelection();
    void setUpTabEnvironment();
    void setUpTreeEnvironment();
    void setUpFrameEnvironment();
    QVBoxLayout *setUpPedestalsFileSelection();
    QVBoxLayout *setUpRunControl();

    QGroupBox *setUpVariousSettingsSelection();

    int getElementsNumberOfCheckedChildren(QTreeWidgetItem* parentTreeWidget);
    int getElementsNumberOfCheckedChambersApvChips(QTreeWidgetItem* parentTreeWidget);
    int getElementsNumberOfCheckedChambersVmm1Chips(QTreeWidgetItem* parentTreeWidget);

    QWidget *panelLayout;
    void addFrameToTabWidget(QTabWidget *parentTab, QWidget *targetWidget, frame *frameForTab, const char *tabTitle);
    void createConfigurationPanel();

    QStatusBar *onlineDisplayStatusBar;
    QLabel *runNumberLabel;
    QLabel *runNumberLabel_update;
    QLabel *eventCounterLabel;
    QLabel *eventCounterLabel_update;
    QLabel *mmDaqStatusLabel;
    QLabel *mmDaqStatusLabel_update;
    QLabel *statisticsLabel;
    QLabel *statisticsLabel_update;
    QLabel *commentDaqLabel;

    QLabel * pedestalSectionLabel;
    QLineEdit* pedestalsFile;
    QPushButton* loadFile;
    QPushButton* browseFile;


    QLabel *runControlLabel;
    QHBoxLayout *runSettingsLayout;
    QVBoxLayout *runControlLayout;
    QHBoxLayout *buttonLayout;

    QGroupBox *runTypeGroupBox;
    QRadioButton *radioPhysics;
    QRadioButton *radioPedestals;

    QCheckBox *saveCheckBox;
    QPushButton *startButton;
    QPushButton *startPedestalsButton;
    QPushButton *stopButton;
    QPushButton *pauseButton;
    QPushButton *configButton;
    QPushButton *settingsGeneral;
    QLineEdit* commentLine;

    QVBoxLayout *tabLayout;
    QVBoxLayout *leftLayout;

//tab widgets for graphics
    //main tab widget
    QTabWidget *mainTabs;
    QTabWidget *rawTabs;
    QTabWidget *statisticsTabs;

    //tab widget for layer information
    QTabWidget *rawChambers;
    QTabWidget *rawElectronics;

    //tab widget for statistics information
    QTabWidget *statisticsChambers;
    QTabWidget *statisticsElectronics;


//Actual Tabs
    QTabWidget *EventDisplay;
    QTabWidget *Statistics;
    QTabWidget *StatisticsAdvanced;
    QTabWidget *ApvRaw;
    QTabWidget *CrossTalks;
    QTabWidget *DaqStatistics;
    QTabWidget *Pedestals;
    QTabWidget *StatisticsChips;

    //tree widgets for selecting different objects of each category
    QTreeWidget *mainTreeWidget;
    QTreeWidgetItem *SrsTree;
    QTreeWidgetItem *chamberTree;

//frames for canvases to be displayed in each tab
    frame *simple_frame;//for testing (to be removed)
    frame *simple_frame1;//for testing (to be removed)

    //frames for Raw Tab
    //chambers
    frame *rawFrame;
    frame *eventDisplayFrame_rawChambers;
    //electronics
    frame *chipsFrame_rawElectronics;

    //frames for Statistics Tab
    //chambers
    frame *beamProfileFrame_statisticsChambers;
    frame *chargeFrame_statisticsChambers;
    frame *timeFrame_statisticsChambers;
    //electronics
    frame *chipsFrame_statisticsElectronics;


    frame* eventDisplayFrame;
    frame* statisticsFrame;
    frame* statisticsAdvancedFrame;
    frame* apvRawFrame;
    frame* crossTalksFrame;
    frame* daqStatisticsFrame;
    frame* pedestalsFrame;
    frame* statisticsChipsFrame;

    QString filename;


public slots:
    void transmitStartDaqClickedSignal();
    void transmitStopDaqClickedSignal();
    void transmitPauseMonitoringClickedSignal();
    void transmitConfigDaqClickedSignal();
    void updateTabCanvas(int index);

    QString *getSelectedItem_slot(QAbstractButton* button);

    void openConfigurationPanel_slot();

    //virtual void updateFrameCanvasesDivision_slot(QTreeWidgetItem* parentTreeItem) {}
    virtual void fillStatistics() {}
    virtual void fillStatisticsHistos() {}
    virtual void fillStatisticsAdvancedHistos() {}
    virtual void fillChargeStatisticsHistos() {}
    virtual void fillTimeStatisticsHistos() {}
    //virtual void displaySrsHistos() {}
    virtual void reset_slot() {}
    virtual void handleBufferedEvent() {}
    virtual void openPedestalsFile() {}
    virtual void loadPedestalsFile() {}



signals:
    void startDaqClicked();
    void stopDaqClicked();
    void pauseDaqMonitoringClicked();
    void configDaqClicked();

    //virtual void pedestalsFileNameIs(std::string filename);

};

#endif // MAINWINDOW_H
