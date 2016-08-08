//
//  SharedEBBuffer.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 08/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//


#include "SharedEBBuffer.h"
//#include "SharedMMEvent.h"
//#include "DaqServerConfig.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
//#include <boost/foreach.hpp>
#include <cassert>

#include <iostream>

#include <QObject>

using namespace online::display;

namespace bipc= boost::interprocess;

CSharedEBBuffer::CSharedEBBuffer(ShmemVoidAllocator alloc_inst) :
m_mutex(),
m_chips(alloc_inst),
m_max_event_depth(0)
{ }


CSharedEBBuffer::~CSharedEBBuffer()
{
   std::cout << "CSharedEBBuffer() destructor" << std::endl;
}

size_t CSharedEBBuffer::max_event_depth() const
{
   return m_max_event_depth;
}

// void CSharedEBBuffer::add_chip(SrsChipPtr chip, const ShmemVoidAllocator& alloc_inst)
// {
//    m_chips.push_back(CSharedChip(chip, alloc_inst));
// }


// void CSharedEBBuffer::configure(CDaqServerConfig* daq_config)
// {
// }


// void CSharedEBBuffer::insert_mmevent(MMEventPtr p, const ShmemVoidAllocator& alloc_inst)
// {

//    //CMMEvent has:
//    //EBEventIdType      m_event_id;
//    //CSrsEventId        m_srs_event_id;
//    //typedef std::map<SrsChipPtr, SrsChannelList> SrsChipChannelMap;
//    //SrsChipChannelMap  m_chips;

//    m_max_event_depth = 0;
//    EBEventIdType evtid = p->event_id();
//    Q_FOREACH(CSharedChip& chip, m_chips) {
//       const CSrsChipId& chid = chip.chip_id();
//       const SrsChannelList* loc_chans = p->get_channels(chid);
//       chip.insert_channels(evtid, loc_chans, alloc_inst);
//       m_max_event_depth += ch


void CSharedEBBuffer::print() const
{
   std::cout << "=== CSharedEBBuffer::print() === " << std::endl;
   std::cout << " chips size = "<< m_chips.size() << std::endl;
   int ii =0;
   Q_FOREACH(CSharedChip chip, m_chips) {

            chip.print();
   }
}
