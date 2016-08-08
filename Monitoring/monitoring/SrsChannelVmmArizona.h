//
//  SrsChannelVmmArizona.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 31.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannelVmmArizona_h
#define online_display_SrsChannelVmmArizona_h

#include "SrsChannel.h"
#include "SrsChannelId.h"
#include "SrsChannelData.h"

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#endif

namespace online {
   namespace display {
      
      class CDetConnector;
      
      class CSrsChannelVmmArizona : public CSrsChannel
      {
         
      public:
         explicit CSrsChannelVmmArizona(const CSrsChannelId& channel_id, 
                                        const CSrsChannelData& data);

         virtual ~CSrsChannelVmmArizona();
         //virtual void prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn);

         
      };
   }
} // namespace

#endif
