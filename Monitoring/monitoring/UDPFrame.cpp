//
//  UDPFrame.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "UDPFrame.h"
#include "Logger.h"
#include "SrsNetworkDevice.h"
//#include "SrsEventApvRaw.h"
#include "SrsChip.h"
#include "SrsFec.h"

#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/foreach.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace online::display;

//unsigned short mmdaq3::server::CUDPFrame::SizeUInt32Bytes = sizeof(uint32_t);
//unsigned short mmdaq3::server::CUDPFrame::SRSMaxChipNumber = 15;
//unsigned short mmdaq3::server::CUDPFrame::SRSHeaderLength = 2; // with frame counting field
//uint32_t mmdaq3::server::CUDPFrame::SRSEndOfEventMarker = 0xFAFAFAFA;

SRSEqHdrKeys online::display::CUDPFrame::SrsHeaderKeys = SRSEqHdrKeys();


CUDPFrame::CUDPFrame(size_t seq_number, 
                     const unsigned char* buffer, 
                     size_t size_bytes, 
                     boost::shared_ptr<CSrsNetworkDevice> srs_device,
                     boost::posix_time::ptime time_stamp) :
m_daq_frame_number(seq_number),
m_srs_frame_number(0),
m_srs_device(srs_device),
//m_remote_endpoint(remote_endpoint),
m_daq_time_stamp(time_stamp),
m_srs_time_stamp(0),
m_daq_fec_event_number(0),
m_udp_data_size32(size_bytes/SizeUInt32Bytes),
m_data(),
m_srs_equipment_type(SRSEquipmentKey(SRSEqHdrKeys::SRSEquipmentBad,0)),
m_srs_source_chip(),
m_srs_data_size(0),
m_equipment_header_info(m_data.end()),
m_equipment_data_begin(m_data.end()),
m_error_decode_fec_header(false),
m_end_of_event_marker(false),
m_frame_event_ready(false)
{
   const uint32_t* iptr = reinterpret_cast<const uint32_t*> (buffer);
   m_data.assign(iptr, iptr + m_udp_data_size32);
   // TODO is switching in place better, second: use ntoh?
   std::transform(m_data.begin(), m_data.end(), m_data.begin(), endian_swap32_copy()); //change from net standard
   m_equipment_header_info = m_data.end();
   m_equipment_data_begin = m_data.end();
   
}

CUDPFrame::~CUDPFrame()
{
   
}

void CUDPFrame::sanity_check()
{
   if (SizeUInt32Bytes != 4) {
      throw std::range_error("UDPFrame: SizeUInt32Bytes") ;
   }
}

void CUDPFrame::print()
{
//   for (UdpRawDataType::const_iterator idt = m_data.begin(); idt != m_data.end(); ++idt) {
//      std:: cout << " " << std::setw(10) << std::hex << *idt <<std::dec ;
//   }
   
   std::cout << std::setw(18) << "m_daq_frame_number " << m_daq_frame_number << "\n";
   std::cout << std::setw(18) << "m_srs_frame_number " << m_srs_frame_number << "\n";
   std::cout << std::setw(18) << "m_srs_device " << m_srs_device << "\n";
   std::cout << std::setw(18) << "m_daq_time_stamp " << m_daq_time_stamp << "\n";
   std::cout << std::setw(18) << "m_srs_time_stamp " << m_srs_time_stamp << "\n";
   std::cout << std::setw(18) << "m_udp_data_size32 " << m_udp_data_size32 << "\n";
   std::cout << std::setw(18) << "m_srs_equipment_type " 
   << m_srs_equipment_type.first << " " 
   << m_srs_equipment_type.second << "\n";
   std::cout << std::setw(18) << "m_srs_source_chip " << m_srs_source_chip->to_string() << "\n";
   std::cout << std::setw(18) << "m_error_decode_fec_header " << m_error_decode_fec_header << "\n";
   std::cout << std::setw(18) << "m_end_of_event_marker " << m_end_of_event_marker << "\n";
   std::cout << std::setw(18) << "m_frame_event_ready " << m_frame_event_ready << "\n";
}


void CUDPFrame::set_event_number(const size_t& event_number)
{
   m_daq_fec_event_number = event_number;
}

const size_t& CUDPFrame::get_event_number() const
{
   return m_daq_fec_event_number;
}
/**
 decode SRS FEC layer
 https://espace.cern.ch/rd51-wg5/srs/Documentation/SRS%20Data%20Format.pdf
 32 bit words: each frame has: 
 1) frame_counter (3modes: bytes:  0,0,0,#f or TS,Ts,Ts, #f or global_counter)
 2) data header ( ADC#, APZ#, AZC0), # is chip number
 ------------------------------------------
 3+) header info (reserved) - can be more than one - how to know?- this is equipment dependent
 n) followed by equipment data
 */
void CUDPFrame::decode(const CSrsFec::TimeStampModeType& ts_mode)
{
   if (m_data.empty()) {
      throw bad_udp_data("CUDPFrame::decode() m_data.empty()");
   }
   // look for EOE single word frames FAFAFAFA 
   if (m_data.size() == 1 && m_data[0] == SRSEndOfEventMarker) {
      m_frame_event_ready = true;
      m_end_of_event_marker = true;
      return;
   }
   
   if (m_data.size() < 2) {
      throw bad_udp_data("CUDPFrame::decode() no 2 words of FEC header data in UDP frame");
   }
   
   
   try {
      UdpRawDataType::iterator diter = m_data.begin();
      decode_fec_frame_counter(ts_mode, diter++);
      decode_fec_equipment_type(diter++);
      m_equipment_data_begin = diter;
      m_equipment_header_info = diter;
          
   } catch (bad_udp_data& ex) {
      m_error_decode_fec_header = true;
      std::cerr << "UDPFrame: bad data in decode(): ";
      std::cerr << ex.what() << std::endl;
      throw;
   }
   catch (std::out_of_range& ex) {
      m_error_decode_fec_header = true;
      std::cerr << "UDPFrame: bad data in decode(): ";
      std::cerr << ex.what() << std::endl;
      throw;
   }
   
   m_frame_event_ready = true;
//   print();
}



//TODO: field depends on DCS settings 
void CUDPFrame::decode_fec_frame_counter(const CSrsFec::TimeStampModeType& ts_mode, CUDPFrame::UdpRawDataType::iterator data_iter)
{
   /*
    SRS frame count and time stamp: srs_frame_cnt
    https://espace.cern.ch/rd51-wg5/srs/Documentation/SRS%20Data%20Format.pdf
    F# - starts from 0 for a new event and increases for every frame.
    TS - 3-byte timestamp tag attributed to each event
    Single-FEC mode (default): 0x00, 0x00, 0x00, F# 
    Multiple-FEC mode :        TS, F#
    Test Mode: ￼￼                GF#
    */
   
   switch (ts_mode) {
      case CSrsFec::fecTsModeEventFrame:
         m_srs_frame_number = *(data_iter) & 0xFF;
         m_srs_time_stamp  = 0;
         break;
      case CSrsFec::fecTsModeClockCycle:
         m_srs_frame_number = *(data_iter) & 0xFF;
         m_srs_time_stamp  = (*(data_iter) >> 8 )& 0xFFFFFF; //trigger number in BNL
         break;
      case CSrsFec::fecTsModeGlobalFrame:
         m_srs_frame_number = *(data_iter);
         m_srs_time_stamp  = 0;
         break;
         
      default:
         std::cout << "CUDPFrame::decode_fec_frame_counter mode BAD MODE" << std::endl;
         break;
   }

}



//::SRSEquipmentBnlDaqCard = 0x424E4C

void CUDPFrame::decode_fec_equipment_type(CUDPFrame::UdpRawDataType::iterator hw_descriptor)
{
   /*
    SRS data header in FEC layer
    https://espace.cern.ch/rd51-wg5/srs/Documentation/SRS%20Data%20Format.pdf
    The Data Header is a mandatory field that essentially identifies the data source
    ADC mode: “A” “D” “C” C#
    APV with zero-suppression: “A” “P” “Z” C#
    ￼Arizona C-Card:  “A” “Z” “C” 0x00
    (...)
    and isolate header range: begin - hw_desc
    */
   
   bool known_hw_type = std::binary_search(SrsHeaderKeys.keys.begin(), 
                                           SrsHeaderKeys.keys.end(), 
                                           SRSEquipmentKey((*hw_descriptor >>8 )& 0xffffff, false), 
                                           FirstLess<SRSEquipmentKey>());
//                                           SRSEqHdrKeys::EqualKey());
   if (!known_hw_type) {
      std::stringstream ss;
      ss << "UDPFrame: no known hardware descriptor (like 0x414443) in frame received: " 
      << std::hex << std::setw(8) << std::setfill('0') << *hw_descriptor 
      << "(" << std::setw(8) << std::setfill('0')<< ( (*hw_descriptor >>8 )& 0xffffff )<< ")"
      << std::dec ;
      throw bad_udp_data(ss.str().c_str());
   }
   
   
   //stores electronics type and single fram flag (all chips' data in a single udp frame)
   m_srs_equipment_type = SRSEquipmentKey((*hw_descriptor >> 8 ) & 0xFFFFFF, 
                                          SrsHeaderKeys.is_single_frame(m_srs_equipment_type.first));
   
   std::cout << "UDPFrame: equipment type is " << std::setw(8) << std::setfill('0') 
   <<  std::hex << m_srs_equipment_type.first << std::dec << ", " 
   << m_srs_equipment_type.second << std::endl;
   
   //m_srs_source_chip_id is 0  for multi-chip frames, skip this 
   if (m_srs_equipment_type.second == false) {
      int chip_no = (*hw_descriptor) & 0xFF;
      m_srs_source_chip = m_srs_device->get_srs_chip(chip_no);
      if (!m_srs_source_chip) {
         m_error_decode_fec_header = true;
         std::stringstream ss;
         ss << "UDPFrame: undeclared chip id " << chip_no << "in frame from fec " << m_srs_device->to_string();
         throw bad_udp_data(ss.str().c_str());
      }
      else {
         //check hw code against chip type
         if (!m_srs_source_chip->check_equipment_key(m_srs_equipment_type.first)) {
            m_error_decode_fec_header = true;
            std::stringstream ss;
            ss << "UDPFrame: predefined chip type of ("  << m_srs_source_chip->name() 
            << " in FEC: " << m_srs_device->name() <<") does not agree with received equipment type: " 
            << std::setw(8) << std::setfill('0') << std::hex << m_srs_equipment_type.first ;
            throw bad_udp_data(ss.str().c_str());
         }
      }
   }
   else {
      //to do check chip types later ...
      std::cout << "UDPFrame: multi-chip frame from FEC: " << m_srs_device->name() << std::endl;
   }
   std::cout << "UDPFrame: OK decode_fec_equipment_type " << std::endl;
}


bool CUDPFrame::get_equipment_header_info(std::pair<CUDPFrame::UdpRawDataCIter, CUDPFrame::UdpRawDataCIter>& range32) const
{
   range32 = std::pair<UdpRawDataCIter, UdpRawDataCIter>(m_equipment_header_info, m_data.end());   
   return m_error_decode_fec_header;
}


SrsChipPtr CUDPFrame::srs_source_chip()
{
   return m_srs_source_chip;
}

SrsNetDevPtr CUDPFrame::srs_source_device()
{
   return m_srs_device;
}


