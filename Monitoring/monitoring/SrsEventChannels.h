//
//  SrsEventChannels.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 20/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsEventChannels_h
#define online_display_SrsEventChannels_h

#include "EventTypes.h"
#include "SrsTypes.h"

#include "SrsEventId.h"

#ifndef Q_MOC_RUN

#include <boost/thread/mutex.hpp>
#include <boost/weak_ptr.hpp>
#endif

namespace online {
   namespace display {
      
      //class CRootWriter;
      //class CRootTreeFiller;
      class CDetConnector;
      
      
      class CSrsEventChannels
      {
         
      public:
         CSrsEventChannels();
         CSrsEventChannels(const CSrsEventId& eventid,
                           SrsChannelList& channels);
         
         
         virtual ~CSrsEventChannels();
         friend std::ostream& operator<< (std::ostream & out, const CSrsEventChannels & chan);
         
         CSrsEventChannels(const CSrsEventChannels& rhs) :
         m_local_mutex(),
         m_eventid(rhs.m_eventid),
         m_channels(rhs.m_channels)
         { }
         
         CSrsEventChannels& operator=(const CSrsEventChannels& rhs) {
            if (&rhs != this) {
               m_eventid = rhs.m_eventid;
               m_channels = rhs.m_channels;
            }
            return *this;
         }
         
         
         
         void move_event_channels(SrsChannelList& channels);
         size_t size() const { return m_channels.size(); }
         SrsChannelList& channels() { return m_channels;}
         
         void assign(const SrsChannelList& chans) {m_channels.assign(chans.begin(), chans.end()); }
         void process(SrsChipPtr chip);
         //void prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn);
         
         
         //      void move_to_eb(boost::shared_ptr<CEventBuilderInputBuffer> eb_buffer);
         
         
      protected:
         boost::mutex   m_local_mutex;
         CSrsEventId    m_eventid;
         SrsChannelList m_channels;
         
      };
      
      std::ostream& operator<< (std::ostream & out, const CSrsEventChannels & chan);
   }
} // namespace

#endif
