//
//  DetStripId.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetStripId.h"

#include <iostream>

using namespace online::display;


CDetStripId::CDetStripId() :
m_chamber_name("unmapped"),
m_multi_layer_id(0),
m_layer_id(0),
m_readout_name('E'),
m_strip_no(0)
{ }


CDetStripId::CDetStripId(const std::string& chamber_name,
                         size_t multi_layer_id, 
                         size_t layer_id,
                         char readout_name,
                         size_t strip_no) :
m_chamber_name(chamber_name),
m_multi_layer_id(multi_layer_id),
m_layer_id(layer_id),
m_readout_name(readout_name),
m_strip_no(strip_no)
{
   
}


CDetStripId::~CDetStripId()
{
   
}


std::ostream& online::display::operator<< (std::ostream & out, const CDetStripId & sid)
{
   return out << sid.m_chamber_name << " "
   << sid.m_multi_layer_id << " " 
   << sid.m_layer_id << " " 
   << sid.m_readout_name << " " 
   << sid.m_strip_no;
}
