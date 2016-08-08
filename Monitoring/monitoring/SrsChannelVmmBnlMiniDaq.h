//
//  SrsChannelVmmBnlMiniDaq.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 12/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannelVmmBnlMiniDaq_h
#define online_display_SrsChannelVmmBnlMiniDaq_h
#include "SrsChannel.h"

#ifndef Q_MOC_RUN

#include <boost/weak_ptr.hpp>
#endif

namespace online {
   namespace display {
      
      class CRootTreeFiller;
      class CDetConnector;
      class CSrsChannelVmmBnlMiniDaq : public CSrsChannel
      {
         
      public:
         CSrsChannelVmmBnlMiniDaq(const CSrsChipId& chip_id, const uint32_t& word);
         virtual ~CSrsChannelVmmBnlMiniDaq();
         
         //virtual void prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn);
      };
   }
} // namespace

#endif
