//
//  SrsEventId.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 25/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsEventId_h
#define online_display_SrsEventId_h

#include "EventTypes.h"

#ifndef Q_MOC_RUN

#include <boost/date_time/posix_time/posix_time.hpp>
#endif

#include <iostream>

namespace online {
   namespace display {
      
      class CSrsEventId
      {
         
      public:
         explicit CSrsEventId();
         explicit CSrsEventId(size_t eb_event_number, 
                     size_t srs_time_stamp, 
                     boost::posix_time::ptime daq_time_stamp,
                     size_t daq_fec_event_number );
         virtual ~CSrsEventId();
         
         size_t                     m_event_number; // given by EB
         size_t                     m_srs_time_stamp; ///< srs time stamp from udp frames srs time stamp (clocks)        
         boost::posix_time::ptime   m_daq_time_stamp; ///< timestamp by daq
         size_t                     m_daq_fec_event_number; ///< fec's event number given by daq (counting FAFA frames)
         
         
         EBEventIdType event_assembly_id(EBAssemblyType assembly_by) const;
         EBEventIdType daq_time_stamp() const;
         size_t daq_time_stamp_seconds() const;
         int daq_time_stamp_microseconds() const;

         
         static const boost::posix_time::ptime time_t_epoch;
         
      };
   }
} // namespace

#endif
