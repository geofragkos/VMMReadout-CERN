//
//  PropertyTreeParser.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 30.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_PropertyTreeParser_h
#define online_display_PropertyTreeParser_h

#include "DaqServerConfig.h"

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#endif

namespace online {
   namespace display {
      
      
      class CDaqServerConfig;
      

      
      /**
       base class for parsers of boost property tree
       */
      class CPropertyTreeParser
      {
         
      public:
         CPropertyTreeParser(const boost::property_tree::ptree& ptree, CDaqServerConfig& daqconfig);
         virtual ~CPropertyTreeParser();
        
      protected:
         long id_number_from_string(const std::string& idstr) const;

         
         const boost::property_tree::ptree& ptree_;
         CDaqServerConfig& daqconfig_; 

      };
   }
} // namespace

#endif
