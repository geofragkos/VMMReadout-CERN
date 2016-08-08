//
//  SharedBufferHandler.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 09/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SharedBufferHandler_h
#define online_display_SharedBufferHandler_h

#include "SharedEBBuffer.h"

#include "SharedTypes.h"
//#include "SharedMMEvent.h"

//#include <boost/foreach.hpp>

namespace online {
   namespace display {
      
     //class CDaqServerConfig;
      
      
      /**
       handles IO and confureation of the shared buffer with event data
       */
      class CSharedBufferHandler
      {
         
      public:
         CSharedBufferHandler();
         virtual ~CSharedBufferHandler();
         
         template <class InputIterator>
         void insert_events_by_id(InputIterator first, InputIterator last);
         void clear();
	 // void configure(const CDaqServerConfig* daqconfig);
         size_t max_event_depth() const;
         void print();

      protected:
         bipc::managed_shared_memory m_shm_manager;
         ShmemMutexType* m_shm_mutex;
//         CSharedEBBuffer* m_shm_buffer;
         ShmemChipVector* m_shm_chips;
         size_t m_max_event_depth;
      };
      
      
      
      template <class InputIterator>
      void CSharedBufferHandler::insert_events_by_id(InputIterator first, InputIterator last)
      {
         
//         bipc::managed_shared_memory segment(bipc::open_only, "mmDaqSharedEBSegment");
//         CSharedEBBuffer *mybuff = segment.find<CSharedEBBuffer>("mmDaqSharedEvents").first;
//         std::cout << "*** CSharedBufferHandler::insert() mmDaqSharedEvents = "<< mybuff  << std::endl;
//         ShmemVoidAllocator alloc_inst (segment.get_segment_manager());
//         if (!mybuff) {
//            return;
//         }
//         
//         bipc::scoped_lock<bipc::interprocess_mutex> lock(mybuff->m_mutex);
         
         
         
         //insert MMEVT data into 
         // typedef bipc::vector<CSharedChip, ShmemChipAllocator> ShmemChipVector;
         // ShmemChipVector m_chips;
         //
         // each chip has:
         // SharedChipType m_chip_type;
         // typedef std::pair<const uint64_t, ShmemChannelVector> EventMapValueType;
         // ShmemEventMap m_events;
         
         while (first!=last) {
            
            //TODO: as in writer
            
//            boost::shared_ptr<CMMEvent> mmevt = *first;
//            std::cout << " *** insert_mmevt() to shared buffer id=" << mmevt->event_id() << " *** ";
//            m_max_event_depth = mybuff->max_event_depth();
//            mybuff->insert_mmevent(*first, alloc_inst);
            
            
//            mybuff->m_val *= 2;
//            //            mybuff->m_dvec.push_back(mybuff->m_val);
//            CSharedMMEvent ev(*first, alloc_inst);
//            ev.dvec.push_back(mybuff->m_val);
//            mybuff->m_evts.push_back(ev);
//            
//            
//            Q_FOREACH(CSharedMMEvent& mevt, mybuff->m_evts) {
//               mybuff->m_val *= 2;
//               std::cout << "pushing "<<  mybuff->m_val << " to " << mevt.event_id() << std::endl;
//               ShmemDblVector vec(alloc_inst);
//               vec.push_back(3.1 * mybuff->m_val);
//               vec.push_back(3.2 * mybuff->m_val);
//               mevt.m_ch_user_data.push_back(vec);
//            }
//            
//            
//            //            mybuff->m_svec.push_back(ShmemCharString("kota", alloc_inst));
//            std::cout << " *** ("  << mybuff->m_val << ") ***";
//            
//            Q_FOREACH(CSharedMMEvent& mevt, mybuff->m_evts) {
//               std::cout << "printing mmevt " << mevt.event_id() << ": "; 
//               
//               Q_FOREACH(ShmemDblVector& dv, mevt.m_ch_user_data) {
//                  Q_FOREACH(double d, dv) {
//                     std::cout << " " << d;
//                  }
//               }
//               std::cout << "" << std::endl;
//            }
//            
//            
//            std::cout << "" << std::endl;
            
            first++;    
         }
         
      }
      
   }
} // namespace

#endif
