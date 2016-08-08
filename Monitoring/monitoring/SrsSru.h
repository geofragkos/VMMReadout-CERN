//
//  SrsSru.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsSru_h
#define online_display_SrsSru_h

#include "SrsNetworkDevice.h"

#ifndef Q_MOC_RUN
#include <boost/asio/ip/address.hpp>
#endif

namespace online {
   namespace display {
      
      class CUDPFrame;
      
      /*
       SRU unit (different event processing than FEC card), maybe it should be user as base class 
       */
      class CSrsSru : public CSrsNetworkDevice
      {
         
      public:
         CSrsSru(size_t idnum, const std::string& name, const std::string& ip_address);
         virtual ~CSrsSru();
         virtual void reset();
         virtual void insert_receiver_data(boost::shared_ptr<CUDPFrame> udpframe);
         virtual void configure(const CDaqServerConfig* ) {};

      protected:
         
      };
      
   }
} // namespace

#endif
