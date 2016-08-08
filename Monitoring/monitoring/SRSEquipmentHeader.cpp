//
//  SRSEquipmentHeader.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SRSEquipmentHeader.h"

#include <iostream>

using namespace online::display;

//hardware description in the header types
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentBad = 0x000000;
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentApvRaw = 0x414443; //"ADC" APV25 SRS FEC unsuppressed
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentApvZeroSuppressed = 0x41505A; //"APZ" , APV25 SRS FEC unsuppressed
//or that one? 0x41445A = DZ
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentBnlArizonaCard = 0x415A43; // "" VMM1 AZ 2-chip card
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentBnlMiniDaqCard = 0x424E4C; // "BNL" VMM1 BNL-DAQ 1 chip card
const uint32_t  online::display::SRSEqHdrKeys::SRSEquipmentMmDaqTest = 0xAB1CDE;


   
//
//CSRSEquipmentHeader::CSRSEquipmentHeader()
//{
//   
//}
//
//
//CSRSEquipmentHeader::~CSRSEquipmentHeader()
//{
//   
//}

