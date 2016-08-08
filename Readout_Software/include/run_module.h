
#ifndef RUN_MODULE_H
#define RUN_MODULE_H

// vmm
#include "socket_handler.h"
#include "config_handler.h"
#include "message_handler.h"

// qt
#include <QObject>

class RunModule : public QObject
{
    Q_OBJECT

    public :
        explicit RunModule(QObject *parent = 0);
        virtual ~RunModule(){};
        RunModule& setDebug(bool dbg) { m_dbg = dbg; return *this; }
        bool dbg() { return m_dbg; }

        //addmmfe8
        void setMMFE8(bool set_to_run_mmfe8);
        bool mmfe8() { return m_mmfe8; }

        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        ////////////////////////////////////////////
        // Run toggles
        ////////////////////////////////////////////
        /**
         set flag for running with external or internal
         triggers
        */
        void setExternalTrig(bool doit) { m_externalTrigger = doit; }
        bool externalTrig() { return m_externalTrigger; }

        /// pulse counter for running with internal trigger
        void updatePulseCount() { m_pulseCount++; }
        long int pulseCount() { return m_pulseCount; }

        ///////////////////////////////////////////
        // VMM handles
        ///////////////////////////////////////////
        RunModule& LoadConfig(ConfigHandler& config);
        RunModule& LoadSocket(SocketHandler& socket);

        SocketHandler& socket() { return *m_socketHandler; }
        ConfigHandler& config() { return *m_configHandler; }

        ///////////////////////////////////////////
        // Methods for setting up T/DAQ
        // and sending the configuration
        ///////////////////////////////////////////
        void prepareRun();
        void setTriggerAcqConstants();
        void setTriggerMode();
        void ACQon();
        void ACQoff();
        void beginTimedRun();
        void beginPulserRun();


        //////////////////////////////////////////////////////
        // Misc. methods
        //////////////////////////////////////////////////////
        void setEventHeaders(const int evbld_info, const int evbld_mode, bool highRes=false);
        void resetASICs();
        void resetFEC(bool do_reset);
        void setMask();
        void enableART(bool enabling, bool enableHoldOff);
        void checkLinkStatus();
        void resetLinks();
        void s6clocks(int cktk, int ckbc, int ckbc_skew);
        void setS6Resets(int s6_tk_pulses, bool set_s6_autoReset, bool set_s6_fecReset,
                                int s6_fec_periodReset);
        void configTP(int tpskew, int tpwidth, int tppolarity);

    private :
        bool m_dbg;
        //addmmfe8
        bool m_mmfe8;
        MessageHandler *m_msg;
        bool m_externalTrigger;
        long int m_pulseCount; 
        bool m_initSocketHandler;
        bool m_initConfigHandler;

        SocketHandler *m_socketHandler;
        ConfigHandler *m_configHandler;

    signals :
        void EndRun();
        void checkLinks();
        void s6resetStatus(bool);

    public slots :
        void Run();
        void readEvent();
        void sendPulse();
        void finishRun();

}; // class RunModule

#endif
