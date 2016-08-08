//
//  AsioService.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_AsioService_h
#define online_display_AsioService_h

#include <iostream>

#include <string>
#ifndef Q_MOC_RUN
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#endif

namespace online {
   namespace display {

      /**
       boost Asio service with N threads and signals
       */
       
      class CAsioService
      {
         
      public:
         
         typedef boost::function<void(void)> MainLoopFunction;
         
         CAsioService();
         CAsioService(size_t thread_pool_size);
         virtual ~CAsioService();
         
         boost::asio::io_service& io_service();

         void post(MainLoopFunction f)
         {
            m_stopping = false;
            m_io_service.post(f);
         }
         void stopping(bool v) {
            m_stopping = v;
         }
         bool stopping() const { return m_stopping;}
         void handle_stop();
         void stop();
         
      protected:
         /// The io_service used to perform asynchronous operations.
         boost::asio::io_service m_io_service;
         
         ///keep service from terminating
         boost::asio::io_service::work m_idle_work;
         
         ///worker threads
         boost::thread_group m_thread_group;
         
         
         /// The number of threads that will call io_service::run().
         std::size_t m_thread_pool_size;
         
         /// The signal_set is used to register for process termination notifications.
         boost::asio::signal_set m_signals;
         
         bool m_stopping;
         
      };
      
   }
} // namespace


#endif

