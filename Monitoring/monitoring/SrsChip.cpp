//
//  SrsChip.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChip.h"
#include "SrsChannel.h"
//#include "EventBuilderInputBuffer.h"
#include "SrsEventChannels.h"
#include "DetConnector.h"
#include "mainwindow.cpp"

//#include "RootTreeFiller.h"
//#include <TFile.h>

#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <vector>
#include <string.h>

using namespace online::display;

CSrsChip::CSrsChip(const CSrsChipId& chip_id, const std::string& name) :
CSrsBase(chip_id.chip_id(), name),
m_chip_id(chip_id),
//m_event_channels(),
m_event_channels_flatmap(),
m_chip_mutex(),
m_connector(),
m_channel_strip_map(),
m_chip_raw_histo(new TH1D(name.c_str(),name.c_str(),4000,0,4000)),
  m_chip_statistics_histo(new TH1D(std::string("Statistics_"+name).c_str(),std::string("Statistics_"+name).c_str(),620,0,620)),
  m_chip_charge_statistics_histo(new TH1D(std::string("Charge_"+name).c_str(),std::string("Charge_"+name).c_str(),400,0,2000)),
  m_chip_time_statistics_histo(new TH1D(std::string("Time_"+name).c_str(),std::string("Time_"+name).c_str(),400,0,800)),
  m_chip_pedestal_mean(new TH2D(std::string("PedMean_"+name).c_str(),std::string("PedMean_"+name).c_str(),128,1,129,350,0,3500)),
  m_chip_pedestal_sigma(new TH2D(std::string("PedSigma_"+name).c_str(),std::string("PedSigma_"+name).c_str(),128,1,129,300,0,3000))
{


    //m_chip_raw_histo = new TH1D(name.c_str(),name.c_str(),4000,0,4000);
    m_chip_raw_histo->SetLineColor(kGreen+4);
    //m_chip_raw_histo->SetFillColor(38);
    m_chip_raw_histo->SetTitleSize(0.04);
    //m_chip_raw_histo->GetXaxis()->SetLabelSize(0.06);

//    m_chip_statistics_histo = new TH1D(histoName,histoName,640,0,620);
    m_chip_statistics_histo->SetLineColor(38);
    m_chip_statistics_histo->SetFillColor(38);
    m_chip_statistics_histo->SetTitleSize(0.04);
    m_chip_statistics_histo->GetXaxis()->SetLabelSize(0.06);

//    m_chip_charge_statistics_histo = new TH1D(chargehistoName,chargehistoName,300,0,3000);
    m_chip_charge_statistics_histo->SetLineColor(46);
    m_chip_charge_statistics_histo->SetFillColor(46);
    m_chip_charge_statistics_histo->GetXaxis()->SetLabelSize(0.06);
    m_chip_charge_statistics_histo->SetTitleSize(0.04);

//    m_chip_time_statistics_histo = new TH1D(timehistoName,timehistoName,30,0,600);
    m_chip_time_statistics_histo->SetLineColor(8);
    m_chip_time_statistics_histo->SetFillColor(8);
    m_chip_time_statistics_histo->GetXaxis()->SetLabelSize(0.06);
    m_chip_time_statistics_histo->SetTitleSize(0.04);

    // std::cout << "new chip " << m_chip_id.get_string() << " "  << name << std::endl;

    chip1dHistos.push_back(m_chip_statistics_histo);
    chip1dHistos.push_back(m_chip_charge_statistics_histo);
    chip1dHistos.push_back(m_chip_time_statistics_histo);
    chip1dHistos.push_back(m_chip_raw_histo);

    chip2dHistos.push_back(m_chip_pedestal_mean);
    chip2dHistos.push_back(m_chip_pedestal_sigma);
}

CSrsChip::~CSrsChip()
{

}



std::ostream& online::display::operator<< (std::ostream & out, const CSrsChip & chip)
{
   if (!chip.m_event_channels_flatmap.empty()) {
      SrsEventChannelsPtr evchans = chip.m_event_channels_flatmap.begin()->second;
      Q_FOREACH(SrsChannelPtr chan, evchans->channels()) {
         //out << chip.m_chip_id.chip_id() << *chan;
      }
   }
   return out;
}


void CSrsChip::reset()
{
   boost::mutex::scoped_lock lock(m_chip_mutex);
   m_event_channels_flatmap.clear();
   m_connector.reset();
}

size_t CSrsChip::uid() const
{
   return m_chip_id.chip_id();
}

void CSrsChip::set_chip_id_to_fec(size_t fec_id)
{
   boost::mutex::scoped_lock lock(m_chip_mutex);
   m_chip_id = CSrsChipId(fec_id, m_chip_id.chip_no());
}


void CSrsChip::move_received_event_data(const CSrsEventId& srs_event_id, SrsChannelList& channels )
{
   //if runModePhysics
   move_received_event_channels(srs_event_id, channels); // move and process

   //else if runModePedestals
   //calculate means, stdev, sigma
   calculate_pedestals(srs_event_id, channels);

}


void CSrsChip::move_received_event_channels(const CSrsEventId& srs_event_id, SrsChannelList& channels)
{
   typedef std::pair<EBEventIdType, SrsEventChannelsPtr> SrsEventChannelsMapType;

   boost::mutex::scoped_lock lock(m_chip_mutex);
   EBEventIdType event_id = srs_event_id.event_assembly_id(AssembleBySrsTimeStamp);

   SrsEventChannelsMap::iterator evtfound = std::find_if(m_event_channels_flatmap.begin(),
                                                      m_event_channels_flatmap.end(),
                                                      FirstComparator<SrsEventChannelsMapType>(event_id));
   if (evtfound == m_event_channels_flatmap.end()) {
      std::cout << "CSrsChip::move_received_event_channels() : in chip " << name() << " eventid="<< event_id << " not found" <<std::endl;

      std::pair<SrsEventChannelsMap::iterator, bool > rc =
      m_event_channels_flatmap.insert(SrsEventChannelsMapType(event_id,
                                                SrsEventChannelsPtr(new CSrsEventChannels(srs_event_id, channels)) ));
      evtfound = rc.first;
   }
   else {
      std::cout << "CSrsChip::move_received_event_channels() in chip "<< name()
      << "moving channels " << channels.size() << std::endl;
       evtfound->second->move_event_channels(channels);
   }


//   SrsEventChannelsPtr ecp = m_event_channels_flatmap[event_id];
//   if(!ecp) {
//      ecp.reset(new CSrsEventChannels(srs_event_id, channels));
//   }
//   else {
//      ecp->move_event_channels(channels);
//   }
   evtfound->second->process(shared_from_this());
}


void CSrsChip::calculate_pedestals(const CSrsEventId& srs_event_id, SrsChannelList& channels)
{
   std::cout << "CSrsChip::calculate_pedestals" << std::endl;

}


/**
 for each channel calculate qmax, and tbqmax as charge and time values
 */
void CSrsChip::calculate_channel_qt(SrsChannelList& channels)
{
   std::for_each(channels.begin(), channels.end(), boost::bind(&CSrsChannel::calculate_qt, _1));
}


void CSrsChip::insert_eventids_to_eb( boost::shared_ptr<CEventBuilderInputBuffer> eb_buffer)
{
//   typedef std::pair<EBEventIdType, SrsEventChannelsPtr> SrsEventMapPair;

//   boost::mutex::scoped_lock lock(m_chip_mutex);

//   if (m_event_channels_flatmap.empty())
//      return;


//   for (SrsEventChannelsMap::const_iterator it = m_event_channels_flatmap.begin();
//        it != m_event_channels_flatmap.end(); ++it) {
////      std::cout << "CSrsChip::insert_eventids_to_eb() evid = " << it->first  << "chans = " << it->second->size() << std::endl;
//      eb_buffer->insert_eventid(it->first);
//   }

////   Q_FOREACH(const SrsEventMapPair p, m_event_channels_flatmap) {
////      eb_buffer->insert_eventid(p.first);
////   }

}


//void CSrsChip::move_data_to_eb(EBEventIdType event_id,  boost::shared_ptr<CEventBuilderInputBuffer> eb_buffer)
//{
//
//   SrsEventChannelsPtr ecp = m_event_channels_flatmap[event_id];
//   if(ecp) {
//      ecp->move_to_eb(eb_buffer);
//   }
//   boost::mutex::scoped_lock(m_chip_mutex);
//   m_event_channels_flatmap.erase(event_id);
//}


/**
// copy event data (once) and channels' data to root tree filler
// */
//void CSrsChip::prefill(const EBEventIdType& evid, CRootWriter* writer)
//{
////   std::cout << " CSrsChip::prefill() chip " << name() << " for evid =" << evid << std::endl;
//   boost::mutex::scoped_lock lock(m_chip_mutex);
//   typedef std::pair<EBEventIdType, SrsEventChannelsPtr> SrsEventChannelsMapType;
//   SrsEventChannelsMap::iterator evtfound = std::find_if(m_event_channels_flatmap.begin(),
//                                                         m_event_channels_flatmap.end(),
//                                                         FirstComparator<SrsEventChannelsMapType>(evid));
//   if (evtfound != m_event_channels_flatmap.end()) {
//      CRootTreeFiller* filler = get_root_tree_filler(writer); // get correct filler for chip type
//      evtfound->second->prefill(filler, m_connector);
//      m_event_channels_flatmap.erase(evid);
//   }
//}



size_t CSrsChip::total_number_channels()
{
   typedef std::pair<EBEventIdType, SrsEventChannelsPtr> SrsEventMapPair;
   size_t counter = 0;
   boost::mutex::scoped_lock lock(m_chip_mutex);
   Q_FOREACH(SrsEventMapPair p, m_event_channels_flatmap) {
      if(p.second) {
         counter += p.second->size();
      }

   }
   return counter;
}

void CSrsChip::connect_to_connector(boost::shared_ptr<CDetConnector> conn)
{

//   m_connector = boost::dynamic_pointer_cast<CDetConnector>(conn);
   m_connector = conn;

   std::cout << "CSrsChip::connect_to_connector()  chip " << name()
   << " is connected to " << conn->name()  << " on " << (conn->parent().lock()?conn->parent().lock()->name(): "NOT CONNECTED") << std::endl;
}


bool CSrsChip::get_event_data(const EBEventIdType& evid, CSrsEventChannels& channels)
{
   typedef std::pair<EBEventIdType, SrsEventChannelsPtr> SrsEventChannelsMapType;

   SrsEventChannelsMap::iterator evtfound = std::find_if(m_event_channels_flatmap.begin(),
                                                         m_event_channels_flatmap.end(),
                                                         FirstComparator<SrsEventChannelsMapType>(evid));
   if (evtfound != m_event_channels_flatmap.end() && evtfound->second) {
      channels.assign(evtfound->second->channels());
      return true;
   }
   return false;
}

void CSrsChip::set_channel_strip_map(std::vector<ChannelStripPair>& ccmap)
{
   m_channel_strip_map = ccmap;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TH1D *CSrsChip::getRawHisto()
{
    return m_chip_raw_histo;
}

TH1D *CSrsChip::getChipStatistcsHisto()
{
    return m_chip_statistics_histo;
}

TH1D *CSrsChip::getChipChargeStatistcsHisto()
{
    return m_chip_charge_statistics_histo;
}

TH1D *CSrsChip::getChipTimeStatistcsHisto()
{
    return m_chip_time_statistics_histo;
}


TH2D* CSrsChip::getChipPedestalMean()
{
    return m_chip_pedestal_mean;
}

TH2D* CSrsChip::getChipPedestalSigma()
{
    return m_chip_pedestal_sigma;
}



void CSrsChip::fill_histogram(std::vector <float> *fill_values_x, std::vector <float> *fill_values_y)
{
    for(size_t iHisto=0; iHisto<fill_values_x->size(); iHisto++)
    {
        m_chip_raw_histo->SetBinContent(fill_values_x->at(iHisto), fill_values_y->at(iHisto));
    }
}

//void CSrsChip::clear_histograms()
//{
//    m_chip_raw_histo->Reset();
//}

