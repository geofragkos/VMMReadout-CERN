
// Qt
#include <QUdpSocket>

// vmm
#include "message_handler.h"

/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  VMMSocket
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

class VMMSocket : public QObject
{
    Q_OBJECT;

    public :
        explicit VMMSocket(QObject *parent = 0);
        virtual ~VMMSocket(){};

        VMMSocket& setDebug(bool dbg) { m_dbg = dbg; return *this; }
        bool dbg() { return m_dbg; }

        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        void setName(std::string n = "") { m_name = n; }
        std::string getName() { return m_name; }
        void setBindingPort(quint16 port) { m_bindingPort = port; }
        quint16 getBindingPort() { return m_bindingPort; }

        // QUdpMethods
        virtual bool hasPendingDatagrams();
        virtual quint64 pendingDatagramSize();
        virtual quint64 readDatagram(char* databuffer, quint64 maxSize,
                QHostAddress* address = 0, quint16* port = 0);

        bool bindSocket(quint16 port = 0,
            QAbstractSocket::BindMode mode = QAbstractSocket::DefaultForPlatform);
        bool isBound();
        quint64 writeDatagram(const QByteArray& data, const QHostAddress& host,
                    quint16 port);

        // dummy function to send string datagram to specific port on local host
        void TestUDP();

        bool checkAndReconnect(std::string fromWhere="");
        void closeAndDisconnect(std::string fromWhere="");


        QUdpSocket& socket() { return *m_socket; }
        QByteArray buffer() { return m_buffer; }
        QByteArray processReply(const QString &ip_sent_to, quint32 cmd_delay = 0,
                                quint32 count = 0);

        void Print();

    signals :
        void dataReady();

    public slots :
        void readyRead();

    private :
        bool m_dbg;
        MessageHandler *m_msg;
        std::string m_name;
        quint16 m_bindingPort;
        QUdpSocket *m_socket;
        QByteArray m_buffer;


}; // class VMMSocket
