//
//  DetDetector.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetDetector.h"

#include <iostream>

using namespace online::display;

/*
 CDetBase(CDetDetector* detector,
 CDetBase* parent,
 size_t idnumber,
 const std::string& name,
 const Coord3& size,
 const Coord3& position,
 const Coord3& rotation);
 */

/*
 explicit CDetChamber(CDetDetector* detector,
 CDetBase* parent,
 size_t idnumber,
 const std::string& name,
 const Coord3& size,
 const Coord3& position,
 const Coord3& rotation);
 
 */

CDetDetector::CDetDetector(const std::string& name,
                           const Coord3& size,
                           const Coord3& position,
                           const Coord3& rotation) :
CDetBase(DetBasePtr(), DetBasePtr(), 1, name, size, position, rotation)
{
   
}


CDetDetector::~CDetDetector()
{
   
}

void CDetDetector::print() const
{
   std::cout << "(*) Detector ";
   CDetBase::print();
}


void CDetDetector::add_child(DetBasePtr elem, size_t idnum)
{
//   elem->m_idnumber = idnum;
//   elem->m_detector = shared_from_this();
//   elem->m_parent = shared_from_this();
   elem->set_idnumber(idnum);
   elem->set_detector(shared_from_this());
   elem->set_parent(shared_from_this());
   CDetBase::add_child(elem);
}

