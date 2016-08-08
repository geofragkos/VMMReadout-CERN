#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <constants.h>
#include "mythread.h"
#include "tcphandler.h"
//tabs
#include <QStackedWidget>
#include <QTabWidget>
#include <QGridLayout>


#include <vector>
#include "configuration_module.h"
#include "run_module.h"
#include "dataprocessor.h"
//using std::vector;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QVector< QVector<int> > calibConstants;
    QString currentAddressForRecipe;

    QByteArray bufferTCP;
    QByteArray buffer;
    QByteArray bufferDAQ;
    bool spiSent;
    int boardThreshold[20];
    QString words[300];
    QByteArray wordsQBA[300];
    int packageSizeGl;

    QFile fileDaq;
    QFile fileDaq_fpga;

    TFile *fileDaqRoot;

    //tabs
    //QTabWidget *stackedWidget;
    QGroupBox *groupModify;
    QGridLayout *grid;

    TBranch *eventNumberBranch;
    TBranch *eventNumberFAFABranch;
    TBranch *eventNumberCounterBranch;
    TBranch *triggerTimestampBranch;
    TBranch *triggerCounterBranch;
    TBranch *chipIdBranch;
    TBranch *boardBranch;
    TBranch *fecBranch;
    TBranch *evSizeBranch;
    TBranch *tdoBranch;
    TBranch *flgBranch;
    TBranch *thrBranch;
    TBranch *bcidBranch;
    TBranch *GrayDecodedBranch;
    TBranch *pdoBranch;
    TBranch *chIdBranch;


    TBranch *neighborBranchForCalibration;
    TBranch *pulserBranchForCalibration;
    TBranch *gainBranchForCalibration;
    TBranch *intTimeBranchForCalibration;
    TBranch *thrBranchForCalibration;
    TBranch *calibRunBranch;

    TBranch *triggerTimeBranch;
    TBranch *triggerChannelBranch;
    TBranch *triggerBoardBranch;
    TBranch *triggerEventBranch;
    TTree *vmm2;
    TTree *vmm2Calibration;
    TTree *run_properties;
    TTree *vmm1Trigger;


    std::vector<double> triggerTimeVariable;
    std::vector<double> triggerBoardVariable,triggerChannelVariable;
    std::vector<std::vector<int> > tdoVariable, pdoVariable, chIdVariable, bcidVariable, flgVariable, thrVariable, GrayDecodedVariable,neighborVariableForCalibration;
    std::vector<int> boardVariable, chipIdVariable, fecVariable, evSizeVariable, triggerTimestampVariable, triggerCounterVariable;

    QTabWidget *monitoringTabWidget ;
    int triggerEventVariable;
    int boardEvents[16];
    bool calibrationStop;
    double gainVariableForCalibration;
    int pulserVariableForCalibration,intTimeVariableForCalibration, thrVariableForCalibration;
    int eventNumberVariable,eventNumberCounterVariable,eventNumberFAFAVariable, calibRunVariable;
    int currentEvent;
    bool dataNotEmpty;

//    TQtWidget *Quality;
//    TQtWidget *profiles;
//    TQtWidget *times;
//    TQtWidget *charges;

    bool communicationAlive[16];
    bool communicationDead[16];
    bool communicationAliveFPGA;
    void CreateChannelsField();
    bool headerSet;
    bool multicastingFlag;
    bool pulserDi;

    QString ips[16];
    QString ips_tr;
    QUdpSocket *socket;
    QUdpSocket *socketFPGA;
    QTcpSocket *socketTCP;
    QUdpSocket *socketDAQ;

    quint32 commandCounter;
    quint16 VMMAPPPort;
    quint16 VMMASICPort;
    quint16 S6Port;
    quint16 DCARDPort;
    quint16 FECPort;
    quint16 DAQPort;
    quint16 FPGAPort;
    quint16 channelMap;

    quint32 addr[67], vars[67];
    int maxIter;
    bool bnd;
    bool bndFPGA;
    bool bndTCP;
    bool daqFile_fpga;
    int idForCustomCommands;
    int daqCnt;
    int daqCntTemp;
    int daqCnt_fpga;

    QDir runDirectory;
    QRadioButton *command[5];
    QGroupBox *groupBox;

    QLineEdit *VMM1Channel[64];
    QPushButton *VMM1NegativeButton[64];
    QComboBox *VMM1SDVoltage[64];
    QComboBox *VMM1SZ010bCBox[64];
    QComboBox *VMM1SZ08bCBox[64];
    QComboBox *VMM1SZ06bCBox[64];

    QPushButton *VMM1SC[64];
    QPushButton *VMM1SL[64];
    QPushButton *VMM1ST[64];
    QPushButton *VMM1SM[64];
    QPushButton *VMM1SMX[64];

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

    bool VMM1SCBool[64];
    bool VMM1SLBool[64];
    bool VMM1STBool[64];
    bool VMM1SMBool[64];
    bool VMM1SMXBool[64];
    bool VMM1SPBool[64];
    quint8 VMM1SDValue[64];
    quint8 VMM1SZ010bValue[64];
    quint8 VMM1SZ08bValue[64];
    quint8 VMM1SZ06bValue[64];


    bool VMM1SPBoolAll;
    bool VMM1SCBoolAll;
    bool VMM1SLBoolAll;
    bool VMM1STBoolAll;
    bool VMM1SMBoolAll;
    bool VMM1SMXBoolAll;
    bool VMM1SZ010bBoolAll;
    bool VMM1SZ08bBoolAll;
    bool VMM1SZ06bBoolAll;

    bool VMM1SPBoolAll2;
    bool VMM1SCBoolAll2;
    bool VMM1SLBoolAll2;
    bool VMM1STBoolAll2;
    bool VMM1SMBoolAll2;
    bool VMM1SMXBoolAll2;
    bool VMM1SZ010bBoolAll2;
    bool VMM1SZ08bBoolAll2;
    bool VMM1SZ06bBoolAll2;
    QFont Font;

    MyThread* threadDi;
    MyThread* threadFill;

    TH1D *T1Profile;
    TH1D *T2Profile;
    TH1D *T3Profile;
    TH1D *T4Profile;
    TH1D *T5Profile;
    TH1D *T6Profile;
    TH1D *T7Profile;
    TH1D *T8Profile;

    TH1D *T1Times;
    TH1D *T2Times;
    TH1D *T3Times;
    TH1D *T4Times;
    TH1D *T5Times;
    TH1D *T6Times;
    TH1D *T7Times;
    TH1D *T8Times;

    TH1D *T1Charges;
    TH1D *T2Charges;
    TH1D *T3Charges;
    TH1D *T4Charges;
    TH1D *T5Charges;
    TH1D *T6Charges;
    TH1D *T7Charges;
    TH1D *T8Charges;

    TH1D *BoardsHits;
    TH1D *EventNumber;
    TH1D *EventNumberTrigger;


    // ***** Mapping *****
    int T1Chips[4];
    int T2Chips[4];
    int T3Chips[4];
    int T4Chips[4];
    int T5Chips[4];
    int T6Chips[4];
    int T7Chips[4];
    int T8Chips[4];


private:
    Ui::MainWindow *ui;
    tcpHandler triggerTCP;
    Configuration* _config;
    RunDAQ* _runDAQ;
    DataProcessor* _dataProcessor;
    bool m_daqConstantsLoaded;

public slots:
    uint grayToBinary(uint num);
    void PingFec();
    void PingFPGA();
    void SetNumberOfFecs(int);
    void SetFPGAIP();
    void Connect();
    void dataPending();
    void ConnectTCP();
    void ConnectFPGA();
    void readTCP();
    void Disconnect();
    void Merger();
    void MergerFPGA();
    void updateChannelState();
    void updateChannelVoltages(int index);
    void updateChannelADCs(int index);
    void Sender(QByteArray blockOfData);
    void SenderFPGA(QByteArray blockOfData);
    void displayError(QAbstractSocket::SocketError socketError);
    QByteArray bitsToBytes(QBitArray bits);
    void hideChannels(int state);
    void showDebugScreen(int state);
    int packageHandler(QByteArray package);
    void clearDebugScreen();
    void customCommandHandler();
    void downloadSPI();
    void UpdateCounter();
    quint32 ValueToReplace(QString datagramHex, int bitToChange, bool bitValue);
    quint32 reverse32(QString datagramHex);
    void customCommandWithoutResponse();
    void constructHeader(int);
    void sendDis(int);
    void threadHandler();
    void triggerHandler();
    void dataDAQPending();
    void triggerHandler_fpga();
    void writeFPGAFile(QString inputPacket);
    void changeDACtoMVs(int value);
    void openMonitoring(int);
    void initializePlots();
    void updateCanvas(int);
    std::vector<double> bytesTobits(QByteArray bytes);
    std::vector<double> bytesTobitsTrigger(QByteArray bytes);
    void SetChambersMapping();
    void selectWorkingDirectory();
    void startCalibration();
    void loadCalibrationConstants(int);
    void HandleRecipe();
    void LoadConfigurationFromFile();
    void WriteConfigurationFromFile();
    void LoadDAQConfigurationFromFile();
    void WriteDAQConfigurationFile();
    void setChannelMaps(int);
	void SendParamsToConfigModule();
    void SendRunParamsToDAQModule();
    void CreateChannelMap(int state);
    void DecodeChannelMap(int chanmap);
    void LoadThresholds(int state);
    int TMapping(int PanasonicPin, int ChipNumber);
    int TMappingMini2(int VMMChannel, int ChipNumber);
    void updateTriggerCount();
};


#endif // MAINWINDOW_H
