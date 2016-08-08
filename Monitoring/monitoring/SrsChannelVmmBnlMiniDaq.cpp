//
//  SrsChannelVmmBnlMiniDaq.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 12/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannelVmmBnlMiniDaq.h"
//#include "RootTreeFiller.h"
#include "DetStripId.h"
#include "DetConnector.h"

#include <iostream>



using namespace online::display;

// data	1 32-bit word              bits 0-5 ad, bits 6-18 time, bits19-31 amp			
//   int address = word & 0x0000003F; // bits 0-5
//   int time =    word & 0x0007FFC0; // bits 6-18
//   int amp =     word & 0xFFF80000; // bits 19-31


CSrsChannelVmmBnlMiniDaq::CSrsChannelVmmBnlMiniDaq(const CSrsChipId& chip_id, const uint32_t& word)
: CSrsChannel(CSrsChannelId(chip_id, word & 0x0000003F), 
              CSrsChannelData(word & 0x0007FFC0, 
                              word & 0xFFF80000))
{

}


CSrsChannelVmmBnlMiniDaq::~CSrsChannelVmmBnlMiniDaq()
{
   
}

//void CSrsChannelVmmBnlMiniDaq::prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn)
//{
//   if(DetConnPtr cptr = conn.lock()) {
//      CDetStripId strip_id = cptr->get_mapped_strip_id(m_channel_id);
//      std::cout << "CSrsChannelVmmBnlMiniDaq::prefill() "<< m_channel_id.to_string() << std::endl;
//      filler->prefill_channel(m_channel_id, m_channel_data, strip_id);
//   }
//   else {
//      std::cout << "CSrsChannelVmmBnlMiniDaq::prefill connector weak ptr lock failed" << std::endl;
//   }
//}

