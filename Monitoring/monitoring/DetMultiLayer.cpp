//
//  DetMultiLayer.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetMultiLayer.h"

#include <iostream>

using namespace online::display;

CDetMultiLayer::CDetMultiLayer()
{
   
}

CDetMultiLayer::CDetMultiLayer(DetBaseWeakPtr detector, DetBaseWeakPtr parent) :
CDetBase(detector, parent)
{
   
}

CDetMultiLayer::~CDetMultiLayer()
{
   
}

void CDetMultiLayer::print() const
{
   std::cout << "(*) MultiLayer ";
   CDetBase::print();
}

