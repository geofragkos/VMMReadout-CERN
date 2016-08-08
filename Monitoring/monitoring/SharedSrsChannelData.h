//
//  SharedSrsChannelData.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SharedSrsChannelData_h
#define online_display_SharedSrsChannelData_h

#include "SharedTypes.h"

#ifndef Q_MOC_RUN

#include <boost/interprocess/managed_shared_memory.hpp>
#endif

#include <cstdlib> //std::system
#include <vector>
#include <utility>

namespace online {
   namespace display {
      
//      namespace bipc = boost::interprocess;

      
      
      class CSrsChannelData;
      
      class CSharedSrsChannelData
      {
         
      public:
         //Since void_allocator is convertible to any other allocator<T>, we can simplify
         //the initialization taking just one allocator for all inner containers.  
         
         CSharedSrsChannelData(const CSrsChannelData& data, 
                               const ShmemVoidAllocator& alloc_inst);
         
         CSharedSrsChannelData(const ShmemVoidAllocator& alloc_inst) 
         : m_time(0.0), m_charge(0.0), m_user_data(alloc_inst) {
            
         };
         
         
         CSharedSrsChannelData(const CSharedSrsChannelData& rhs):
         m_time(rhs.m_time), m_charge(rhs.m_charge), m_user_data(rhs.m_user_data)
         { }
         
         CSharedSrsChannelData& operator= (const CSharedSrsChannelData& rhs)
         {
            if (&rhs != this) {
               m_time = rhs.m_time;
               m_charge = rhs.m_charge;
               m_user_data = rhs.m_user_data;
            }
            return *this;
         }
         
         
         //      CSharedSrsChannelData();
         virtual ~CSharedSrsChannelData();
         
      protected:
         double m_time;
         double m_charge;
         ShmemDblVector m_user_data;
      };
      
      typedef bipc::allocator<CSharedSrsChannelData, ShmemSegmentManagerType>    ShmemChannelDataAllocator;
      typedef bipc::vector<CSharedSrsChannelData, ShmemChannelDataAllocator>     ShmemChannelDataVector;
      
      
      
   }
} // namespace

#endif
