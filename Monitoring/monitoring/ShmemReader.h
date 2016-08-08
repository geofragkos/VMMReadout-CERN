//
//  ShmemReader.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef SHMEMREADER_H
#define SHMEMREADER_H

#include "AsioService.h"
#include "mainwindow.h"
#include "DisplayDrawer.h"

#include "DetBase.h"
#include "DetChamber.h"
#include "DetReadout.h"
#include "SrsTypes.h"
#include "DetTypes.h"
#include "SrsFec.h"
#include "SrsChip.h"


#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QTimer>
#include <QVector>

#include <vector>

#ifndef Q_MOC_RUN
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio/ip/address.hpp>
//#include <QThread>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
//#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_condition.hpp>


#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#endif
//+++++++++++++++++++++++++++namespaces for shared memory data++++++++++++++++++++++++++++++++++++++++++++++++++
namespace bipc = boost::interprocess;

typedef bipc::managed_shared_memory::segment_manager ShmemSegmentManagerType;

typedef boost::interprocess::interprocess_mutex ShmemMutexType;
typedef boost::interprocess::scoped_lock< ShmemMutexType > ShmemScopedLock;

typedef boost::interprocess::named_mutex ShmemNamedMutexType;
typedef boost::interprocess::scoped_lock< ShmemNamedMutexType > ShmemNamedScopedLock;

typedef boost::interprocess::named_condition ShmemNamedCondition;

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
//int16
typedef bipc::allocator<int16_t, ShmemSegmentManagerType>  ShmemInt16Allocator;
typedef bipc::vector<int16_t, ShmemInt16Allocator>        ShmemInt16Vector;


//all this to have map of vectors ShmemRawDataMap

//Note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
//so the allocator must allocate that pair.
//map is [chipid] = (raw vector int16)
typedef uint32_t  ShmemRawMapKeyType;
typedef ShmemInt16Vector ShmemRawMapMappedType;
typedef std::pair<const ShmemRawMapKeyType, ShmemRawMapMappedType> ShmemRawMapValueType;

//Alias an STL compatible allocator of for the map.
//This allocator will allow to place containers
//in managed shared memory segments
typedef bipc::allocator<ShmemRawMapValueType, ShmemSegmentManagerType> ShmemRawMapAllocator;

//Alias a map of ints that uses the previous STL-like allocator.
//Note that the third parameter argument is the ordering function
//of the map, just like with std::map, used to compare the keys.

//typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;
typedef bipc::map<ShmemRawMapKeyType, ShmemRawMapMappedType, std::less<ShmemRawMapKeyType>, ShmemRawMapAllocator> ShmemRawMap;
//bool FAKEDATA;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class ShmemReader : public QObject
{
    Q_OBJECT

public:
    ShmemReader(std::vector<uint32_t> apvList, std::vector<std::string> apvChips, std::vector<boost::shared_ptr<online::display::CSrsChip> > chipvec,
                std::vector<std::pair<boost::shared_ptr<online::display::CDetChamber>, std::vector<boost::shared_ptr<online::display::CDetReadout> > > > chamberElements,
                MainWindow *window,
                std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingChip1dElements,
                std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingChip2dElements,
                std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingReadout1dElements,
                std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingReadout2dElements);

    ~ShmemReader();

    struct SPublisherIpcData {
             boost::interprocess::interprocess_mutex mutex;
             boost::interprocess::interprocess_condition cond_event;
             bool flag_new_config;
             bool flag_new_event_data;
             int server_state;

             SPublisherIpcData() :
             mutex(),
             cond_event(),
             flag_new_config(false),
             flag_new_event_data(false),
             server_state(0)
             {}
    };

    /// memory shared by DAQ server
    typedef boost::interprocess::shared_memory_object IpcShmemObjectType;
    boost::scoped_ptr<IpcShmemObjectType> m_shm_object;

    /// memory shared by DAQ server
    typedef boost::interprocess::mapped_region IpcShmemMappedRegion;
    boost::scoped_ptr<IpcShmemMappedRegion> m_shm_mapped_region;

    /// data structure shared by DAQ server
    SPublisherIpcData* m_shm_shared_data;
    const char *shmem;
    const char *shCond;

    //online::display::CDaqServerConfig *currentConfiguration;
    MainWindow *mainWindow;
    DisplayDrawer *mainDrawer;
    online::display::CAsioService *service;
    bipc::managed_shared_memory m_shm_manager;
    ShmemNamedCondition m_shm_condition;
    bool terminate;
//    boost::posix_time::ptime timeout;

    //boost::condition_variable readCondition;
    boost::mutex readMutex;
    std::string dataLine;
    std::vector<std::string> apvChipsList;
    std::vector<uint32_t> apvChipIdList;
    std::vector<std::pair<boost::shared_ptr<online::display::CDetChamber>, std::vector<boost::shared_ptr<online::display::CDetReadout> > > > configuredChamberElements;
    std::vector<boost::shared_ptr<online::display::CSrsChip> > configuredChipvec;
    std::vector <std::string> stripDataEvent;
    QVector<std::pair<QString, QVector<int> > > rawEvent;
    int eventDisplayed;


    //QTimer *readingTimer;
    //std::ifstream *file;

    void connect_shared_memory();
    void setCondition();
    std::string getLine();
    std::string readLine();
    void notify();
    void mainLoop();

private:
    bool isProcessing;
    bool realEvent;
public slots:

    void handleSharedData();
    void read_event_number();
    std::string msg();
    void read_raw_data();
    void read_event_strips();

    void fillApvChipsList(std::vector<uint32_t> apvList,std::vector<std::string > apvChips);
    std::string getNameFromChipId(uint32_t chipId);
//    void read_chip_raw(QString name, QVector<int> rawVector);
//    int handleBufferedEvent(QString line_qstr);


    //void read_chip_raw(apvChipId name);

//    void readoutHistoFiller();

//    void fillReadoutHistos(std::string chamberName, std::string readoutName, int strip, int charge, int time);
//    TH1D *fillReadout1dHisto(std::string chamberName, std::string readoutName, std::string histoType);
//    TH2D *fillReadout2dHisto(std::string chamberName, std::string readoutName, std::string histoType);
//    TH1D *fillChip1dHisto(std::string chipName, std::string histoType);
//    TH2D *fillChip2dHisto(std::string chipName, std::string histoType);


signals:
    void readEvent();
    void readRaw();
    void readStrip();

    void fillHistograms(QVector<std::pair<QString, QVector<int> > >, std::vector <std::string>, int);

    void runNumberIs(QString);
    void fillApvRaw(QString, QVector<int>);
    void runReadoutHistoFiller();
    void fillReadout(QString line);
    void newLine();
    void newEventReceived(QString);
    void drawHistograms();
};

#endif // SHMEMREADER_H
