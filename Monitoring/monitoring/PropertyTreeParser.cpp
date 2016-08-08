//
//  PropertyTreeParser.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 30.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "PropertyTreeParser.h"
#include "DaqServerConfig.h"


#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>

using namespace online::display;

CPropertyTreeParser::CPropertyTreeParser(const boost::property_tree::ptree& ptree, CDaqServerConfig& daqconfig) :
ptree_(ptree), daqconfig_(daqconfig)
{
   
}


CPropertyTreeParser::~CPropertyTreeParser()
{
   
}



long CPropertyTreeParser::id_number_from_string(const std::string& idstr) const
{
   long fid = -1;
   try {
      unsigned fidnum = boost::numeric_cast<unsigned>(boost::lexical_cast<size_t>( idstr ));
      fid = fidnum;
      return fid;      
   } catch( boost::bad_lexical_cast const& ) {
      std::cout << "ERROR: id '" << idstr << "' is not valid" << std::endl;
      throw std::runtime_error("bad parameters");
   } catch( boost::bad_numeric_cast const& ) {
      std::cout << "ERROR: id '" << idstr << "' is out of range" << std::endl;
      throw std::runtime_error("bad parameters");
   }
   if (fid < 0) {
      std::cout << "ERROR: id has not been specified (ex: <fec id = \"0\"> )" << std::endl;
      throw std::runtime_error("bad parameters");
   }
}
