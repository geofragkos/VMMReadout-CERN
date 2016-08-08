//
//  DetChamber.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetChamber.h"

#include "PropertyTreeParserChamber.h"
#include "DetConnector.h"


#define BOOST_FILESYSTEM_NO_DEPRECATED
//#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>
//#include <cstdlib> //std::system
//#include <cstddef>
//#include <cassert>
#include <utility>

using namespace online::display;


namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;


CDetChamber::CDetChamber(DetBaseWeakPtr detector,
                         DetBaseWeakPtr parent,
                         size_t idnumber,
                         const std::string& name,
                         const Coord3& size,
                         const Coord3& position,
                         const Coord3& rotation):
CDetBase(detector, parent, idnumber, name, size, position, rotation),
m_chamber_cfg(),
m_connectors()
{
   
}


CDetChamber::~CDetChamber()
{
   
}

void CDetChamber::clear()
{
   m_chamber_cfg.clear();
   m_connectors.clear();
   CDetBase::clear();
}


void CDetChamber::print() const
{
   std::cout << "(*) Chamber (todo: print connected chips) ";
   Q_FOREACH(DetConnPtr c, m_connectors) {
      c->print();
      std::cout << " ";
   }
   
   CDetBase::print();
   //print connectors ..
// print connected chips   std::for_each(m_con, <#_InputIterator __last#>, <#_Function __f#>)
   
}

std::string CDetChamber::getMyChamberName()
{
    return m_name;
}

void CDetChamber::read_config_file(const std::string& filename, CDaqServerConfig &daqconfig)
{
   read_xml(filename, m_chamber_cfg, boost::property_tree::xml_parser::trim_whitespace );
   CPropertyTreeParserChamber parser(m_chamber_cfg, daqconfig);
   parser.configure( boost::dynamic_pointer_cast<CDetChamber>(shared_from_this()));
}



void CDetChamber::add_connector(const boost::shared_ptr<CDetConnector> conn)
{
 std::cout << "implement CDetChamber::add_connector " << conn->name() << " to " << name() << " (todo: test uniqueness)" << std::endl;
   m_connectors.push_back(conn);
   
}


void CDetChamber::configure_connector_connection(const boost::shared_ptr<CDetConnector> conn)
{
   //check if conn exist in this chammber
   //will throw if chamber does not have connector

   //get connector on chamber
   std::vector<DetConnPtr> ::iterator found = std::find_if(m_connectors.begin(), m_connectors.end(),
                                                           CDetBase::NameEquals<CDetConnector>(conn->name()));


   if (found != m_connectors.end()) {
      (*found)->set_chip_connection_from(conn, (*found));
      //chip?
   }
  
   
   //move info: chip id, chip mapping 
   
   
}


DetConnPtr CDetChamber::get_connector( const std::string& name)
{
   std::vector<DetConnPtr> ::iterator found = std::find_if(m_connectors.begin(), m_connectors.end(),
                                                           CDetBase::NameEquals<CDetConnector>(name));
   if (found == m_connectors.end()) {
      return DetConnPtr();
   }
   return *found;

}


