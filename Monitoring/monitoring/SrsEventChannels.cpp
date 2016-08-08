//
//  SrsEventChannels.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 20/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsEventChannels.h"
#include "SrsChip.h"
#include "EventBuilderInputBuffer.h"
//#include "RootTreeFiller.h"
#include "SrsChannel.h"

//#include <boost/foreach.hpp>

#include <iostream>

using namespace online::display;
CSrsEventChannels::CSrsEventChannels() : 
m_local_mutex(),
m_eventid(), 
m_channels() 
{
}


CSrsEventChannels::CSrsEventChannels(const CSrsEventId& eventid,
                                     SrsChannelList& channels) : 
m_local_mutex(),
m_eventid(eventid), m_channels() 
{
   boost::mutex::scoped_lock lock(m_local_mutex);
   m_channels.splice(m_channels.end(), channels);
}


CSrsEventChannels::~CSrsEventChannels()
{
   
}


std::ostream& online::display::operator<< (std::ostream & out, const CSrsEventChannels & chan)
{
   
   int ii = 0;
   Q_FOREACH(SrsChannelPtr ch, chan.m_channels) {
//      out  << ii << " "<< *ch << std::endl;
//      ii ++;
   }
   return out;
}


void CSrsEventChannels::move_event_channels(SrsChannelList& channels)
{
   boost::mutex::scoped_lock lock(m_local_mutex);
   m_channels.splice(m_channels.end(), channels);
}

void CSrsEventChannels::process(SrsChipPtr chip)
{
   std::cout << "CSrsEventChannels::process chip " << chip->name() << std::endl;
   boost::mutex::scoped_lock lock(m_local_mutex);
   chip->process_event(m_channels);
}

/** pass event data to root writer (once) and all channel data*/
//void CSrsEventChannels::prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn)
//{
//   //filler is of correct type for the chip type
//   boost::mutex::scoped_lock lock(m_local_mutex);
//   filler->set_event_data(m_eventid);
//   std::cout << "CSrsEventChannels::prefill evid=" << m_eventid.event_assembly_id(AssembleBySrsTimeStamp)
//   << " m_channels.size=" << m_channels.size() << std::endl;
//   std::for_each(m_channels.begin(), m_channels.end(),
//                 boost::bind(&CSrsChannel::prefill, _1, filler, conn));
//   m_channels.clear();
//}


//void CSrsEventChannels::move_to_eb(boost::shared_ptr<CEventBuilderInputBuffer> eb_buffer)
//{
//   boost::mutex::scoped_lock(m_local_mutex);
//   eb_buffer->move_data_to_eb(m_channels);
//}

