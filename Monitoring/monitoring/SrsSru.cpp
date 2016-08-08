//
//  SrsSru.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsSru.h"

#include <iostream>


using namespace online::display;

CSrsSru::CSrsSru(size_t idnum, const std::string& name, const std::string& ip_address) : 
CSrsNetworkDevice(idnum, name, boost::asio::ip::address::from_string(ip_address))
{
}


CSrsSru::~CSrsSru()
{
   
}


void CSrsSru::reset() 
{
   //
}

//TODO: implement CSrsSru::insert_receiver_data
/**
 handle data received from network
 */
void CSrsSru::insert_receiver_data(boost::shared_ptr<CUDPFrame> udpframe) 
{
   std::cerr << "CSrsSru::insert_receiver_data NOT IMPLEMENTED\n";
}


