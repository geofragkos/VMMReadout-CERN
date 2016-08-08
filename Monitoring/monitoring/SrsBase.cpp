//
//  SrsBase.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsBase.h"
#include "SrsChipId.h"
#include "Logger.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace online::display;

CSrsBase::CSrsBase(size_t idnum, const std::string& name) :
m_uid(idnum), m_name(name)
{
   
}


CSrsBase::~CSrsBase()
{
   
}

inline size_t CSrsBase::uid() const 
{
   return m_uid;
}


void CSrsBase::print() const 
{
   std::cout << "id=" << m_uid << " name=" << m_name << std::endl;   
}

boost::shared_ptr<CSrsChip> CSrsBase::get_srs_chip(int chip_no)
{
   std::stringstream ss;
   ss << "CSrsBase::get_srs_chip(): chip number " << chip_no << " out of range for srs device id=" << m_uid << " '" << m_name << "'";
   CLogger::instance()->warn(ss.str());
   throw std::out_of_range(ss.str());
}

std::string CSrsBase::to_string() const
{
   std::stringstream ss;
   ss <<  "id=" << m_uid << " name=" << m_name ;
   return ss.str();
}

