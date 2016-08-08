//
//  SrsEventId.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 25/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsEventId.h"

#include <boost/date_time.hpp>

#include <iostream>

using namespace online::display;


const boost::posix_time::ptime CSrsEventId::time_t_epoch = boost::posix_time::ptime(boost::gregorian::date(1970,1,1));


CSrsEventId::CSrsEventId() :
m_event_number(0),
m_srs_time_stamp(0),
m_daq_time_stamp(),
m_daq_fec_event_number(0)
{
   
}

CSrsEventId::CSrsEventId(size_t eb_event_number, 
                         size_t srs_time_stamp, 
                         boost::posix_time::ptime daq_time_stamp,
                         size_t daq_fec_event_number ) :
m_event_number(eb_event_number),
m_srs_time_stamp(srs_time_stamp),
m_daq_time_stamp(daq_time_stamp),
m_daq_fec_event_number(daq_fec_event_number)
{
   
}


CSrsEventId::~CSrsEventId()
{
   
}

EBEventIdType CSrsEventId::event_assembly_id(EBAssemblyType assembly_by) const
{
   switch (assembly_by) {
      case AssembleBySrsTimeStamp:
         return m_srs_time_stamp;
      case AssembleByDaqTimeStamp:
         return daq_time_stamp();
      case AssembleByDaqEventNumber:
         return m_daq_fec_event_number;
      default:
         break;
   }
   return AssembleBySrsTimeStamp;
}


EBEventIdType CSrsEventId::daq_time_stamp() const
{
   boost::posix_time::time_duration diff = m_daq_time_stamp - time_t_epoch;
   return diff.total_microseconds(); // that needs 7 bytes (52 bits)
}

size_t CSrsEventId::daq_time_stamp_seconds() const
{
   boost::posix_time::time_duration diff = m_daq_time_stamp - time_t_epoch;
   return diff.total_seconds();
}

int CSrsEventId::daq_time_stamp_microseconds() const
{
   boost::posix_time::time_duration diff = m_daq_time_stamp - time_t_epoch;
   return diff.total_microseconds()%1000000;
}
