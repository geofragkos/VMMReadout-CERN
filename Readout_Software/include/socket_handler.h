
#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

// Qt
#include <QObject>
#include <QUdpSocket>
#include <QStringList>

// vmm
#include "vmmsocket.h"
#include "message_handler.h"

// std/stl
#include <string>


/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  SocketHandler
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

class SocketHandler : public QObject
{
    Q_OBJECT;

    public :
        explicit SocketHandler(QObject *parent = 0);
        virtual ~SocketHandler(){};
        SocketHandler& setDebug(bool dbg) { m_dbg = dbg; return *this; }
        bool dbg() { return m_dbg; }

        //addmmfe8
        void setMMFE8(bool set_to_run_mmfe8);
        bool mmfe8() { return m_mmfe8; }

        //addxadc
        void setXADC(bool set_to_run_xadc);
        bool xadc() {return m_xadc;}


        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        void setDryRun();
        bool dryrun() { return m_dryrun; }

        // Load in the list of IP's
        SocketHandler& loadIPList(const QString& iplist);
        QStringList& idList() { return m_idlist; }
        QStringList& ipList() { return m_iplist; }
        bool ping();
        bool pinged() { return m_pinged; }

        // update global command counter
        void updateCommandCounter();
        quint32 commandCounter() { return n_globalCommandCounter; }
        // reset global command counter
        void resetCommandCounter();

        // add sockets
        void addSocket(std::string name = "",
            quint16 bindingPort = 0,
            QAbstractSocket::BindMode mode = QAbstractSocket::DefaultForPlatform);

        // send data
        void SendDatagram(const QByteArray& datagram, const QString& ip,
                            const quint16& destinationPort,
                            const QString& whichSocket = "",
                            const QString& callingFn = "");

        virtual bool waitForReadyRead(std::string socketName="", int msec=1000);
        QByteArray processReply(std::string name="", const QString& ip_sent_to="",
                        quint32 cmd_delay=0);
        void closeAndDisconnect(std::string name="", std::string callingFn="");

        // retrieve sockets
        bool fecSocketOK();
        VMMSocket& fecSocket()    { return *m_fecSocket; }
        bool vmmappSocketOK();
        VMMSocket& vmmappSocket() { return *m_vmmappSocket; }
        bool daqSocketOK();
        VMMSocket& daqSocket()    { return *m_daqSocket; }
        QByteArray buffer(std::string name="");

        // Print
        void Print();

    private :
        bool m_dbg;
        //addmmfe8
        bool m_mmfe8;

        //addxadc
        bool m_xadc;
        MessageHandler *m_msg;
        bool m_pinged;
        bool m_dryrun;
        bool m_skipProcessing;
        quint32 n_globalCommandCounter;
        VMMSocket *m_fecSocket;
        bool m_fecSetup;
        VMMSocket *m_vmmappSocket;
        bool m_vmmappSetup;
        VMMSocket *m_daqSocket;
        bool m_daqSetup;

        QStringList m_idlist;
        QStringList m_iplist;

        // retrieve socket by name
        VMMSocket& getSocket(std::string whichSocket="");

    signals :
        void commandCounterUpdated();

    public slots :

}; // class SocketHandler


#endif
