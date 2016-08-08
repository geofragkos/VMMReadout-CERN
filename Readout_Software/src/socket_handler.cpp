
// vmm
#include "socket_handler.h"

// Qt
#include <QProcess>
#include <QByteArray>

// std/stl
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------- //
//  SocketHandler
// ---------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////

SocketHandler::SocketHandler(QObject* parent) :
    QObject(parent),
    m_dbg(false),
    //addmmfe8
    m_mmfe8(false),
    m_msg(0),
    m_pinged(false),
    m_dryrun(false),
    m_skipProcessing(false),
    n_globalCommandCounter(0),
    m_fecSocket(0),
    m_fecSetup(false),
    m_vmmappSocket(0),
    m_vmmappSetup(false)
{
}
// ---------------------------------------------------------------------- //
//addmmfe8
void SocketHandler::setMMFE8(bool set_for_mmfe8)
{
    m_mmfe8 = set_for_mmfe8;
    if(dbg() && m_mmfe8) {
        msg()("Socket handling set to MMFE8","SocketHandler::setMMFE8");
    }

    if(m_mmfe8) {
        msg()("Set to MMFE8. Will skip reply processing.","SocketHandler::setMMFE8");
        m_skipProcessing = true;
    }
}
// ---------------------------------------------------------------------- //
void SocketHandler::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ---------------------------------------------------------------------- //
void SocketHandler::setDryRun()
{
    stringstream sx;
    sx << "SocketHandler configured for dry run (will not send anything "
       << "over the network)";
    msg()(sx,"SocketHandler::setDryRun");
    m_dryrun = true;
}

// ---------------------------------------------------------------------- //
SocketHandler& SocketHandler::loadIPList(const QString& ipstring)
{
    m_iplist.clear();
    m_iplist << ipstring.split(",");
    stringstream sx;
    sx << "Loaded " << m_iplist.size() << " IP addresses:";
    msg()(sx, "SocketHandler::loadIPList"); sx.str("");
    for(const auto& ip : m_iplist) {
        sx << "    > " << ip.toStdString() << "\n";
        msg()(sx,"SocketHandler::loadIPList"); sx.str("");
    }
    return *this;
}
// ---------------------------------------------------------------------- //
bool SocketHandler::ping()
{
    //addmmfe8
    if(!mmfe8()) {
        if(m_dbg) {
            msg()("Pinging IP address...","SocketHandler::ping");
        }
        if(m_iplist.size()==0) {
            stringstream sx;
            sx << "ERROR There are no IP addresses loaded. Please use method 'loadIPList'";
            msg()(sx,"SocketHandler::ping");
            m_pinged = false;
        }
        for(const auto& ip : m_iplist) {
            #ifdef __linux__
            int status_code = QProcess::execute("ping", QStringList()<<"-c1"<<ip);
            #elif __APPLE__
            int status_code = QProcess::execute("ping", QStringList()<<"-t1"<<ip);
            #endif

            //////////////////////////////////
            if(!dryrun()) {
                if(status_code == 0)
                    m_pinged = true;
                else {
                    m_pinged = false;
                    msg()("ERROR Unable to successfully ping the IP: " + ip.toStdString(),
                            "SocketHandler::ping");
                } 
            }
            else {
                m_pinged = true;
            }
            //////////////////////////////////
        }
    } else {
        //mmfe8 does not receive ping
        m_pinged = true;
    }
    return m_pinged;
}
// ---------------------------------------------------------------------- //
void SocketHandler::updateCommandCounter()
{
    n_globalCommandCounter++;
    emit commandCounterUpdated();
}
// ---------------------------------------------------------------------- //
void SocketHandler::resetCommandCounter()
{
    n_globalCommandCounter = 0;
    emit commandCounterUpdated();
}
// ---------------------------------------------------------------------- //
void SocketHandler::addSocket(std::string name, quint16 bindingPort,
                                    QAbstractSocket::BindMode mode)
{
    bool bind = true;
    QString lname = QString::fromStdString(name).toLower();
    if(lname=="fec") {
        if(m_fecSetup) return;
        m_fecSocket = new VMMSocket();
        m_fecSocket->LoadMessageHandler(msg());
        m_fecSocket->setDebug(dbg());
        m_fecSocket->setName(name);
        m_fecSocket->setBindingPort(bindingPort);
        if(!dryrun())
            bind = m_fecSocket->bindSocket(bindingPort, mode);

        if(bind) m_fecSetup = true;
        msg()("VMMSocket added:","SocketHandler::addSocket");
        m_fecSocket->Print();

    } // fec
//    else if(lname=="daq") {
//     //   if(m_daqSetup) return;
//        m_daqSocket = new VMMSocket();
//        m_daqSocket->LoadMessageHandler(msg());
//        m_daqSocket->setDebug(dbg());
//        m_daqSocket->setName(name);
//        m_daqSocket->setBindingPort(bindingPort);
//     //   if(!dryrun())
//     //       m_daqSocket->bindSocket(bindingPort, mode);
//        
//        m_daqSetup = true;
//        msg()("VMMSocket added:","SocketHandler::addSocket");
//        m_daqSocket->Print();
//    } //daq
    else if(lname=="vmmapp") {
        if(m_vmmappSetup) return;
        m_vmmappSocket = new VMMSocket();
        m_vmmappSocket->LoadMessageHandler(msg());
        m_vmmappSocket->setDebug(dbg());
        m_vmmappSocket->setName(name);
        m_vmmappSocket->setBindingPort(bindingPort);
        if(!dryrun())
            bind = m_vmmappSocket->bindSocket(bindingPort, mode);

        if(bind) m_vmmappSetup = true;
        msg()("VMMSocket added:","SocketHandler::addSocket");
        m_vmmappSocket->Print();
    } //vmmapp
    else {
        stringstream sx;
        sx << "ERROR Current can only add the 'fec' or 'vmmapp' sockets\n"
           << "ERROR You have attemped to add a socket named: " << name;
        msg()(sx,"SocketHandler::addSocket",true);
        exit(1);
    }
}
// ---------------------------------------------------------------------- //
bool SocketHandler::fecSocketOK()
{
    bool status = true;
    if(!m_fecSocket) status = false;
    return status;
}
// ---------------------------------------------------------------------- //
bool SocketHandler::vmmappSocketOK()
{
    bool status = true;
    if(!m_vmmappSocket) status = false;
    return status;
}
// ---------------------------------------------------------------------- //
void SocketHandler::SendDatagram(const QByteArray& datagram, const QString& ip,
            const quint16& destPort, const QString& whichSocket, const QString& callingFn)
{
    std::string fn = "";
    if(callingFn!="") fn = "(" + callingFn.toStdString() + ") ";

    // get the requested VMMSocket
    VMMSocket& socket = getSocket(whichSocket.toStdString());

    //CHECK STATUS ENUM
    #warning TODO add STATUS ENUM AND CHECK (FSM)
    if(!pinged()) {
        msg()("ERROR Boards are not in pinged OK state...",
                "SocketHandler::SendDatagram", true);
        exit(1);
    }

    // make sure the socket is connected (bound) to the correct port
    if(!dryrun()) {
        if(!socket.checkAndReconnect(callingFn.toStdString()))
            exit(1);
    }

    // now send the data
    bool ok;
    if(dbg()) {
        stringstream sx;
        sx << fn
           << (dryrun() ? "[dry run]" : "")
           << " Data from socket '" << socket.getName() << "' sent to "
           << "(IP,port) = (" << ip.toStdString() << ", " << destPort << ") :\n"
           << datagram.toHex().toStdString() << "\n";
        msg()(sx,"SocketHandler::SendDatagram");

    }
    if(!dryrun())
        socket.writeDatagram(datagram, QHostAddress(ip), destPort);
}
// ---------------------------------------------------------------------- //
bool SocketHandler::waitForReadyRead(std::string name, int msec)
{
    bool status = false;
    if(dryrun() || m_skipProcessing) status = true;
    else {
        VMMSocket& vmmsocket = getSocket(name);
        status = vmmsocket.socket().waitForReadyRead(msec);
    }
    return status;
}
// ---------------------------------------------------------------------- //
QByteArray SocketHandler::processReply(std::string name, const QString& ip_to_check,
                    quint32 cmd_delay)
{
    QByteArray outbuffer;
    quint32 count = commandCounter();

    if(dryrun() || m_skipProcessing) {
        cout << "SocketHandler::processReply    Not processing reply from FE" << endl;
        return outbuffer;
    }

    VMMSocket& socket = getSocket(name);
    outbuffer = socket.processReply(ip_to_check, cmd_delay, count);
    return outbuffer;
}
// ---------------------------------------------------------------------- //
void SocketHandler::closeAndDisconnect(std::string name, std::string callingFn)
{
    if(dryrun())
        return;

    VMMSocket& vmmsocket = getSocket(name);
    vmmsocket.closeAndDisconnect(callingFn);
}
// ---------------------------------------------------------------------- //
QByteArray SocketHandler::buffer(std::string name)
{
    VMMSocket& vmmsocket = getSocket(name);
    return vmmsocket.buffer(); 
}
// ---------------------------------------------------------------------- //
VMMSocket& SocketHandler::getSocket(std::string whichSocket)
{
    stringstream sx;
    if(whichSocket=="") {
        sx.str("");
        sx << "ERROR This method must be passed a string containing the name of "
           << "the desired socket";
        msg()(sx,"SocketHandler::getSocket",true);
        exit(1);
    }
    QString lname = QString::fromStdString(whichSocket).toLower();
    if(lname=="fec") {
        if(m_fecSocket) return *m_fecSocket;
        else {
            msg()("Requested socket (fec) is null!","SocketHandler::getSocket",true);
            exit(1);
        }
    }
    else if(lname=="vmmapp") {
        if(m_vmmappSocket) return *m_vmmappSocket;
        else {
            msg()("Requested socket (vmmapp) is null!","SocketHandler::getSocket",true);
            exit(1);
        }
    }
    else {
        sx.str("");
        sx << "ERROR Currently can only retrieve the 'fec' or 'vmmapp' sockets.\n"
           << "ERROR You have attempted to retrieve a socket named: " << whichSocket;
        msg()(sx, "SocketHandler::getSocket",true);
        exit(1);
    }
}
// ---------------------------------------------------------------------- //
void SocketHandler::Print()
{
    if(!m_fecSocket && !m_vmmappSocket) {
        if(dbg())
            msg()("SocketHandler currently holds no sockets!",
                                                "SocketHandler::Print");
    }
    if(m_fecSocket)
        fecSocket().Print();
    if(m_vmmappSocket)
        vmmappSocket().Print();
}
