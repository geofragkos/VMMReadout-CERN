//
//  Logger.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 22.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "Logger.h"

#include <boost/thread/recursive_mutex.hpp>
#include <iostream>

using namespace online::display;

CLogger* volatile CLogger::m_instance = 0;

CLogger::CLogger()
{
   
}


CLogger::~CLogger()
{
   
}

CLogger* CLogger::instance()
{
   //read 7 in http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
   // if lock overhead a problem? - acquire one per thread(?)
   
   boost::recursive_mutex::scoped_lock(m_mutex);
   if (m_instance == 0) {
      CLogger* volatile tmp = new CLogger();
      m_instance = tmp;
   }
   return m_instance;
}

void CLogger::msg(const std::string& msg)
{
   boost::recursive_mutex::scoped_lock(m_mutex);
   std::cout << "MSG:  " <<msg << std::endl;
}

void CLogger::warn(const std::string& msg)
{
   boost::recursive_mutex::scoped_lock(m_mutex);
   std::cout << "WARN: " << msg << std::endl;
}

void CLogger::error(const std::string& msg)
{
   boost::recursive_mutex::scoped_lock(m_mutex);
   std::cout <<  "ERR:  " << msg << std::endl;
}

