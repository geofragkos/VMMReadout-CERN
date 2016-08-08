#include "sharedmemorywriter.h"

//boost
#include <boost/bind.hpp>


////////////////////////////////////////////////////////////////////////
// SHAREDMEMORYWRITER
////////////////////////////////////////////////////////////////////////
SharedMemoryWriter::SharedMemoryWriter() :
    n_event_counter(0),
    m_stop(false),
    m_shm_event_id(0),
    n_event_counterNum(0),
    m_shm_event_strings(0),
    m_shm_manager(0),
    m_shm_condition(0),
    m_createEvents(0),
    m_service(0)
{
}

void SharedMemoryWriter::initializeSharedMemory()
{

    boost::interprocess::shared_memory_object::remove("mmDaqSharedMemory");
    boost::interprocess::named_condition::remove("mmDaqSharedCondition");

    m_shm_manager = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
                        "mmDaqSharedMemory", (1048576 * 10) );
    m_shm_condition = new boost::interprocess::named_condition(boost::interprocess::open_or_create,
                        "mmDaqSharedCondition" );
    m_shm_event_id = m_shm_manager->find_or_construct< uint64_t >( "mmDaqSharedEventNumber" )( 0 );
    m_shm_event_strings = m_shm_manager->construct<ShmemCharStringVector>
                                                ("mmDaqSharedEventData")
                                                (m_shm_manager->get_allocator<ShmemCharVectorAllocator>());

}

void SharedMemoryWriter::clearSharedMemory()
{
    delete m_shm_manager;
    delete m_shm_condition;

    this->initializeSharedMemory();
}

void SharedMemoryWriter::run()
{
    cout << " *** *** *** *** *** *** *** *** *** *** *** *** *** ***   " << endl;
    cout << "SharedMemoryWriter::run    Starting event publishing thread" << endl;
    cout << " *** *** *** *** *** *** *** *** *** *** *** *** *** ***   " << endl;

    m_service->reset();
    m_service->stopping(false);
    CAsioService::MainLoopFunction f = boost::bind(&SharedMemoryWriter::publishEvent, this);
    //auto f = boost::bind(&SharedMemoryWriter::publishEvent, this,_1);
    m_service->post(f);
}

void SharedMemoryWriter::stopThread()
{
    boost::interprocess::shared_memory_object::remove("mmDaqSharedMemory");
    boost::interprocess::named_condition::remove("mmDaqSharedCondition");

    m_service->stopping(true);
    m_service->stop();
}

void SharedMemoryWriter::publishEvent()
{
    // dummy function
    return;
}

void SharedMemoryWriter::publishEvent(vector<string> event)
{
    int counter = 0;
  //  cout << "-------------------------------------" << endl;
  //  cout << "  shmem manager size: " << m_shm_manager->get_size() << endl;
  //  cout << "  shmem manager free: " << m_shm_manager->get_free_memory() << endl;
  //  cout << "-------------------------------------" << endl;

    

//    // initialize the lock on the shared memory
//    static boost::interprocess::named_mutex shm_mutex(boost::interprocess::open_or_create, "mmDaqSharedMutex");
//    static boost::interprocess::scoped_lock< boost::interprocess::named_mutex > lock(shm_mutex,
//                                                    boost::interprocess::defer_lock_type());
//
//    try {
//        boost::posix_time::ptime timeout(boost::posix_time::second_clock::universal_time() +
//                                            boost::posix_time::seconds(50));
//        if(!lock.timed_lock(timeout)) {
//            cout << "SharedMemoryWriter::publishEvent    Lock timed out (fail)" << endl;
//            #warning what is this for?
//        }

        publish_event_info();
        //publish_raw_data();
        publish_event_strips(event);

//        m_shm_condition->notify_all();
//
//        if(lock) {
//            lock.unlock();
//            cout << "SharedMemoryWriter::publishEvent    shm_mutex unlocked" << endl;
//            #warning is this needed?
//        }
//        counter++;
//    } //
//    catch(boost::interprocess::interprocess_exception &e) {
//        cout << "SharedMemoryWriter::publishEvent    ERROR: " << e.what() << endl;
//    }

}

void SharedMemoryWriter::publish_event_info()
{
    *m_shm_event_id = n_event_counterNum;
  //  cout << "SharedMemoryWriter::publish_event_info    event number: " << n_event_counterNum
  //              << "  shm_event_id: " << *m_shm_event_id << endl;
    n_event_counterNum++;
}

void SharedMemoryWriter::publish_raw_data()
{
    #warning pretty sure this function doesn't do anything

    ShmemRawMapAllocator shm_raw_allocator(m_shm_manager->get_segment_manager());
    ShmemRawMap* shm_raw_data = m_shm_manager->find_or_construct< ShmemRawMap >("mmDaqSharedRawData")
                                        (std::less<uint32_t>(), shm_raw_allocator);

    int size = 50;
    vector<int16_t> vec(size);

    uint32_t chip_id = 256;
    std::pair<ShmemRawMap::iterator, bool> emp =
        shm_raw_data->emplace(ShmemRawMapValueType(chip_id, ShmemInt16Vector(m_shm_manager->get_segment_manager() ) ));
        emp.first->second.assign(vec.begin(), vec.end());
    
}

void SharedMemoryWriter::publish_event_strips(vector<string> event)
{
    ShmemCharString local_string(m_shm_manager->get_allocator<ShmemCharAllocator>());
    int nev = 0;
//    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
//    cout << "SharedMemoryWriter::publish_event_strips  strips size: " << m_shm_event_strings->size() << endl;
//    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    if(m_shm_event_strings->size()>1000){
         m_shm_event_strings->clear();
    }
    for(auto ev : event) {
        local_string = ev.c_str();
        //cout << "SharedMmeoryWriter::publish_event_strips: strip[" << nev << "] : " << local_string << endl;
        m_shm_event_strings->push_back(local_string);
        nev++;
    } // ev
    return;
}






