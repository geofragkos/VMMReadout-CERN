#ifndef CONFMOD_H
#define CONFMOD_H

#include <QSettings>
#include <QFile>
#include <QList>
#include <QByteArray>
#include <QUdpSocket>
#include <QBitArray>
#include <QStringList>
#include <QDomDocument>

using namespace std;

class Configuration : public QObject
{

  Q_OBJECT

    public: 

        Configuration();
        ~Configuration();
        int Ping();
        int ReadCFile(QString &filename);
        int WriteCFile(QString &filename);
		int DumpConfigParams();
		int LoadRunParamsFromGUI(int tpd, QString tp, int as, int aw, QString rm);
		int LoadConfigFromGUI(int iBaseIP, int nIPs, quint16 chanMap, int spg, int slg, int sdrv,
                            int sfm, int sg, int st, int stc, int sng, int sdp, int sfa, int sfam,
                            int sdcka, int sbfm, int sbfp, int sbft, int sm5_sm0, int scmx,
                            int sbmx, int adcs, int hyst, int dtime, int dtimeMode1, int dtimeMode2,
                            int ebitconvmode, int sbitenable, int adc10b, int adc8b, int adc6b, 
                            int dualclockdata, int dualclock6bit, int thresDAC, int tpDAC, bool VMM1SPBool[64],
                            bool VMM1SCBool[64], bool VMM1SLBool[64], bool VMM1STBool[64], bool VMM1SMBool[64],
                            quint8 VMM1SDVoltage[64], bool VMM1SMXBool[64], quint8 VMM1SZ010bCBox[64],
                            quint8 VMM1SZ08bCBox[64], quint8 VMM1SZ06bCBox[64], bool debug_flag=false);
        int SendConfig();
        void Sender(QByteArray blockOfData, const QString &ip, quint16 destPort);
        void Sender(QByteArray blockOfData, const QString &ip, quint16 destPort, QUdpSocket& socket);
        quint32 ValueToReplace(QString datagramHex, int bitToChange, bool bitValue);
        int ResetTriggerCounter();
        QByteArray bitsToBytes(QBitArray bits);
        QBitArray bytesToBits(QByteArray bytes);

        //Getters
        bool getDebug() { return debug; }
		int getChPolarity() {return _chSP;}
		int getLeakCurrent() {return _leak;}
		int getAnalogTristates() {return _nanaltri;}
		int getDoubleLeak() {return _doubleleak;}
		int getGainVal() {return _gain;}
		int getPeakt() {return _peakt;}
		int getTACslope() {return _TACslop;}
		int getDisAtPeak() {return _dpeak;}
		int getART() {return _art;}
		int getARTmode() {return _artm;}
		int getDualClock() {return _dualclock;}
		int getSBFM() {return _sbfm;}
		int getSBFP() {return _sbfp;}
		int getSBFT() {return _sbft;}
		int getChanMon() {return _cmon;}
		int getSCMX() {return _scmx;}
		int getSBMX() {return _sbmx;}
		int getADCs() {return _adcs;}
		int getHyst() {return _hyst;}
		int getDirectTime() {return _dtime;}
		int getDirectTimeModeOne() {return _dtimeMode[0];}
		int getDirectTimeModeTwo() {return _dtimeMode[1];}
		int getEbitConvMode() {return _ebitconvmode;}
		int getSbitEnable() {return _sbitenable;}
		int getADC10b() {return _adc10b;}
		int getADC8b() {return _adc8b;}
		int getADC6b() {return _adc6b;}
		int getDualClockData() {return _dualclockdata;}
		int getDualClocksbit() {return _dualclock6bit;}
        int getThresholdDAC() {return _thresDAC;}
        int getTestPulseDAC() {return _tpDAC;}
        QString getGain() {return _gainstring;}
        int getPeakTime() {return _peakint;}
        QString getNeighborTrigger() {return _ntrigstring;}
        QStringList getIPs() { return _ips; }
        void getIPs(QStringList &list) {list=_ips;}
        void getVMMIDs(QStringList &list) {list=_ids;}
        QString getComment() {return _comment;}
        quint16 getChannelMap() {return channelMap;} 

        void getTrigDAQ(QStringList &slist, vector<int> &ilist){
		    	slist << Trig_Period;
		    	slist << Run_mode;
		    	slist << Out_Path;
		    	slist << Out_File;
		    	ilist.clear();
		    	ilist.push_back(TP_Delay);
		    	ilist.push_back(ACQ_Sync);
		    	ilist.push_back(ACQ_Window);
		    	ilist.push_back(Run_count);
        }
		void getChannelArrays (bool (&SPBool)[64], bool (&SCBool)[64], bool (&SLBool)[64],
                        bool (&STBool)[64], bool (&SMBool)[64], quint8 (&SDVoltage)[64],
                        bool (&SMXBool)[64], quint8 (&SZ010bCBox)[64], quint8 (&SZ08bCBox)[64],
                        quint8 (&SZ06bCBox)[64]){
			for (int ichan=0; ichan<64; ++ichan) {
				SPBool[ichan] = SP[ichan];
				SCBool[ichan] = SC[ichan];
				SLBool[ichan] = SL[ichan];
				STBool[ichan] = ST[ichan];
				SMBool[ichan] = SM[ichan];
				SDVoltage[ichan] = trim[ichan];
				SMXBool[ichan] = SMX[ichan];
				SZ010bCBox[ichan] = tenbADC[ichan];
				SZ08bCBox[ichan] = eightbADC[ichan];
				SZ06bCBox[ichan] = sixbADC[ichan];
			}
		}

        quint16 getFECPort()      { return FECPort; }
        quint16 getDAQPort()      { return DAQPort; }
        quint16 getVMMASICPort()  { return VMMASICPort; }
        quint16 getVMMAPPPort()   { return VMMAPPPort; }
        quint16 getS6Port()       { return S6Port; }
        QString getGainString()   { return _gainstring; }
        int getTACSlope()         { return _TACslop; }
        int getPeakInt()          { return _peakint; }

        void setMask();
        void linkPB();
        QByteArray getLinkStatusData();
        void resetLinks();
        void setEventHeaders(int bldInfo, int bldMode);
        void checkFEC(bool reset = false);
        void setS6_ck(int cktk_, int ckbc_, int ckbc_skew_);
        void setS6_Tp(int tp_skew_, int tp_width_, int tp_polarity_);
        

    private:

        QStringList _ips;
        QStringList _ids;
        QStringList _replies;
        QHostAddress* boardip;
        quint16 FECPort, DAQPort, VMMASICPort, VMMAPPPort, S6Port;
        quint32 commandCounter;
        bool pinged;
        bool bnd;
        QUdpSocket* socket;
        QByteArray buffer;

        bool debug;
	    QSettings* settings;
	    QDomNode *n; 
	    QDomNode n_sub;
	    QDomNodeList hdmis;
	    QFile *file;

	    //Trigger.DAQ setting
	    int TP_Delay, ACQ_Sync, ACQ_Window, Run_count;
	    QString Trig_Period, Run_mode, Out_Path, Out_File;

	    //Channel Map settings
	    bool hdmisw; 
	    QString hdmifi, hdmise;
	    quint16 channelMap;

        //Global settings
	    int _chSP;//ch.polarity
				QStringList _polarities;
        QString _comment;
        int _gain;//gain
        QString _gainstring;
				QStringList _gains;
        int _peakt;//peak.time
				QList<int> _peakts;
        int _peakint;
        int _art;//ART
        int _artm;//ART.mode
				QStringList _ARTmodes;
        int _cmon;//channel.monitoring
        int _scmx;//monitoring control
        int _sbmx;//monitor.pdo.out
        int _tpDAC;//test.pulse.DAC
        int _thresDAC;//threshold.DAC
        int _ntrig;//neighbor.trigger
        QString _ntrigstring;
        int _nanaltri;//analog.tristates
	    int _leak;//ch.leakage.current
	    int _doubleleak;//double.leakage
	    int _dualclock;//dual.clock.ART
        int _adcs;//ADCs
        int _TACslop;//TAC.slop.adj
				QList<int> _TACslops;
        int _hyst;//sub.hyst.discr
        int _dtime;//direct.time
        int _dtimeMode[2];//direct.time.mode
				QStringList _dTimeModes;
				QString _sdtm; //String for direct.time.mode
        int _sbfm;//out.buffer.mo
        int _sbfp;//out.buffer.pdo
        int _sbft;//out.buffer.tdo
        int _dpeak;//disable.at.peak
	    int _ebitconvmode;//conv.mode.8bit
	    int _sbitenable;//enable.6bit
	    int _adc10b;//ADC.10bit
			QStringList _adc10bs;
	    int _adc8b;//ADC.8bit
			QStringList _adc8bs;
	    int _adc6b;//ADC.6bit
			QStringList _adc6bs;
	    int _dualclockdata;//dual.clock.data
	    int _dualclock6bit;//dual.clock.6bit

        //Channel-by-channel info
        bool SMX[64];//smx
        int trim[64];//trim
        int tenbADC[64];//10bADCtimeset
        int eightbADC[64];//08bADCtimeset
        int sixbADC[64];//06bADCtimeset
        bool SM[64];//hidden.mode
        bool SP[64];//polarity
        bool SC[64];//sc
        bool SL[64];//sl
        bool ST[64];//test.pulse
 
        //Utilies
        void UpdateCounter();
        quint32 reverse32(QString datagramHex);
        bool EnaDisableKey( const QString &childKey, const QString &tmp);
        bool EnaDisableKey( const QString &childKey, const QString &tmp, int channel);
        bool OnOffKey( const QString &childKey, const QString &tmp);
        bool OnOffChannel( const QString &key, const QString &tmp, int channel);
		QString ValToOnOffXML(bool value, QString valOn, QString valOff);

        void Validate(QString sectionname);

  private slots:
        void processReply(const QString &_sentip, QUdpSocket* socket);
        void dumpReply(const QString &_sentip);
        void dataPending();

  signals:
        void checkStatus();
};

#endif
