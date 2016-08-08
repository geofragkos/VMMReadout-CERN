//
//  SharedChannel.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 11/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SharedChannel_h
#define online_display_SharedChannel_h

#include "SharedTypes.h"
#include "SrsTypes.h"
#include "SrsChannelId.h"
#include "SharedSrsChannelData.h"


namespace online {
   namespace display {
      
      class CSharedChannel
      {
         
      public:
         CSharedChannel(SrsChannelPtr local_channel, const ShmemVoidAllocator& alloc_inst);
         virtual ~CSharedChannel();
         
         
      protected:
         CSrsChannelId           m_id;
         CSharedSrsChannelData   m_data;
         
      };
  
      typedef bipc::allocator<CSharedChannel, ShmemSegmentManagerType>  ShmemChannelAllocator;
      typedef bipc::vector<CSharedChannel, ShmemChannelAllocator> ShmemChannelVector;

      
      
   }
   
} // namespace

#endif
