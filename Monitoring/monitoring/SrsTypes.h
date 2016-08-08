//
//  SrsTypes.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsTypes_h
#define online_display_SrsTypes_h

#include "EventTypes.h"

#ifndef Q_MOC_RUN
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#endif

#include <stdint.h>
#include <vector>
#include <map>
#include <utility>
#include <list>

namespace online {
   namespace display {
      
      class CSrsBase;
      class CSrsFec;
      class CSrsNetworkDevice;
      class CSrsChip;
      class CUDPFrame;
      class CSrsChannel;
      class CSrsEventId;
      class CSrsEventChannels;
      
      
      typedef boost::shared_ptr<CSrsBase> SrsBasePtr;
      typedef boost::shared_ptr<CSrsNetworkDevice> SrsNetDevPtr;
      typedef boost::shared_ptr<CSrsFec> SrsFecPtr;
      typedef boost::shared_ptr<CSrsChip> SrsChipPtr;
      typedef boost::shared_ptr<CSrsChannel> SrsChannelPtr;
      typedef boost::shared_ptr<CSrsEventChannels> SrsEventChannelsPtr;

      typedef boost::shared_ptr<CUDPFrame> UDPFramePtr;
//      typedef std::vector<SrsChannelPtr> SrsChannelCont;
      typedef std::list<SrsChannelPtr> SrsChannelList;
      typedef std::pair<CSrsEventId, SrsChannelList> SrsEventChannels;
      typedef std::map<EBEventIdType, SrsEventChannels> SrsEventMap;
      
      typedef std::map<EBEventIdType, SrsEventChannelsPtr>   SrsEventChannelsMap;
      
      
       //weak pointers
      typedef boost::weak_ptr<CSrsBase> SrsBaseWeakPtr;
      typedef boost::weak_ptr<CSrsNetworkDevice> SrsNetDevWeakPtr;
      typedef boost::weak_ptr<CSrsFec> SrsFecWeakPtr;
      typedef boost::weak_ptr<CSrsChip> SrsChipWeakPtr;
      typedef boost::weak_ptr<CSrsChannel> SrsChannelWeakPtr;
      
     
      

      
      template <class P1>
      class FirstComparator : public std::unary_function< P1, bool> {
         typename P1::first_type m_first;
         
      public:
         explicit FirstComparator(const typename P1::first_type& p) : m_first(p) {}
         bool operator() (const P1& p) const {
            return p.first == m_first;
         }
      };
      
      template <class P1>
      class FirstLess : public std::binary_function< P1, P1, bool> {   
      public:
         explicit FirstLess()  {}
         bool operator() (const P1& p, const P1& q) const {
            return p.first < q.first;
         }
      };
      
      
      
      template <class P1>
      class SecondComparator : public std::unary_function< P1, bool> {
         typename P1::second_type m_sec;
         
      public:
         explicit SecondComparator(const typename P1::second_type& p) : m_sec(p) {}
         bool operator() (const P1& p) const {
            return p.second == m_sec;
         }
      };
      
      
   }
}




#endif
