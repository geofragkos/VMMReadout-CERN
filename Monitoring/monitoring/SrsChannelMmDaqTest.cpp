//
//  SrsChannelMmDaqTest.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13/11/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannelMmDaqTest.h"
#include "DetStripId.h"
//#include "DetConnector.h"
//#include "RootTreeFiller.h"
//#include "Logger.h"


#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace online::display;

CSrsChannelMmDaqTest::CSrsChannelMmDaqTest(const CSrsChannelId& chan_id, const uint32_t& word) :
CSrsChannel(chan_id, CSrsChannelData( double((word >> 16)&0xFFFF) , double(word & 0xFFFF)) )
{
// std::cout << "CREATE: TESTER CSrsChannelMmDaqTest() " << chan_id.to_string() << " q=" <<  m_channel_data.m_charge << std::endl;
}


CSrsChannelMmDaqTest::~CSrsChannelMmDaqTest()
{
   
}

void CSrsChannelMmDaqTest::prefill(CRootTreeFiller* filler, boost::shared_ptr<CDetConnector> conn)
{
//   try {
//      if(!conn) {
//         CLogger::instance()->error("DAQ ERROR conn = 0 in CSrsChannelMmDaqTest::prefill");
//         return;
//      }
//      CDetStripId strip_id = conn->get_mapped_strip_id(m_channel_id);
//      filler->prefill_channel(m_channel_id, m_channel_data, strip_id);
//   } catch (std::range_error & re)
//   {
//      // do not fill unmapped strips
//      std::stringstream ss;
//      ss <<  "CSrsChannelMmDaqTest ROOT prefill skips unmapped channel " + m_channel_id.to_string();
//      CLogger::instance()->warn(ss.str());
//   }
}


void CSrsChannelMmDaqTest::calculate_channel_pedestal(CSrsPedestal& ped) const
{
   ped.mean   = m_channel_data.m_charge;
   ped.stddev = 0;
   ped.sigma  = 0.0;
}
