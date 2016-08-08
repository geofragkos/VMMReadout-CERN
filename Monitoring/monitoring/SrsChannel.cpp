//
//  SrsChannel.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannel.h"
#include "SrsTypes.h"
//#include "EventTypes.h"

//#include "SrsEventId.h"

//#include <boost/thread/mutex.hpp>

#include <iostream>

using namespace online::display;

CSrsChannel::CSrsChannel(const CSrsChannelId& channel_id) :
m_channel_id(channel_id),
m_channel_data(),
m_suppressed(false)
{
   
}

CSrsChannel::CSrsChannel(const CSrsChannelId& channel_id, const CSrsChannelData& data) :
m_channel_id(channel_id),
m_channel_data(data),
m_suppressed(false)
{
   
}

CSrsChannel::~CSrsChannel()
{
   
}

//std::ostream& online::display::operator<< (std::ostream & out, const CSrsChannel & chan)
//{
//   out
//   << chan.m_channel_id << " "
//   << chan.m_channel_data;
//   return out;
//}

double CSrsChannel::calculate_channel_pedestal()
{
   

}


/*
 //calculate for channel icurr_ch->first
 double curr_mean  = icurr_ch->second[0];
 double curr_stdev = icurr_ch->second[1];
 //double curr_sigma = icurr_ch->second[2];
 
 double prevmean  = pedestal_map[icurr_ch->first][0];
 double prevstdev = pedestal_map[icurr_ch->first][1];
 
 pedestal_map[icurr_ch->first][0] += (curr_mean - prevmean) / (m_event_counter_pedestals + 1); //mean of mean
 pedestal_map[icurr_ch->first][1] += (curr_stdev - prevstdev) / (m_event_counter_pedestals + 1); //mean of stdev
 pedestal_map[icurr_ch->first][2] += ( (curr_mean - prevmean) * (curr_mean - pedestal_map[icurr_ch->first][0] ) ); //sigma
 
 */
