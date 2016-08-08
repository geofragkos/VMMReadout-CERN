//
//  RootWriter.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 25/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "RootWriter.h"
//#include "IpcManager.h"
#include "SharedChip.h"
//#include "RootTreeFillerApvRaw.h"
//#include "RootTreeFillerApvPeak.h"
//#include "RootTreeFillerBnlMiniCard.h"


#include "SharedEBBuffer.h"

#include "RootThread.h"
//#include "AsioService.h"

#include "DaqServer.h"
#include "DaqServerConfig.h"
//#include "EventTypes.h"
//#include "EventBuilder.h"
//#include "EventBuilderOutputBuffer.h"
#include "MMEvent.h"

#include <boost/foreach.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <cassert>
#include <iostream>
#include <list>

using namespace online::display;


//TODO:
// die if no mmdaq3 shared data exist


CRootWriter::CRootWriter() :
m_shmem_segment(0),
m_mutex(),
m_cond(&m_mutex),
m_keep_running(true),
m_filler_apv_raw(0),
m_filler_bnl_mini(0),
m_rootfile(0),
m_testtree(0),
m_common_branches(),
m_tb_evt(0),
m_tb_time_s(0),
m_tb_time_us(0),
m_tb_srs_time_stamp(0)
{
   bipc::message_queue::remove("mmdaq3Queue_mmdaq3RootWriter");
   
}


CRootWriter::~CRootWriter()
{
//   m_service->stopping(true);
//   m_service->stop();
   boost::interprocess::managed_shared_memory* shmptr = 
   static_cast<bipc::managed_shared_memory*>(m_shmem_segment);
   delete shmptr;
   m_shmem_segment = 0;
   
   
   delete m_filler_apv_raw;
   m_filler_apv_raw =0;
   delete m_filler_bnl_mini;
   m_filler_bnl_mini = 0;
   
   bipc::message_queue::remove("mmdaq3Queue_mmdaq3RootWriter");
   
   std::cout << " +++ CRootWriter ends +++ " << std::endl;
}


//void CRootWriter::configure(CDaqServerConfig* daqconfig)
//{
//   handle_stop();
   
//   // fillers are on chips (1 per chip type) - the chip class have a static filler
//   // filler needs to create tree in the roto file...
   
//   // m_filler_apv_raw
//   // m_filler_apv_peak
//   // az
//   // bnl
   
//}


//void CRootWriter::create_file(const std::string& filename)
//{
//   assert(m_rootfile==0);
   
////   CRootThreadScopedLock lock;
   
//   m_rootfile = new TFile(filename.c_str(), "RECREATE");
//   if (!m_rootfile->IsOpen()) {
//      std::stringstream ss;
//      ss << "Error: CRootWriter::create_file): Can't open " << filename << " for writing!";
//      throw std::runtime_error(ss.str());
//   }
//	m_rootfile->cd();
   
      
   
//}



void CRootWriter::main_loop()
{
   bool error = 0;

   bipc::message_queue::remove("mmdaq3Queue_mmdaq3RootWriter");

   
   //set up message pipes
   bipc::message_queue msg_queue(bipc::open_or_create, 
                                 "mmdaq3Queue_mmdaq3RootWriter", 
                                 100,
                                 sizeof(CIpcManager::IpcMessageType));
   
   try {
      //ipc message queue commands max msg size 100 bytes
      //set up message pipes
      std::cout << "->->-> CRootWriter::main_loop" << std::endl;
//      bipc::message_queue msg_queue(bipc::open_only, 
//                                    "mmdaq3Queue_mmdaq3RootWriter");
            
      while (!error && m_keep_running) {
         std::cout << ":" << std::endl;
         unsigned int priority = 0;
         size_t recvd_size = 0;
         CIpcManager::IpcMessageType msq = CIpcManager::ipcMsgBadMessage;
         
         boost::system_time timeout = boost::get_system_time() + boost::posix_time::seconds(1);
         bool newmsg = msg_queue.try_receive(&msq, sizeof(msq), recvd_size, priority);
//         handle_command(msq);
         
         //bool newmsg = msg_queue.timed_receive(&cmd, sizeof(cmd), recvd_size, priority, timeout);
         if (newmsg) {
            handle_command(msq);
         }
         else {
            std::cout << "r" << std::endl;
         }
         boost::thread::sleep(timeout);
      }
      
   } catch (bipc::interprocess_exception& e) {
      std::cout << "CRootWriter::main_loop() error : " << e.what() << std::endl;
      error = true;
   }
   std::cout << " CRootWriter::main_loop EXITS" << std::endl;

}


void CRootWriter::handle_command(CIpcManager::IpcMessageType msq)
{
std::cout << "CRootWriter::handle_command " << msq << std::endl;
   
   switch (msq) {
      case CIpcManager::ipcMsgNewData:
         handle_new_data();
         break;   
      case CIpcManager::ipcMsgTerminate:
         handle_terminate();
         break;
      case CIpcManager::ipcMsgConfigure:
         handle_configure();
         break;
      case CIpcManager::ipcMsgStart:
         handle_start();
         break;
      case CIpcManager::ipcMsgStop:
         handle_stop();
         break;
      default:
         break;
   }
}


void CRootWriter::handle_new_data()
{
   std::cout << "CRootWriter::handle_new_data() " << std::endl;
   bipc::managed_shared_memory* shmptr = static_cast<bipc::managed_shared_memory*>(m_shmem_segment);
   std::pair<CSharedEBBuffer*, size_t> res = shmptr->find<CSharedEBBuffer> ("mmDaqSharedEvents");
   res.first->print();
   
   //         CRootThreadScopedLock rootlock;
   //         std::for_each(mmevents.begin(), mmevents.end(), boost::bind(&CMMEvent::fill_writer, _1));
}


void CRootWriter::handle_terminate()
{
   std::cout << "CRootWriter::handle_terminate() " << std::endl;

   handle_stop();
   m_keep_running = false;
   exit(EXIT_SUCCESS);
}

void CRootWriter::handle_configure()
{
   std::cout << "CRootWriter::handle_configure() " << std::endl;

}

void CRootWriter::handle_start()
{
   std::cout << "CRootWriter::handle_start() " << std::endl;
   //Create shared memory
   bipc::managed_shared_memory* shmptr = new bipc::managed_shared_memory(bipc::open_only, "mmDaqSharedEBSegment");
   m_shmem_segment = shmptr;
   //Find the object
   std::pair<CSharedEBBuffer*, size_t> res = shmptr->find<CSharedEBBuffer> ("mmDaqSharedEvents");
   //Length should be 1
   if(res.second == 1) {
      std::cout << "*** CRootWriter::handle_start() OK while finding mmDaqSharedEvents - found: r.f=" << res.first << "r.s=" << res.second << std::endl;
   }
   else {
      std::cout << "*** CRootWriter::handle_start() error while finding mmDaqSharedEvents - not found: res.second = " << res.second << std::endl;
   }
   
   std::cout << "=== CRootWriter::handle_start() shared data ===" << std::endl;
   
   res.first->print();
   
   //new root file
//   create_file("__daqtest.root");
   
   //////////////////
   
   //cd to file
//   m_rootfile->cd();
   
      std::vector<CRootTreeFiller*> fillers;
   
   //loop over chips to create fillers
   const ShmemChipVector& chips = res.first->m_chips;
   BOOST_FOREACH(const CSharedChip& chip, chips) {
      if (chip.get_chip_type() == CSharedChip::shChipTypeAPV25 ) {
         //create_tree
//         if (!m_filler_apv_raw) {
//            m_filler_apv_raw = new CRootTreeFillerApvRaw(m_rootfile);
//            fillers.push_back(m_filler_apv_raw);
//         }
      }
      else if (chip.get_chip_type() == CSharedChip::shChipTypeVMM1) {
       //create tree 
//         if(!m_filler_bnl_mini) {
//            m_filler_bnl_mini = new CRootTreeFillerBnlMiniCard(m_rootfile);
//            fillers.push_back(m_filler_bnl_mini);
//         }
      }
      else {
         std::cout << "CRootWriter::handle_start() unknown chip type " << chip.get_chip_type() << std::endl;
      }
      
   }
   
//   int a = 40;
//   std::vector<int> v(3,7);
//   std::vector<int>* pv = &v;
   
//   TTree* tr = new TTree("testtree","testTREE");
//   if (!tr) {
//      throw std::runtime_error("CRoot::create_tree runtime error ");
//   }
//   create_common_branches(tr);
//   tr->Fill();

//   std::vector<SrsChipPtr>  test = daq_config.locate_srs_chips();
//   BOOST_FOREACH(SrsChipPtr chip, test) {
//      //get chip types fillers
//      chip->handle_run_start(m_rootfile);
//      fillers.push_back(chip->get_root_tree_filler());
//   }
//   MakeElementsUnique(fillers);
//   m_rootfile->cd();

   
   
//   BOOST_FOREACH(CRootTreeFiller* filler, fillers) {
      //make trees
//      TTree* t = filler->create_tree();
//      t->Branch("test",&a);
//      filler->fill();
//   }

}



////void CRootWriter::create_common_branches(TTree* atree)
////{
   
////   std::vector<UInt_t>*       pfec = &m_fec;
////   std::vector<UInt_t>*       pchip = &m_chip;
////   std::vector<UInt_t>*       pchan = &m_channel;
////   std::vector<std::string>*  pchamb = &m_chamber; //mm_id
////   std::vector<int>*          player = &m_layer;    //plane(layer) id number
////   std::vector<char>*         pread  = &m_readout;  //mm_readout
////   std::vector<int>*          pstrip = &m_strip;    //mm_strip
   
//   m_common_branches.push_back(atree->Branch("evt",          &m_tb_evt));
//   m_common_branches.push_back(atree->Branch("daqSec",       &m_tb_time_s));
//   m_common_branches.push_back(atree->Branch("daqMicroSec",  &m_tb_time_us));
//   m_common_branches.push_back(atree->Branch("srsTimeStamp", &m_tb_srs_time_stamp));
////   m_common_branches.push_back(atree->Branch("srsFec",    &pfec));
////   m_common_branches.push_back(atree->Branch("srsChip",   &pchip));
////   m_common_branches.push_back(atree->Branch("srsChan",   &pchan));
////   m_common_branches.push_back(atree->Branch("mmChamber", &pchamb));
////   m_common_branches.push_back(atree->Branch("mmLayer",   &player));
////   m_common_branches.push_back(atree->Branch("mmReadout", &pread));
////   m_common_branches.push_back(atree->Branch("mmStrip",   &pstrip));
////
//   size_t ii = 0;
//   for (std::vector<TBranch*>::iterator it = m_common_branches.begin(); it != m_common_branches.end(); ++it, ++ii) {
//      if (!*it) {
//         std::stringstream ss;
//         ss << "Error: CRootTreeFiller::create_common_branches(): branch " << ii << " is NULL" ;
//         throw std::runtime_error(ss.str());
//      }
//   }

//}


void CRootWriter::handle_stop()
{
//   std::cout << "CRootWriter::handle_stop() " << std::endl;
   
   boost::interprocess::managed_shared_memory* shmptr = 
   static_cast<bipc::managed_shared_memory*>(m_shmem_segment);
   delete shmptr;
   m_shmem_segment = 0;
   
//   m_rootfile->Write();
//   m_rootfile->Close();
//   m_rootfile = 0;
}




/**
 set event id data once per event
 */


///**
// put channel data to an APV tree filler
// */
//void CRootWriter::prefill_channel_apv_raw(const CSrsChannelId& chid, const CSrsChannelData& data, bool data_format_peak)
//{
//   CRootThreadScopedLock lock;
//   if (data_format_peak) {
//      m_filler_apv_peak->prefill_channel(chid, data);
//   }else {
//      m_filler_apv_raw->prefill_channel(chid, data);
//   }  
//   
//}
//
///**
// put channel data to an VMM tree filler
// */
//void CRootWriter::prefill_channel_vmm_arizona(const CSrsChannelId& chid, const CSrsChannelData& data)
//{
//   CRootThreadScopedLock lock;
//   //m_filler_vmm_arizona->prefill_channel(chid, data);
//}
