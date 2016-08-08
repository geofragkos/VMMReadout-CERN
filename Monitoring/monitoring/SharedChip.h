//
//  SharedChip.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 11/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SharedChip_h
#define online_display_SharedChip_h

#include "SharedTypes.h"
#include "SrsTypes.h"
#include "SharedChannel.h"
#include "SrsChipId.h"

#ifndef Q_MOC_RUN

#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/containers/list.hpp>

#endif

namespace online {
   namespace display {

      /**
       chip class that can be placed in the shared memory
       */
      class CSharedChip
      {
      public:


         typedef std::pair<const EBEventIdType, ShmemChannelVector> EventMapValueType;
         typedef bipc::allocator<EventMapValueType, ShmemSegmentManagerType>  EventMapValueAllocator;
         typedef bipc::map<EBEventIdType, ShmemChannelVector, std::less<EBEventIdType>, EventMapValueAllocator> ShmemEventMap;

      public:
         CSharedChip( const ShmemVoidAllocator& alloc_inst);
         virtual ~CSharedChip();
         CSharedChip(const CSharedChip& rhs);
         CSharedChip& operator=(const CSharedChip& rhs);


         void print() const;

      protected:



         uint32_t          m_chip_id;
         SharedChipType    m_chip_type;
//         ShmemCharString   m_chip_name;
         ShmemDblVector    m_q;
         ShmemDblVector    m_t;
//         ShmemDblVectorVector m_user;
      };

      typedef bipc::allocator<CSharedChip, ShmemSegmentManagerType>  ShmemChipAllocator;
      typedef bipc::vector<CSharedChip, ShmemChipAllocator> ShmemChipVector;



   }
} // namespace

#endif
