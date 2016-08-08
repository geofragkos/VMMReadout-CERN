//
//  EventBuilderInputBuffer.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 03/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_EventBuilderInputBuffer_h
#define online_display_EventBuilderInputBuffer_h


#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif


#include "EventTypes.h"
#include "EventBase.h"
//#include "MMEvent.h"
#include "SrsTypes.h"
#include "SharedBufferHandler.h"


#include <boost/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <boost/multi_index/ordered_index.hpp>

#include <cstddef>
#include <map>
#include <vector>


namespace online {
   namespace display {
      
      class CDaqServer;
      class CEventBase;
      class CEventBuilderOutputBuffer;
      class CSharedEBBuffer;
      class CRootWriter;
      class CPublisherBase;
      
      
      //////////////////////////////////////////////////////////////////////////
      /**
       event bulder input buffer gets data from all srs network deevices and
       assembles CMMevents by selected EBAssemblyType
       internal indices by event_id, timeout, depth
       */
      class CEventBuilderInputBuffer
      {
         
         
         
         
         
      public:
         explicit CEventBuilderInputBuffer(CDaqServer& daqserver);
         virtual ~CEventBuilderInputBuffer();
         
         void clear();
         void insert_eventid(EBEventIdType eventid);
         
         
         bool move_from_chips(SrsChipPtr chip, SrsEventMap& events);

         void pop();
         bool empty() const;
         
         /// events are ready to be taken out
         bool ready();
         
         size_t size() const;
//         EventBasePtr get_event(EBEventIdType time_stamp);

         // EB will wait for events' data until one of the following is met:
         static const size_t EBInputBufferWaitMaxSizeEvents = 300; ///< max size of input buffer
         static const size_t EBInputBufferWaitDelayMicroSeconds = 400000; ///< max size of EB delay in input buffer, from first push
         
         void move_to_output(boost::shared_ptr<CRootWriter> writer,
                             boost::shared_ptr<CSharedBufferHandler> shbuff_handler);
         
         

         
//         bool get_timedout_events(boost::posix_time::ptime time_limit, 
//                                  MMEventList& events);
         
//         bool get_events_by_depth(size_t depth, MMEventList& events);
         
         struct update_event_list {
            
         };
         
         void configure(size_t chip_count);
         
      protected:
         
         CDaqServer& m_daqserver;
         
         
         struct EBInBufferDataType {
            EBInBufferDataType(const EBEventIdType& eid, const boost::posix_time::ptime& tm) : eventid_(eid), timeout_(tm) {}
            EBEventIdType eventid_;
            boost::posix_time::ptime timeout_;
            
            struct SetEventId {
               EBEventIdType evid_;
               SetEventId(EBEventIdType e) : evid_(e) {}
               void operator() (EBInBufferDataType& data) const {
                  data.eventid_ = evid_;
               }
            };
         };
         
         
         
                  
//         struct event_id{}; // tag
//         struct timeout{}; // tag 
//         
//         
//         typedef boost::multi_index::multi_index_container<
//            EBInBufferDataType,
//            boost::multi_index::indexed_by<
//               boost::multi_index::ordered_unique<      
//                  boost::multi_index::tag<event_id>, 
//                  boost::multi_index::member<EBInBufferDataType, EBEventIdType, &EBInBufferDataType::eventid_> >,
//               boost::multi_index::ordered_non_unique<      
//                  boost::multi_index::tag<timeout>, 
//                  boost::multi_index::member<EBInBufferDataType, boost::posix_time::ptime, &EBInBufferDataType::timeout_> >
//            >
//         > MiStorageType;
//         typedef MiStorageType::index<event_id>::type MiStorageByEventId;
//         typedef MiStorageType::index<timeout>::type MiStorageByTimeOut;
//         
//         MiStorageType m_multi_storage;
         
         typedef std::pair<boost::posix_time::ptime, EBEventIdType> StoragePair;
         typedef std::map<boost::posix_time::ptime, EBEventIdType> StorageMap;
         StorageMap m_storage;
         
         EBEventIdType m_min_accepted_id; // careful! this wraps around - allowed could be the high values if this is low...

         boost::mutex m_storage_mutex;
         
         /// select varialbe on which to assembe events 
         EBAssemblyType m_assembly_type; 
         size_t m_chip_count;
         
         bool m_publisher_request_pending;
         // reserve number_of_devices, 
         // 
   
      };
   }
} // namespace

#endif
