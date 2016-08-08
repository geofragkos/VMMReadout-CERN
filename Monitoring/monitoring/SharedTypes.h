//
//  SharedTypes.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SharedTypes_h
#define online_display_SharedTypes_h

#include "SrsChannelId.h"
//#include "SharedSrsChannelData.h"

#ifndef Q_MOC_RUN

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#endif

//#include <string>
#include <cstdlib> //std::system
//#include <vector>


namespace online {
   namespace display {

//      class CSrsChannelId;
//      class CSharedSrsChannelData;


      typedef enum {
         shChipTypeInvalid,
         shChipTypeAPV25,
         shChipTypeAPV25Raw,
         shChipTypeAPV25ZsRaw,
         shChipTypeAPV25ZsPeak,
         shChipTypeVMM1,
         shChipTypeVMM1BNLMiniDaq
      } SharedChipType;


      namespace bipc = boost::interprocess;
      typedef bipc::managed_shared_memory::segment_manager ShmemSegmentManagerType;

      //1) Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
      //This allocator will allow placing containers in the segment

      //2) Alias a vector that uses the previous STL-like allocator so that allocates
      //its values from the segment


      //Typedefs of allocators and containers
      typedef bipc::allocator<void, ShmemSegmentManagerType>                        ShmemVoidAllocator;


      //string, vector of string
      typedef bipc::allocator<char, ShmemSegmentManagerType>                        ShmemCharAllocator;
      typedef bipc::basic_string<char, std::char_traits<char>, ShmemCharAllocator>  ShmemCharString;

      typedef bipc::allocator<ShmemCharString, ShmemSegmentManagerType>             ShmemCharVectorAllocator;
      typedef bipc::vector<ShmemCharString, ShmemCharVectorAllocator>               ShmemCharStringVector;

      //uint32
      typedef bipc::allocator<uint32_t, ShmemSegmentManagerType>  ShmemUint32Allocator;
      typedef bipc::vector<uint32_t, ShmemUint32Allocator>        ShmemUint32Vector;

      //double vector
      typedef bipc::allocator<double, ShmemSegmentManagerType>    ShmemDblAllocator;
      typedef bipc::vector<double, ShmemDblAllocator>             ShmemDblVector;

      //vector of vectors double
      typedef bipc::allocator<ShmemDblVector, ShmemSegmentManagerType>  ShmemDblVectorAllocator;
      typedef bipc::vector<ShmemDblVector, ShmemDblVectorAllocator>     ShmemDblVectorVector;


      //enum SharedChiptype
      typedef bipc::allocator<SharedChipType, ShmemSegmentManagerType>  ShmemChipTypeAllocator;
      typedef bipc::vector<SharedChipType, ShmemChipTypeAllocator>      ShmemChipTypeVector;


      typedef boost::interprocess::interprocess_mutex ShmemMutexType;
      typedef boost::interprocess::scoped_lock< ShmemMutexType > ShmemScopedLock;

      //to be obsolete
      typedef bipc::allocator<CSrsChannelId, ShmemSegmentManagerType>   ShmemChannelIdAllocator;
      typedef bipc::vector<CSrsChannelId, ShmemChannelIdAllocator>      ShmemChannelIdVec;






//      typedef bipc::allocator<double, ShmemSegmentManagerType>  ShmemUserDataAllocator;
//      typedef bipc::vector<double, ShmemUserDataAllocator>      ShmemUserDataVec;


//      typedef allocator<int, segment_manager_t>                            int_allocator;
//      typedef vector<int, int_allocator>                                   int_vector;
//      typedef allocator<int_vector, segment_manager_t>                     int_vector_allocator;
//      typedef vector<int_vector, int_vector_allocator>                     int_vector_vector;


   }
}

#endif
