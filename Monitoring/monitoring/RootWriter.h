//
//  RootWriter.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 25/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_RootWriter_h
#define online_display_RootWriter_h


#ifndef __CINT__
//#include "AsioService.h"
//#include "EventTypes.h"
#endif

//#include "IpcManager.h"

#include <TMutex.h>
#include <TCondition.h>


#include <vector>
#include <string>
#include <algorithm>


namespace online {
   namespace display {
      
      class CDaqServer;
      class CDaqServerConfig;
      
//      class CRootTreeFillerApvRaw;
//      class CRootTreeFillerApvPeak;
//      class CRootTreeFillerBnlMiniCard;
      class CSrsChannelId;
//      class CSrsChannelData;
      
//      class CAsioService;
      
      /**
       handles root file writing and data filling
       (no boost shared pointers here! unless hidden from root interpreter)
       */
      class CRootWriter 
      {
         
         
      public:
         CRootWriter();
         virtual ~CRootWriter();
         
         void main_loop();
         
         void configure(CDaqServerConfig* daqconfig);
         
                  
      protected:
//         void create_file(const std::string& filename);
         
//         void handle_command(CIpcManager::IpcMessageType msq);

         void handle_new_data();
         void handle_terminate();
         void handle_configure();
         void handle_start();
         void handle_stop();
         

         ///mutex for buffer
//         boost::mutex m_root_mutex;

         boost::interprocess::managed_shared_memory* m_shmem_segment;
         
         TMutex m_mutex;  
         TCondition m_cond;
         bool m_keep_running;
//         CAsioService* m_service;

         
//         CDaqServer& m_daqserver;
//         CRootTreeFillerApvRaw* m_filler_apv_raw;
//         CRootTreeFillerBnlMiniCard* m_filler_bnl_mini;
//         CRootTreeFillerApvPeak* m_filler_apv_peak;
         
         
      };
      
      template <class T>
      size_t MakeElementsUnique(T& container )
      {
         std::sort(container.begin(), container.end());
         typename T::iterator iter = unique(container.begin(), container.end());
         container.resize(iter - container.begin());
         return container.size();
      }
      
      
   }
} // namespace

#endif
