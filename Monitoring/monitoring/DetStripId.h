//
//  DetStripId.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetStripId_h
#define online_display_DetStripId_h

#include <cstddef>
#include <string>
#include <ostream>

namespace online {
   namespace display {
   
   class CDetStripId
   {
      
   public:
      explicit CDetStripId();
      CDetStripId(const std::string& m_chamber_name,
                  size_t multi_layer_id, 
                  size_t layer_id,
                  char readout_name,
                  size_t strip_id);
      virtual ~CDetStripId();
      
      friend std::ostream& operator<< (std::ostream & out, const CDetStripId & sid);

      const std::string& chamber_name() const { return m_chamber_name;}
      const size_t& multilayer_id() const { return m_multi_layer_id;}
      const size_t& layer_id() const { return m_layer_id;}
      const char& readout_name() const { return m_readout_name;}
      const size_t& strip_no() const { return m_strip_no;}
      
   protected:
      
      std::string m_chamber_name;
      size_t m_multi_layer_id;
      size_t m_layer_id;
      char   m_readout_name;
      size_t m_strip_no;
      
      
   };
      
      std::ostream& operator<< (std::ostream & out, const CDetStripId & sid);
   }
} // namespace

#endif
