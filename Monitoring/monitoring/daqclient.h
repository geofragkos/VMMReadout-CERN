#ifndef DAQCLIENT_H
#define DAQCLIENT_H

#include "daqclientconfig.h"
#include <boost/asio.hpp>

namespace online {
   namespace display {

        class daqClient
        {

            daqClientConfig m_config;

        public:
            daqClient(int &argc, char * argv[], boost::asio::io_service& io_service);
            virtual ~daqClient();

        private:
            void run_command_receiver();
            //ipc_command_type get_command(std::string msg);

        };
    }
}
#endif // DAQCLIENT_H
