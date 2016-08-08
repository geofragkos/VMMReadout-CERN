//
//  SharedChannel.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 11/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SharedChannel.h"
#include "SrsChannel.h"

#include <iostream>

using namespace online::display;

CSharedChannel::CSharedChannel(SrsChannelPtr local_channel, const ShmemVoidAllocator& alloc_inst) :
m_id(local_channel->channel_id()),
m_data(local_channel->channel_data(), alloc_inst)
{
   
}


CSharedChannel::~CSharedChannel()
{
   
}

