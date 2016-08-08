//
//  PropertyTreeParserSrs.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 30.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "PropertyTreeParserSrs.h"
#include "SrsSru.h"
#include "SrsFec.h"
#include "SrsChipId.h"
#include "SrsChipApv.h"
#include "SrsChipBnl.h"
#include "SrsChipMmDaqTest.h"

#include "Logger.h"

#include <QObject>


//#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>

namespace bpt = boost::property_tree;

using namespace online::display;

CPropertyTreeParserSrs::CPropertyTreeParserSrs(const boost::property_tree::ptree& ptree, CDaqServerConfig& daqconfig) :
CPropertyTreeParser(ptree, daqconfig),
srs_elements_(),
chip_names_(),
apv25_zero_suppression_threshold_(),
apv25_pedestal_file_(),
bnl_pedestal_file_()
{
   
}


CPropertyTreeParserSrs::~CPropertyTreeParserSrs()
{
   
}

const std::vector<SrsBasePtr>& CPropertyTreeParserSrs::make_srs_elements() 
{
   
   Q_FOREACH(const bpt::ptree::value_type &v, ptree_.get_child("srs")) {
      /* if (v.first == "sru") {
         try {
            SrsBasePtr sru = parse_ptree_srs_node<CSrsSru>(v);
            push_new_srs_element(sru);
         } catch (std::exception& e) {
            std::cout << "---> ERROR creating SRU" << std::endl;
            throw;
         }
      }
      else */
      if (v.first == "fec") {
         try {
            SrsBasePtr fec = parse_ptree_srs_node<CSrsFec>(v);
            push_new_srs_element(fec);
         } catch (std::exception& e) {
            std::cout << "---> ERROR creating FEC" << std::endl;
            throw;
         }
      }
      else if (v.first == "APV25") {
         try {
            parse_ptree_srs_node_apv25(v);
         } catch (std::exception& e) {
            std::cout << "ERROR: srs config:  parsing APV25 node" << std::endl;
            //throw;
         }
      }
      else if (v.first == "BNL") {
         try {
            parse_ptree_srs_node_bnl(v);
         } catch (std::exception& e) {
            std::cout << "ERROR: srs config:  parsing BNL node" << std::endl;
            //throw;
         }
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in file: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         //         std::cout << "<xmlattr>=" << v.second.data() << std::endl;
      }
      else {
         std::cout << "WARN: bad data in config: unknown key '" << v.first << "' in <srs> section" << std::endl;
      }
   }
   
   std::cout << "......................................................................." << " check srs" << std::endl;

   //test consistency
//   Q_FOREACH(std::string& name, chip_names_) {
//      locate_srs<CSrsChip>(-1, name);
//   }
   
   
   
   return srs_elements_;
}


void CPropertyTreeParserSrs::push_new_srs_element(SrsBasePtr elem)
{
   if (std::binary_search(srs_elements_.begin(), srs_elements_.end(), elem, CSrsBase::IdNumberCompare())) {
      CLogger::instance()->error("Duplicate SRS id number");
      throw std::runtime_error("Duplicate SRS id number");
   }
   srs_elements_.push_back(elem);
   std::sort(srs_elements_.begin(), srs_elements_.end(), CSrsBase::IdNumberCompare());  
}



boost::shared_ptr<CSrsChip> CPropertyTreeParserSrs::parse_ptree_srs_chip_node(const boost::property_tree::ptree::value_type& node)
{
   //make a new chip
   std::string idstr;
   std::string typestr;
   std::string namestr;
   
   Q_FOREACH(const boost::property_tree::ptree::value_type &v, node.second) {      
      if (v.first == "name") {
         namestr = v.second.data();
      }
      else if (v.first == "type") {
         typestr = v.second.data();
      }
      else if (v.first == "<xmlcomment>") {
         //         std::cout << "comment in SRU section: " << v.second.data() << std::endl;
      }
      else if (v.first == "<xmlattr>") {
         idstr = v.second.get<std::string>("id");
         typestr = v.second.get<std::string>("type","");
         //         std::cout << "read id=" << idstr << std::endl;
      }
      else {
         std::cout << "WARN: bad data in config: unknown key '" << v.first 
         << "' in section " << node.first << std::endl;
      }
   }
   
   chip_names_.push_back(namestr);
   
   boost::shared_ptr<CSrsChip> chipptr;
   
   if (typestr == "chip") {
      chipptr.reset(new CSrsChipApv(CSrsChipId(0, id_number_from_string(idstr)), namestr) );   
   } else if (typestr == "VMM1") {
      chipptr.reset(new CSrsChipBnl(CSrsChipId(0, id_number_from_string(idstr)), namestr) );   
      //TODO: rename class VMM1 ?
   }
   else if (typestr == "TESTER") {
        chipptr.reset(new CSrsChipMmDaqTest(CSrsChipId(0, id_number_from_string(idstr)), namestr) );
     }
   else {
      throw std::runtime_error("Unknown SRS chip type");
   }
   
   return chipptr;
   
}


void CPropertyTreeParserSrs::parse_ptree_srs_node_apv25(const boost::property_tree::ptree::value_type& node)
{
   std::string str_zsth;
   std::string pedestal_file;
   Q_FOREACH(const boost::property_tree::ptree::value_type &v, node.second) {
      if (v.first == "zero_suppression_threshold") {
         str_zsth = v.second.data();
      }
      else if (v.first == "pedestal_file") {
         pedestal_file = v.second.data();
      }
      else {
         std::cout << "WARN: bad data in config: unknown key '" << v.first
         << "' in section " << node.first << std::endl;
      }
   }
   apv25_zero_suppression_threshold_ = boost::lexical_cast<double>(str_zsth);
   apv25_pedestal_file_ = pedestal_file;
   
   boost::filesystem::path ped_path(apv25_pedestal_file_);
   
   //make absolute paths for included config files
   if (!ped_path.has_parent_path() && !ped_path.empty()) {
      ped_path = daqconfig_.get_config_path().parent_path() / ped_path;
   }
   apv25_pedestal_file_ = ped_path.string();
   std::cout << "CPropertyTreeParserSrs::parse_ptree_srs_node_apv2() APV25 pedestal file is " << apv25_pedestal_file_ << std::endl;

}



void CPropertyTreeParserSrs::parse_ptree_srs_node_bnl(const boost::property_tree::ptree::value_type& node)
{
   std::string str_zsth;
   std::string pedestal_file;
   Q_FOREACH(const boost::property_tree::ptree::value_type &v, node.second) {      
      if (v.first == "zero_suppression_threshold") {
         str_zsth = v.second.data();
      }
      else if (v.first == "pedestal_file") {
         pedestal_file = v.second.data();
      }
      else {
         std::cout << "WARN: bad data in config: unknown key '" << v.first 
         << "' in section " << node.first << std::endl;
      }
   }
//   apv25_zero_suppression_threshold_ = boost::lexical_cast<double>(str_zsth);
   bnl_pedestal_file_ = pedestal_file;
   
   boost::filesystem::path ped_path(bnl_pedestal_file_);
   
   //make absolute paths for included config files
   if (!ped_path.has_parent_path() && !ped_path.empty()) {
      ped_path = daqconfig_.get_config_path().parent_path() / ped_path;
   }
   bnl_pedestal_file_ = ped_path.string();
   std::cout << "CPropertyTreeParserSrs::parse_ptree_srs_node_apv2() BNL pedestal file is " << bnl_pedestal_file_ << std::endl;
}




void CPropertyTreeParserSrs::configure_chips(std::vector<SrsChipPtr> chips) 
{
   bool apv25_done = false;
   bool bnl_done = false;
   Q_FOREACH(SrsChipPtr chip, chips) {
     /* if (boost::shared_ptr<CSrsChipApv> apv = boost::dynamic_pointer_cast<CSrsChipApv>(chip)) {
         if (!apv25_done) {
            apv->set_zero_suppression_threshold(apv25_zero_suppression_threshold_);
            apv->set_pedestal_filename(apv25_pedestal_file_);
            apv25_done = true;
         }
         apv->load_pedestal_file();//apv25_pedestal_file_ 
      }
      else */if (boost::shared_ptr<CSrsChipBnl> bnl = boost::dynamic_pointer_cast<CSrsChipBnl>(chip)) {
         if (!bnl_done) {
//            bnl->set_zero_suppression_threshold(apv25_zero_suppression_threshold_);
            bnl->set_pedestal_filename(bnl_pedestal_file_);
            bnl_done = true;
         }
         bnl->load_pedestal_file();//apv25_pedestal_file_ 
      }
      
   }
}

