//
//  SrsFec.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsFec.h"
#include "UDPFrame.h"
//#include "EventBuilder.h"
//#include "SrsEventBuilder.h"
#include "SrsChip.h"
#include "Logger.h"
#include "DaqServerConfig.h"


#include <boost/thread.hpp>
//#include <boost/foreach.hpp>

#include <sstream>
#include <iostream>
#include <algorithm>


using namespace online::display;

CSrsFec::TimeStampModeType CSrsFec::SrsTimeStampMode = CSrsFec::fecTsModeInvalid;


CSrsFec::CSrsFec(size_t idnum, const std::string& name, const std::string& ip_address) :
CSrsNetworkDevice(idnum, name, boost::asio::ip::address::from_string(ip_address)),
m_chips(),
m_event_number(0),
m_counter_error_decode_fec_header(0),
m_counter_error_decode_udp(0),
m_counter_error_create_event(0)
{
   
}

CSrsFec::CSrsFec(size_t idnum, const std::string& name, const boost::asio::ip::address& ip_address) :
CSrsNetworkDevice(idnum, name, ip_address),
m_chips(),
m_event_number(0),
m_counter_error_decode_fec_header(0),
m_counter_error_decode_udp(0),
m_counter_error_create_event(0)
{
   
}

CSrsFec::~CSrsFec()
{
   
}


void CSrsFec::configure(const CDaqServerConfig* daqconfig)
{
   //CSrsFec::SrsTimeStampMode = daqconfig->get_fec_time_stamp_mode();
}

void CSrsFec::reset() 
{
   m_event_number = 0;
   m_counter_error_decode_fec_header = 0;
   m_counter_error_decode_udp = 0;
   m_counter_error_create_event = 0;
   std::for_each(m_chips.begin(), m_chips.end(), boost::bind(&CSrsChip::reset, _1));
}

void CSrsFec::add_chip(boost::shared_ptr<CSrsChip> chip_ptr) 
{
   chip_ptr->set_chip_id_to_fec(m_uid);
   std::cout << "CSrsFec::add_chip to FEC id=" << uid() << " " 
   << chip_ptr->get_chip_id().get_string() << " '" 
   << chip_ptr->name() << "'" << std::endl;
   
   SrsChipPtr old = locate_chip(chip_ptr->get_chip_id().chip_no(), "");

   if (!old) {
      m_chips.push_back(chip_ptr);
   }
   else {
      std::stringstream ss;
      ss << "FEC add chip error: duplicate id " << chip_ptr->get_chip_id().get_string();
      throw std::runtime_error(ss.str().c_str());
   }
}


const std::vector<SrsChipPtr>& CSrsFec::get_chips() const
{
   return m_chips;
}

void CSrsFec::clear()
{
   //m_event_list.clear();
   m_chips.clear();
   m_event_number = 0;
   m_counter_error_decode_fec_header = 0;
   m_counter_error_decode_udp = 0;
   m_counter_error_create_event = 0;
//   CSrsNetworkDevice::clear();
}


boost::shared_ptr<CSrsChip> CSrsFec::get_srs_chip(int chip_no)
{
   typedef std::vector<boost::shared_ptr<CSrsChip> > chip_cont;
   
   chip_cont::iterator found = std::find_if(m_chips.begin(), 
                                         m_chips.end(), 
                                         CSrsChip::ChipNoEquals(chip_no));
   if (found == m_chips.end()) {
      return boost::shared_ptr<CSrsChip>();
   }
   return *found;
}

const size_t& CSrsFec::daq_event_number() const
{
   return m_event_number;
}


void CSrsFec::next_event()
{
   m_event_number+=1;
}


void CSrsFec::increment_counter_decode_fec()
{
   m_counter_error_decode_fec_header += 1;  
}


/**
 decode fec layer and insert data frame to the device's data queue 
 start event builder if end of event
 */
/*
void CSrsFec::insert_receiver_data(boost::shared_ptr<CUDPFrame> udpframe) 
{
   try {
//      decode_udp_frame(udpframe); // this is still udp receiver thread, check performance and queue sizes
//      if (udpframe->is_end_of_event()) {    
//         std::stringstream ss;
//         ss << " CSrsFec " << uid() <<" : received EoE #" << m_event_number << " m_input_buffer.size()=" << m_input_buffer.size();
//         CLogger::instance()->msg(ss.str());
//         m_event_number+=1;
//      }
//      else {
//         //fafa frames are not in the buffer
//         //lock inbuffer from udpreceiver thread
//         udpframe->set_event_number(m_event_number);
//         boost::mutex::scoped_lock(m_in_buffer_mutex);
//         m_input_buffer.push_back(udpframe); 
//      }
      
      
      //fafa frames are not in the buffer
      //lock inbuffer from udpreceiver thread
      udpframe->set_event_number(m_event_number);

      {
         boost::mutex::scoped_lock lock(m_in_buffer_mutex);
         m_input_buffer.push_back(udpframe);         
      }

      
      run_event_builder();
//      build_srs_event();
      
   } catch (std::exception& ex) {
      m_counter_error_decode_udp += 1;

      std::cout << "CSrsFec : error decoding udp frame " << m_counter_error_decode_udp << ". dropping frame \n";
      //std::cout << ex.what() << std::endl;
   }
}
*/

///**
// decode SRS FEC headers in the udp frame
// */
//void CSrsFec::decode_udp_frame(boost::shared_ptr<CUDPFrame> udpframe) 
//{
//   std::cout << "CSrsFec::decode_udp_frame( by CSrsFec::SrsTimeStampMode)" << std::endl;
//   udpframe->decode(CSrsFec::SrsTimeStampMode);
//}


/**
 search for chip by id or name
 */
boost::shared_ptr<CSrsChip> CSrsFec::locate_chip(long chip_id, const std::string& name)
{
   typedef  std::vector<SrsChipPtr> srs_chip_cont;
   typedef  std::vector<SrsChipPtr>::iterator srs_chip_iter;
   
   int chip_no = chip_id;
   if (chip_id<0 && name.empty()) {
      throw std::runtime_error("CSrsFec::locate_chip() no data to search by ");
//      return boost::shared_ptr<CSrsChip>();
   }
   
   if (chip_id<0) {
      //search by name
      srs_chip_iter chip1 = std::find_if(m_chips.begin(), m_chips.end(), CSrsBase::NameEquals<CSrsChip>(name));
      if (chip1 != m_chips.end()) {
         return *chip1; 
      }
   }
   else if (name.empty()) {
      //search by id
      srs_chip_iter chip1 = std::find_if(m_chips.begin(), m_chips.end(), CSrsChip::ChipNoEquals(chip_no));
      if (chip1 != m_chips.end()) {
         return *chip1; 
      }

   }
   else {
      //search by name and id
      srs_chip_iter chip1 = std::find_if(m_chips.begin(), m_chips.end(), 
                                         CSrsBase::NameEquals<CSrsChip>(name));
      srs_chip_iter chip2 = std::find_if(m_chips.begin(), m_chips.end(), 
                                         CSrsChip::ChipNoEquals(chip_no));
      if (chip1 != chip2) {
         std::stringstream ss ;
         ss << "CSrsFec::locate_chip() chip name='" << name << "' and id=" 
         << chip_no << " do not designate same chip or no such chip in FEC " << uid();
         throw std::runtime_error(ss.str().c_str());
      }
      else {
         if (chip1 != m_chips.end()) {
            return *chip1;
         }
      }
   }
   
   return boost::shared_ptr<CSrsChip>();
}



/**
 build event from any number of udp frames in the buffer, within fec's service thread
 frame knows its HW type, we ask it to provide correct Event class.
 must not throw
*/

void CSrsFec::build_srs_event()
{
//   CLogger::instance()->msg("CSrsFec::build_srs_event()");
//   std::list<UDPFramePtr> buffer;
//   {
//      //move all input udp frames to tmp buffer
//      boost::mutex::scoped_lock lock(m_in_buffer_mutex);
//      // get only ready udp frames
//      std::list<boost::shared_ptr<CUDPFrame> >::iterator first_notready =
//      std::partition(m_input_buffer.begin(), m_input_buffer.end(), CUDPFrame::CheckReady() );
//      buffer.splice(buffer.end(), m_input_buffer, m_input_buffer.begin(), first_notready);
//   }
   
//   m_counter_error_decode_fec_header += std::count_if(buffer.begin(), buffer.end(),
//                                                      boost::bind(&CUDPFrame::error_decode_fec_header, _1));
//   buffer.remove_if(boost::bind(&CUDPFrame::error_decode_fec_header, _1) );
   
//   try {
//      //in serial create equipment events (process equipment data in frames)
//      Q_FOREACH(UDPFramePtr frame, buffer) {
//         CSrsEventBuilder::create_event(frame);
//         //channel data sorted by eventId are now in the chips
//      }
//      m_event_builder->notify();
      
//   } catch (bad_udp_data& badudp) {
//      m_counter_error_create_event += 1;
//      std::cout << "Dropping event ! CSrsFec::build_srs_event(): error creating FEC event :\n";
//      std::cout << badudp.what() << std::endl;
//   } catch (std::range_error& re) {
//      m_counter_error_create_event += 1;
//      std::cout << "Dropping event ! CSrsFec::build_srs_event(): error creating FEC event :\n";
//      std::cout << re.what() << std::endl;
//   } catch (std::runtime_error& re) {
//      m_counter_error_create_event += 1;
//      std::cout << "Dropping event ! CSrsFec::build_srs_event(): error creating FEC event :\n";
//      std::cout << re.what() << std::endl;
//   }
   
//   //udp frames not in events disappear here
//   //events disappear here
}



