//
//  SrsChannelId.cpp
//  browser
//
//  Created by Marcin Byszewski on 10.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannelId.h"
#include "SrsChipId.h"

#include <iostream>
#include <sstream>

using namespace online::display;

CSrsChannelId::CSrsChannelId() : m_id(0) 
{ }

CSrsChannelId::CSrsChannelId(uint32_t full_id) : m_id(full_id) 
{ }

CSrsChannelId::CSrsChannelId(uint32_t fecNo, uint32_t chipNo, uint32_t chanNo) 
: m_id( from_fec_chip_channel(fecNo, chipNo, chanNo ) )
{ }

//CSrsChannelId::CSrsChannelId(uint32_t chipId,uint32_t chanNo) : m_id( (chipId << 8) | chanNo ) 
//{ }

CSrsChannelId::CSrsChannelId(const CSrsChipId& chipId, uint32_t chanNo) 
: m_id( from_chipid_channel(chipId, chanNo) ) 
{ }

//CSrsChannelId::CSrsChannelId(const CSrsChannelId& rhs) : m_id(rhs.m_id) 
//{ }

CSrsChannelId::~CSrsChannelId() 
{}


uint32_t CSrsChannelId::channel_id()  const 
{ 
   return m_id;
}

uint32_t CSrsChannelId::fec_no() const 
{ 
   return (m_id >> 16) & 0xFF  ;
} 

uint32_t CSrsChannelId::chip_no() const 
{ 
   return (m_id >> 8)  & 0xF   ;
}

CSrsChipId CSrsChannelId::chip_id() const 
{ 
   return  CSrsChipId (fec_no(), chip_no() ); // should limit? & 0xFFF
} 

uint32_t CSrsChannelId::channel_no() const 
{ 
   return (m_id)    & 0xFF  ;
}

uint32_t CSrsChannelId::from_fec_chip_channel(uint32_t fecNo,  uint32_t chipNo, uint32_t chanNo)
{
   return ( ((fecNo<<8 | chipNo ) << 8) | chanNo );
}

uint32_t CSrsChannelId::from_chipid_channel(const CSrsChipId& chipId, uint32_t chanNo)
{
   return ( (chipId.chip_id() << 8) | chanNo );
}

std::string CSrsChannelId::to_string() const
{
   std::stringstream ss;
   ss << fec_no() << ":" << chip_no() << ":" << channel_no();
   return ss.str();
}
