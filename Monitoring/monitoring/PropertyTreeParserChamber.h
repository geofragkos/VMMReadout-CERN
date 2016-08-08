//
//  PropertyTreeParserChamber.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_PropertyTreeParserChamber_h
#define online_display_PropertyTreeParserChamber_h

#include "PropertyTreeParser.h"
#include "DetTypes.h"
#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/shared_ptr.hpp>
#endif
namespace online {
   namespace display {
      
      class CPropertyTreeParserChamber : public CPropertyTreeParser
      {
         
      public:
         CPropertyTreeParserChamber(const boost::property_tree::ptree& m_det_cfg, CDaqServerConfig& daqconfig);
         virtual ~CPropertyTreeParserChamber();
         
         void configure(DetChamPtr chamber);
      protected:
         
         void parse_ptree_chamber_node( const boost::property_tree::ptree& mainnode, DetChamPtr chamber);
         DetBasePtr parse_ptree_chamber_multilayer_node(const boost::property_tree::ptree::value_type& node, DetBasePtr chamber);
         DetBasePtr parse_ptree_chamber_layer_node(const boost::property_tree::ptree::value_type& node, DetBasePtr chamber);
         DetBasePtr parse_ptree_chamber_readout_node(const boost::property_tree::ptree::value_type& node, DetBasePtr chamber);
         DetConnPtr parse_ptree_chamber_connector_node(const boost::property_tree::ptree::value_type& node, DetChamPtr chamber);

         
//         std::vector<DetBasePtr> m_multilayers;
//         std::vector<DetConnPtr> m_connectors;
         
      };
   }
} // namespace

#endif
