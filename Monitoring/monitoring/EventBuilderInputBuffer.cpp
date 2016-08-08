//
//  EventBuilderInputBuffer.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 03/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "EventBuilderInputBuffer.h"
#include "EventBuilderOutputBuffer.h"
#include "SrsEvent.h"
#include "Logger.h"
#include "SrsTypes.h"
#include "SharedBufferHandler.h"
#include "RootWriter.h"
#include "DaqServer.h"
#include "PublisherBase.h"


#include <boost/foreach.hpp>

#include <iostream>
#include <algorithm>
#include <utility>

using namespace mmdaq3::server;
namespace bpt = boost::posix_time;
/** 
 create internal buffers of the size of the number  of devices
 */
CEventBuilderInputBuffer::CEventBuilderInputBuffer(CDaqServer& daqserver) :
m_daqserver(daqserver),
//m_multi_storage(),
m_storage(),
m_min_accepted_id(0),
m_storage_mutex(),
m_assembly_type(AssembleBySrsTimeStamp),
m_chip_count(0),
m_publisher_request_pending(false)
{
   
}


CEventBuilderInputBuffer::~CEventBuilderInputBuffer()
{
   
}


void CEventBuilderInputBuffer::clear()
{
//   boost::recursive_mutex::scoped_lock lock(m_storage_mutex);
//   m_internal_storage.clear();
   
}


void CEventBuilderInputBuffer::insert_eventid(EBEventIdType eventid)
{
   boost::posix_time::ptime timeout(bpt::microsec_clock::local_time() 
                                    + bpt::microseconds(EBInputBufferWaitDelayMicroSeconds));
 
   boost::mutex::scoped_lock lock(m_storage_mutex);
   
   StorageMap::iterator found = std::find_if(m_storage.begin(), m_storage.end(), SecondComparator<StoragePair>(eventid));
                                       
   if(found == m_storage.end() ){
      m_storage.insert(std::pair<boost::posix_time::ptime, EBEventIdType>(timeout, eventid) );
   }   
   
                                             
   
   
//   
//   
//   MiStorageByEventId& idx_by_id = m_multi_storage.get<event_id>();
//   std::pair<MiStorageByEventId::iterator, bool> resp =   idx_by_id.insert( EBInBufferDataType(eventid, timeout));
//   
//   if (!resp.second) {
//      //found existing, must add data
//      m_multi_storage.modify(resp.first, EBInBufferDataType::SetEventId(eventid));
//   }
////   
//   MiStorageByEventId::iterator found = idx_by_id.find(eventid);
//   if(found == idx_by_id.end() ) {
//   
//   }
  
   
}

void CEventBuilderInputBuffer::configure(size_t chip_count)
{
   m_chip_count = chip_count;
}

/**
 
 //TODO: keep data in the chips, get the eventID only, build event on that
 
 move events from chips to buffer
 insert data from a device to the input buffer (lock first)
 skips stale event data (compares timestamp(or event _id ) with min_accepted_id
 when inserting mmevent with new EBEventIdType the timeout is set for that mmevent
 when inserting new srsevent to existing mmevent timeout is preserved
 */
//bool CEventBuilderInputBuffer::move_from_chips(SrsChipPtr chip, SrsEventChannelsMap& events)
//{
////   typedef std::map<EBEventIdType, SrsChannelList> SrsEventMap;
//
//   boost::mutex::scoped_lock lock(m_storage_mutex);
//   boost::posix_time::ptime timeout(bpt::microsec_clock::local_time() 
//                                    + bpt::microseconds(EBInputBufferWaitDelayMicroSeconds));
//   
//
//   for (SrsEventMap::iterator it = events.begin(); it != events.end(); ++it) {
//      
//      const EBEventIdType& evid = it->first;//assamble by this
//      
//      CSrsEventId& srsevid = it->second.first;
//      SrsChannelList& chans = it->second.second;
////      if (m_min_accepted_id > evid) {
////         //careful! wraps around
////         // this event data is late (ts < min_accepted_ts) -> drop frame
////         //std::cout << "CEventBuilderInputBuffer::push() droping stale CSrsEvent eid= " << evid  << std::endl;
////         //continue;
////      }
//      
//      MiStorageByEventId& idx_by_id = m_multi_storage.get<event_id>();
//      MiStorageByEventId::iterator mmevt   = idx_by_id.end();
//      //returns index of existing element or of a new one, secound = true means new has been inserted
//      std::pair<MiStorageByEventId::iterator, bool> resp = idx_by_id.insert(MMEventPtr(new CMMEvent(evid, srsevid)));
//      mmevt = resp.first;
//      if (resp.second) {
//         //this is a new event with this event_id(aka time stamp, set timeout)
//         m_multi_storage.modify( mmevt, CMMEvent::AddNewChipData(chip, boost::ref(chans), timeout) );
//      }
//      else {
//         m_multi_storage.modify( mmevt, CMMEvent::AddChipData(chip, boost::ref(chans)) );
//      }
//
//      
//   }
//   return true;
//}




/**
 move to out buffer events that fulfil criteria: time, number of chips
 */
void CEventBuilderInputBuffer::move_to_output(boost::shared_ptr<CRootWriter> writer,
                                              boost::shared_ptr<CSharedBufferHandler> shbuff_handler)
{
   boost::mutex::scoped_lock lock(m_storage_mutex);
   if (m_storage.empty()) {
      return;
   }
   
   StorageMap::iterator first_good = m_storage.upper_bound(boost::posix_time::microsec_clock::local_time());
   if (first_good != m_storage.begin()) {
      std::stringstream ss;
      ss << "CEventBuilderInputBuffer::move_events() matured events "<< std::distance(m_storage.begin(), first_good);
      CLogger::instance()->msg(ss.str());
   }
   
   
//   //TOO: re do - must lock output buffer, within the output buffer
//   for (MiStorageByTimeOut::iterator it = idx_by_time.begin(); it != first_good; ++it) {
//      outbuffer->insert(*it);
////      sharedbuffer->insert(*it);
////      m_min_accepted_id = std::max(m_min_accepted_id, (*it)->event_id());
//   } 
   
//   size_t sz_before = idx_by_time.size();
   writer->insert_events_by_id(m_storage.begin(), first_good);
   shbuff_handler->publish_event(m_storage.begin()->second);
   m_daqserver.get_publisher()->get_event_data(m_storage.begin()->second);
   m_daqserver.get_publisher()->notify();
   
   m_storage.erase(m_storage.begin(), first_good);
//   size_t sz_after =    idx_by_time.size();   

//   std::cout << "CEventBuilderInputBuffer::move_to_output before=" << sz_before << " after=" << sz_after << std::endl;
   
   //TODO REMOVE
//   shbuff_handler->clear();
   // move ready events by chip count - disabled - possible out of order move to outbuffer
   // do we care?
   
//   //select ready events
//   MiStorageByChipCount & idx_by_chips = m_multi_storage.get<chip_count>();
//   MiStorageByChipCount::iterator first_ready = idx_by_chips.lower_bound(m_chip_count);
//   for (MiStorageByChipCount::iterator it = first_ready; it != idx_by_chips.end(); ++it) {
//      outbuffer->insert(*it);
//      m_min_accepted_id = std::max(m_min_accepted_id, (*it)->event_id());
//   }
//   idx_by_chips.erase(first_ready, idx_by_chips.end());
      
}




//
//   // check for late incoming frames
//   //what to do if after we removed  events , new frame from that event comes late 
//   //-> make earliest accepted timestamp = removed timestamp
//   
//   
//   //check for buffer size
//   
//   
//   //
//   for(modulo_indexed_set::iterator it=m.begin();it!=m.end();++it){
//      std::cout<<*it<<" -> ( ";
//      
//      nth_index<modulo_indexed_set,1>::type::iterator it0,it1;
//      boost::tie(it0,it1)=get<1>(m).equal_range(*it);
//      std::copy(it0,it1,std::ostream_iterator<unsigned int>(std::cout," "));
//      
//      std::cout<<")"<<std::endl;
//   }




//TODO: CHECKS:
//1. check for timed-out events
//2. check for late incoming frames
//what to do if after we removed  events , new frame from that event comes late 
//-> make earliest accepted timestamp = removed timestamp
//3. check for buffer size




/**
 copy event ids of all events before the time_limit
 true if such events exist
 */
//bool CEventBuilderInputBuffer::get_timedout_events( boost::posix_time::ptime time_limit, 
//                                               MMEventList& events)
//{
////   TimeoutCont::iterator lb = m_event_timeout.lower_bound(time_limit);
////   for (TimeoutCont::iterator it= m_event_timeout.begin(); it != m_event_timeout.end(); ++it) {
////      events.push_back(it->second);
////   }
////   return lb != m_event_timeout.begin();
//   
//}

/**
 copy event ids of all events with more data than depth frames
 true if such events exist
 */
//bool CEventBuilderInputBuffer::get_events_by_depth(size_t depth, MMEventList& events)
//{
////   DepthEventCont::const_iterator ub = m_event_depth.upper_bound(depth);
////   for (DepthEventCont::const_iterator it = ub; it != m_event_depth.end(); ++it) {
////      events.push_back(it->second);
////   }
////   return ub != m_event_depth.end();
//}


//EventBasePtr CEventBuilderInputBuffer::get_event(EBEventIdType time_stamp)
//{
//   
//}


void CEventBuilderInputBuffer::pop()
{
   
}


bool CEventBuilderInputBuffer::empty() const
{
   return m_storage.empty();
}


bool CEventBuilderInputBuffer::ready()
{
   boost::mutex::scoped_lock lock(m_storage_mutex);
   if (m_storage.empty()) {
      return false;
   }
   StorageMap::const_iterator first_good = m_storage.upper_bound(boost::posix_time::microsec_clock::local_time());   
   return (first_good != m_storage.begin());
}



size_t CEventBuilderInputBuffer::size() const
{
   size_t sz = 0;
   
   return m_storage.size();
}
