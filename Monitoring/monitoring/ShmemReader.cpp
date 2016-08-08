//
//  ShmemReader.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "ShmemReader.h"
#include "SrsChipId.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include <iostream>
#include <stdexcept>

using namespace online::display;

namespace bpt = boost::property_tree;
//namespace bip = boost::interprocess;
namespace bptm = boost::posix_time;

int eventDisplayed_lastdisplayed=0;

//aikoulou: just trying to debug the code. will fake data.
bool FAKEDATA=false;
int FAKE_EVENT_NO=0;


ShmemReader::ShmemReader(std::vector<uint32_t> apvList, std::vector<std::string> apvChips,
                         std::vector<boost::shared_ptr<online::display::CSrsChip> > chipvec,
                         std::vector<std::pair<boost::shared_ptr<online::display::CDetChamber>,
                         std::vector<boost::shared_ptr<online::display::CDetReadout> > > > chamberElements,
                         MainWindow *window,
                         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingChip1dElements,
                         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingChip2dElements,
                         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingReadout1dElements,
                         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingReadout2dElements) :
    m_shm_shared_data(0), shmem(), shCond(), mainWindow(window), mainDrawer(0), service(new online::display::CAsioService(1)),
    m_shm_manager(bipc::open_only, "mmDaqSharedMemory") ,
    m_shm_condition(bipc::open_only, "mmDaqSharedCondition"), terminate(false),
    readMutex(), dataLine(), apvChipsList(), apvChipIdList(), configuredChamberElements(chamberElements), configuredChipvec(chipvec), isProcessing(false), stripDataEvent(0), rawEvent(0), realEvent(false)
{
    //aikoulou: debug
    //    std::cout << "call ShmemReader constractor" << std::endl;
    eventDisplayed = 0; // angelos
    if(m_shm_manager.check_sanity())    {
        //terminate=false;
        fillApvChipsList(apvList,apvChips);
        //        connect_shared_memory();		ANGELOS
        mainDrawer = new DisplayDrawer(mainWindow);

        qRegisterMetaType< QString >("QVector<int>");
        qRegisterMetaType< QVector<int> >("QVector<int>");
        qRegisterMetaType< QVector<QString> >("QVector<QString>");
        qRegisterMetaType< QVector<std::pair<QString, QVector<int> > > >("QVector<std::pair<QString, QVector<int> > >");


        //connectors for passing data from shared memory to local copies
        connect(this,SIGNAL(readEvent()),this,SLOT(read_event_number()));
        connect(this,SIGNAL(readRaw()),this,SLOT(read_raw_data()));
        connect(this,SIGNAL(readStrip()),this,SLOT(read_event_strips()));


        //connectors for controlling filling and drawing of the histograms from the displayDrawer
        connect(this,SIGNAL(runNumberIs(QString)),
                mainDrawer->monitoringMainWindow->runNumberLabel_update,SLOT(setText(QString)),Qt::QueuedConnection);

        connect(this, SIGNAL(fillHistograms(QVector<std::pair<QString,QVector<int> > >,std::vector<std::string>,int)),
                mainDrawer,SLOT(NotifyFill(QVector<std::pair<QString,QVector<int> > >,std::vector<std::string>,int)),Qt::DirectConnection);
        connect(this, SIGNAL(drawHistograms()),
                mainDrawer,SLOT(NotifyDraw()),Qt::DirectConnection);

        online::display::CAsioService::MainLoopFunction f = boost::bind(&ShmemReader::handleSharedData, this);
        service->post(f);
    }else
        std::cout<<"Shared Memory not found.../n mmDaq-server probably not running"<<std::endl;
}

ShmemReader::~ShmemReader()
{
    service->stopping(true);
    service->stop();
    delete mainDrawer;
    mainDrawer = 0;
    //delete readMutex;
    //readMutex=0;
    //    delete m_shm_manager;
    //    m_shm_manager = 0;
    //    delete m_shm_condition;
    //    m_shm_condition=0;
}

void ShmemReader::connect_shared_memory()
{

    //connecting shared memory with daq
    try{
        boost::scoped_ptr<IpcShmemObjectType> obj(new
                                                  IpcShmemObjectType(bipc::open_only,
                                                                     "IPCPublisherSharedMemory" ,
                                                                     bipc::read_write));
        m_shm_object.swap(obj);


        //Map the whole shared memory in this process
        boost::scoped_ptr<IpcShmemMappedRegion> reg(new bipc::mapped_region(*m_shm_object,
                                                                            boost::interprocess::read_write ));

        m_shm_mapped_region.swap(reg);


        //Get the address of the mapped region
        void * addr = m_shm_mapped_region->get_address();

        //Construct the shared structure in memory
        m_shm_shared_data = static_cast<SPublisherIpcData*>(addr);

    }

    catch(...) {
        std::cout << "TODO : CShmemReader() connect_shared_memory() *** exception ***" << std::endl;
        m_shm_shared_data = 0;
    }

}

void ShmemReader::handleSharedData()
{

    ShmemNamedMutexType shm_mutex(bipc::open_only, "mmDaqSharedMutex");
    int time_ = 5;
    while(!service->stopping()) {
        {

            bptm::ptime timeout(bptm::second_clock::universal_time() + bptm::seconds(time_));

            ShmemNamedScopedLock lock(shm_mutex);
            if(m_shm_condition.timed_wait(lock, timeout)) {
                std::cout << " *** MONITOR : TIMED WAIT PASSED *** " << std::endl;
                read_event_number();
            }
            else {
                std::cout << " *** MONITOR : TIMEOUT FROM SCOPED LOCK *** " << std::endl;
                time_++;
                continue;
            }

        }

        if(realEvent && stripDataEvent.size()!=0) {
            std::cout << " *** MONITOR : EMIT FILL AND DRAW SIGNALS *** " << std::endl;
            emit fillHistograms(rawEvent, stripDataEvent, eventDisplayed);
            emit drawHistograms();
            //rawEvent.clear();
            //stripDataEvent.clear();
        } // have data

    } // service not stopped


    //ORIGINAL [BEGIN]
    //std::cout << "MONITORING RUNNING...Waiting for data on shared memory." << std::endl;

    ////    ShmemNamedMutexType shm_mutex(bipc::open_only, "mmDaqSharedMutex");

    ////new
    ////boost::interprocess::scoped_lock<bipc::interprocess_mutex> ipc_lock(m_shm_shared_data->mutex);

    //int timeInt=50;
    //while(!service->stopping() ) //&& m_shm_shared_data)
    //{
    //    {//lock
    //        //            ShmemNamedScopedLock lock(shm_mutex, bipc::defer_lock_type());
    //        //            if (!lock.try_lock()) {
    //        //               std::cout << "mmDaqSharedMutex failed lock" << std::endl;
    //        //               sleep(1);
    //        //               continue;
    //        //            }
    //        //            bptm::ptime timeout(bptm::second_clock::universal_time() + bptm::seconds(timeInt));
    //        //lock
    //        // ShmemNamedScopedLock lock(shm_mutex);
    //        //new
    //        //            if(!m_shm_shared_data->flag_new_event_data){
    //        //                     m_shm_shared_data->cond_event.wait(ipc_lock);
    //        //                     if(!isProcessing)   {
    //        //                                         read_event_number();
    //        //                                    }
    //        //            }

    //        //            if(m_shm_condition.timed_wait(lock, timeout))   {
    //        //usleep(1000*1000);
    //        //                std::cout << "notified be daq" <<std::endl;
    //        if(!isProcessing)   {
    //            read_event_number();
    //            //                }
    //        }
    //        //            else    {
    //        //                //timeout
    //        //                std::cout << "************************************************" <<std::endl;
    //        //                std::cout << "timeout from shmemNamedScopedLock" <<std::endl;
    //        //                std::cout << "no new data" <<std::endl;
    //        //                std::cout << "************************************************" <<std::endl;
    //        //                timeInt++;
    //        //                continue;
    //        //            }
    //    }//unlock

    //    if(realEvent/* && rawEvent.size()!=0*/ && stripDataEvent.size()!=0) {
    //        std::cout<<"Emitting fill signal from ShmemReader"<<std::endl;
    //        emit fillHistograms(rawEvent, stripDataEvent, eventDisplayed);
    //        emit drawHistograms();
    //        //aikoulou
//  //          rawEvent.clear();
    //        stripDataEvent.clear();
    //    }
    //}

    /////////////////////// ORIGINAL [END]

}

void ShmemReader::read_event_number()
{
    //Open managed shared memory
    std::pair<uint64_t*, size_t> res;
    qDebug(">>> read_event_number");
    res = m_shm_manager.find<uint64_t> ("mmDaqSharedEventNumber");

    //    int dat_size = m_shm_manager.get_size();
    //    int real_size = m_shm_manager.get_free_memory();
    //   int user_size =  m_shm_manager.get_user_size();

    //   qDebug("MEMORY SIZE = %i",dat_size);
    //   qDebug("real_size SIZE = %i",real_size);
    //   qDebug("user_size SIZE = %i",user_size);



    //std::cout << "RES int = " << res.second << std::endl;
//    qDebug(">>> going to grab event No");
    realEvent=false;
    if(res.second == 1)
    {
        //std::cout << "found : evid = " << *res.first << std::endl;
//        qDebug(">>> Here is event number");
        if(eventDisplayed!=eventDisplayed_lastdisplayed)
        {
            std::cout << "eventDisplayed -->" << eventDisplayed << "<--" << std::endl;    //angelos
            eventDisplayed_lastdisplayed=eventDisplayed;
        }
        //            std::cout << "*res.first -->" << *res.first << "<--" << std::endl;

        if((((int)(*res.first))!=eventDisplayed) && (((int)(*res.first))!=0))   { //angelos
            eventDisplayed = *res.first;
            realEvent=true;
            std::cout<<"Trigger # : "<<(int)*res.first<<std::endl;
            //emit drawHistograms();
            //                read_raw_data(); //den xriazete

            read_event_strips();
            //emit newEventReceived(QString::number(*res.first));
            //emit drawHistograms();
        }
    }
    else {
        // std::cout << "ERR: mmDaqSharedEventNumber count = " << res.second << std::endl;
    }

}



std::string ShmemReader::msg() {


    bool whatever=true;
    std::stringstream oss;
    oss << std::to_string(FAKE_EVENT_NO) << " ";
    if(rand()%2==0)
        /*chamber*/    oss << "Tmm2" << " ";
    else
        /*chamber*/    oss << "TL2" << " ";

    /*multilayer*/oss << "X" << " ";

    /*layer*/      oss<< "X" << " ";
    if(rand()%2==0)
    {
        /*readout*/    oss<< "X" << " ";
        whatever=true;
    }
    else
    {
        /*readout*/   oss << "Y" << " ";
        whatever=false;
    }
    //    /*strip no*/  oss << std::to_string(rnd_strip_no)<< " ";
    /*strip no*/  oss << std::to_string(  (rand()%100)/2  +   (rand())%100/2    +50   )<< " ";

    /*fec no*/     oss<< "2" << " ";



    if(whatever)
        /*chip string*/oss<< "APV.1.00" << " ";
    else
        /*chip string*/oss<< "APV.1.02" << " ";



    /*channel*/    oss<< std::to_string(0 + ( rand() % ( 64 - 0 + 1 ) )) << " ";

    //        /*charge!!!*/  oss<< std::to_string(100) << " ";
    /*charge!!!*/  oss<< std::to_string(0 + ( rand() % ( 2173 - 0 + 1 ) )) << " ";

    /* TIME */     oss<< std::to_string(0 + ( rand() % ( 750 - 0 + 1 ) ));
    //    /* TIME */     oss<< std::to_string(100) << " ";
    std::string var = oss.str();

    //    std::cout << (var);
    return var;
}

void ShmemReader::read_event_strips()
{

    std::pair<ShmemCharStringVector*, size_t> res_str;
    res_str = m_shm_manager.find<ShmemCharStringVector> ("mmDaqSharedEventData");
    qDebug() << "void ShmemReader::read_event_strips RES vec = " << res_str.second;
    std::string line_str;
    if(res_str.second == 1)
    {
        stripDataEvent.clear();
        ShmemCharStringVector* vec = res_str.first;
        //std::cout<<"number of strips with data : "<<vec->size()<<std::endl;
        std::cout<<"Strip data vector size is "<<vec->size()<<std::endl;
        for (size_t ii = 0; ii < vec->size(); ++ii) {
            //std::cout << "vec : " << vec->at(ii) << std::endl;
            line_str = vec->at(ii).c_str();
            qDebug("DATA RCVD >>++++++++++++++++++++>>>");
            std::cout<<"+++++++++++"<<line_str<<"+++++++++++"<<std::endl;
            stripDataEvent.push_back(line_str);
            qDebug("DATA RCVD >>>+++++++++++++++++++>>");
//            qDebug(QString(line_str));
            //handleBufferedEvent(QString(line_str.c_str()));
        }
        vec->erase (vec->begin(), vec->end());
        //        m_shm_manager.destroy<ShmemCharStringVector>("mmDaqSharedEventData"); //TODO remove data after read
        //       readoutHistoFiller();
        //       if(vec->size()!=0) emit drawHistograms();
        //emit runReadoutHistoFiller();

    }

    else {
        std::cout << "ERR: mmDaqSharedEventData resvec.second=" << res_str.second << std::endl;
    }
}

//function to pass list of apv chips from the configuration to the memory reader
void ShmemReader::fillApvChipsList(std::vector<uint32_t> apvList, std::vector<std::string> apvChips)
{
    if(apvChipsList.size()!=0) apvChipsList.clear();
    for(int iChip=0; iChip<apvList.size(); iChip++)
    {
        apvChipsList.push_back(apvChips.at(iChip));
        apvChipIdList.push_back(apvList.at(iChip));
    }
    std::cout<<"Chips to read for data : "<<apvChipsList.size()<<std::endl;
}

//read all chips from raw data segment
void ShmemReader::read_raw_data()
{
    std::pair<ShmemRawMap*, size_t>
            res = m_shm_manager.find< ShmemRawMap >( "mmDaqSharedRawData" ) ;

    int16_t rawValue;
    if (res.second != 1) {
        std::cout << "ERR: mmDaqSharedRawData resvec.second=" << res.second << std::endl;
    }
    //std::cout << "ShmemReader::read_raw_data() map size = : " << res.first->size() << std::endl;
    ShmemRawMap::iterator it = res.first->begin();
    std::cout<<"Raw data vector size is "<<res.first->size()<<std::endl;
    for (; it != res.first->end(); ++it) {
        std::pair<QString, QVector< int > > rawData;
        std::string apvChipName;
        QVector<int> rawVector;
        //rawVector.fromStdVector(it->second);
        //std::cout << "name: " << CSrsChipId(it->first).get_string() << std::endl;
        //std::cout << "raw : " << it->second.size() << std::endl;
        apvChipName = getNameFromChipId(it->first);
        //std::cout<<apvChipName<<std::endl;

        if(!apvChipName.empty()) {
            for(size_t ii = 0; ii < it->second.size(); ++ii)    {
                rawValue = static_cast<int16_t>(it->second.at(ii));
                rawVector.push_back(rawValue);
            }
        }
        if(rawVector.size()>1)  {
            rawData.first=QString(apvChipName.c_str());
            rawData.second=rawVector;
            rawEvent.push_back(rawData);
        }
        //        if(!apvChipName.empty() && rawVector.size()>1)
        //            emit fillApvRaw(QString(apvChipName.c_str()),rawVector);
        //read_chip_raw(QString(apvChipName.c_str()),rawVector);
    }

}

std::string ShmemReader::getNameFromChipId(uint32_t chipId)
{
    //std::cout<<"Size of name List: "<<apvChipsList.size()<<std::endl;
    //std::cout<<"Size of id list: "<<apvChipIdList.size()<<std::endl;
    //return 0;
    for(int iList=0; iList<apvChipsList.size(); ++iList)
    {
        //std::cout<<chipId<<" : "<<apvChipIdList.at(iList)<<std::endl;
        if(apvChipIdList.at(iList)==chipId)
            return apvChipsList.at(iList);
    }
    return std::string();
}

