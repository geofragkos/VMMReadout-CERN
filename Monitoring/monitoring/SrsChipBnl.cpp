//
//  SrsChipBnl.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 18/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChipBnl.h"
#include "SrsChannelVmmArizona.h"
//#include "RootTreeFillerBnlMiniCard.h"
//#include "RootWriter.h"
#include "SRSEquipmentHeader.h"
//#include "RootReader.h"

#include <TFile.h>

#include <iostream>

using namespace online::display;

std::string CSrsChipBnl::BnlPedestalFileName = "";

//CRootTreeFillerBnlMiniCard* CSrsChipBnl::m_root_tree_filler_bnlmini = 0;


CSrsChipBnl::CSrsChipBnl(const CSrsChipId& chip_id, const std::string& name) :
CSrsChip(chip_id, name)
{
   
}


CSrsChipBnl::~CSrsChipBnl()
{
   
}

//CRootTreeFiller* CSrsChipBnl::get_root_tree_filler(CRootWriter* writer)
//{
//   return writer->get_filler_bnl_mini();
//}


bool CSrsChipBnl::check_equipment_key(uint32_t eqkey)
{
   return SRSEqHdrKeys::SRSEquipmentBnlArizonaCard == eqkey 
   || SRSEqHdrKeys::SRSEquipmentBnlMiniDaqCard == eqkey;
}

void CSrsChipBnl::set_pedestal_filename(const std::string& filename)
{
   CSrsChipBnl::BnlPedestalFileName = filename;
}


void CSrsChipBnl::load_pedestal_file()
{
//   if (CSrsChipBnl::BnlPedestalFileName.empty()) {
//      std::cout << "WARN: BNL pedestal file not specified" << std::endl;
//      return;
//   }
//   m_pedestals.clear();
//   //load file (root)
//   CRootReader::load_pedestal_file_bnl(CSrsChipBnl::BnlPedestalFileName, m_chip_id, m_pedestals);
//   std::cout << "CSrsChipBnl::load_pedestal_file() pedestal info for " <<  m_pedestals.size() << " channels" <<  std::endl;
//   std::cout << "TODO: check if all my channels have pedestal values?" << std::endl;
  
}
