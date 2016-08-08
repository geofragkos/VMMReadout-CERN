//
//  SrsChannelVmmArizona.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 31.5.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SrsChannelVmmArizona.h"
//#include "RootWriter.h"
//include "RootTreeFiller.h"
#include "DetStripId.h"
#include "DetConnector.h"

#include <iostream>

using namespace online::display;

//vmm data in sequence: event number, amplitude , time

CSrsChannelVmmArizona::CSrsChannelVmmArizona(const CSrsChannelId& channel_id, const CSrsChannelData& data) :
CSrsChannel(channel_id, data)
{
   
}



CSrsChannelVmmArizona::~CSrsChannelVmmArizona()
{
   
}


//void CSrsChannelVmmArizona::prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn)
//{
//   if(DetConnPtr cptr = conn.lock()) {
//      CDetStripId strip_id = cptr->get_mapped_strip_id(m_channel_id);
//      std::cout << "CSrsChannelVmmArizona::prefill() "<< m_channel_id.to_string() << std::endl;
//      filler->prefill_channel(m_channel_id, m_channel_data, strip_id);
//   }
//   else {
//      std::cout << "CSrsChannelVmmArizona::prefill connector weak ptr lock failed" << std::endl;
//   }
//}

