//
//  UDPFrame.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_UDPFrame_h
#define online_display_UDPFrame_h

#include "SRSEquipmentHeader.h"
#include "SrsTypes.h"
#include "SrsFec.h"

#ifndef Q_MOC_RUN

#include <boost/asio/ip/udp.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#endif

#include <cstddef>
#include <stdexcept>
#include <functional>

namespace online {
   namespace display {
      
      struct SRSEqHdrKeys;
      class CEventBase;
      class CSrsChip;
      
      class CUDPFrame
      {
         
         
      public:
         typedef std::vector<uint32_t> UdpRawDataType;
         typedef UdpRawDataType::const_iterator UdpRawDataCIter;
         
         //CUDPFrame(m_run_frame_counter, buf, size_bytes, m_remote_endpoint, m_prev_time_stamp)
         CUDPFrame(size_t seq_number, 
                   const unsigned char* buffer, 
                   size_t size_bytes, 
                   boost::shared_ptr<CSrsNetworkDevice> srs_device,
                   boost::posix_time::ptime time_stamp);
         virtual ~CUDPFrame();
         static void sanity_check(); 
         void print();
         bool is_ready() const;
         bool is_end_of_event() const;
         bool error_decode_fec_header() const;
         void decode(const CSrsFec::TimeStampModeType& ts_mode); //process SRS FEC frame
         void set_event_number(const size_t& event_number);
         const size_t& get_event_number() const;
         
         boost::shared_ptr<CEventBase> create_event(boost::shared_ptr<CUDPFrame> this_frame_ptr);
         
         bool get_equipment_header_info(std::pair<UdpRawDataCIter, UdpRawDataCIter>& range32) const;
         SrsChipPtr srs_source_chip();
         SrsNetDevPtr srs_source_device();
         
         const SRSEquipmentKey& equipment_type() const { return m_srs_equipment_type; }
         const size_t& srs_time_stamp() const { return m_srs_time_stamp;}
         const boost::posix_time::ptime& daq_time_stamp() const { return m_daq_time_stamp;}
         
         class CheckReady;
      protected:
         size_t m_daq_frame_number; ///< frame number given by the daq
         size_t m_srs_frame_number; ///< frame number received from srs
         SrsNetDevPtr m_srs_device; ///< source device
         boost::posix_time::ptime m_daq_time_stamp; ///< timestamp by daq
         size_t m_srs_time_stamp; ///< srs time stamp (clocks)
         size_t m_daq_fec_event_number; ///< fec's event number given by daq (counting FAFA frames)
         size_t m_udp_data_size32;    ///< data size in 32 bit words
         UdpRawDataType m_data; ///< data in 32 bit words
         
         
         SRSEquipmentKey  m_srs_equipment_type; ///< type of srs electronics
         SrsChipPtr m_srs_source_chip; // chip id of the source of dta (-1 for multi chip udp frames)
         size_t m_srs_data_size;
         UdpRawDataCIter m_equipment_header_info;
         UdpRawDataCIter m_equipment_data_begin;
         
         //result values
         
         bool m_error_decode_fec_header; ///< ex. no equipment descr.
         bool m_end_of_event_marker; ///< is special frame marking EOE
         bool m_frame_event_ready; ///< event has been processed and is ready
         
         
         //statics
         static const unsigned short SizeUInt32Bytes = sizeof(uint32_t); ///< size of uint32 , sanity check
         static const unsigned short SRSMaxChipNumber = 15; /// one FEC max chip number is 0-15
         static const unsigned short SRSHeaderLength = 2; ///< SRS header is 2 int32 words
         static const uint32_t SRSEndOfEventMarker = 0xFAFAFAFA;   /// 0xFAFAFAA
         static SRSEqHdrKeys SrsHeaderKeys;// = SRSEqHdrKeys();
         
         
         
      private:
         
         void decode_fec_equipment_type(UdpRawDataType::iterator data_iter);
         void decode_fec_frame_counter(const CSrsFec::TimeStampModeType& ts_mode, UdpRawDataType::iterator data_iter);
//         UdpRawDataType::iterator decode_fec_header_info(UdpRawDataType::iterator hwdesc);  
         
         
         struct endian_swap32_copy : public std::unary_function<uint32_t, uint32_t>
         {
            endian_swap32_copy() {};
            //TODO:use ntoh??
            uint32_t operator() (const uint32_t& x) {
               return (x>>24) | 
               ((x<<8) & 0x00FF0000) |
               ((x>>8) & 0x0000FF00) |
               (x<<24);
            }
         };
         
         
         
      };
      
      class CUDPFrame::CheckReady : public std::unary_function <boost::shared_ptr<CUDPFrame> , bool> {
      public:
         bool operator() (const boost::shared_ptr<CUDPFrame> pfr) const {
            return pfr->m_frame_event_ready;
         }
      };
      
      class bad_udp_data : public std::runtime_error
      {
      public:
         bad_udp_data(const char* str) : std::runtime_error(str) {};
      };
      
      
      inline bool CUDPFrame::is_end_of_event() const
      {
         return m_end_of_event_marker;
      }

      inline bool CUDPFrame::error_decode_fec_header() const {
         return m_error_decode_fec_header;
      }
      inline bool CUDPFrame::is_ready() const {
         return m_frame_event_ready;
      }
   }
} // namespace

#endif
