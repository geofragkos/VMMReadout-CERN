//
//  SrsNetworkDevice.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 15.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsNetworkDevice_h
#define online_display_SrsNetworkDevice_h

#include "SrsTypes.h"
#include "SrsBase.h"

#ifndef Q_MOC_RUN

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#endif

#include <string>
#include <list>

namespace online {
   namespace display {
      
      class CUDPFrame;
      class CEventBuilder;
      class CDaqServer;
      class CDaqServerConfig;
      
      /**
       network connectivity for SRS devices (FEC, SRU)
       */
      class CSrsNetworkDevice : public CSrsBase
      {
         
      public:
         CSrsNetworkDevice(size_t idnum, const std::string& name, boost::asio::ip::address ip_address);
         virtual ~CSrsNetworkDevice();
 //        virtual void insert_receiver_data(boost::shared_ptr<CUDPFrame> ) {};
         virtual void next_event() {}
 //        virtual void decode_udp_frame(boost::shared_ptr<CUDPFrame> ) {};
         virtual void reset() = 0;
         virtual void configure(const CDaqServerConfig* daqconfig) = 0;
         void print() const;
         const boost::asio::ip::address& get_ip_adress() const;
         
         void register_builder(boost::shared_ptr<CEventBuilder> builder);
         void deregister_builder();
         class IpAddressEquals;
         
         void run_event_builder();
         
      protected:
         virtual void build_srs_event();
         void clear();
         
         boost::asio::ip::address m_ip_address;
//         std::list<boost::shared_ptr<CUDPFrame> > m_input_buffer;
         boost::mutex m_in_buffer_mutex;
         boost::shared_ptr<CEventBuilder> m_event_builder;
         
         
      private:
         
         /// The io_service used to perform asynchronous operations.
         boost::asio::io_service m_io_service;
         
         ///keep service from terminating
         boost::asio::io_service::work m_idle_work;
         
         ///worker threads
         boost::thread_group m_thread_group;
         
      };
      
      
      class CSrsNetworkDevice::IpAddressEquals : public std::unary_function<SrsBasePtr, bool> {
         boost::asio::ip::address m_address;
      public:
         IpAddressEquals(const boost::asio::ip::address& address) : m_address(address) {}
         bool operator() (const SrsBasePtr& rhs) const {
            SrsNetDevPtr ndev = boost::dynamic_pointer_cast<CSrsNetworkDevice>(rhs);
            if (!ndev) {
               return false;
            }
            return ndev->get_ip_adress() == m_address;
         }
      };
      
      
   }
} // namespace

#endif
