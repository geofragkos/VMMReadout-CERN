//
//  DetBase.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 12.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetBase.h"
#include "DetDetector.h"

#include <boost/bind.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>


using namespace online::display;

CDetBase::CDetBase():
m_detector(),
m_parent(),
m_children(),
m_idnumber(),
m_name(),
m_size(),
m_position(),
m_rotation()
{ }

CDetBase::CDetBase(DetBaseWeakPtr detector, DetBaseWeakPtr parent) :
m_detector(detector),
m_parent(parent),
m_children(),
m_idnumber(),
m_name(),
m_size(),
m_position(),
m_rotation()
{

}


CDetBase::CDetBase(DetBaseWeakPtr detector,
                   DetBaseWeakPtr parent,
                   size_t idnumber,
                   const std::string& name,
                   const Coord3& size,
                   const Coord3& position,
                   const Coord3& rotation) :
m_detector(detector),
m_parent(parent),
m_children(),
m_idnumber(idnumber),
m_name(name),
m_size(size),
m_position(position),
m_rotation(rotation)
{
}

CDetBase::~CDetBase()
{

}

void CDetBase::clear()
{
   m_detector.reset();
   m_parent.reset();
   m_idnumber = 0;
   m_name.clear();
   m_size = Coord3();
   m_position = Coord3();
   m_rotation = Coord3();

   std::for_each(m_children.begin(), m_children.end(), boost::bind(&CDetBase::clear, _1));
   m_children.clear();
}



void CDetBase::print() const
{
   std::cout << "{id=" << m_idnumber << " " << m_name << "}" << std::endl;
   std::for_each(m_children.begin(), m_children.end(), boost::bind(&CDetBase::print, _1));
}


void CDetBase::add_child(DetBasePtr elem)
{
   if (std::binary_search(m_children.begin(), m_children.end(), elem,
                          CDetBase::IdNumberLess<CDetBase>())) {
      std::stringstream ss;
      ss << "ERROR: can not add detector element '" << elem->name() << "' to " << m_name << " - id number=" << elem->idnumber() << " is not unique";
      DetBasePtrCont::iterator it = std::find_if(m_children.begin(), m_children.end(), CDetBase::IdNumberEquals<CDetBase>(elem->idnumber()));
      ss << "; exisiting element " << (*it)->name();
      throw std::runtime_error(ss.str().c_str());
   }

   DetBasePtrCont::iterator found = std::find_if(m_children.begin(), m_children.end(), CDetBase::NameEquals<CDetBase>(elem->name()));
   if (found != m_children.end()) {
      std::stringstream ss;
      ss << "ERROR: can not add detector element '" << elem->name() << "' to '" << m_name << "' - name is not unique";
      throw std::runtime_error(ss.str().c_str());
   }
   m_children.push_back(elem);
   m_children.sort(CDetBase::IdNumberLess<CDetBase>());
}

void CDetBase::add_child(DetBasePtr elem, size_t idnum)
{
   elem->m_idnumber = idnum;
   elem->m_detector = m_detector;
   elem->m_parent = shared_from_this();
   add_child(elem);
}

std::string CDetBase::getMyChamberName()
{

    if(DetBasePtr p = m_parent.lock())
    {
       return p->getMyChamberName();

    }
    return std::string();
}

std::string CDetBase::getMyReadoutName()
{

    if(DetBasePtr p = m_parent.lock())
    {
       return p->getMyChamberName();

    }
    return std::string();
}


void CDetBase::set(size_t idnumber, const std::string& name, const Coord3& size, const Coord3& pos, const Coord3& rot)
{
   m_idnumber = idnumber;
   m_name = name;
   m_size = size;
   m_position = pos;
   m_rotation = rot;
}



void CDetBase::set_idnumber(size_t idnum)
{
   m_idnumber = idnum;
}

void CDetBase::set_detector(DetBasePtr det)
{
   m_detector = det;
}

void CDetBase::set_parent(DetBasePtr par)
{
   m_parent = par;
}


DetBaseWeakPtr CDetBase::detector()
{
   return m_detector;
}

DetBaseWeakPtr CDetBase::parent()
{
   return m_parent;
}



DetBasePtrCont& CDetBase::get_children()
{
   return m_children;
}

const DetBasePtrCont& CDetBase::get_children() const
{
   return m_children;
}

DetBasePtr CDetBase::get_child(const std::string& name)
{
   DetBasePtrCont::iterator found = std::find_if(m_children.begin(), m_children.end(),
                                                 CDetBase::NameEquals<CDetBase>(name));
   if (found == m_children.end()) {
      return DetBasePtr();
   }
   return *found;
}

DetBasePtr CDetBase::get_child(size_t idnum)
{
   DetBasePtrCont::iterator found = std::find_if(m_children.begin(), m_children.end(),
                                                 CDetBase::IdNumberEquals<CDetBase>(idnum));
   if (found == m_children.end()) {
      return DetBasePtr();
   }
   return *found;
}


size_t CDetBase::idnumber() const
{
   return m_idnumber;
}

const std::string& CDetBase::name() const
{
   return m_name;
}

const Coord3& CDetBase::size() const
{
   return m_size;
}

const Coord3& CDetBase::position() const
{
   return m_position;
}

const Coord3& CDetBase::rotation() const
{
   return m_rotation;
}
