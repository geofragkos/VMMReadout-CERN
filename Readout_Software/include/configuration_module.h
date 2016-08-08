
#ifndef CONFIGURATION_MODULE_H
#define CONFIGURATION_MODULE_H

// vmm
#include "config_handler.h"
#include "socket_handler.h"
#include "message_handler.h"

// Qt
#include <QString>


//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  Configuration
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////

class Configuration : public QObject
{
    Q_OBJECT

    public :
        explicit Configuration(QObject *parent = 0);
        virtual ~Configuration(){};
        Configuration& setDebug(bool dbg) { m_dbg = dbg; return *this; }
        bool dbg() { return m_dbg; }

        //addmmfe8
        void setMMFE8(bool set_to_run_mmfe8);
        bool mmfe8() { return m_mmfe8; }

        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        Configuration& LoadConfig(ConfigHandler& config);
        Configuration& LoadSocket(SocketHandler& socket);

        quint32 reverse32(QString binary_in);
        QString reverseString(QString string);
        void SendConfig();
        void fillGlobalRegisters(std::vector<QString>& globalRegisters);
        void fillChannelRegisters(std::vector<QString>& channelRegisters);
        SocketHandler& socket() { return *m_socketHandler; }
        ConfigHandler& config() { return *m_configHandler; }

    private :
        bool m_dbg;
        //addmmfe8
        bool m_mmfe8;

        SocketHandler *m_socketHandler;
        ConfigHandler *m_configHandler;
        MessageHandler* m_msg;


}; // class Configuration



#endif
