//
//  DetLayer.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetLayer.h"

#include <iostream>

using namespace online::display;

CDetLayer::CDetLayer()
{
   
}

CDetLayer::CDetLayer(DetBaseWeakPtr detector, DetBaseWeakPtr parent) :
CDetBase(detector, parent)
{
   
}


CDetLayer::~CDetLayer()
{
   
}

void CDetLayer::print() const
{
   std::cout << "(*) Layer ";
   CDetBase::print();
}


