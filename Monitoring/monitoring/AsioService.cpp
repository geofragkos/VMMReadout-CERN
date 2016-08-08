//
//  AsioService.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "AsioService.h"

#include <iostream>

using namespace online::display;

CAsioService::CAsioService() :
m_io_service(),
m_idle_work(m_io_service),
m_thread_group(),
m_thread_pool_size(1),
m_signals(m_io_service),
m_stopping(false)
{
   // Register to handle the signals that indicate when the server should exit.
   // It is safe to register for the same signal multiple times in a program,
   // provided all registration for the specified signal is made through Asio.
   m_signals.add(SIGINT);
   m_signals.add(SIGTERM);
#if defined(SIGQUIT)
   m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
   m_signals.async_wait(boost::bind(&CAsioService::handle_stop, this));
   
   m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));

   
   
}

CAsioService::CAsioService(size_t thread_pool_size) :
m_io_service(),
m_idle_work(m_io_service),
m_thread_group(),
m_thread_pool_size(thread_pool_size),
m_signals(m_io_service),
m_stopping(false)
{
 
   // Register to handle the signals that indicate when the server should exit.
   // It is safe to register for the same signal multiple times in a program,
   // provided all registration for the specified signal is made through Asio.
   m_signals.add(SIGINT);
   m_signals.add(SIGTERM);
#if defined(SIGQUIT)
   m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
   m_signals.async_wait(boost::bind(&CAsioService::handle_stop, this));
   
   m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));

//   boost::thread* t1 = new boost::thread(boost::bind(&boost::asio::io_service::run, &m_io_service));
//   boost::thread::native_handle_type posix_thread =  t1->native_handle();
//   
//   int s, opt, inheritsched, use_null_attrib, policy;
//   pthread_t thread;
//   pthread_attr_t attr;
//   pthread_attr_t *attrp;
//   char *attr_sched_str, *main_sched_str, *inheritsched_str;
//   struct sched_param param;
//
//   
//   
//   s = pthread_attr_setschedparam(&attr, &param);
//   
//   
//   m_thread_group.add_thread(t1);
//
//   
//   int pthread_attr_getschedpolicy(pthread_attr_t * attr, int * policy);
   

   
   
}


CAsioService::~CAsioService()
{
   m_io_service.stop();
}

boost::asio::io_service& CAsioService::io_service()
{
   return m_io_service;
}



void CAsioService::handle_stop()
{
   m_stopping = true;
}


void CAsioService::stop()
{
   m_io_service.stop();
}




