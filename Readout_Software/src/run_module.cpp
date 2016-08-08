// vmm
#include "run_module.h"
#include "data_handler.h"

// std/stl
#include <math.h>
#include <iostream>
using namespace std;

// qt
#include <QDataStream>
#include <QTimer>
#include <QDataStream>

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  RunModule
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
RunModule::RunModule(QObject *parent) :
    QObject(parent),
    m_dbg(true),
    //addmmfe8
    m_mmfe8(false),
    m_msg(0),
    m_externalTrigger(false),
    m_pulseCount(0),
    m_initSocketHandler(false),
    m_initConfigHandler(false),
    m_socketHandler(0),
    m_configHandler(0)
{
}
// ------------------------------------------------------------------------ //
void RunModule::setMMFE8(bool set_for_mmfe8)
{
    m_mmfe8 = set_for_mmfe8;
    if(dbg() && m_mmfe8) {
        msg()("DAQ configuration set to MMFE8","RunModule::setMMFE8");
    }
}
// ------------------------------------------------------------------------ //
void RunModule::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------ //
RunModule& RunModule::LoadConfig(ConfigHandler& inconfig)
{
    m_configHandler = &inconfig;
    if(!m_configHandler) {
        msg()("ERROR ConfigHandler instance null", "RunModule::LoadConfig", true);
        exit(1);
    }
    else if(dbg()){
        msg()("ConfigHandler instance loaded", "RunModule::LoadConfig");
    }

    //depending on the run_mode set in the configuration
    //set whether doing a timed run or not
    bool setExternal = false;
    if(config().daqSettings().run_mode=="pulser")
        setExternal = false;
    else if(config().daqSettings().run_mode=="external") {
        setExternal = true;
        config().daqSettings().run_count *= 1000;
    }
    setExternalTrig(setExternal);

    m_initConfigHandler = true;

    return *this;
}
// ------------------------------------------------------------------------ //
RunModule& RunModule::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    if(!m_socketHandler) {
        msg()("ERROR SocketHandler instance null","RunModule::LoadSocket",true);
        exit(1);
    }
    else if(dbg()){
        msg()("SocketHandler instance loaded","RunModule::LoadSocket");
        m_socketHandler->Print();
    }

    m_initSocketHandler = true;

    return *this;
}
// ------------------------------------------------------------------------ //
void RunModule::prepareRun()
{
    if(dbg()) msg()("Preparing for DAQ...","RunModule::prepareRun");

    //configure the FE DAQ configuration
    setTriggerAcqConstants();
    setTriggerMode();
}
// ------------------------------------------------------------------------ //
void RunModule::Run()
{
    msg()("Starting run...","RunModule::Run");
    if(externalTrig())
        beginTimedRun();
    else
        beginPulserRun();
}
// ------------------------------------------------------------------------ //
void RunModule::beginTimedRun()
{
    QTimer::singleShot(config().daqSettings().run_count,
                        this, SLOT(finishRun()));
}
// ------------------------------------------------------------------------ //
void RunModule::beginPulserRun()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendPulse()));
    timer->start(100);
}
// ------------------------------------------------------------------------ //
void RunModule::finishRun()
{
    msg()("Ending run...","RunModule::finishRun");
    ACQoff();
    socket().closeAndDisconnect("daq","RunModule::finishRun");
    emit EndRun();
}
// ------------------------------------------------------------------------ //
void RunModule::readEvent()
{
    msg()("This method is not implemented!","RunModule::readEvent", true);
    exit(1);
    return;
}
// ------------------------------------------------------------------------ //
void RunModule::sendPulse()
{
    if(dbg()) msg()("Sending pulse...","RunModule::sendPulse");

    bool ok;
    QByteArray datagram;
    int send_to_port = config().commSettings().vmmapp_port;

    for(const auto& ip : socket().ipList()) {
 
        socket().updateCommandCounter();

        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);

        ///////////////////////////
        // header info
        ///////////////////////////
        QString header = "FEAD";
        out << (quint32) socket().commandCounter()  //[0,3]
            << (quint16) header.toUInt(&ok,16); //[4,5]

        ///////////////////////////
        // header info
        ///////////////////////////
        out << (quint16) 2  //[6,7]
            << (quint32) 1  //[8,11]
            << (quint32) 32; //[12,15]


        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "RunModule::SendPulse");

        bool readOK = false;
        readOK = socket().waitForReadyRead("fec");
        if(readOK)
            socket().processReply("fec", ip);
        else
            msg()("Timeout [1] while waiting for replies from VMM. Pulse lost.",
                    "RunModule::sendPulse");

        out.device()->seek(12);
        out << (quint32) 0;
        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                        "RunModule::SendPulse");
        readOK = socket().waitForReadyRead("fec");
        if(readOK)
            socket().processReply("fec", ip);
        else
            msg()("Timout [2] while waiting for replies from VMM. Pulse lost.",
                    "RunModule::sendPulse");
    } // ip loop


    socket().closeAndDisconnect("fec","RunModule::SendPulse");

    updatePulseCount();
    if(pulseCount() == config().daqSettings().run_count) {
        bool read = socket().waitForReadyRead("daq");
        if(read) {
            readEvent();
        }
        finishRun();
    }

}
// ------------------------------------------------------------------------ //
void RunModule::setTriggerAcqConstants()
{
    if(dbg()) msg()("Sending trigger ACQ constants...","RunModule::setTriggerAcqConstants");

    bool ok;
    QByteArray datagram;

    // send T/DAQ constants to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    for(const auto& ip : socket().ipList()) {
        // UPDATE COUNTER, ETC... SHOULD NOW BE DONE
        // SOLELY IN SOCKETHANDLER TO WHICH WE PASS
        // THE DATAGRAMS

        socket().updateCommandCounter();

        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); // rewind

        ///////////////////////////
        // header info
        ///////////////////////////
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd         = "AA";
        cmdType     = "AA";
        cmdLength   = "FFFF";
        msbCounter  = "0x80000000"; 
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            //<< (quint32) config().getHDMIChannelMap() //[8,11]
            << (quint8)  cmd.toUInt(&ok,16) //[8]
            << (quint8)  cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

        ///////////////////////////
        // trigger constants
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            //trigger period
            << (quint32) 2 //[16,19]
            << (quint32) config().daqSettings().trigger_period.toInt(&ok,16) //[20,23]
            //pulser delay
            << (quint32) 4 //[24,27]
            << (quint32) config().daqSettings().tp_delay //[28,31]
            //acq. sync
            << (quint32) 5 //[32,35]
            << (quint32) config().daqSettings().acq_sync //[36,39]
            //acq. window
            << (quint32) 6 //[40,43]
            << (quint32) config().daqSettings().acq_window //[44,47]
            //bcid reset
            << (quint32) 9 //[48,51]
            << (quint32) config().daqSettings().bcid_reset; //[52,55]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                "RunModule::setTriggerAcqConstants"); 

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::setTriggerAcqConstants");
            socket().processReply("fec", ip);
        }
        else {
            msg()("Timeout while waiting for replies from VMM",
                        "RunModule::setTriggerAcqConstants", true);
            socket().closeAndDisconnect("fec","RunModule::setTriggerAcqConstants");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::setTriggerAcqConstants");
}
// ------------------------------------------------------------------------ //
void RunModule::setTriggerMode()
{
    if(dbg()) msg()("Setting trigger mode...","RunModule::setTriggerMode");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    for(const auto& ip : socket().ipList()) {
        // UPDATE COUNTER, ETC... SHOULD NOW BE DONE
        // SOLELY IN SOCKETHANDLER TO WHICH WE PASS
        // THE DATAGRAMS

        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); // rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd         = "AA";
        cmdType     = "AA";
        cmdLength   = "FFFF";
        msbCounter  = "0x80000000"; 

        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // trigger mode
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 0; //[16,19]
        if(config().daqSettings().run_mode == "external"){
            out << (quint32) 4; //[20,23]
            if(dbg()) msg()("External trigger enabled","RunModule::setTriggerMode");
        } // external
        else {
            out << (quint32) 7; //[20,23]
            if(dbg()) msg()("Internal trigger enabled","RunModule::setTriggerMode");
        }
        
        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                "RunModule::setTriggerMode");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::setTriggerMode");
            socket().processReply("fec", ip);
        }
        else {
            msg()("Timeout while waiting for replies from VMM",
                        "RunModule::setTriggerMode",true);
            socket().closeAndDisconnect("fec","RunModule::setTriggerMode");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::setTriggerMode");
}
// ------------------------------------------------------------------------ //
void RunModule::ACQon()
{
    if(dbg()) msg()("Setting ACQ ON","RunModule::ACQon");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd        = "AA";
        cmdType    = "AA";
        cmdLength  = "FFFF";
        msbCounter = "0x80000000"; 

        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0  //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // ACQ on
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 15 //[16,19]
            << (quint32) 1; //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                    "RunModule::ACQon");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...", "RunModule::ACQon");
            socket().processReply("fec", ip);
        } // readOK
        else {
            msg()("Timeout while waiting for replies from VMM","RunModule::ACQon",true);
            socket().closeAndDisconnect("fec","RunModule::ACQon");
            exit(1);
        }
    } // ip
    socket().closeAndDisconnect("fec", "RunModule::ACQon");
}
// ------------------------------------------------------------------------ //
void RunModule::ACQoff()
{
    if(dbg()) msg()("Setting ACQ OFF","RunModule::ACQoff");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        QString cmd, cmdType, cmdLength, msbCounter;
        cmd         = "AA";
        cmdType     = "AA";
        cmdLength   = "FFFF";
        msbCounter  = "0x80000000";

        //stringstream sx;
        //sx << "AQCOFF command counter = " << socket().commandCounter();
        //msg()(sx);sx.str("");

        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // ACQ off
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 15 //[16,19]
            << (quint32) 0; //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::ACQoff [1]");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::ACQoff");
            QByteArray buffer;
            //buffer = socket().fecSocket().processReply(ip, 0, socket().commandCounter()); //.processReply("fec", ip);
            socket().processReply("fec", ip);

            //QByteArray buffer = socket().buffer("fec");

            // dantrim May 26 not sure why this second word is sent -- legacy from VMM1/MCgill code?
            //QString bin, hex;
            //QDataStream out (&buffer, QIODevice::WriteOnly);
            //hex = buffer.mid(12,4).toHex();
            //quint32 tmp32 = DataHandler::ValueToReplaceHEX32(hex, 0, false);
            //out.device()->seek(12);
            //out << tmp32;
            //out.device()->seek(6);
            //out << (quint16) 2; // change to write mode ?
            //socket().SendDatagram(buffer, ip, send_to_port, "fec",
            //                                    "RunModule::ACQoff [2]");
        }
        else {
            msg()("Timeout [1] while waiting for replies from VMM",
                    "RunModule::ACQoff", true); 
            socket().closeAndDisconnect("fec","RunModule::ACQoff");
            exit(1);
        }

        // not doing second loop
        //readOK = socket().waitForReadyRead("fec");
        //if(readOK) {
        //    socket().processReply("fec", ip);
        //}
        //else {
        //    msg()("Timeout [2] while waiting for replies from VMM",
        //            "RunModule::ACQoff", true);
        //    socket().closeAndDisconnect("fec","RunModule::ACQoff"); 
        //    exit(1);
        //}
    } // ip loop

    socket().closeAndDisconnect("fec", "RunModule::ACQoff");
}
// ------------------------------------------------------------------------ //
void RunModule::setEventHeaders(const int bld_info, const int bld_mode, bool highRes)
{
    if(dbg()) msg()("Setting event headers...","RunModule::setEventHeaders");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    // headers
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    // setup the word
    quint32 evbldinfo = 0;
    if(bld_info==0)             evbldinfo = 0;
    else if(bld_info==1)        evbldinfo = 256;
    else if(bld_info==2)        evbldinfo = 512;
    else if(bld_info==3)        evbldinfo = 768;
    else if(bld_info==4)        evbldinfo = 1024;
    else if(bld_info==5)        evbldinfo = 1280;
    //quint32 evbldinfo = (quint32) 256*bld_info;
    quint32 evbldmode = (quint32)bld_mode;

    //resolution
    quint32 resolutionBits = 0;
    if(highRes)
        resolutionBits = 32768;

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16) //[8,11]
            << (quint32) 0; //[12,15]

        ///////////////////////////
        // event header
        ///////////////////////////
        out << (quint32) 10 //[16,19]
            << (quint32) evbldmode //[20,23]
            << (quint32) 12 //[24,27]
            << (quint32) (evbldinfo + resolutionBits); //[28,31]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::setEventHeaders");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::setEventHeaders");
            socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                    "RunModule::setEventHeaders",true);
            socket().closeAndDisconnect("fec","RunModule::setEventHeaders");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::setEventHeaders");
}
// ------------------------------------------------------------------------ //
void RunModule::resetASICs()
{
    //if(dbg())
        msg()("Resetting VMMs...","RunModule::resetASICs");

    bool ok;
    QByteArray datagram;

    // send trigger mode to VMMAPP port
    int send_to_port = config().commSettings().vmmapp_port;

    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000"; 

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5] 
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // reset
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 128 //[16,19]
            << (quint32) 2; //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::resetASICs");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::resetASICs");
            socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                    "RunModule::resetASICs",true);
            socket().closeAndDisconnect("fec","RunModule::resetASICs");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::resetASICs");
}
// ------------------------------------------------------------------------ //
void RunModule::resetFEC(bool do_reset)
{
    if(dbg()) msg()("Resetting FEC...","RunModule::resetFEC");

    bool ok;
    QByteArray datagram;

    // send reset call to FEC port
    int send_to_port = config().commSettings().fec_port;

    // headers
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    // setup
    QString address = "FFFFFFFF";
    QString value = "";
    if(do_reset) {
        value = "FFFF8000";
        if(dbg()) msg()("Rebooting FEC...","RunModule::resetFEC");
    } else {
        value = "FFFF0001";
        if(dbg()) msg()("WarmInit FEC...","RunModule::resetFEC");
    } 

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ///////////////////////////
        // header info
        ///////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3] 
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ///////////////////////////
        // word
        ///////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) address.toUInt(&ok,16) //[16,19]
            << (quint32) value.toUInt(&ok,16); //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::resetFEC");
        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::resetFEC");
            socket().processReply("fec", ip);
        } else {
            if(dbg()) msg()("Timeout while waiting for replies from VMM",
                            "RunModule::resetFEC",true);
            socket().closeAndDisconnect("fec","RunModule::resetFEC");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::resetFEC");
}
// ------------------------------------------------------------------------ //
void RunModule::setMask()
{
    if(dbg()) msg()("Setting HDMI mask and ART...","RunModule::setMask");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = config().commSettings().vmmapp_port;

    // header
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 8 //[16,19]
            << (quint32) config().getHDMIChannelMapART(); //[20,23]
          //  << (quint32) config().getHDMIChannelMap(); //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::setMask");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::setMask");
            socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                    "RunModule::setMask",true);
            socket().closeAndDisconnect("fec", "RunModule::setMask");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::setMask");

}
// ------------------------------------------------------------------------ //
void RunModule::enableART(bool enabling, bool holdoff)
{
    if(enabling)
        msg()("Enabling ART...","RunModule::enableART");
    else
        msg()("Disabling ART...","RunModule::enableART");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = config().commSettings().s6_port;

    // header
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000"; 

    int holdoff_enable = 233;
    if(holdoff) holdoff_enable = 232;

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32) (socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]
          
        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 1 //[19,19]
            << (quint32) holdoff_enable; //[20,23]


        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::enableART");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::enableART");
            socket().processReply("fec",ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                        "RunModule::enableART", true);
            socket().closeAndDisconnect("fec","RunModule::enableART");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::enableART");

}
// ------------------------------------------------------------------------ //
void RunModule::checkLinkStatus()
{
    if(dbg()) msg()("Checking link status...","RunModule::checkLinkStatus");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = config().commSettings().vmmapp_port;

    // header
    QString cmd = "BBAAFFFF";
    QString msbCounter = "0x80000000"; 

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 16; //[16,19]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::checkLinkStatus");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            emit checkLinks();
            //if(dbg()) msg()("Processing replies...","RunModule::checkLinkStatus");
            //socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                    "RunModule::checkLinkStatus", true);
            socket().closeAndDisconnect("fec", "RunModule::checkLinkStatus");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::checkLinkStatus");

}
// ------------------------------------------------------------------------ //
void RunModule::resetLinks()
{
    if(dbg()) msg()("Resetting links...","RunModule::resetLinks");

    bool ok;
    QByteArray datagram;

    // send call to vmmapp port
    int send_to_port = config().commSettings().vmmapp_port;

    QString cmd, cmdType, cmdLength, msbCounter, cmdReset;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";
    cmdReset = "FF";

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header (1)
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok,16); //[10,11]

        ////////////////////////////
        // command (1)
        ////////////////////////////
        out << (quint32) 13 //[12,15]
            << (quint32) cmdReset.toUInt(&ok,16); //[16,19]
        
        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::resetLinks");

        datagram.clear();
        out.device()->seek(0);
        socket().updateCommandCounter();
        ////////////////////////////
        // header (2)
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint16) 0 //[4,5]
            << (quint16) config().getHDMIChannelMap() //[6,7]
            << (quint8) cmd.toUInt(&ok,16) //[8]
            << (quint8) cmdType.toUInt(&ok,16) //[9]
            << (quint16) cmdLength.toUInt(&ok, 16); //[10,11]

        ////////////////////////////
        // command (2)
        ////////////////////////////
        out << (quint32) 13 //[12,15]
            << (quint32) 0; //[16,19]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::resetLinks");
    } // ip

    socket().closeAndDisconnect("fec", "RunModule::resetLinks");

}
// ------------------------------------------------------------------------ //
void RunModule::s6clocks(int cktk, int ckbc, int ckbc_skew)
{
    if(dbg()) msg()("Setting S6 clocks...","RunModule::s6clocks");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = config().commSettings().s6_port;

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 6 //[16,19]
            << (quint32) (cktk*16) //[20,23]
            << (quint32) 7 //[24,27]
            << (quint32) ( ckbc + (ckbc_skew*16) ); //[28,31]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                            "RunModule::s6clocks");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::s6clocks");
            socket().processReply("fec",ip);
        } else {
            msg()("Timout while waiting for replies from VMM",
                    "RunModule::s6clocks", true);
            socket().closeAndDisconnect("fec","RunModule::s6clocks");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::s6clocks");

}
// ------------------------------------------------------------------------ //
void RunModule::setS6Resets(int s6_tk_pulses, bool set_s6_autoReset, bool set_s6_fecReset,
                                    int s6_fec_periodRest)
{
    if(dbg()) msg()("Setting s6 reset settings...","RunModule::setS6Resets");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = config().commSettings().s6_port;

    // header
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000"; 
    int s6_auto_reset = 0;
    int s6_fec_reset = 0;
    bool fec_reset = false;
    if(set_s6_autoReset) s6_auto_reset = 8;
    if(set_s6_fecReset) { s6_fec_reset = 32; fec_reset = true; }

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0; //[12,15]

        out << (quint32) 9 //[16,19]
            << (quint32)( s6_tk_pulses + s6_auto_reset + s6_fec_reset); //[20,23]

    //    int s6_auto = 0;
    //    int s6_fec = 0;
    //    if(set_s6_autoReset) s6_auto = 8;
    //    if(set_s6_fecReset) s6_fec = 16;

    //    out << (quint32) 9 //[16,19]
    //        << (quint32)( s6_tk_pulses + s6_auto + s6_fec); //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::setS6Resets");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...", "RunModule::setS6Resets");
            socket().processReply("fec", ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                                    "RunModule::setS6Resets", true);
            socket().closeAndDisconnect("fec","RunModule::setS6Resets");
            exit(1);
        }

        ////////////////////////////////
        // set periodic reset
        ////////////////////////////////

        bool resetSeek = out.device()->reset();
        if(resetSeek) {
            emit s6resetStatus(true);
            socket().updateCommandCounter();
            out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16))
                << (quint32) config().getHDMIChannelMap()
                << (quint32) cmd.toUInt(&ok,16);

            /////////////////////
            // command
            /////////////////////
            out << (quint32) 9
                << (quint32) s6_fec_periodRest;

            socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::setS6Resets");

            readOK = socket().waitForReadyRead("fec");
            if(readOK) {
                if(dbg()) msg()("Processing replies [2]...", "RunModule::setS6Resets");
                socket().processReply("fec", ip);
            } else {
                msg()("Timeout while waiting for replies from VMM [2]",
                                    "RunModule::setS6Resets", true);
                socket().closeAndDisconnect("fec","RunModule::setS6Resets");
                exit(1);
            } // readok
        } //resetSeek
        else {
            msg()("Error upon resetting datastream seek. Unable to send period reset command for FEC",
                                        "RunModule::setS6Resets");
            emit s6resetStatus(false);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::setS6Resets");

}


// ------------------------------------------------------------------------ //
void RunModule::configTP(int tpskew, int tpwidth, int tppolarity)
{
    if(dbg()) msg()("Configuring the pulser...","RunModule::configTP");

    bool ok;
    QByteArray datagram;

    // send call to s6 port
    int send_to_port = config().commSettings().s6_port;

    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000"; 

    for(const auto& ip : socket().ipList()) {
        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind

        socket().updateCommandCounter();

        ////////////////////////////
        // header
        ////////////////////////////
        out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
            << (quint32) config().getHDMIChannelMap() //[4,7]
            << (quint32) cmd.toUInt(&ok,16); //[8,11]

        ////////////////////////////
        // command
        ////////////////////////////
        out << (quint32) 0 //[12,15]
            << (quint32) 2 //[16,19]
            << (quint32) (tpskew + (tpwidth*16) + (tppolarity*128)); //[20,23]

        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                                "RunModule::configTP");

        bool readOK = true;
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...","RunModule::configTP");
            socket().processReply("fec",ip);
        } else {
            msg()("Timeout while waiting for replies from VMM",
                    "RunModule::configTP", true);
            socket().closeAndDisconnect("fec","RunModule::configTP");
            exit(1);
        }
    } // ip

    socket().closeAndDisconnect("fec","RunModule::configTP");

}












