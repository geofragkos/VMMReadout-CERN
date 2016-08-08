
#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

// qt
#include <QObject>
#include <QFile>
#include <QMap>
#include <QHostAddress>
class QByteArray;
class QBitArray;
class QUdpSocket;

// std/stl
#include <iostream>

// vmm
#include "config_handler.h"
#include "message_handler.h"
#include "daq_monitor.h"
class VMMSocket;

// ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

//mmdaq
#include "daqconfig.h"
#include "sharedmemorywriter.h"
#include "createevents.h"

//boost
#include <boost/shared_ptr.hpp>


//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  DataHandler
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
class DataHandler : public QObject
{
    Q_OBJECT

    public :
        explicit DataHandler(QObject *parent = 0);
        virtual ~DataHandler(){};

        void setDebug(bool dbg);
        bool dbg() { return m_dbg; }

        //addmmfe8
        void setMMFE8(bool set_to_run_mmfe8);
        bool mmfe8() { return m_mmfe8; }

        //addxadc
        void setXADC(bool set_to_run_xadc);
        bool xadc() {return m_xadc;}

        bool monitoring() { return m_doMonitoring; }
        void setupMonitoring();
        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        void setEventCountStop(int count_to_this) { m_eventCountStop = count_to_this; }

        void startDAQMonitor();//daqmon
        void closeDAQMonitor();//daqmon

        bool calibRun() { return m_calibRun; }
        bool ignore16() { return m_ignore16; }
        bool useChannelMap() { return m_use_channelmap; }
        int channelToStrip(int chipNumber, int channelNumber);
        bool writeNtuple() { return m_write; }

        void LoadDAQSocket(VMMSocket& socket); 
        void connectDAQ();

      //  bool setupOutputFiles(TriggerDAQ& daq, QString outdir = "",
      //                                      QString filename = "");
        static int checkForExistingFiles(std::string dirname="", int expectedRunNumber=0);
        bool checkQFileOK(std::string fname="");
        bool checkQFileFound(std::string fname="");

        void dataFileHeader(CommInfo& comm, GlobalSetting& global,
                                TriggerDAQ& daq);
        void getRunProperties(const GlobalSetting& global, int runNumber,
                                            int angle, double tpSkew);

        QString getRootFileName(const QString& outdir);


        /////////////////////////////////////////
        // general methods
        // not necessarily class specific
        /////////////////////////////////////////
        static quint32 ValueToReplaceHEX32(QString datagramHex, int bitToChange,
                                                bool newBitValue);
        static QByteArray bitsToBytes(QBitArray bits);
        static QBitArray bytesToBits(QByteArray bytes); 
        static QString QBitArrayToString(const QBitArray &arr);
        static quint32 reverse32(QString hex);
        static uint grayToBinary(uint num);

        /////////////////////////////////////////
        // data handling
        /////////////////////////////////////////
        VMMSocket& daqSocket() { return *m_daqSocket; }
        void decodeAndWriteData(const QByteArray& datagram, QHostAddress& ip);
        void decodeAndWriteData_mmfe8(const QByteArray& datagram, QHostAddress& ip);

        //decode and write xadc
        void decodeAndWriteData_xadc(const QByteArray& datagram, QHostAddress& ip);

        void resetDAQCount() { (*n_daqCnt) = 0; }
        int getDAQCount() { return (*n_daqCnt); } 
        void updateDAQCount() { (*n_daqCnt)++; }
        void fillEventData();
        

        //int n_daqCnt;
        //daqmon
        boost::shared_ptr< int > n_daqCnt;

    private :
        bool m_dbg;
        //addmmfe8
        bool m_mmfe8;
        //addmxadc
        bool m_xadc;
        bool m_doMonitoring;
        bool m_monitoringSetup;
        bool m_calibRun;
        bool m_write;
        bool m_ignore16;
        bool m_use_channelmap;

        //test shared
        DaqConfig* m_daqConf;
        CreateEvents* m_ce;
        SharedMemoryWriter* m_sh;
        std::vector<std::string> m_sharedDataStrips;

        //thread
        QUdpSocket *m_DAQSocket;

        QString m_mapping_file; ///wait... is this needed?
        std::string m_mapping; //type of mapping file/ELx
        QMap<int, std::vector<int> > m_map_mini2;

        VMMSocket *m_daqSocket;
        MessageHandler *m_msg;

        // if >0 this will be the number of events to read before stopping
        int m_eventCountStop;

        // for monitoring daq daqmon
        DaqMonitor* m_daqMonitor; 

        QFile m_daqFile;
        bool m_fileOK;
        QString m_outDir;

        ///////////////////////////////////////////
        // data decoding
        ///////////////////////////////////////////
        void clearData();

        // run data
        double m_gain;
        int m_runNumber;
        int m_tacSlope;
        int m_peakTime;
        int m_dacCounts;
        int m_pulserCounts;
        double m_tpSkew; // ns
        int m_angle;

        // xadc data
        int m_chip;
        int m_board;
        int m_read1;
        int m_read2;
        int m_read3;
        int m_read4;
        int m_read5;

        // event data OTF
        std::vector<int> _pdo;
        std::vector<int> _tdo;
        std::vector<int> _bcid;
        std::vector<int> _gray;
        std::vector<int> _channelNo;
        std::vector<int> _flag;
        std::vector<int> _thresh;
        std::vector<int> _neighbor;

        int m_eventNumberFAFA;
        int m_daqCnt;
        std::vector<int> m_fromIp;
        std::vector<int> m_triggerTimeStamp;
        std::vector<int> m_triggerCounter;
        std::vector<int> m_chipId;
        std::vector<int> m_eventSize;
        std::vector<int> m_art;
        std::vector<int> m_artFlag;
        std::vector< std::vector<int> > m_tdo;
        std::vector< std::vector<int> > m_pdo;
        std::vector< std::vector<int> > m_flag;
        std::vector< std::vector<int> > m_threshold;
        std::vector< std::vector<int> > m_bcid;
        std::vector< std::vector<int> > m_channelId;
        std::vector< std::vector<int> > m_grayDecoded;

        // calibration data
        int m_channel_for_calib;
        int m_pulserCounts_calib;
        double m_gain_calib;
        int m_peakTime_calib;
        int m_dacCounts_calib;
        double m_tpSkew_calib;
        std::vector< std::vector<int> > m_neighbor_calib;
        

        ///////////////////////////////////////////
        // output ntuples
        ///////////////////////////////////////////

        TFile* m_daqRootFile;
        bool m_rootFileOK;
        bool m_treesSetup;
        bool m_runPropertiesFilled;

        // output trees
        TTree*  m_vmm2;
        TBranch *br_eventNumberFAFA;
        TBranch *br_triggerTimeStamp;
        TBranch *br_triggerCounter;
        TBranch *br_fromIp;
        TBranch *br_chipId;
        TBranch *br_evSize;
        TBranch *br_tdo;
        TBranch *br_pdo;
        TBranch *br_flag;
        TBranch *br_thresh;
        TBranch *br_bcid;
        TBranch *br_grayDecoded;
        TBranch *br_channelId;
        TBranch *br_pulserCalib;
        TBranch *br_gainCalib;
        TBranch *br_peakTimeCalib;
        TBranch *br_threshCalib;
        TBranch *br_s6TPskewCalib;
        TBranch *br_calibRun;
        TBranch *br_neighborCalib;

        TTree*  m_runProperties;
        TBranch *br_runNumber;
        TBranch *br_gain;
        TBranch *br_tacSlope;
        TBranch *br_peakTime;
        TBranch *br_dacCounts;
        TBranch *br_pulserCounts;
        TBranch *br_s6TPskew;
        TBranch *br_angle;
        TBranch *br_calibrationRun;

        TTree*  m_artTree;
        TBranch *br_art;
        TBranch *br_artFlag;

        //------------------------XADC tree----------------------//
        TTree* m_xadctree;
        TBranch *br_xadc_chip;
        TBranch *br_xadc_board;
        TBranch *br_xadc_read1;
        TBranch *br_xadc_read2;
        TBranch *br_xadc_read3;
        TBranch *br_xadc_read4;
        TBranch *br_xadc_read5;
        //------------------------XADC tree--------------------------//


        
    signals :
        void checkDAQCount(bool);
        void setRunDirOK(bool);
        void eventCountStopReached();

    public slots :
        //testing sharted memory
        void testSharedMem();
        void readEvent();
        void EndRun();
        void writeAndCloseDataFile();
        void set_monitorData(bool);
        void clearSharedMemory();
        void setUseChannelMap(bool);
        void loadELxChannelMapping(QString);
        void setWriteNtuple(bool);
        void setIgnore16(bool);
        void setCalibrationRun(bool);
        void setCalibrationChannel(int);
        void updateCalibrationState(int,int,int,int,int);
        bool setupOutputFiles(QString, QString);
        void setupOutputTrees();
        void connectDAQSocket();
        void closeDAQSocket();

        //daqmon
        void daqHanging();

    private slots :


}; // class DataHandler

#endif
