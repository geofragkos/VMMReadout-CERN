//
//  SharedEBBuffer.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 08/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//


#ifndef online_display_SharedEBBuffer_h
#define online_display_SharedEBBuffer_h

#include "SharedTypes.h"
#include "SharedChip.h"
//#include "MMEvent.h"

//#include <boost/interprocess/offset_ptr.hpp>

#include <iterator>

namespace online {
   namespace display {

      class CDaqServerConfig;

      class CSharedEBBuffer
      {

      public:
         CSharedEBBuffer(ShmemVoidAllocator alloc_inst);
         virtual ~CSharedEBBuffer();

     //         void configure(CDaqServerConfig* daq_config);

         //void insert_mmevent(MMEventPtr mmevent, const ShmemVoidAllocator& alloc_inst);

         //void add_chip(SrsChipPtr chip, const ShmemVoidAllocator& alloc_inst);

         void clear() {
            //clear members
            m_chips.clear();
         }

         void print() const;

         size_t max_event_depth() const;
         //Mutex to protect access to the queue
         boost::interprocess::interprocess_mutex m_mutex;


//      protected:
         ShmemChipVector m_chips;
         size_t m_max_event_depth;
      };

      typedef bipc::allocator<CSharedEBBuffer, ShmemSegmentManagerType> ShmemEBBufferAllocator;

   }
} // namespace

#endif
