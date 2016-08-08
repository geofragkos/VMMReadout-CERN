#ifndef SHAREDMEMORYWRITER_H
#define SHAREDMEMORYWRITER_H

#include <iostream>
#include <vector>
#include <random>

//boost
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

//mmdaq
#include "createevents.h"
#include "AsioService.h"

//typedefs for sending to shared memory

//uint16
typedef boost::interprocess::allocator<int16_t, boost::interprocess::managed_shared_memory::segment_manager> ShmemInt16Allocator;
typedef boost::interprocess::vector<int16_t, ShmemInt16Allocator> ShmemInt16Vector;

//map is [chipId] = (raw vector int16)
typedef uint32_t ShmemRawMapKeyType;
typedef ShmemInt16Vector ShmemRawMapMappedType;
typedef std::pair<const ShmemRawMapKeyType, ShmemRawMapMappedType> ShmemRawMapValueType;

//this allocator will allow to place containers in managed shared memory
// segments
typedef boost::interprocess::allocator<ShmemRawMapValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmemRawMapAllocator;

//alias a map of ints that uses the previous STL-like allocator
// note: the third parameter argument is the ordering function
// of the map used to compare the keys
typedef boost::interprocess::map<ShmemRawMapKeyType, ShmemRawMapMappedType, std::less<ShmemRawMapKeyType>, ShmemRawMapAllocator> ShmemRawMap;


//strips
typedef boost::interprocess::managed_shared_memory::segment_manager ShmemSegmentManagerType;

typedef boost::interprocess::allocator<char, ShmemSegmentManagerType> ShmemCharAllocator;
typedef boost::interprocess::basic_string<char, std::char_traits<char>, ShmemCharAllocator> ShmemCharString;
typedef boost::interprocess::allocator<ShmemCharString, ShmemSegmentManagerType> ShmemCharVectorAllocator;
typedef boost::interprocess::vector<ShmemCharString, ShmemCharVectorAllocator> ShmemCharStringVector;

class SharedMemoryWriter {
    public :
        SharedMemoryWriter();
        //virtual ~SharedMemoryWriter();
        void initializeSharedMemory();
        void clearSharedMemory();

        void publishEvent(std::vector<std::string> event);
        void publishEvent();

        uint64_t n_event_counter;

        void run();
        void stopThread();

    private :
        bool m_stop;

        void publish_event_info();
        void publish_raw_data();
        void publish_event_strips(std::vector<std::string> event);

        uint64_t* m_shm_event_id;
        int n_event_counterNum; //??

        ShmemCharStringVector*  m_shm_event_strings;

        boost::interprocess::managed_shared_memory*     m_shm_manager;
        boost::interprocess::named_condition*           m_shm_condition;

        CreateEvents* m_createEvents;
        CAsioService* m_service;
}; //class





#endif
