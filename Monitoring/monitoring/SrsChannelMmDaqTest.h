//
//  SrsChannelMmDaqTest.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13/11/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef mmdaq3_server_SrsChannelMmDaqTest_h
#define mmdaq3_server_SrsChannelMmDaqTest_h

#include "SrsChannel.h"

#include <boost/weak_ptr.hpp>


namespace mmdaq3 {
   namespace server {
      
      class CRootTreeFiller;
      class CDetConnector;
      class CSrsPedestal;
      
      
      class CSrsChannelMmDaqTest : public CSrsChannel
      {
         
      public:
         CSrsChannelMmDaqTest(const CSrsChannelId& chan_id, const uint32_t& word);
         virtual ~CSrsChannelMmDaqTest();
         
         virtual void prefill(CRootTreeFiller* filler, boost::shared_ptr<CDetConnector> conn);
         
      protected:
         
         virtual void calculate_channel_pedestal(CSrsPedestal& ped) const ;
         
         
         
      };
      
   }
} // namespace

#endif
