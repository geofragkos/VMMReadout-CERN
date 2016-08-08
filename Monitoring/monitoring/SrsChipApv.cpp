//
//  SrsChipApv.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 18/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChipApv.h"
#include "SrsChannelApvRaw.h"
//#include "RootTreeFillerApvRaw.h"
//#include "RootWriter.h"
#include "SrsEventChannels.h"
#include "SRSEquipmentHeader.h"
//#include "RootReader.h"

#include <TFile.h>

#include <boost/bind.hpp>
//#include <boost/foreach.hpp>

#include <iostream>
#include <stdexcept>
#include <utility>

using namespace online::display;

double CSrsChipApv::ApvZsThreshold = 1.123;
double CSrsChipApv::ApvTimeBinDurationNanoSec = 25.0;
std::string CSrsChipApv::ApvPedestalFileName = "";
CSrsChipApv::ApvCrosstalkMapType CSrsChipApv::ApvCrosstalkMap = CSrsChipApv::ApvCrosstalkMapType();

//CRootTreeFillerApvRaw* CSrsChipApv::m_root_tree_filler_apvraw = 0;


CSrsChipApv::CSrsChipApv(const CSrsChipId& chip_id, const std::string& name) :
CSrsChip(chip_id, name),
//m_pedestals(),
m_pedestals_map(),
m_data_in_peak_mode(false)
{ }


CSrsChipApv::~CSrsChipApv()
{
   
}
 
bool CSrsChipApv::check_equipment_key(uint32_t eqkey)
{
   return SRSEqHdrKeys::SRSEquipmentApvRaw == eqkey 
   || SRSEqHdrKeys::SRSEquipmentApvZeroSuppressed == eqkey;
}


void CSrsChipApv::set_zero_suppression_threshold(double threshold_factor)
{
   CSrsChipApv::ApvZsThreshold = threshold_factor;
}


void CSrsChipApv::set_pedestal_filename(const std::string& filename)
{
   CSrsChipApv::ApvPedestalFileName = filename;
}

void CSrsChipApv::load_pedestal_file()
{   
   
//   if (CSrsChipApv::ApvPedestalFileName.empty()) {
//      std::cout << "WARN: APV pedestal file not specified" << std::endl;
//      return;
//   }
//   clear_pedestals();
//   //load file (root)
//   CRootReader::load_pedestal_file_apv(CSrsChipApv::ApvPedestalFileName, m_chip_id, m_pedestals_map);
//   std::cout << "CSrsChipApv::load_pedestal_file() pedestal info for " <<  m_pedestals_map.size() << " channels" <<  std::endl;
//   std::cout << "TODO: check if all my channels have pedestal values?" << std::endl;
}



SApvPedestal CSrsChipApv::get_pedestal(const uint32_t& channel_id) const
{
   ApvPedestalMap::const_iterator found = m_pedestals_map.find(channel_id);
   if (found == m_pedestals_map.end()) {
      return SApvPedestal(0, 0.0, 0.0, 0.0);
   }
   return found->second;
   
//   ApvPedestalVect::const_iterator found = std::find_if(m_pedestals.begin(), m_pedestals.end(), SApvPedestal::EqualChannel(channel_id));
//   if (found == m_pedestals.end()) {
//      return SApvPedestal(0, 0.0, 0.0, 0.0);
//   }
//   return *found;
}


void CSrsChipApv::set_pedestal(const uint32_t& channel_id,  const std::pair<double, double>& ped)
{
   m_pedestals_map[channel_id] = SApvPedestal(CSrsChannelId(channel_id).channel_no(), ped.first, ped.second, 0.0);
   
//   ApvPedestalVect::iterator found = std::find_if(m_pedestals.begin(), m_pedestals.end(), SApvPedestal::EqualChannel(channel_id));
//   if (found == m_pedestals.end()) {
//      m_pedestals.insert(m_pedestals.end(), SApvPedestal(channel_id, ped.first, ped.second, 0.0));
//      std::sort(m_pedestals.begin(), m_pedestals.end(), SApvPedestal::LessChannel());
//   }
//   else {
//      found->mean = ped.first;
//      found->stddev = ped.second;
//   }
}


void CSrsChipApv::calculate_pedestals(const CSrsEventId& srs_event_id, SrsChannelList& channels)
{
   std::cout << "CSrsChipApv::calculate_pedestals" << std::endl;
   
   for (SrsChannelList::iterator chan = channels.begin(); chan != channels.end(); ++chan) {
      size_t channo = (*chan)->channel_id().channel_no();
      (*chan)->calculate_channel_pedestal();

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
}


/** 
 clear pedestals and crosstalk definitions
 */
void CSrsChipApv::clear_pedestals()
{
   m_pedestals_map.clear();
}


void CSrsChipApv::set_crosstalk_map(const ApvCrosstalkMapType& xtalkmap)
{
   ApvCrosstalkMap = xtalkmap;
}

void CSrsChipApv::clear_crosstalk_map() 
{
   ApvCrosstalkMap.clear();  
}


void CSrsChipApv::set_peak_mode(bool enable)
{
   m_data_in_peak_mode = enable;
}

bool CSrsChipApv::get_peak_mode() const
{
   return m_data_in_peak_mode;
}

void CSrsChipApv::process_event( SrsChannelList& channels)
{
   
   //should not run for ZS data
   if (m_data_in_peak_mode) {
      process_event_zs(channels);
   } else {
      process_event_raw(channels);
      calculate_channel_qt(channels);
   }
}

void CSrsChipApv::process_event_raw(SrsChannelList& channels)
{
//   //   std::cout << "CSrsEventApvRaw::process_event" << std::endl;
//   //   CSrsEvent::TheDaq
   
//   if (channels.empty())
//      return;
   
//   //   std::cout << " CApvRawSrsEvent::process_event() channels to suppress = " <<
//   //   std::count_if(m_channels.begin(), m_channels.end(), boost::bind(&CSrsChannel::is_suppressed, _1))
//   //   << std::endl;
   
   
//   //subtract pedestals and check for signal, mark for suppression
//   std::for_each(channels.begin(), channels.end(), boost::bind(&CSrsChannel::process_channel , _1, this) );
   
   
//   //   std::cout << "  -> after process_channel channels to suppress = " <<
//   //   std::count_if(m_channels.begin(), m_channels.end(), boost::bind(&CSrsChannel::is_suppressed, _1))
//   //   << std::endl;
   
   
//   //correct crosstalk, and mark for suppression
//   correct_apv_crosstalk(channels);
   
////   CSrsChannelApvRaw::CheckForSignal signal_checker(CSrsChipApv::ApvZsThreshold, m_pedestals_map);
      
////   Q_FOREACH(SrsChannelPtr chan, channels) {
////      if (chan->check) {
////       //  <#statements#>
////      }
////   }
   
   
//    //remove suppressed channels
//   channels.remove_if(channels.begin(), channels.end(),
//                      boost::bind( &CSrsChannelApvRaw:: check_for_signal, _1, m_sigma_cut, m_pedestals_map[_1.channel_id().channel_id()] ));
                      
//   //   std::cout << "  -> after correct_apv_crosstalk channels to suppress = " <<
//   //   std::count_if(m_channels.begin(), m_channels.end(), boost::bind(&CSrsChannel::is_suppressed, _1))
//   //   << std::endl;
   

}

void CSrsChipApv::process_event_zs(SrsChannelList& channels)
{
   std::cout << "TODO: implement CSrsChipApv::process_event_zs()" << std::endl;
   
}


/* myComments 20/11/2014
void CSrsChipApv::correct_apv_crosstalk(SrsChannelList& channels) 
{
   std::cout << "TODO: review CSrsEventApvRaw::correct_apv_crosstalk (add variation by chip)" << std::endl;
   
   typedef CSrsChipApv::ApvCrosstalkMapType::const_iterator xtalkmap_citer;
   if (CSrsChipApv::ApvCrosstalkMap.empty()) {
      return;
   }   
   
   int counter = 0;
   for ( SrsChannelList::iterator istrip = channels.begin(); istrip != channels.end(); ++istrip) {
      
      unsigned chnum = (*istrip)->channel_id().channel_no(); //ignore chip num and fec num
      CSrsChipId ghostApvId = (*istrip)->channel_id().chip_id();//apvIdCh >> 8;  //same apvId
      
      
      xtalkmap_citer ictalks = CSrsChipApv::ApvCrosstalkMap.find(chnum);
      if (ictalks == CSrsChipApv::ApvCrosstalkMap.end()) {
         continue; //this channel doesn't talk
      }
      unsigned ghostChnum = ictalks->second.at(0);	 //vector<int>(3) read from -> CConfiguration -> LoadCrosstalkMapFile()
      CSrsChannelId ghostChId = CSrsChannelId(ghostApvId, ghostChnum);
      double ghostCorrection = (double)(ictalks->second.at(1)) / 100.0;  
      
      SrsChannelList::iterator ighost = std::find_if(channels.begin(), channels.end(), 
                                                   CSrsChannel::ChannelIdEquals(ghostChId) );
      if (ighost == channels.end()) {
         continue; // no ghost, nothing to correct
      }
      const CSrsChannelApvRaw* ref_chan = dynamic_cast <const CSrsChannelApvRaw*>(istrip->get());
      CSrsChannelApvRaw* ghost_chan = dynamic_cast <CSrsChannelApvRaw*>(ighost->get());
      
      if (!ref_chan || !ghost_chan) {
         throw std::runtime_error("CSrsEventApvRaw::correct_apv_crosstalk() dynamic_cast of channels failed. ");
      }
      
      if (ref_chan->data_size() != ghost_chan->data_size()) {
         throw std::runtime_error("ERROR: CSrsEventApvRaw::correct_apv_crosstalk(): number of time bins varies between channels ! Data Corrupted. "); //TODO: replace by data corrupted ex
      }
      
      //apply correction to vector of ighost: m_apvevent_data[apvIdCh] that is ichan->second
      ghost_chan->subtract_crosstalk(*ref_chan, ghostCorrection);
      
      ++counter;
      
   } //for m_channels

}
*/

//TTree* CSrsChipApv::create_root_tree_filler(TFile* rootfile)
//{
//   if (!m_root_tree_filler_apvraw) {
//      m_root_tree_filler_apvraw = new CRootTreeFillerApvRaw(rootfile);
//   }
//   return m_root_tree_filler_apvraw->get_tree();
//}
//
//void CSrsChipApv::delete_root_tree_filler()
//{
//   delete m_root_tree_filler_apvraw;
//   m_root_tree_filler_apvraw = 0;
//}
//

//CRootTreeFiller* CSrsChipApv::get_root_tree_filler(CRootWriter* writer)
//{
//   if (m_data_in_peak_mode) {
//      return writer->get_filler_apv_peak();
//   }
//   return writer->get_filler_apv_raw();
//}





