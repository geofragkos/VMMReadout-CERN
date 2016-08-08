#ifndef TESTSHAREDMEM_H
#define TESTSHAREDMEM_H

//boost
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

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


class testSharedMemTool {
    public :
        testSharedMemTool();
        void initializeSharedMemory();
        void sendData();
        void sendEventNumber();
        void sendEventInfo();

        int previous_size;


    private :
        uint64_t* m_shm_eventNumber;
        uint64_t n_event_counter;
        
        ShmemCharStringVector* m_shm_eventinfo_vector;

        boost::interprocess::managed_shared_memory* m_shm_manager;
        boost::interprocess::named_condition*       m_shm_condition;

}; //class
#endif
