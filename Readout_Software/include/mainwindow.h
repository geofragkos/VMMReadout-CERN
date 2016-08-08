#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// qt
#include <QMainWindow>
#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFont>
#include <QThread>
//#include <constants.h>

// vmm
#include "config_handler.h"
#include "configuration_module.h"
#include "run_module.h"
#include "socket_handler.h"
#include "data_handler.h"
#include "calibration_module.h"
#include "message_handler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        bool dbg() { return m_dbg; }

        bool eventFilter(QObject *obj, QEvent *event);




        QFont Font;
        QGridLayout *channelGridLayout;
        QWidget* dummy;
        void CreateChannelsFields();
        void SetInitialState();

        bool configOK() { return m_configOK; }

        //////////////////////////////////////////////////////
        // methods to grab the VMM tools
        //////////////////////////////////////////////////////
        ConfigHandler& configHandle() { return *vmmConfigHandler; }
        SocketHandler& socketHandle() { return *vmmSocketHandler; }
        Configuration& configModule() { return *vmmConfigModule;  }
        RunModule&     runModule()    { return *vmmRunModule;     }
        DataHandler&   dataHandle()   { return *vmmDataHandler;    }
        CalibModule&   calibModule()  { return *vmmCalibModule; }
        MessageHandler& msg()         { return *vmmMessageHandler; }
    
        //////////////////////////////////////////////////////
        // IPs
        //////////////////////////////////////////////////////
        QString ips[16]; 

        /////////////////////////////////////////////////////
        // Channel Fields Buttons
        /////////////////////////////////////////////////////
        QLineEdit *VMMChannel[64];
        QPushButton *VMMNegativeButton[64];
        QComboBox *VMMSDVoltage[64];
        QComboBox *VMMSZ010bCBox[64];
        QComboBox *VMMSZ08bCBox[64];
        QComboBox *VMMSZ06bCBox[64];
    
        QPushButton *VMMSC[64];
        QPushButton *VMMSL[64];
        QPushButton *VMMST[64];
        QPushButton *VMMSM[64];
        QPushButton *VMMSMX[64];
    
        QPushButton *SPLabel;
        QPushButton *SCLabel;
        QPushButton *SLLabel;
        QPushButton *STLabel;
        QPushButton *SMLabel;
        QComboBox *SDLabel;
        QComboBox *SZ010bLabel;
        QComboBox *SZ08bLabel;
        QComboBox *SZ06bLabel;
        QPushButton *SMXLabel;
    
        QPushButton *SPLabel2;
        QPushButton *SCLabel2;
        QPushButton *SLLabel2;
        QPushButton *STLabel2;
        QPushButton *SMLabel2;
        QComboBox *SDLabel2;
        QComboBox *SZ010bLabel2;
        QComboBox *SZ08bLabel2;
        QComboBox *SZ06bLabel2;
        QPushButton *SMXLabel2;
    
        bool VMMSCBool[64];
        bool VMMSLBool[64];
        bool VMMSTBool[64];
        bool VMMSMBool[64];
        bool VMMSMXBool[64];
        bool VMMSPBool[64];
        quint8 VMMSDValue[64];
        quint8 VMMSZ010bValue[64];
        quint8 VMMSZ08bValue[64];
        quint8 VMMSZ06bValue[64];
    
        bool VMMSPBoolAll;
        bool VMMSCBoolAll;
        bool VMMSLBoolAll;
        bool VMMSTBoolAll;
        bool VMMSMBoolAll;
        bool VMMSMXBoolAll;
        bool VMMSZ010bBoolAll;
        bool VMMSZ08bBoolAll;
        bool VMMSZ06bBoolAll;
    
        bool VMMSPBoolAll2;
        bool VMMSCBoolAll2;
        bool VMMSLBoolAll2;
        bool VMMSTBoolAll2;
        bool VMMSMBoolAll2;
        bool VMMSMXBoolAll2;
        bool VMMSZ010bBoolAll2;
        bool VMMSZ08bBoolAll2;
        bool VMMSZ06bBoolAll2;

        /////////////////////////////////
        // calibration loop
        /////////////////////////////////
        void startCalibration();


        /////////////////////////////////
        // misc.
        /////////////////////////////////
        void delay();
        void delayMs();
    
    private:
        Ui::MainWindow *ui;
        bool m_dbg;

        // sets the GUI to whatever state
        // the ConfigHandle object is in
        void updateConfigState();

        const int FECPORT;
        const int DAQPORT;
        const int VMMASICPORT;
        const int VMMAPPPORT;
        const int S6PORT;

        ConfigHandler *vmmConfigHandler;
        SocketHandler *vmmSocketHandler;
        Configuration *vmmConfigModule;
        RunModule     *vmmRunModule;
        DataHandler   *vmmDataHandler;
        CalibModule *vmmCalibModule;
        MessageHandler *vmmMessageHandler;

        //thread
        QThread *daqThread;


        // "FSM" stuff
        bool m_commOK;
        //addmmfe8
        bool m_febSetOK;
        bool m_configOK;
        bool m_tdaqOK;
        bool m_runModeOK;
        QString m_acqMode;
        bool m_daqInProgress;
        bool m_hdmiMaskON;
        bool m_runDirectoryOK;
        bool m_readyToRead;
        bool m_inCalibrationLoop;

    signals :
        void checkFSM();
        void EndRun();
        void stopCalibrationLoop();

        // DataHandler related
        void monitorDataSignal(bool);
        void clearSharedMemory(int);
        void setUseChannelMap(bool);
        void loadELxChannelMapping(QString);
        void setWriteNtuple(bool);
        void setIgnore16(bool);
        void setCalibrationRun(bool);
        void updateCalibrationState(int, int, int, int, int);
        void setupOutputFiles(QString, QString);
        void setupOutputTrees();
        void checkReadyToRead();
        void startDAQSocket();
        void closeDAQSocket();

        //thread
        void testThread(QString);
        void testFunction2();
        void testMultiARG(QString,QString,QString);

    public slots:
        // dac-to-mV
        void changeDACtoMVs(int);
        // ~"FSM"
        void updateFSM();

        // logging
        void readLog();

        // counter
        void updateCounter();

        // event count for run has been reached
        void eventCountReached();

        // send the board configuration
        void prepareAndSendBoardConfig();

        // send the t/daq configuration
        void prepareAndSendTDAQConfig();

        // set the run mode and send
        void setRunMode();

        void keepButtonState(bool);

        // set the acquisition mode
        void setACQMode();

        // select the output directory
        void selectOutputDirectory();

        // load a configuration from an XML
        void loadConfigurationFromFile();

        // write configuration to an XML file
        void writeConfigurationToFile();

        // number of FECs
        void setNumberOfFecs(int);

        /// remaining buttons/widgets

        void setAndSendEventHeaders();
        void resetASICs();
        void resetFEC();
        void setHDMIMask();
        void setART();
        void checkLinkStatus();
        void writeFECStatus();
        void resetLinks();
        void triggerHandler();
        void setS6clocks();
        void setS6Resets();
        void set_s6resetStatus(bool);
        void configureTP();
        void updateLogScreen();
        void toggleDebug();

        // bad directory name
        void badRunDirectory();
        void setRunDirOK(bool);

        // channel fields
        void updateChannelState();
        void updateChannelVoltages(int);
        void updateChannelADCs(int);

        // trigger count
        void updateTriggerCount(bool);

        // connect to IP
        void Connect();

        //addmmfe8
        // set FEB
        void setFEB();
        void propagateFEBSettings(bool);
        void propagateFEBSettings1(bool);

        // calibration-related
        void calibrationLoopState(bool);
        void setPDOCalibrationState(int,int,int);
        void setTDOCalibrationState(int,int,int,int);
        void changeDelayLabels(int);
        void setChannelsForCalib(int);
        void setupCalibrationConfig();
        void setCalibrationACQon(int);
        void setCalibrationACQoff();
        void endCalibrationRun();

        //monitoring
        void setupMonitoring(int);

};


#endif // MAINWINDOW_H
