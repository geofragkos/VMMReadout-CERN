//
//  pedestalsfileloader.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef PEDESTALSFILELOADER_H
#define PEDESTALSFILELOADER_H

//#include "DaqServerConfig.h"
#include "DetBase.h"
#include "DetChamber.h"
#include "DetReadout.h"
#include "SrsTypes.h"
#include "DetTypes.h"
#include "SrsFec.h"
#include "SrsChip.h"
#include "DetReadout.h"
#include "frame.h"
#include "canvas.h"
#include "mainwindow.h"

#include "pedestals.h"
#include "apv_raw_ped.h"

#ifndef Q_MOC_RUN
//#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/filesystem.hpp>
#endif


#include <QObject>

#include <TCanvas.h>
#include <TVirtualX.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFormula.h>
#include <TF1.h>
#include <TH1.h>
#include <TFrame.h>
#include <TTimer.h>
#include <TFile.h>

//namespace online {
//   namespace display {
using namespace online::display;


        class pedestalsFileLoader : public QObject
        {
            Q_OBJECT

        public:
            pedestalsFileLoader(std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec, MainWindow *window);


            void pedestalsDataHandler(std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec);
            void plotPedestals();

            TH2D* returnPedestalsMeanHistograms(std::string apv_name , std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec);
            TH2D* returnPedestalsSigmaHistograms(std::string apv_name , std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec);

            MainWindow *mainWindow;
            std::vector< boost::shared_ptr<online::display::CSrsChip> > m_chipvec;
            std::string m_pedestalsFileName;
            TFile *m_pedestals_file;
            apv_raw_ped *m_pedestalTree;
            std::vector< std::pair<std::string , std::vector< std::string >  > >chamberChips;


            std::string getChipNameFromNumber(int fecNumber, int chipNumber);

//            void fillMeanPedestalsHisto(unsigned int apv_id , unsigned int apv_ch, float apv_mean);
//            void fillSigmaPedestalsHisto(unsigned int apv_id , unsigned int apv_ch, float apv_sigma);


        public slots:
            void definePedestalsFileName(std::string file);
            void resetChipPedestals();
            void fillChipPedestals(QString chip, int chipChannel, float pedMean, float pedSigma);
            void drawPedestalsHistos();

        signals:
            void drawPedestalHistos();

        };
//   }
//}
#endif // PEDESTALSFILELOADER_H
