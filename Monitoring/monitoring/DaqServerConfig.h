//
//  DaqServerConfig.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef online_display_DaqServerConfig_h
#define online_display_DaqServerConfig_h

#include "mmDaqStartupWindow.h"

#ifndef Q_MOC_RUN
#include "DetBase.h"
#include "DetChamber.h"
#include "DetReadout.h"
#include "SrsTypes.h"
#include "DetTypes.h"
#include "SrsFec.h"
#include "SrsChip.h"
#include "pedestalsfileloader.h"
#include "filereader.h"
#include "ShmemReader.h"
#include "DisplayDrawer.h"
#endif

#include "mainwindow.h"
#include "mmDaqSettingsWindow.h"
#include "ui_mainwindow.h"
#include "frame.h"

#ifndef Q_MOC_RUN

//#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/address.hpp>

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

#include <boost/algorithm/cxx11/copy_if.hpp>

#endif
#include <QObject>
#include <QThread>
#include <QVector>
#include <QDir>
#include <QFile>

#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



namespace online {
   namespace display {
      //class pedestalsFileLoader;
      //class CSrsBase;
      //class CSrsNetworkDevice;
      //class CSrsChip;
      class CDaqServerConfig : public QObject
      {

          Q_OBJECT

      public:
//         typedef boost::shared_ptr<CSrsBase> SrsBasePtr;

         explicit CDaqServerConfig();
         ~CDaqServerConfig();
         mmDaqStartupWindow *mainStartUpWindow;
         MainWindow *mainWindow;
         Ui::MainWindow *ui;
         mmDaqSettingsWindow *settingsWindow;
         boost::asio::io_service io_service;

         //++++++++++++++++client+++++++++++++++++++++
         typedef enum {
                     msq_ignore,
                     msq_heartbeat,
                     msq_stop,
                     msq_start,
                     msq_reset,
                     msq_configure,
                     msq_pedestal
         } ipc_command_type;


         //bipc::managed_shared_memory m_shm_manager;
         //ShmemNamedCondition m_shm_condition;

         ShmemReader* memReader;
         DisplayDrawer* drawer;
         fileReader* dataFile;
         pedestalsFileLoader *pedestals;
         int activeTabIndex;
         QWaitCondition guiUpdateCondition;
         QMutex *guiUpdateMutex;

         //timer for simulation
         int eventNumber;
         QTimer *timer;
         std::pair<boost::shared_ptr<CDetChamber>, std::vector<boost::shared_ptr<CDetReadout> > > chamberElementsPairs;
         std::vector<std::pair<boost::shared_ptr<CDetChamber>, std::vector<boost::shared_ptr<CDetReadout> > > > chamberElements;
         std::vector<std::string> apvChipsList;
         std::vector<uint32_t> apvChipIdsList;
         std::vector<std::string> bnlChipsList;

         template <class T>
         boost::shared_ptr<T> locate_srs(uint32_t uid) const;
         template <class CSrsChip>
         boost::shared_ptr<CSrsChip> locate_srs(long uid, const std::string& name) const;
         boost::filesystem::path get_config_path() const;
         void startDaqClient();
         std::vector<boost::shared_ptr<CSrsChip> > locate_srs_chips() const;

         //structure for connecting tree elements with histograms and data elements
         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingChip1dElements;
         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingChip2dElements;
         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH1D *> > > > mappingReadout1dElements;
         std::vector < std::pair < QTreeWidgetItem*, std::pair <std::vector<std::string>, std::vector <TH2D *> > > > mappingReadout2dElements;

         void clear();
         void displayStartupWindow();
         void daqClient();
         void configureTreeGui(MainWindow *window);
         void divideFrameCanvases(int numberOfElements, frame* frameForDivide);


//         bool isChip1dHistoDisplayEnabled(TH1D* histo);
//         bool isChip2dHistoDisplayEnabled(TH2D* histo);
//         bool isReadout1dHistoDisplayEnabled(TH1D* histo);
//         bool isReadout2dHistoDisplayEnabled(TH2D* histo);

         //void fillSrsTabGui(MainWindow *window);

         QList<QTreeWidgetItem *> buildDetectorTreeGui();
         QList<QTreeWidgetItem *> buildChamberTreeGui(MainWindow *window);
         QList<QTreeWidgetItem*> classifyChips(std::vector<boost::shared_ptr<CSrsChip> > chipvec, QTreeWidgetItem* parentChamber, DetBasePtr chambvecptr, MainWindow *window);

         //CDetReadout *getChannelReadout(std::string ChannelId);
//        TH1D *fillReadout1dHisto(std::string chamberName, std::string readoutName, std::string histoType);
//        TH2D *fillReadout2dHisto(std::string chamberName, std::string readoutName, std::string histoType);
//        TH1D *fillChip1dHisto(std::string chipName, std::string histoType);
//        TH2D *fillChip2dHisto(std::string chipName, std::string histoType);

      public slots:
        //Slots for configuration of the GUI
        void setConfigFilePath(const QString &config_filePath);
        void setConfigFileName(const QString &config_fileName);
        void setPedestalsFilePath(const QString &pedestals_filePath);
        void setPedestalsFileName(const QString &pedestals_fileName);

        void configure(const QString &configFile);

        //++++++++++Slots for client+++++++++++++++++++++++
        void messageDaq(std::string command);
        void make_messages(std::vector<std::string>& messages, std::string buttonCommand);
        void read_commands();
        void transmitStartSignal();
        void transmitStopSignal();
        void openSettingsWindow();
//        void changeActiveTab(int tabIndex);




        void updateFrameCanvasesDivision_slot(QTreeWidgetItem* parentTreeItem);
        void selectDeselectChamberChildren(QTreeWidgetItem* parentTreeWidget);
        int numberOfReadoutsToDisplay(QTreeWidgetItem *parentTreeWidget);
//        void handleSharedData();
//        void read_event_number();
//        void read_event_strips();
//        void read_raw_data();
//        void read_chip_raw(QString name, QVector<int> rawVector);
//        int handleBufferedEvent(std::string line_str);

        //void openPedestalsFile();
        //void loadPedestalsFile();

      signals:
        void sendStartDaq(std::string command);
        void sendStartPedestalsDaq(std::string command);
        void sendStopDaq(std::string command);
        void sendConfigDaq(std::string command);
        void configWithFile(const QString &);
        void pedestalsFileNameIs(std::string);

      private:
         void read_config_file(const std::string& filename);
         void read_srs_config_file(const std::string& file);
         void read_detector_config_file(const std::string& file);
         void push_new_srs_element(SrsBasePtr elem);
         void makeListItemsCheckable(QList<QTreeWidgetItem*> list);


//         long id_number_from_string(const std::string& idstr) const;

//         std::vector<std::string> m_args;
//         bool m_bad_config;
//         boost::program_options::variables_map m_varmap;
         boost::property_tree::ptree m_daq_cfg;
         boost::property_tree::ptree m_srs_cfg;
         boost::property_tree::ptree m_det_cfg;
         
//         std::string m_receiver_ip;
//         int         m_receiver_port;
//         std::string m_command_ip;
//         int         m_command_port;
         
         std::string m_daq_config_file;
         std::string m_srs_config_file;
         std::string m_det_config_file;

         boost::filesystem::path m_config_path;
//commands for messaging the daq
         QString input_config_path;
         QString input_config_fileName;
         QString pedestal_path;
         QString pedestal_path_prefix;
         QString pedestal_fileName;
         QString message_config_path;
         QString message_pedestal_path;
         QString message_runType;
         QString message_zsapv;
         QString message_commentForDaq;
         QString message_nosave;
         QString message_stop;


         /**
          these are the known sources of data - each needs separate queue for its events
          */
         std::vector<SrsBasePtr> m_srs_elements; 
         /**
          this is current detector configuration
          */
         boost::shared_ptr<CDetBase> m_detector;
         
         ///zero suppress threshold for APV chips
         double m_apv_zs_threshold;
         
         
         //interprocess
         
         boost::interprocess::managed_shared_memory m_shared_segment;
      };
      
      
      //from S.Meyers Effective STL
      template <typename InputIterator, typename OutputIterator, typename Predicate>
      OutputIterator copy_if(InputIterator begin, InputIterator end, OutputIterator destBegin, Predicate p) {
         while (begin != end) {
            if (p(*begin)) *destBegin++ = *begin;
            ++begin;
         }
         return destBegin;
      }

      template <class T>
      boost::shared_ptr<T> CDaqServerConfig::locate_srs(uint32_t uid) const {
         typedef std::vector<SrsBasePtr> srs_vec_type;
         typedef std::vector<SrsBasePtr>::iterator srs_vec_iter;
         std::vector<SrsBasePtr> se(m_srs_elements.size());
         
         srs_vec_iter last = boost::algorithm::copy_if(m_srs_elements.begin(), m_srs_elements.end(), se.begin(),
                 CSrsBase::IdNumberEquals<CSrsBase>(uid));
         se.erase(last, se.end());
      
         for (srs_vec_iter it = se.begin(); it != se.end(); ++it) {
            boost::shared_ptr<T> srselem = boost::dynamic_pointer_cast<T>(*it);
            if (srselem) {
               return srselem;
            }
         }
         return boost::shared_ptr<T>();
      }
      
      
      template <class CSrsChip>
      boost::shared_ptr<CSrsChip> CDaqServerConfig::locate_srs(long uid, const std::string& name) const {
         typedef std::vector<SrsBasePtr> srs_vec_type;
         typedef std::vector<SrsBasePtr>::iterator srs_vec_iter;
         std::vector<SrsChipPtr> chips;
         
         Q_FOREACH(SrsBasePtr sb, m_srs_elements) {
            if (SrsFecPtr sf = boost::dynamic_pointer_cast<CSrsFec>(sb)) {
               if (SrsChipPtr sc = sf->locate_chip(uid, name)) {
                  chips.push_back(sc);
               }
            }
         }
         
         if (chips.size() == 1) {
            return chips.front();
         }
         else if (chips.empty()) {
            std::stringstream ss;
            ss << "locate_srs<CSrsChip> no chip " << uid << " '" << name << "'";
            throw std::runtime_error(ss.str().c_str());
         }
         else {
            std::stringstream ss;
            ss << "locate_srs<CSrsChip> chip not unique " << uid << " '" << name << "'";
            throw std::runtime_error(ss.str().c_str());
         }
      }
      
      
            
      
   } // namespace display
} // namespace online

#endif
