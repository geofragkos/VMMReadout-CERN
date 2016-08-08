//
//  SrsChannel.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannel_h
#define online_display_SrsChannel_h

#include "SrsChannelId.h"
#include "SrsChannelData.h"
//#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#endif


#include <ostream>

namespace online {
   namespace display {
      
      class CSrsChip;
   //   class CRootTreeFiller;
      class CDetConnector;
      
      /**
       base class representing an electronics channel, has id, and channel data
       */
      class CSrsChannel
      {
         
      public:
         explicit CSrsChannel(const CSrsChannelId& channel_id);
         explicit CSrsChannel(const CSrsChannelId& channel_id, const CSrsChannelData& data);
         virtual ~CSrsChannel();
         friend std::ostream& operator<< (std::ostream & out, const CSrsChannel & chan);

         virtual void process_channel(const CSrsChip* ) {};
         //Apv specific
         virtual void calculate_qt() {};
         
     //    virtual void prefill(CRootTreeFiller* filler, boost::shared_ptr<CDetConnector> conn) = 0;
         
         const CSrsChannelId& channel_id() const { return  m_channel_id;};
         const CSrsChannelData& channel_data() const { return m_channel_data;}
         bool is_suppressed() const { return m_suppressed;}
         
         
           double calculate_channel_pedestal();
         
         class ChannelIdEquals;
         
      
      protected:
         

         
         
         CSrsChannelId   m_channel_id;
         CSrsChannelData m_channel_data;
         bool m_suppressed;
      };
      
      /**
       test for equality of SrsChannelId (aka uint32_t) values within CSrsChannel
       */
      class CSrsChannel::ChannelIdEquals : public std::unary_function<boost::shared_ptr<CSrsChannel>, bool>
      {
         CSrsChannelId m_chan_id;
      public:
         explicit ChannelIdEquals(const CSrsChannelId& chan_id) : m_chan_id(chan_id) {}
         bool operator() (const boost::shared_ptr<CSrsChannel> rhs) const {
            return m_chan_id == rhs->m_channel_id;
         }
      };

      
      
   }
} // namespace

#endif
