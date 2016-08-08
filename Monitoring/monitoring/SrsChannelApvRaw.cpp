//
//  SrsChannelApvRaw.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//
#define EPS 0.000001

#include "SrsChannelApvRaw.h"
#include "SrsChip.h"
//#include "RootWriter.h"
//#include "RootTreeFiller.h"
#include "DetConnector.h"

#include <iterator>
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace online::display;

CSrsChannelApvRaw::CSrsChannelApvRaw(const CSrsChannelId& channel_id, const std::vector<int16_t>& data) :
CSrsChannel(channel_id),
m_peak_data_only(m_channel_data.m_user_data.empty()),
m_adc_data(m_channel_data.m_user_data)
//m_suppressed(false)
{
   m_channel_data.m_user_data.assign(data.begin(), data.end());
   
   
   //TODO: DUMMY DATA
   m_channel_data.m_user_data.push_back(13.134);
   m_channel_data.m_user_data.push_back(13.135);
   m_channel_data.m_user_data.push_back(13.136);
}


CSrsChannelApvRaw::CSrsChannelApvRaw(const CSrsChannelId& channel_id, const CSrsChannelData& data) :
CSrsChannel(channel_id, data),
m_adc_data(m_channel_data.m_user_data)
{
   
}

CSrsChannelApvRaw::~CSrsChannelApvRaw()
{
   
}

/**
 remove pedestals, chech for zero suppression
 */
void CSrsChannelApvRaw::process_channel(const CSrsChip* source_chip) 
{
   if (m_peak_data_only) {
      // data are coming from FEC ZS enabled with peak data only q,t already in the channeldata 
      return;
   }
   
   if (m_adc_data.empty() || m_suppressed) {
      m_suppressed = true;
      return;
   }
   const CSrsChipApv* apv_chip = dynamic_cast<const CSrsChipApv*>(source_chip);
   if (!apv_chip) {
      throw std::runtime_error("CSrsChannelApvRaw in bad chip type, not a CSrsChipApv ");
   } 
   
   // this will mark for suppression unmapped channels - pedestal is 0,0 -> signal negative
   SApvPedestal pedestal = apv_chip->get_pedestal(m_channel_id.channel_no());
   if ( std::fabs(pedestal.mean) > EPS || std::fabs(pedestal.stddev > EPS)) {
      //subtract_pedestal, 
      std::transform(m_adc_data.begin(), m_adc_data.end(), m_adc_data.begin(), SubtractPedestal(pedestal.mean));
      
      //flag suppressed for zero suppression
      m_suppressed = check_for_signal(CSrsChipApv::ApvZsThreshold, pedestal.stddev);
   }
   else {
      //TODO:
      //std::cerr << "CSrsChannelApvRaw::process_channel no pedestal for channel " << m_channel_id.to_string() << std::endl;
   }
}

/**
 check if channel has data above/below specified threshold and should be zero suppressed
 */
bool CSrsChannelApvRaw::check_for_signal(double sigma_cut, double& pedstddev) const
{
   double sumdiff = GetVectorSumOfDiff<double, double>(m_adc_data, 0.0);
   return (sumdiff/m_adc_data.size() > sigma_cut * pedstddev);
}

/**
 remove crosstalk from adc data (subtract ghost correction from each time bin charge)
 */
void CSrsChannelApvRaw::subtract_crosstalk(const CSrsChannelApvRaw& ref_chan, double ghostCorrection)
{
   const std::vector<double>& ref_data = ref_chan.m_adc_data;
   int tbin = 0;
   for (std::vector<double>::iterator itb = m_adc_data.begin() ; itb != m_adc_data.end() ; ++itb , ++tbin) {
      double newval = (double)(ref_data[tbin]) - std::fabs((double)(*itb) * ghostCorrection);
      *itb = (newval > 0.0) ? newval : 0.0;
   }
}

/**
 calculate charge and time : qmax and tbqmax in nanoseconds from adc data
 */
void CSrsChannelApvRaw::calculate_qt()
{
   if (m_peak_data_only) {
      return;
   }
   user_data_type::iterator it = std::max_element(m_adc_data.begin(), m_adc_data.end());
   m_channel_data.m_charge = *it;
   m_channel_data.m_time = std::distance(m_adc_data.begin(), it) * CSrsChipApv::ApvTimeBinDurationNanoSec;
}

/**
 call correct prefill function from the writer
 */
//void CSrsChannelApvRaw::prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn)
//{
//   if(DetConnPtr cptr = conn.lock()) {
//      CDetStripId strip_id = cptr->get_mapped_strip_id(m_channel_id);
//      std::cout << "CSrsChannelApvRaw::prefill() "<< m_channel_id.to_string() << std::endl;
//      std::cout << "CSrsChannelApvRaw::prefill() implement mapping! "<< m_channel_id.to_string() << std::endl;
//      filler->prefill_channel(m_channel_id, m_channel_data, strip_id );
//   }
//   else {
//      std::cout << "CSrsChannelApvRaw::prefill connector weak ptr lock failed" << std::endl;
//   }
   
   

   
//}




