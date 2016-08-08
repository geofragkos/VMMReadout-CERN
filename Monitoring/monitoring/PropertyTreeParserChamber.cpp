//
//  PropertyTreeParserChamber.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "PropertyTreeParserChamber.h"
#include "DetChamber.h"
#include "DetMultiLayer.h"
#include "DetLayer.h"
#include "DetReadout.h"
#include "DetConnector.h"


#include <iostream>

using namespace online::display;

namespace bpt = boost::property_tree;

CPropertyTreeParserChamber::CPropertyTreeParserChamber(const bpt::ptree& chamber_cfg, CDaqServerConfig& daqconfig) :
CPropertyTreeParser(chamber_cfg, daqconfig)
{
   
}


CPropertyTreeParserChamber::~CPropertyTreeParserChamber()
{
   
}

void CPropertyTreeParserChamber::configure(DetChamPtr chamber)
{
   //TODO: apply read config to this
   
//   std::vector<DetBasePtr> local_multilayers;
//   std::vector<DetConnPtr> local_connectors;
   
   try {
      parse_ptree_chamber_node(ptree_.get_child("chamber"), chamber);
      
//      Q_FOREACH(DetConnPtr conn, m_connectors) {
//         chamber->add_connector(conn);
//      }
//      Q_FOREACH(DetBasePtr ml, m_multilayers) {
//         chamber->add_child(ml);
//      }
      
   } catch (std::runtime_error &re) {
      std::cout << "ERROR: in chamber configursation: CPropertyTreeParserChamber::configure failed:\n";
      std::cout << re.what() << std::endl;
      throw;
   }
   
}

void CPropertyTreeParserChamber::parse_ptree_chamber_node(const boost::property_tree::ptree& mainnode, DetChamPtr chamber)
{
   std::string idstr;
   std::string name;
   Coord3 pos;
   Coord3 rot;
   Coord3 size;
      
   Q_FOREACH(const bpt::ptree::value_type &v, mainnode) {
      if (v.first == "size") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         size = Coord3(x,y,z);
      }
//      else if (v.first == "position") {
//         double x = v.second.get<double>("x", 0.0);
//         double y = v.second.get<double>("y", 0.0);
//         double z = v.second.get<double>("z", 0.0);
//         pos = Coord3(x,y,z);
//      }
//      else if (v.first == "rotation") {
//         double x = v.second.get<double>("x", 0.0);
//         double y = v.second.get<double>("y", 0.0);
//         double z = v.second.get<double>("z", 0.0);
//         rot = Coord3(x,y,z);
//      }
//      else if (v.first == "drift_gap") {
//         //         name = v.second.data();
//      }
      else if (v.first == "name") {
         name = v.second.data();
      }
      else if (v.first == "id") {
         idstr = v.second.data();
      }

      else if (v.first == "multilayer") {
         try {
            DetBasePtr ml = parse_ptree_chamber_multilayer_node(v, chamber);
            chamber->add_child(ml);
//            m_multilayers.push_back(ml);
         } catch (std::exception& e) {
            std::cout << "ERROR creating multilayer in " << name << " :\n";
            std::cout << e.what() << std::endl;
         }
      }
      else if (v.first == "connector") {
         try {
            DetConnPtr conn = parse_ptree_chamber_connector_node(v, chamber);
            
         } catch (std::exception& e) {
            std::cout << "ERROR creating chamber in " << name << " :\n";
            std::cout << e.what() << std::endl;
         }
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         //         std::cout << "<xmlattr>=" << v.second.data() << std::endl;
         std::string idstr2 = v.second.get("id", "");
         std::string namestr2 = v.second.get("name", "");
         if (!idstr2.empty()) {  
            idstr = idstr2;
         }
         if (!namestr2.empty()) {
            name = namestr2;
         }
      }
      else {
         std::cout << "WARN: bad data in chamber config: unknown key '" << v.first << "'" << std::endl;
      }
   }
   
   
    
}

DetBasePtr CPropertyTreeParserChamber::parse_ptree_chamber_multilayer_node(const bpt::ptree::value_type& node, DetBasePtr parent)
{
   std::string idstr;
   std::string name;
   Coord3 pos;
   Coord3 rot;
   Coord3 size;

   DetBasePtr multilayer(new CDetMultiLayer(parent->detector(), parent));
   
   Q_FOREACH(const bpt::ptree::value_type &v, node.second) {
      if (v.first == "position") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         pos = Coord3(x,y,z);
      }
      else if (v.first == "size") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         size = Coord3(x,y,z);
      }
      else if (v.first == "rotation") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         rot = Coord3(x,y,z);
      }
      else if (v.first == "name") {
         name = v.second.data();
      }
      else if (v.first == "id") {
         idstr = v.second.data();
      }
      else if (v.first == "layer") {
         try {
            DetBasePtr layer = parse_ptree_chamber_layer_node(v, multilayer);
            multilayer->add_child(layer);
         } catch (std::exception& e) {
            std::cout << "ERROR creating layer in " << node.first << " " << name << ":\n";
            throw;
         }
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         //         std::cout << "<xmlattr>=" << v.second.data() << std::endl;
         std::string idstr2 = v.second.get<std::string>("id","");
         std::string namestr2 = v.second.get<std::string>("name","");
         if (!idstr2.empty()) {  
            idstr = idstr2;
         }
         if (!namestr2.empty()) {
            name = namestr2;
         }
         //         std::cout << "read name=" << name << std::endl;
      }
      else {
         std::cout << "WARN: bad data in chamber config: unknown key '" << v.first << "'"
         << "' in node "   <<  node.first << std::endl;
      }
   }
   size_t idnum = boost::lexical_cast<size_t>(idstr);
   multilayer->set(idnum, name, size, pos, rot);
   return multilayer;
}

DetBasePtr CPropertyTreeParserChamber::parse_ptree_chamber_layer_node(const bpt::ptree::value_type& node, DetBasePtr parent)
{
   std::string idstr;
   std::string name;
   Coord3 pos;
   Coord3 rot;
   Coord3 size;

   DetBasePtr layer(new CDetLayer(parent->detector(), parent));
   
   Q_FOREACH(const bpt::ptree::value_type &v, node.second) {
      if (v.first == "position") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         pos = Coord3(x,y,z);
      }
      else if (v.first == "size") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         size = Coord3(x,y,z);
      }
      else if (v.first == "rotation") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         rot = Coord3(x,y,z);
      }
      else if (v.first == "name") {
         name = v.second.data();
      }
      else if (v.first == "id") {
         idstr = v.second.data();
      }
      else if (v.first == "readout") {
         try {
            DetBasePtr rd = parse_ptree_chamber_readout_node(v, layer);
            layer->add_child(rd);
         } catch (std::exception& e) {
            std::cout << "ERROR creating layer in " << node.first << " " << name << ":\n";
            throw;
         }
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         //         std::cout << "<xmlattr>=" << v.second.data() << std::endl;
         std::string idstr2 = v.second.get<std::string>("id","");
         std::string namestr2 = v.second.get<std::string>("name","");
         if (!idstr2.empty()) {  
            idstr = idstr2;
         }
         if (!namestr2.empty()) {
            name = namestr2;
         }
         //         std::cout << "read name=" << name << std::endl;
      }
      else {
         std::cout << "WARN: bad data in chamber config: unknown key '" << v.first << "'"
         << "' in node "   <<  node.first << std::endl;
      }
   }
   size_t idnum = boost::lexical_cast<size_t>(idstr);
   layer->set(idnum, name, size, pos, rot);
   return layer;
}



DetBasePtr CPropertyTreeParserChamber::parse_ptree_chamber_readout_node(const bpt::ptree::value_type& node, DetBasePtr parent)
{
   std::string idstr;
   std::string name;
   Coord3 pos;
   Coord3 rot;
   Coord3 size;
   double pitch;
   std::pair<int,int> strip_range;


   Q_FOREACH(const bpt::ptree::value_type &v, node.second) {
      if (v.first == "position") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         pos = Coord3(x,y,z);
      }
      else if (v.first == "size") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         size = Coord3(x,y,z);
      }
      else if (v.first == "rotation") {
         double x = v.second.get<double>("x", 0.0);
         double y = v.second.get<double>("y", 0.0);
         double z = v.second.get<double>("z", 0.0);
         rot = Coord3(x,y,z);
      }
      else if (v.first == "name") {
         name = v.second.data();
      }
      else if (v.first == "id") {
         idstr = v.second.data();
      }
      else if (v.first == "strip_range") {
         double x1 = v.second.get<double>("min", 0.0);
         double x2 = v.second.get<double>("max", 0.0);
         if (x1 == 0 || x2 == 0) {
            throw std::runtime_error("Config error strip range out of bounds");
         }
         strip_range = std::pair<int, int>(x1, x2);
      }
      else if (v.first == "pitch") {
         pitch = v.second.get_value<double>(); //?
         std::cout << "is OK ? pitch is " << pitch << std::endl;
      }
      
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         //         std::cout << "<xmlattr>=" << v.second.data() << std::endl;
         std::string idstr2 = v.second.get<std::string>("id","");
         std::string namestr2 = v.second.get<std::string>("name","");
         if (!idstr2.empty()) {  
            idstr = idstr2;
         }
         if (!namestr2.empty()) {
            name = namestr2;
         }
         //         std::cout << "read name=" << name << std::endl;
      }
      else {
         std::cout << "WARN: bad data in chamber config: unknown key '" << v.first << "'"
         << "' in node "   <<  node.first << std::endl;
      }
   }
   size_t idnum = boost::lexical_cast<size_t>(idstr);

    DetReadPtr readout(new CDetReadout(parent->detector(), parent, idnum, name, size, pos, rot)); //TODO: constr
    readout->set(idnum, name, size, pos, rot);
    readout->setStrips(strip_range, pitch);
   return readout;
}

DetConnPtr CPropertyTreeParserChamber::parse_ptree_chamber_connector_node(const bpt::ptree::value_type& node, DetChamPtr chamber)
{   
   std::string name;
   std::string idstr;
   std::string map_file;
   
   Q_FOREACH(const bpt::ptree::value_type &v, node.second) {
      if (v.first == "map_file") {
         map_file =  v.second.data();
      }
      else if (v.first == "name") {
         name =  v.second.data();
      }
      else if (v.first == "id") {
          idstr =  v.second.data();
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         std::string idstr2 = v.second.get("id", "");
         std::string namestr2 = v.second.get("name", "");
         if (!idstr2.empty()) {  
            idstr = idstr2;
         }
         if (!namestr2.empty()) {
            name = namestr2;
         }
      }
      else {
         std::cout << "WARN: bad data in detector config: unknown key '" << v.first << "'"
         << "' in node "   <<  node.first << std::endl;
      }
   }

   //set correct path to the map file
   boost::filesystem::path map_path(map_file);
   //make absolute paths for included config files
   if (!map_path.has_parent_path()) {
      map_path = daqconfig_.get_config_path().parent_path() / map_path;
   }
   
   boost::shared_ptr<CDetConnector> conn(new CDetConnector(chamber->detector(), chamber, 0,name)); 
   conn->read_strip_map_file(map_path.string());
   chamber->add_connector(conn);
   return conn;
}


