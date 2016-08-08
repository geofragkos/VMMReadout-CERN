//
//  PropertyTreeParserDetector.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 30.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_PropertyTreeParserDetector_h
#define online_display_PropertyTreeParserDetector_h

#include "PropertyTreeParser.h"
#include "DetTypes.h"
#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/shared_ptr.hpp>
#endif

#include <utility>

namespace online {
   namespace display {
      
      class CDetDetector;
      class CDetBase;
      class CDetConnector;
      class CSrsChip;
      
      class CPropertyTreeParserDetector : public CPropertyTreeParser
      {
         
      public:
         CPropertyTreeParserDetector(const boost::property_tree::ptree& m_det_cfg, CDaqServerConfig& daqconfig);
         virtual ~CPropertyTreeParserDetector();
         
         DetBasePtr build_detector();
         
      protected:
         DetBasePtr parse_ptree_det_detector_node(const boost::property_tree::ptree& mainnode);
         DetBasePtr parse_ptree_det_chamber_node(const boost::property_tree::ptree::value_type& node, boost::shared_ptr<CDetBase> detector);
         DetConnPtr parse_ptree_det_connector_node(const boost::property_tree::ptree::value_type& node,  DetChamPtr chamber);
         std::pair<int, boost::shared_ptr<CSrsChip> >  parse_ptree_det_chip_node(const boost::property_tree::ptree::value_type& node);
         
      };
      
      
      
      
      
      
      
   }
} // namespace

#endif
