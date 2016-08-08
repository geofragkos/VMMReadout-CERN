//
//  SrsChipId.h
//  browser
//
//  Created by Marcin Byszewski on 10.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChipId_h
#define online_display_SrsChipId_h

#include <stdint.h>
#include <functional>
#include <string>
#include <ostream>

namespace online {
   namespace display {
   
   /**
    SRS chip unique id built from FEC number(1 - N) and local to fec chip number (0-15)
    */
   class CSrsChipId
   {
      
   public:
      explicit CSrsChipId();
      explicit CSrsChipId(uint32_t chip_id);
      CSrsChipId(uint32_t fecNo, uint32_t chipNo);
      virtual ~CSrsChipId();
      
      void        set_fec_no(uint32_t fec_no);
      uint32_t    chip_id() const;
      uint32_t    chip_no() const;
      uint32_t    fec_no()  const;
      std::string get_string() const;
      
         
      bool operator==(const CSrsChipId& rhs) const { return m_id == rhs.m_id;}
      bool operator<(const CSrsChipId& rhs) const { return m_id < rhs.m_id;}
      struct less : std::binary_function<CSrsChipId, CSrsChipId, bool>  {
         bool operator() (const CSrsChipId& x, const CSrsChipId& y) const
         {return x < y;}
      };
      
      static uint32_t InvalidChipId;
      friend std::ostream& operator<< (std::ostream & out, const CSrsChipId & cid)
      {
         return out << cid.get_string();
      }
      
         private:
      uint32_t m_id;
   };
      
      
      
      
//      std::ostream& operator<< (std::ostream & out, const CSrsChipId & cid)
      
      
   }
} // namespace

#endif


