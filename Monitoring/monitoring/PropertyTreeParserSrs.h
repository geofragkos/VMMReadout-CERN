//
//  PropertyTreeParserSrs.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 30.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_PropertyTreeParserSrs_h
#define online_display_PropertyTreeParserSrs_h

#include "PropertyTreeParser.h"
#include "SrsTypes.h"
#include "SrsFec.h"

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/shared_ptr.hpp>
//#include <boost/foreach.hpp>
#endif

#include <string>
#include <vector>

namespace online {
   namespace display {
      
      /**
       parses ptree (from xml file) that defines srs elements
       */
      class CPropertyTreeParserSrs : public CPropertyTreeParser
      {
         
      public:
         CPropertyTreeParserSrs(const boost::property_tree::ptree& ptree, CDaqServerConfig& daqconfig);
         virtual ~CPropertyTreeParserSrs();
         
         const std::vector<SrsBasePtr>& make_srs_elements() ;
         void configure_chips(std::vector<SrsChipPtr> chips);
         
      protected:
         
         void push_new_srs_element(SrsBasePtr elem);
         template<class T>
         boost::shared_ptr<T>  parse_ptree_srs_node (const boost::property_tree::ptree::value_type& node);
         boost::shared_ptr<CSrsChip> parse_ptree_srs_chip_node(const boost::property_tree::ptree::value_type& node);
         void parse_ptree_srs_node_apv25(const boost::property_tree::ptree::value_type& node);
         void parse_ptree_srs_node_bnl(const boost::property_tree::ptree::value_type& node);

         template <class T>
         boost::shared_ptr<T> locate_srs(uint32_t uid) const;
         template <class CSrsChip>
         boost::shared_ptr<CSrsChip> locate_srs(long uid, const std::string& name) const;
         
         std::vector<SrsBasePtr> srs_elements_;
         
         std::vector<std::string> chip_names_;
         
         double apv25_zero_suppression_threshold_;
         std::string apv25_pedestal_file_;
         std::string bnl_pedestal_file_;
      };
      
      
      /**
       parse boost::property_tree node with SRS element configuration and instantiate
       input elements: id, name and IP address
       */
      template<class T>
      boost::shared_ptr<T>  CPropertyTreeParserSrs::parse_ptree_srs_node (const boost::property_tree::ptree::value_type& node)
      {
         std::string idstr;
         std::string ipstr;
         std::string namestr;
         std::vector<SrsChipPtr> chips;
         
         Q_FOREACH(const boost::property_tree::ptree::value_type &v, node.second) {      
            if (v.first == "ip") {
               ipstr = v.second.data();
            }
            else if (v.first == "name") {
               namestr = v.second.data();
            }
            else if (v.first == "chip") {
               boost::shared_ptr<CSrsChip> chip = parse_ptree_srs_chip_node(v);
               chips.push_back(chip);
            }
            else if (v.first == "<xmlcomment>") {
               //         std::cout << "comment in SRU section: " << v.second.data() << std::endl;
            }
            else if (v.first == "<xmlattr>") {
               idstr = v.second.get<std::string>("id");
               //               std::cout << "read id=" << idstr << std::endl;
            }
            else {
               std::cout << "WARN: bad data in config: unknown key '" << v.first 
               << "' in section " << node.first << std::endl;
            }
         }
         
         //TODO: check duplicate chips: id, names
         
         
         boost::shared_ptr<T> elem;
         size_t idnum = id_number_from_string(idstr);
         //make standard constructor for all elem
         elem = boost::shared_ptr<T> (new T(idnum, namestr, ipstr));
         //add chips is this is fec
         SrsFecPtr fec = boost::dynamic_pointer_cast<CSrsFec>(elem);
         if (fec) {
            Q_FOREACH(boost::shared_ptr<CSrsChip> chip, chips) {
               fec->add_chip(chip);
            }
         }
         else {
            if (!chips.empty()) {
               std::stringstream ss;
               ss << "'chip' keyword not supported in section " << node.first;
               throw std::runtime_error(ss.str().c_str());
            }
         }
         return elem;
      }
      
      
      
      
      template <class T>
      boost::shared_ptr<T> CPropertyTreeParserSrs::locate_srs(uint32_t uid) const {
         typedef std::vector<SrsBasePtr> srs_vec_type;
         typedef std::vector<SrsBasePtr>::iterator srs_vec_iter;
         std::vector<SrsBasePtr> se(srs_elements_.size());
         
         srs_vec_iter last = boost::algorithm::copy_if(srs_elements_.begin(), srs_elements_.end(), se.begin(), 
                                     CSrsBase::IdNumberEquals<CSrsBase>(uid));
         se.erase(last, se.end());
         
         for (srs_vec_iter it = se.begin(); it != se.end(); ++it) {
            boost::shared_ptr<T> srselem = boost::dynamic_pointer_cast<T>(*it);
            if (srselem) {
               return srselem;
            }
         }
         return boost::shared_ptr<T>();
      }
      
      
      template <class CSrsChip>
      boost::shared_ptr<CSrsChip> CPropertyTreeParserSrs::locate_srs(long uid, const std::string& name) const {
         typedef std::vector<SrsBasePtr> srs_vec_type;
         typedef std::vector<SrsBasePtr>::iterator srs_vec_iter;
         std::vector<SrsChipPtr> chips;
         
         Q_FOREACH(SrsBasePtr sb, srs_elements_) {
            if (SrsFecPtr sf = boost::dynamic_pointer_cast<CSrsFec>(sb)) {
               if (SrsChipPtr sc = sf->locate_chip(uid, name)) {
                  chips.push_back(sc);
               }
            }
         }
         
         if (chips.size() == 1) {
            return chips.front();
         }
         else if (chips.empty()) {
            std::stringstream ss;
            ss << "locate_srs<CSrsChip> no chip " << uid << " '" << name << "'";
            throw std::runtime_error(ss.str().c_str());
         }
         else {
            std::stringstream ss;
            ss << "locate_srs<CSrsChip> chip not unique " << uid << " '" << name << "'";
            throw std::runtime_error(ss.str().c_str());
         }
      }
      
      
      
   }
   
} // namespace

#endif
