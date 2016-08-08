
// vmm
#include "vmmsocket.h"

// Qt
#include <QByteArray>

// std/stl
#include <sstream>
#include <iostream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  VMMSocket
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

VMMSocket::VMMSocket(QObject* parent) :
    QObject(parent),
    m_dbg(false),
    m_msg(0),
    m_name(""),
    m_bindingPort(0),
    m_socket(0)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

}
// ----------------------------------------------------------------------- //
void VMMSocket::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::bindSocket(quint16 port, QAbstractSocket::BindMode mode)
{
    #warning TODO check status and if already bound
    bool bind = true;
    if(m_socket) {
        bind = m_socket->bind(port, mode);
    }
    else {
        msg()("Socket named " + getName() + " is null!",
                "VMMSocket::bindSocket",true);
        bind = false;
    }
    if(!bind) {
        msg()("Unable to bind socket named " + getName(),
                "VMMSocket::bindSocket", true);
    }
    else {
        stringstream sx;
        sx << "Socket named " << getName() << " successfully bound to port " << port;
        msg()(sx, "VMMSocket::bindSocket");
    }
    return bind;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::isBound()
{
    return socket().state() == 4;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::hasPendingDatagrams()
{
    return m_socket->hasPendingDatagrams();
}
// ----------------------------------------------------------------------- //
quint64 VMMSocket::pendingDatagramSize()
{
    return m_socket->pendingDatagramSize();
}
// ----------------------------------------------------------------------- //
quint64 VMMSocket::readDatagram(char* data, quint64 maxSize,
                    QHostAddress* address, quint16* port)
{
    return m_socket->readDatagram(data, maxSize, address, port);
}
// ----------------------------------------------------------------------- //
void VMMSocket::TestUDP()
{
    checkAndReconnect("VMMSocket::TestUDP");

    QByteArray data;
    data.append("hello from udp");
    data.append("  from socket named: ");
    data.append(QString::fromStdString(getName()));
    m_socket->writeDatagram(data, QHostAddress::LocalHost, 1234);

    closeAndDisconnect();
}
// ----------------------------------------------------------------------- //
void VMMSocket::readyRead()
{
    if(dbg()) msg()(getName() + " socket receiving data...",
                "VMMSocket::readyRead");

    if     (getName()=="fec" || getName()=="FEC")
        emit dataReady();
}
// ----------------------------------------------------------------------- //
quint64 VMMSocket::writeDatagram(const QByteArray& datagram,
            const QHostAddress& host, quint16 port)
{
    return m_socket->writeDatagram(datagram, host, port);
}
// ----------------------------------------------------------------------- //
bool VMMSocket::checkAndReconnect(std::string callingFn)
{
    stringstream sx;
    bool status = true;
    if(m_socket->state() == QAbstractSocket::UnconnectedState) {
        status = false;
        string fn = "";
        if(callingFn!="") fn = "(" + callingFn + ") ";

        if(dbg()) {
            sx << fn << "About to rebind socket named " << getName() << " to port "
               << getBindingPort();
            msg()(sx, "VMMSocket::checkAndReconnect");
        }
        bool bnd = m_socket->bind(getBindingPort(), QUdpSocket::ShareAddress);
        if(!bnd) {
            status = false;
            sx.str("");
            sx << fn << "ERROR Unable to re-bind socket named " << getName() << "to port "
               << getBindingPort();
            msg()(sx, "VMMSocket::checkAndReconnect");
            closeAndDisconnect(callingFn);
        }
        else {
            status = true;
            if(dbg()) { 
                sx.str("");
                sx << fn << "Socket named " << getName() << " successfully rebound to port "
                   << getBindingPort();
                msg()(sx,"VMMSocket::checkAndReconnect");
            }
        }
    }
    return status;
}
// ----------------------------------------------------------------------- //
void VMMSocket::closeAndDisconnect(std::string callingFn)
{
    stringstream sx;
    string fn = "";
    if(callingFn!="") fn = "(" + callingFn + ") ";
    if(dbg())
        sx << fn << "Closing and disconnecting from host the socket"
           << " named " << getName() << " (bound on port " << getBindingPort()
           << ")";
        msg()(sx,"VMMSocket::closeAndDisconnect");
    m_socket->close();
    m_socket->disconnectFromHost();
}
// ----------------------------------------------------------------------- //
QByteArray VMMSocket::processReply(const QString &ip_to_check, quint32 cmd_delay,
                                    quint32 globalCount)
{
    stringstream sx;
    //debug
    sx << getName() << " socket processing replies for IP: " + ip_to_check.toStdString();
    if(dbg()) msg()("Processing datagram replies for IP: " + ip_to_check.toStdString(),
                        "VMMSocket::processReply");

    bool ok;
    QString datagram_hex;
    unsigned int cmd_cnt_to_check = globalCount - cmd_delay; 
    QHostAddress vmmIP;

    QStringList replies; 
    replies.clear();
    QByteArray datagram;
    datagram.clear();

    while(socket().hasPendingDatagrams()) {

        //debug
        if(dbg()) {
            sx.str("");
            sx << "socket " << getName() << " has datagrams";
            msg()(sx,"VMMSocket::processReply");sx.str("");
        }

        //debug
        datagram.resize(socket().pendingDatagramSize());
        socket().readDatagram(datagram.data(), datagram.size(), &vmmIP);
        //QString vmm_ip = vmmIP.toIPv4Address().toString();

      //  buffer().resize(socket().pendingDatagramSize());
      //  socket().readDatagram(buffer().data(), buffer().size(), &vmmIP);
      //  qDebug() << "BLAH BLAHreceived datagram hex: " << buffer().toHex();

        if(dbg()) {
            //debug
            sx.str("");
            sx << "Received datagram (hex): \n"
               << datagram.toHex().toStdString() << "\n"
               //<< "from VMM with IP: " << vmm_ip 
               << "from VMM with IP: " << vmmIP.toString().toStdString()
               << " and message size is " << datagram.size();
            msg()(sx,"VMMSocket::processReply");sx.str("");

          //  sx.str("");
          //  sx << "Received datagram (hex): \n"
          //     << buffer().toHex().toStdString() << "\n"
          //     << "from VMM with IP: " << vmmIP.toString().toStdString()
          //     << " and message size is " << buffer().size();
          //  msg()(sx, "VMMSocket::processReply");
        }

        datagram_hex.clear();
        //debug
        datagram_hex = datagram.mid(0,4).toHex();
        //datagram_hex = buffer().mid(0,4).toHex();
        quint32 received = datagram_hex.toUInt(&ok,16);
        if(received != cmd_cnt_to_check) {
            sx.str("");
            sx << "Command number received (" << received << ") does not match "
               << "internal command counter expected (" << cmd_cnt_to_check
               << ")";
            msg()(sx, "VMMSocket::processReply",true);
            //debug
            //exit(1);
        }

        // fill our list of VMM replies
        //replies.append(vmm_ip);
        replies.append(vmmIP.toString());
    }//while

    if(dbg()) {
        for(const auto& ip : replies) {
            sx.str("");
            sx << "VMM with IP [" << ip.toStdString() << "] sent a reply to "
               << "command number: " << cmd_cnt_to_check;
            msg()(sx,"VMMSocket::processReply");
        } // ip
    }

    if(replies.size()>0) {
        for(const auto& ip : replies) {
            // unexpected ip has replied -- let us know which IP's are bad
            if(ip != ip_to_check) {
                sx.str("");
                sx << "VMM with IP [" << ip.toStdString() << " has sent a reply"
                   << " at command " << cmd_cnt_to_check << " to a command not"
                   << " not sent to it! Out of sync.";
                msg()(sx, "VMMSocket::processReply",true);
                //debug
                //exit(1);
            } // unexpected ip
        } // ip
    } // replies > 0

    if(!replies.contains(ip_to_check)) {
        sx.str("");
        sx << "VMM with IP " << ip_to_check.toStdString() << " did not"
           << " acknowledge command number: " << cmd_cnt_to_check;
        msg()(sx,"VMMSocket::processReply",true);
        //debug
        //exit(1);
    }

    return datagram;

}
// ----------------------------------------------------------------------- //
void VMMSocket::Print()
{
    stringstream ss;
    ss << "    Name          : " << getName() << "\n"
       << "    Bound to port : " << getBindingPort() << "\n"
       << "    Status        : " << m_socket->state();
    msg()(ss,"VMMSocket::Print");
}
