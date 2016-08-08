//
//  DetConnector.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetConnector_h
#define online_display_DetConnector_h

#include "DetBase.h"
#include "SrsChannelId.h"
#include "DetStripId.h"
#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#endif

#include <string>
#include <map>
#include <vector>
#include <cstddef>

namespace online {
   namespace display {
      
      class CSrsChip;
      class CSrsChipId;
      class CDetStripId;
      
      class CDetConnector :public CDetBase
      {
         
      public:
         explicit CDetConnector(DetBaseWeakPtr detector,
                                DetBaseWeakPtr parent,
                                size_t idnumber,
                                const std::string& name);
         
         virtual ~CDetConnector();
         virtual void clear();
         virtual void configure(CDaqServerConfig* daqconfig);
         virtual void print() const;
         
         //there are 2 map files : strip-pin, pin-channel
         void read_strip_map_file(const std::string& filename);
         void read_chip_connection_map_file(const std::string& filename,  const CDaqServerConfig* daqconfig);
         
         
         void add_chip(boost::shared_ptr<CSrsChip> chip);
         void connect_chip(size_t seq_no, boost::shared_ptr<CSrsChip> chip);

         CDetStripId get_mapped_strip_id(const CSrsChannelId& channel_id) const;
         
         void set_chip_connection_from(DetConnPtr from,  DetConnPtr toconnector);
         void set_pin_strip_mapping_from(DetConnPtr other);
         
      protected:
                           
         void create_channel_strip_map();

         typedef std::pair<size_t, CDetStripId> PinStripPair;
         std::vector<PinStripPair> m_pin_strip;
         
         typedef std::pair<size_t, CSrsChannelId > PinChannelPair;
         std::vector<PinChannelPair> m_pin_channel;
         
         typedef std::pair<CSrsChannelId, CDetStripId> ChannelStripPair;
         std::vector<ChannelStripPair> m_channel_strip;
         
         typedef std::pair<size_t, boost::weak_ptr<CSrsChip>/*CSrsChipId*/> ConnectedChipSeqPair;
         std::vector<ConnectedChipSeqPair> m_connected_chips;
         
      };
      
   }
} // namespace

#endif
