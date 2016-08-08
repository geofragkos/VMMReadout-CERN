//
//  SharedSrsChannelData.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SharedSrsChannelData.h"
#include "SrsChannelData.h"

#include <iostream>


using namespace online::display;


//Since void_allocator is convertible to any other allocator<T>, we can simplify
//the initialization taking just one allocator for all inner containers.

CSharedSrsChannelData::CSharedSrsChannelData(const CSrsChannelData& data, 
                                             const ShmemVoidAllocator& alloc_inst) :  
m_time(data.m_time), 
m_charge(data.m_charge), 
m_user_data(alloc_inst)
{
   m_user_data.assign(data.m_user_data.begin(), data.m_user_data.end());
}


CSharedSrsChannelData::~CSharedSrsChannelData()
{
   
}

