//
//  DetConnector.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 28.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetConnector.h"
#include "DaqServerConfig.h"
#include "SrsChip.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <functional>

using namespace online::display;

CDetConnector::CDetConnector(DetBaseWeakPtr detector,
                             DetBaseWeakPtr parent,
                             size_t idnumber,
                             const std::string& name) :
CDetBase(detector, parent, idnumber, name, Coord3(), Coord3(), Coord3()),
m_pin_strip(),
m_pin_channel(),
m_channel_strip(),
m_connected_chips()
{
   std::cout << "CONSTRUCTOR CDetConnector " << name << std::endl;

}


CDetConnector::~CDetConnector()
{
   
   std::cout << "DESTRUCTOR CDetConnector " << name() << (m_parent.lock()?m_parent.lock()->name():" FLOATING") << std::endl;
   
}

void CDetConnector::clear()
{
   m_pin_strip.clear();
   m_pin_channel.clear();
   m_channel_strip.clear();
   m_connected_chips.clear();
}


void CDetConnector::configure(CDaqServerConfig* daqconfig)
{
   clear();
   std::cout << "TODO: implement CDetConnector::configure" << std::endl;
}


void CDetConnector::print() const
{
   std::cout << name() << ": map:" << m_channel_strip.size() << " chips:";
   Q_FOREACH(ConnectedChipSeqPair p, m_connected_chips) {
      if (SrsChipPtr chipptr = p.second.lock() )
         std::cout << p.first << "-" << chipptr->name() << " ";
   }
   
}


/*

 
 # pin counting starts at 1
 # TODO: test empty lines , and no last line empty
 # pin,multilayer,layer,readout,strip
 
 1,0,0,X,1
 2,0,0,X,2
 3,0,0,X,3
 4,0,0,X,4
 
 */

void CDetConnector::read_strip_map_file(const std::string& filename)
{   
   std::cout << "CDetConnector::read_strip_map_file for "  << name()  << std::endl;
   struct stat stFileInfo;
   if (stat(filename.c_str(), &stFileInfo)) {
      std::stringstream ss;
      ss << "CDetConnector::read_strip_map_file(): '"<< filename << "' does not exist.";
      throw ss.str().c_str();
   }
      
   std::ifstream file(filename.c_str(), std::ifstream::in);
   if (!file.is_open()) {
      std::stringstream ss;
      ss << "CDetConnector::read_strip_map_file(): can't open '"<< filename << "'.";
      throw ss.str().c_str();
   }

   std::vector<PinStripPair> local_pin_strip;
   
   int lineCounter = 0;
   char buff[256];
   while (file.getline(buff, 256)) {
      lineCounter += 1;
      std::string line(buff);
      //skip empty and comment lines
      boost::trim(line);
      if (boost::starts_with(line, "#") || line.empty()) {
         continue;
      }
      
      
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(", \t");
      tokenizer tokens(line, sep);
      tokenizer::iterator tok_iter = tokens.begin();

      std::string pinstr;
      std::string mlystr;
      std::string laystr;
      std::string rdostr;
      std::string strstr;
      
      enum {state_pin, state_multi, state_layer, state_readout, state_strip} parse_state;
      parse_state = state_pin;
      
      for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
         if (parse_state == state_pin) {
            pinstr = boost::trim_copy(*tok_iter);
            parse_state = state_multi;
         }
         else if (parse_state == state_multi) {
            mlystr = boost::trim_copy(*tok_iter);
            parse_state = state_layer;
         }
         else if (parse_state == state_layer) {
            laystr = boost::trim_copy(*tok_iter);
            parse_state = state_readout;
         }
         else if (parse_state == state_readout) {
            rdostr = boost::trim_copy(*tok_iter);
            parse_state = state_strip;
         }
         else if (parse_state == state_strip) {
            strstr =  boost::trim_copy(*tok_iter);
         }
      }
      if (parse_state != state_strip) {
         //error
         //TODO throw
      }

//      std::cout << "read line :"
//      << pinstr << " "
//      << mlystr << " "
//      << laystr << " "
//      << rdostr << " "
//      << strstr << " "
//      << std::endl;
   
      try {
         unsigned pin = boost::numeric_cast<unsigned>(boost::lexical_cast<size_t>( pinstr ));
         unsigned mly = boost::numeric_cast<unsigned>(boost::lexical_cast<size_t>( mlystr ));
         unsigned lay = boost::numeric_cast<unsigned>(boost::lexical_cast<size_t>( laystr ));
         char rdo = rdostr.at(0);
         int str = boost::numeric_cast<int>(boost::lexical_cast<int>( strstr ));
         if(str>0)  {
             if(boost::shared_ptr<CDetBase> par = parent().lock())
             {
                local_pin_strip.push_back(PinStripPair(pin, CDetStripId(par->name(),
                                                                        mly,
                                                                        lay,
                                                                        rdo,
                                                                        str) ));
                //std::cout << "PSP:" << local_pin_strip.back().first << " --- " << local_pin_strip.back().second  << std::endl;
             }
             else {
                std::cout << "CDetConnector:: parent weak_ptr lock failed" << std::endl;
             }
         }
         
         
      } catch( boost::bad_lexical_cast const& ) {
         std::cout << "ERROR: CDetConnector::read_strip_map_file '" << filename 
         << "' not valid entry in line " << lineCounter << std::endl;
         throw std::runtime_error("bad parameters");
      } catch( boost::bad_numeric_cast const& ) {
         std::cout << "ERROR: CDetConnector::read_strip_map_file '" << filename 
         << "' value out of range in line " << lineCounter << std::endl;
         throw std::runtime_error("bad parameters");
      }

   }//while getline
   m_pin_strip = local_pin_strip;
   
   create_channel_strip_map();
   
   std::stringstream ss;
   ss << "CDetConnector::read_strip_map_file(): read '"<< m_pin_strip.size() << "' pin mappings to strip";
   std::cout << ss.str() << std::endl;
}

/*
 # pin - chip - channel
 # one could have more than 1 chip connected to a connector on a chamber
 1,0,1
 2,0,2
 3,1,1
 4,1,2..
 */

void CDetConnector::read_chip_connection_map_file(const std::string& filename, const CDaqServerConfig* )
{
   std::cout << "CDetConnector::read_chip_connection_map_file (" << filename << ") for "  << name()  << std::endl;
   
   struct stat stFileInfo;
   if (stat(filename.c_str(), &stFileInfo)) {
      std::stringstream ss;
      ss << "CDetConnector::read_chip_connection_map_file(): '"<< filename << "' does not exist.";
      throw ss.str().c_str();
   }

   std::ifstream file(filename.c_str());
   if (!file.is_open()) {
      std::stringstream ss;
      ss << "CDetConnector::read_chip_connection_map_file(): can't open '"<< filename << "'.";
      throw ss.str().c_str();
   }

   std::vector<PinChannelPair> local_pin_channel;
      
   int lineCounter = 0;
//   char buff[256];
   std::string line;
   while (getline(file, line)) {
      lineCounter += 1;
//      std::string line(buff);
      //skip empty and comment lines
      boost::trim(line);
      if (boost::starts_with(line, "#") || line.empty()) {
         continue;
      }
      

      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(", \t");
      tokenizer tokens(line, sep);
      tokenizer::iterator tok_iter = tokens.begin();
      
      std::string pinstr;
      std::string chipstr;
      std::string chanstr;

      
      enum {state_pin, state_chip, state_channel} parse_state;
      parse_state = state_pin;
      
      for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
         if (parse_state == state_pin) {
            pinstr = boost::trim_copy(*tok_iter);
            parse_state = state_chip;
         }
         else if (parse_state == state_chip) {
            chipstr = boost::trim_copy(*tok_iter);
            parse_state = state_channel;
         }
         else if (parse_state == state_channel) {
            chanstr = boost::trim_copy(*tok_iter);
         }
      }
      if (parse_state != state_channel) {
         std::stringstream ss;
         ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename 
         << "' error parsing line " << lineCounter;
         throw std::runtime_error(ss.str().c_str());      
      }

      
      try {
         size_t pin = boost::numeric_cast<size_t>(boost::lexical_cast<size_t>( pinstr ));
         size_t chipseq = boost::numeric_cast<size_t>(boost::lexical_cast<size_t>(chipstr));
         size_t chan = boost::numeric_cast<size_t>(boost::lexical_cast<size_t>( chanstr ));

            //dantrim
         //for(auto c : m_connected_chips) {
         //   std::cout << "c.first : " << c.first << "  c.second : " << c.second.lock()->name() << std::endl;
         //}
         #warning dantrim forcing chip seq to 0 for monitoring --> I don't think monitoring needs all of this infomration!!!!
        chipseq = 0;
         
         std::vector<ConnectedChipSeqPair>::iterator found = std::find_if(m_connected_chips.begin(),
                                                                          m_connected_chips.end(),
                                                                          FirstComparator<ConnectedChipSeqPair>(chipseq));
         if(found == m_connected_chips.end()) {
            std::stringstream ss;
            ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename 
            << "' local chip sequence number not defined " << chipseq;
            throw std::runtime_error(ss.str().c_str());
         }
         if(SrsChipPtr chipptr = found->second.lock()) {
            local_pin_channel.push_back(PinChannelPair(pin, CSrsChannelId(chipptr->get_chip_id(), chan)));
         }
         
         //std::cout << "PCP:" << local_pin_channel.back().first << " --- " << local_pin_channel.back().second  << std::endl;


         
      } catch( boost::bad_lexical_cast const& ) {
         std::stringstream ss;
         ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename 
         << "' not valid entry in line " << lineCounter;
         throw std::runtime_error(ss.str().c_str());
      } catch( boost::bad_numeric_cast const& ) {
          std::cout << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename
          << "' value out of range in line " << lineCounter;
         std::stringstream ss;
         ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename 
         << "' value out of range in line " << lineCounter;
         throw std::runtime_error(ss.str().c_str());
      } catch (std::range_error& re ) {
         throw;
      }

   }//while getline
   m_pin_channel = local_pin_channel;
   

   create_channel_strip_map();

   std::stringstream ss;
   ss << "CDetConnector::read_chip_connection_map_file(): read for '" << name() << "' " << m_pin_channel.size() << " pin mappings to electronics channels";
   std::cout << ss.str() << std::endl;
}


void CDetConnector::add_chip(boost::shared_ptr<CSrsChip> chip)
{
   std::cout << "implement CDetConnector::add_chip " << std::endl;
}


void CDetConnector::connect_chip(size_t seq_no, boost::shared_ptr<CSrsChip> chip)
{
   
   if (std::binary_search(m_connected_chips.begin(),
                          m_connected_chips.end(),
                          ConnectedChipSeqPair(seq_no, boost::weak_ptr<CSrsChip>()/* CSrsChipId()*/),
                          FirstLess<ConnectedChipSeqPair>()
                          )) 
   {
      throw std::runtime_error("CDetConnector::connect_chip local sequence number not unique");
   }
   
   
   m_connected_chips.push_back (ConnectedChipSeqPair (seq_no, chip));
   DetBasePtr par = parent().lock();
   std::cout << "CDetConnector::connect_chip " << name() << " " << (par?par->name():" NOT CONNECTED") << " " << seq_no << " " << chip->name() <<std::endl;

}

void CDetConnector::create_channel_strip_map()
{
   if (m_pin_strip.empty() ) { 
      std::cout << "CDetConnector::create_channel_strip_map() m_pin_strip empty for " << name() << std::endl;
      return;
   }
   if( m_pin_channel.empty()) {
      std::cout << "CDetConnector::create_channel_strip_map() m_pin_channel empty for " << name() << std::endl;
      return;
   }
   
   //sizes can be different   
   std::sort(m_pin_strip.begin(), m_pin_strip.end(), FirstLess<PinStripPair>());
   std::sort(m_pin_channel.begin(), m_pin_channel.end(), FirstLess<PinChannelPair>());
   
   std::vector<size_t> ps;
   std::vector<size_t> pc;
   std::vector<size_t> pcommon; //common pin numbers in both connector maps
   
   std::transform(m_pin_strip.begin(), m_pin_strip.end(), std::back_inserter(ps),
                  boost::bind( &PinStripPair::first , _1) );
   std::transform(m_pin_channel.begin(), m_pin_channel.end(), std::back_inserter(pc),
                  boost::bind( &PinChannelPair::first , _1) );   
   std::set_intersection(ps.begin(), ps.end(), pc.begin(), pc.end(), std::back_inserter(pcommon));
   std::cout << "CDetConnector::create_channel_strip_map() common elements:  in maps " << pcommon .size() << std::endl;

   std::vector<ChannelStripPair> local_channel_strip;
   Q_FOREACH(size_t pin, pcommon) {
      std::vector<PinStripPair>::iterator ips = std::find_if(m_pin_strip.begin(), m_pin_strip.end(), FirstComparator<PinStripPair>(pin));
      std::vector<PinChannelPair>::iterator ipc = std::find_if(m_pin_channel.begin(), m_pin_channel.end(), FirstComparator<PinChannelPair>(pin));
      if (ips == m_pin_strip.end() || ipc == m_pin_channel.end()) {
         std::stringstream ss;
         ss << "ERROR: CDetConnector::create_channel_strip_map error finding back common elements (prg logic)";
         throw std::runtime_error(ss.str().c_str());
      }
      local_channel_strip.push_back(ChannelStripPair(ipc->second, ips->second));
   }
   m_channel_strip = local_channel_strip;
   std::cout << "CDetConnector::create_channel_strip_map() m_channel_strip.size= " << m_channel_strip .size() << std::endl;
//   std::cout << "CDetConnector::create_channel_strip_map() print out channel map for " << name() << std::endl;
//   for (std::vector<ChannelStripPair>::iterator it = m_channel_strip.begin(); it != m_channel_strip.end(); ++it) {
//      std::cout << it->first << " --> " << it->second << std::endl;
//   }
}


CDetStripId CDetConnector::get_mapped_strip_id(const CSrsChannelId& channel_id) const
{
   std::vector<ChannelStripPair>::const_iterator found = std::find_if(m_channel_strip.begin(),
                                                                m_channel_strip.end(),
                                                                FirstComparator<ChannelStripPair>(channel_id));
   if (found == m_channel_strip.end()) {
      throw std::range_error("unmapped strip");
   }
   return found->second;
}


void CDetConnector::set_chip_connection_from(DetConnPtr other, DetConnPtr toconnector)
{
   std::cout << "CDetConnector::set_chip_connection_from " << other->name() 
   << " to " << name() 
   << " with chips " << other->m_connected_chips.size() << std::endl;

   
   m_pin_channel = other->m_pin_channel;
   m_connected_chips = other->m_connected_chips;
   
   Q_FOREACH(ConnectedChipSeqPair sqchip, m_connected_chips) {
         //get chip
      if(SrsChipPtr chipptr = sqchip.second.lock()) {
         
         chipptr->connect_to_connector(toconnector);      
      }
      else {
         std::cout << "CDetConnector::set_chip_connection_from LOCK FAILED" << std::endl;
      }
   }

   create_channel_strip_map();
}


void CDetConnector::set_pin_strip_mapping_from(DetConnPtr other)
{
   m_pin_strip = other->m_pin_strip;
   create_channel_strip_map();
}

