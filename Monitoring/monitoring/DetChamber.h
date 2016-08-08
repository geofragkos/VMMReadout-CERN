//
//  DetChamber.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetChamber_h
#define online_display_DetChamber_h

#include "DetBase.h"

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/enable_shared_from_this.hpp>
#endif

namespace online {
   namespace display {
      
      class CDetConnector;
      class CDaqServerConfig;
      
      
      class CDetChamber : public CDetBase
      {
         
      public:
         explicit CDetChamber(DetBaseWeakPtr detector,
                     DetBaseWeakPtr parent,
                     size_t idnumber,
                     const std::string& name,
                     const Coord3& size,
                     const Coord3& position,
                     const Coord3& rotation);
         virtual ~CDetChamber();
         virtual void clear();
         virtual void configure(CDaqServerConfig* ) {};
         virtual void print() const;
         std::string getMyChamberName();


         void read_config_file(const std::string& filename,  CDaqServerConfig &daqconfig);

         void add_connector(const boost::shared_ptr<CDetConnector> conn);
         void configure_connector_connection(const boost::shared_ptr<CDetConnector> conn);
         DetConnPtr get_connector( const std::string& name);
      protected:
         boost::property_tree::ptree m_chamber_cfg;
        
         std::vector<DetConnPtr> m_connectors;

      };
      
   }
} // namespace

#endif
