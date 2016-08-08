//
//  SharedBufferHandler.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 09/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "SharedBufferHandler.h"
//#include "DaqServerConfig.h"

//#include <boost/foreach.hpp>

#include <iostream>
#include <vector>
using namespace online::display;

CSharedBufferHandler::CSharedBufferHandler() :
m_shm_manager(bipc::open_or_create, "mmDaqSharedEBSegment", 65536),
m_shm_mutex(m_shm_manager.find< ShmemMutexType >( "mutex" ).first ),
m_shm_chips(m_shm_manager.find< ShmemChipVector >( "mmDaqSharedEBBuffer" ).first  ) ,
m_max_event_depth(0)
{
   
   std::cout << "*** CSharedBufferHandler() constructor" << std::endl;
  
}

void CSharedBufferHandler::print() {
  std::cout << "locking mutex... "<< std::endl;
  if((m_shm_manager.find< ShmemMutexType >( "mutex" ).second != 1))
     return;

  ShmemScopedLock lock(*m_shm_mutex);
   std::cout << "CSharedBufferHandler::print():  #shmchips = " << m_shm_chips->size() << std::endl;
   
   for (ShmemChipVector::const_iterator isc = m_shm_chips->begin(); isc != m_shm_chips->end(); ++isc) {
      isc->print();
   }

}

CSharedBufferHandler::~CSharedBufferHandler()
{
   std::cout << "*** CSharedBufferHandler() destructor" << std::endl;
 try
   {
      m_shm_mutex->unlock();
   }
   catch ( ... )
   {
   }
}

// void CSharedBufferHandler::configure(const CDaqServerConfig* daqconfig) 
// {
//    //clear and configure vector of shared chips
   
//    ShmemScopedLock lock(*m_shm_mutex);
//    m_shm_chips->clear();
   
   
//    std::vector<SrsChipPtr> chips = daqconfig->locate_srs_chips();
   
//    // create shared chips in CSharedBuffer
//    Q_FOREACH(SrsChipPtr chip, chips) {
//       m_shm_chips->push_back(CSharedChip(chip, m_shm_manager.get_allocator<ShmemVoidAllocator>() ));
//    }
   
   
//    std::cout << "CSharedBufferHandler::configure():  #shmchips = " << m_shm_chips->size() << std::endl;
   
//    for (ShmemChipVector::iterator isc = m_shm_chips->begin(); isc != m_shm_chips->end(); ++isc) {
//       isc->print();
//    }
  
  

   
   
   
// //   std::vector<SrsChipPtr>::iterator ichip = chips.begin();
// //   std::vector<ShmemVoidAllocator> allocs(chips.size(), alloc_inst);
//    //Construct the shared memory array
// //   CSharedChip *ebb = m_shm_segment.construct_it<CSharedChip>("mmDaqSharedChips")
// //   [chips.size()](&chips[0], &allocs[0]);

   
// //   //Find the object
// //   std::pair<CSharedChip*, size_t> res = m_shm_segment.find<CSharedChip> ("mmDaqSharedChips");
// //   //Length should be >0
// //   if(res.second > 0) {
// //      std::cout << "*** CSharedBufferHandler OK while creating mmDaqSharedEvents - found: r.size=" << res.second << std::endl;
// //   }
// //   else {
// //      std::cout << "*** CSharedBufferHandler error while creating mmDaqSharedChips - not found: res.second = " << res.second << std::endl;
// //   }

   
   
   
// //   //check first or second?
// //   if (CSharedChip *mybuff = segment.find<CSharedChip>("mmDaqSharedChips").first) {
// ////      
// //      bipc::scoped_lock<bipc::interprocess_mutex> lock(mybuff->m_mutex);
// //      mybuff->clear();
// //      std::vector<SrsChipPtr> chips = daqconfig->locate_srs_chips();
// //      // create shared chips in CSharedBuffer
// //      Q_FOREACH(SrsChipPtr chip, chips) {
// //         mybuff->add_chip(chip, alloc_inst);
// //      }
// //      std::cout << "CSharedBufferHandler::configure():mmDaqSharedEvents #chips = " << mybuff->m_chips->size() << std::endl;
// //      mybuff->print();
// //      
// //      
// //   }
   
// }


void CSharedBufferHandler::clear()
{
//   bipc::managed_shared_memory segment(bipc::open_only, "mmDaqSharedEBSegment");
   
//   segment.destroy<CSharedChip>("mmDaqSharedChips");
   
//   CSharedEBBuffer *mybuff = segment.find<CSharedEBBuffer>("mmDaqSharedChips").first;
//   if (segment.find<CSharedEBBuffer>("mmDaqSharedEvents").first) {
//      bipc::scoped_lock<bipc::interprocess_mutex> lock(mybuff->m_mutex);
//      mybuff->clear();
//      m_max_event_depth = mybuff->max_event_depth();
//   }

//   remove mmDaqSharedChips
   
}

size_t CSharedBufferHandler::max_event_depth() const
{
   return m_max_event_depth;
}

