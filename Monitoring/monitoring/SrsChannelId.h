//
//  SrsChannelId.h
//  browser
//
//  Created by Marcin Byszewski on 10.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannelId_h
#define online_display_SrsChannelId_h

#include <stdint.h>
#include <functional>
#include <string>

namespace online {
   namespace display {
   
   /**
    SRS channel unique id built from FEC number(1 - N) and local to fec chip number (0-15) and channel number (1-128 for APVs)
    ID is a 16 bit number: FCcc, F-FEC, C-chip, cc channel
    */
   
   class CSrsChipId;
   
   class CSrsChannelId
   {
      
   public:
      static uint32_t from_fec_chip_channel(uint32_t fecNo,  uint32_t chipNo, uint32_t chanNo);
      static uint32_t from_chipid_channel(const CSrsChipId& chipId, uint32_t chanNo);
      
      
   public:
      explicit CSrsChannelId();
      explicit CSrsChannelId(uint32_t full_id) ;
      CSrsChannelId(uint32_t fecNo,  uint32_t chipNo, uint32_t chanNo);
//      CSrsChannelId(uint32_t chipId, uint32_t chanNo);
      CSrsChannelId(const CSrsChipId& chipId, uint32_t chanNo);
//      CSrsChannelId(const CSrsChannelId& rhs);
      
      std::string to_string() const;
      virtual ~CSrsChannelId();
      
      uint32_t   channel_id() const; 
      uint32_t   fec_no() const; 	
      uint32_t   chip_no() const;
      CSrsChipId chip_id() const; // should limit? & 0xFFF
      uint32_t   channel_no() const;
      
      bool operator==(const CSrsChannelId& rhs) const { return m_id == rhs.m_id;}
      bool operator<(const CSrsChannelId& rhs) const { return m_id < rhs.m_id;}
      
      struct less : std::binary_function<CSrsChannelId, CSrsChannelId, bool>  {
         bool operator() (const CSrsChannelId& x, const CSrsChannelId& y) const
         { return  x < y;}
      };
      
      
   private:
      uint32_t m_id;
      
   };
      
   }
} // namespace

#endif
