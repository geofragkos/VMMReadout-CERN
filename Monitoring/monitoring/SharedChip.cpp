//
//  SharedChip.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 11/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SharedChip.h"
//#include <boost/foreach.hpp>

#include <iostream>

using namespace online::display;

CSharedChip::CSharedChip( const ShmemVoidAllocator& alloc_inst) :
m_chip_id(),
m_chip_type(shChipTypeInvalid),
//m_chip_name(alloc_inst),
m_q(alloc_inst),
m_t(alloc_inst)

//m_chip_id(chip->get_chip_id()),
//m_chip_type(shChipTypeInvalid),
//m_events(std::less<EBEventIdType>(), alloc_inst)
{
//   m_chip_name = chip->name();
  //m_chip_type = check_chip_type(chip);
}


CSharedChip::~CSharedChip()
{

}

CSharedChip::CSharedChip(const CSharedChip& rhs) :
m_chip_id(rhs.m_chip_id),
m_chip_type(rhs.m_chip_type),
//m_chip_name(rhs.m_chip_name),
m_q(rhs.m_q),
m_t(rhs.m_t)
{

}


CSharedChip& CSharedChip::operator=(const CSharedChip& rhs)
{
   if (this != &rhs) {
      m_chip_id   = rhs.m_chip_id;
      m_chip_type = rhs.m_chip_type;
//      m_chip_name = rhs.m_chip_name;
      m_q = rhs.m_q;
      m_t = rhs.m_t;
   }
   return *this;
}




void CSharedChip::print() const
{
   std::cout << "=== chip id = " << CSrsChipId(m_chip_id).get_string() << " ==== " << std::endl;
//   Q_FOREACH(const EventMapValueType& evt, m_events) {
//      std::cout << " = " << evt.first << ": sz=" << evt.second.size() << std::endl;
//   }
}
