#include "daqclient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

using namespace online::display;
namespace bip = boost::interprocess;

daqClient::daqClient(int &argc, char *argv[], boost::asio::io_service& io_service) :
    m_config(argc, argv)
{
    try {
       //declare maximum message size
       size_t const max_msg_size = 0x1000; //new
       std::string send_string = m_config.get_command_string();
       //Create a shared memory and a message_queue.
       bip::shared_memory_object shm (bip::create_only, "shared_memory", bip::read_write);//new

       bip::message_queue ipc_queue(bip::open_only             //new :open_or_create/create_only? to check
                                     , "mmdaq1_srv_command");
       ipc_queue.send(send_string.data(), send_string.size(), 0);
    } catch (bip::interprocess_exception & e) {
       //bip::shared_memory_object::remove("shared_memory");
       std::cout << "mmdaq-server not running:  icp queue error: " << e.what() << std::endl;
    }


    bip::shared_memory_object::remove("shared_memory");

    ;

}

daqClient::~daqClient()
{

}

void daqClient::run_command_receiver()
{
/*    bool error = 0;
    try {
      //declare maximum message size
      size_t const max_msg_size = 0x1000; //new
      std::string command_received;
      size_t recvd_size;
      unsigned int priority;
      bip::message_queue queue_receive(bip::open_only             //new :open_or_create/create_only? to check
                                    , "mmdaq1_srv_rev_command");
      boost::system_time timeout = boost::get_system_time() + boost::posix_time::seconds(1);

      char command_array[100];
      char *ptr = command_array;
      command_received.resize(max_msg_size);

      //bool newmsg=true;
      bool newmsg = queue_receive.timed_receive(ptr, command_received.size(), recvd_size, priority, timeout);

      command_received = command_array;
      if (newmsg) {
         //handle_command(command_received);
         std::cout<<command_received<<std::endl;
      }

   } catch (bip::interprocess_exception& e) {
      //boost::interprocess::message_queue::remove("mmdaq1_srv_command");
      std::cout << "CCommandReceiver::run_ipc_queue_receiver() error : " << e.what() << std::endl;
      error = true;
   }

   if (!error) {
      run_command_receiver();
   }
*/
}


/*daqClient::ipc_command_type daqClient::get_command(std::string msg)
{
   boost::algorithm::to_lower(msg);
   if (msg == "stop") {
      return msq_stop;
   }
   else if(msg == "start") {
      return msq_start;
   }
   else if (msg == "reset") {
      return msq_reset;
   }
   else if (msg == "configure") {
      return msq_configure;
   }
   else {
      std::cout << "Unknown command" << std::endl;
      exit(EXIT_FAILURE);
      return msq_ignore;
   }



}*/
