//
//  SrsChannelData.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannelData.h"

#include <iostream>

using namespace online::display;

CSrsChannelData::CSrsChannelData()
{
   
}

CSrsChannelData::CSrsChannelData(double time, double charge) :
m_time(time), m_charge(charge), m_user_data()
{
   
}

CSrsChannelData::~CSrsChannelData()
{
   
}

