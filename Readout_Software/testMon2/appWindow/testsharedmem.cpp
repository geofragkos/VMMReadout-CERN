#include "testsharedmem.h"

#include <iostream>
#include <vector>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"


testSharedMemTool::testSharedMemTool() :
    previous_size(0),
    m_shm_eventNumber(0),
    n_event_counter(0),
    m_shm_eventinfo_vector(0),
    m_shm_manager(0),
    m_shm_condition(0)
{
}


//// initialize the shared memory objects
void testSharedMemTool::initializeSharedMemory()
{
    boost::interprocess::shared_memory_object::remove("mmDaqSharedMemory");
    boost::interprocess::named_condition::remove("mmDaqSharedCondition");

    if(!m_shm_manager)
        m_shm_manager = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
                                "mmDaqSharedMemory",
                                (1048576 * 10) );

    if(!m_shm_condition)
        m_shm_condition = new boost::interprocess::named_condition(boost::interprocess::open_or_create,
                                "mmDaqSharedCondition");
    m_shm_eventNumber = m_shm_manager->find_or_construct<uint64_t>("mmDaqSharedEventNumber")(0);

    m_shm_eventinfo_vector = m_shm_manager->construct<ShmemCharStringVector>
                                        ("mmDaqSharedEventData")
                                        (m_shm_manager->get_allocator<ShmemCharVectorAllocator>());

}


void testSharedMemTool::sendData()
{
    std::cout << "testSharedMemTool::sendData" << std::endl;

    boost::interprocess::named_mutex shm_mutex(boost::interprocess::open_or_create, "mmDaqSharedMutex");
    boost::interprocess::scoped_lock< boost::interprocess::named_mutex > lock(shm_mutex,
                               boost::interprocess::defer_lock_type());
            //    m_shm_condition->notify_all();

    int n = 0;
    while(n<1) {
        try {
            std::cout << "in try" << std::endl;

            boost::posix_time::ptime timeout(boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(5));
          //  if(m_shm_condition->timed_wait(lock, timeout)) {
            if(!lock.timed_lock(timeout)) {
                std::cout << " *** DATAWRITER : LOCK TIMED OUT *** " << std::endl;
            }
            else {
                std::cout << "in else" << std::endl;
                sendEventNumber();
                sendEventInfo();

                m_shm_condition->notify_all();
              //  m_shm_condition->wait(lock);
            }
            //    m_shm_condition->wait(lock);

            if(lock) {
                std::cout << " *** DATAWRITER : unlocking *** " << std::endl;
                lock.unlock();
                std::cout << " *** DATAWRITER : SHM_MUTEX UNLCOKED *** " << std::endl;
            }
        } // try
        catch(boost::interprocess::interprocess_exception &e) {
            std::cout << " *** DATAWRITER : ERROR :: " << e.what() << std::endl;
        }
        n++;
    } // while

    //int n=0;
    //while(n<1000) {
    //    sendEventNumber();
    //    sendEventInfo();
    //    n++;
    //}
    return;
}

void testSharedMemTool::sendEventNumber()
{
    *m_shm_eventNumber = n_event_counter;
    n_event_counter++;
}

void testSharedMemTool::sendEventInfo()
{
//    int n = 0;
    std::string x = "10 T6 0 0 X 45 1 20 60 40 40 40 40";
    std::vector<std::string> out;
    for(int i = 0; i < 25; i++) {
        out.push_back(x);
    }
    int data_size;
    int real_size;
    int user_size;
//    while(n<1000){
       // std::cout << "testSharedMemTool::sendEventInfo    sending event " << n << std::endl;
        ShmemCharString local_string(m_shm_manager->get_allocator<ShmemCharAllocator>());
        if(previous_size >= m_shm_eventinfo_vector->size())
            std::cout << "!! SHMEM VECTOR SIZE REDUCED !!" << std::endl;
        previous_size = m_shm_eventinfo_vector->size();
        std::cout << " ********** SHMEM VECTOR SIZE: " << m_shm_eventinfo_vector->size() << " ********* " << std::endl;
        
//214     //    int dat_size = m_shm_manager.get_size();
//215     //    int real_size = m_shm_manager.get_free_memory();
//216     //   int user_size =  m_shm_manager.get_user_size();
//217
//218     //   qDebug("MEMORY SIZE = %i",dat_size);
//219     //   qDebug("real_size SIZE = %i",real_size);
//220     //   qDebug("user_size SIZE = %i",user_size);
        for(auto ev : out) {
            local_string = ev.c_str();
            m_shm_eventinfo_vector->push_back(local_string);
        } // ev
        //data_size = m_shm_manager->get_size();
        //real_size = m_shm_manager->get_free_memory();
        //user_size = m_shm_manager->get_user_size();
        //std::cout << "data size: " << data_size << "  real size: " << real_size << "   user size: " << user_size << std::endl;
   //     n++;
   // }
    out.clear();
    return;

}
