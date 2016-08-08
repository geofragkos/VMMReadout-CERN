//
//  SrsFec.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsFec_h
#define online_display_SrsFec_h

#include "SrsNetworkDevice.h"
#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/asio/ip/address.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#endif

#include <iostream>

namespace online {
   namespace display {
      
//      class CUDPFrame;
      class CEventBase;
      class CDaqServerConfig;
//      class CSrsChip;
      
      /**
       SRS FEC card, maybe used as base class, to handle data differently ?
       */
      class CSrsFec : public CSrsNetworkDevice
      {
         
      public:
         CSrsFec(size_t idnum, const std::string& name, const std::string& ip_address);
         CSrsFec(size_t idnum, const std::string& name, const boost::asio::ip::address& ip_address);
         virtual ~CSrsFec();
         virtual void reset();
         virtual void add_chip(boost::shared_ptr<CSrsChip> chip_ptr);
         void clear();
//         virtual void insert_receiver_data(boost::shared_ptr<CUDPFrame> udpframe);
         virtual void next_event();
         
         void increment_counter_decode_fec();
//         virtual void decode_udp_frame(boost::shared_ptr<CUDPFrame> udpframe);
         virtual boost::shared_ptr<CSrsChip> get_srs_chip(int chip_no);
         const size_t& daq_event_number() const;
         boost::shared_ptr<CSrsChip> locate_chip(long chip_id, const std::string& name);
         
         const std::vector<SrsChipPtr>& get_chips() const;
         
         /**
          F# - starts from 0 for a new event and increases for every frame.
          TS - 3-byte timestamp tag attributed to each event
          Single-FEC mode (default): 0x00, 0x00, 0x00, F# 
          Multiple-FEC mode :        TS, F#
          Test Mode: ￼￼                GF#
          */
         typedef enum {
            fecTsModeInvalid,
            fecTsModeEventFrame,
            fecTsModeClockCycle,
            fecTsModeGlobalFrame
         } TimeStampModeType;
         
         static TimeStampModeType SrsTimeStampMode;
         virtual void configure(const CDaqServerConfig* daqconfig);

         friend class CDaqServerMonitor;

      protected:
         virtual void build_srs_event();
         
         boost::condition_variable m_eb_condition;
         
         std::vector<SrsChipPtr> m_chips;
         size_t m_event_number; ///< event number for this FEC based on FAFA frames
         
         size_t m_counter_error_decode_fec_header;
         size_t m_counter_error_decode_udp; // == m_counter_error_decode_fec_header ?
         size_t m_counter_error_create_event;

      private:
         

//         std::vector<CSrsChip> for integrity check - number of frames
//         size_t m_expected_frame_count;
//         std::list< boost::shared_ptr<CEventBase> > m_event_list; /// Equipment events from udp frames
      };
      
   }
} // namespace


#endif
