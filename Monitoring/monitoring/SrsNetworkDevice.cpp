//
//  SrsNetworkDevice.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 15.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsNetworkDevice.h"

#include <iostream>

#define SRSDEVICE_THREAD_COUNT 1
using namespace online::display;

CSrsNetworkDevice::CSrsNetworkDevice(size_t idnum, const std::string& name, boost::asio::ip::address ip_address)
: CSrsBase(idnum, name), 
m_ip_address(ip_address),
/*m_input_buffer(),*/
m_in_buffer_mutex(),
m_event_builder(),
m_io_service(),
m_idle_work(m_io_service),
m_thread_group()
{
   
   for (std::size_t ii = 0; ii < SRSDEVICE_THREAD_COUNT; ++ii)
      m_thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));
   
}


CSrsNetworkDevice::~CSrsNetworkDevice()
{
   
}

void CSrsNetworkDevice::clear()
{
   boost::mutex::scoped_lock lock(m_in_buffer_mutex);
   //m_input_buffer.clear();
}

const boost::asio::ip::address& CSrsNetworkDevice::get_ip_adress() const
{
   return m_ip_address;
}


void CSrsNetworkDevice::print() const 
{
   std::cout << " ip=" << m_ip_address << std::endl;   
}


void CSrsNetworkDevice::register_builder(boost::shared_ptr<CEventBuilder> builder)
{
   m_event_builder = builder;  
}


void CSrsNetworkDevice::deregister_builder()
{
   m_event_builder.reset();
}

void CSrsNetworkDevice::run_event_builder()
{
   std::cout << "CSrsNetworkDevice::run_event_builder()" << std::endl;
   if (m_event_builder) {
      m_io_service.post(boost::bind(&CSrsNetworkDevice::build_srs_event, this));
   }
   
}

void CSrsNetworkDevice::build_srs_event()
{
   std::cout << "CSrsNetworkDevice::build_srs_event() not implemented - should have been dispatched" << std::endl;
}
