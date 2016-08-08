//
//  SrsChipMmDaqTest.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 14/11/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChipMmDaqTest.h"
#include "SRSEquipmentHeader.h"
//#include "RootWriter.h"


#include <iostream>

using namespace online::display;

CSrsChipMmDaqTest::CSrsChipMmDaqTest(const CSrsChipId& chip_id, const std::string& name) :
CSrsChip(chip_id, name)
{
//   std::cout << "CREATE: TESTER CSrsChipMmDaqTest()" << std::endl;
}


CSrsChipMmDaqTest::~CSrsChipMmDaqTest()
{
   
}

bool CSrsChipMmDaqTest::check_equipment_key(uint32_t eqkey)
{
   return SRSEqHdrKeys::SRSEquipmentMmDaqTest == eqkey;

}

/// get correct root tree filler for the chip type
//CRootTreeFiller* CSrsChipMmDaqTest::get_root_tree_filler(CRootWriter* writer)
//{
//   return writer->get_filler_mmdaq_test();

//}

//void CSrsChipMmDaqTest::load_pedestal_file()
//{
////   if (filename.empty()) {
////      std::cout << "WARN: BNL pedestal file not specified" << std::endl;
////      return;
////   }
//////   m_pedestals_map.clear();
////   std::cout << "CSrsChipMmDaqTest::load_pedestal_file() pedestal info for "
//////   <<  m_pedestals_map.size()
////   << " channels" <<  std::endl;
//}

//void CSrsChipMmDaqTest::clear_pedestals()
//{
//   //   m_pedestals_map.clear();

//}


//void CSrsChipMmDaqTest::prefill_pedestals( CRootWriter* writer)
//{

//     std::cout << " CSrsChipMmDaqTest::prefill_pedestals() chip " << name() << " " << std::endl;

   
//}


//bool CSrsChipMmDaqTest::get_pedestal(const uint32_t& channel_id, CSrsPedestal& ped) const
//{
//   return false;
//}




