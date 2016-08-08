//
//  pedestalsfileloader.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "pedestalsfileloader.h"
#include "DaqServerConfig.h"
#include "SrsBase.h"
#include "SrsSru.h"
#include "SrsFec.h"
#include "SrsChannel.h"
#include "SrsChannelId.h"
#include "SrsChip.h"
#include "Logger.h"
#include "DetBase.h"
#include "DetDetector.h"
#include "DetConnector.h"
#include "DetReadout.h"
#include "DetChamber.h"

#include <string.h>
#include <iostream>

#include <QObject>

//using namespace online::display;

pedestalsFileLoader::pedestalsFileLoader(std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec, MainWindow *window)
    :
      mainWindow(window),m_pedestalsFileName(),m_pedestals_file()
{
    m_chipvec = chipvec;
}

void pedestalsFileLoader::definePedestalsFileName(std::string file)
{
    m_pedestalsFileName = file;
    resetChipPedestals();
    pedestalsDataHandler(m_chipvec);
    drawPedestalsHistos();

}

void pedestalsFileLoader::pedestalsDataHandler(std::vector< boost::shared_ptr<online::display::CSrsChip> > chipvec)
{
    m_pedestals_file = new TFile(m_pedestalsFileName.c_str());
    //TH2D* h_temp;
    m_pedestalTree  = new apv_raw_ped((TTree*)m_pedestals_file->Get("apv_raw_ped"));
    if(m_pedestalTree==NULL) std::cout<<"Not valid pedestal file"<<std::endl;
    else    {
        int nentries = (m_pedestalTree->fChain)->GetEntries();
        std::cout<<"Loaded file "<<m_pedestalsFileName<<" as pedestals file"<<std::endl;
        std::cout<<"Number of entries : "<<nentries<<std::endl;
        std::string chipName;
        (m_pedestalTree->fChain)->GetEntry(0);

        for(size_t i=0; i<m_pedestalTree->srsChip->size(); ++i)
        {
            chipName = getChipNameFromNumber(m_pedestalTree->srsFec->at(i),m_pedestalTree->srsChip->at(i));
            fillChipPedestals(QString(chipName.c_str()),m_pedestalTree->srsChan->at(i),m_pedestalTree->ped_mean->at(i),m_pedestalTree->ped_stdev->at(i));
        }
    }
}

std::string pedestalsFileLoader::getChipNameFromNumber(int fecNumber, int chipNumber)
{
    std::string chipName;
    std::stringstream ss;
    if(chipNumber<10)
        ss<<"APV."<<fecNumber<<".0"<<chipNumber;
    else
        ss<<"APV."<<fecNumber<<"."<<chipNumber;
    chipName = ss.str();
    //std::cout<<chipName<<std::endl;
    return chipName;
}

void pedestalsFileLoader::resetChipPedestals()
{
    for(int i=0; i<mainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(mainWindow->chamberTree->child(i))->childCount();iChip++)    {
                QVariant chipVar = mainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
                chipvecptr->getChipPedestalMean()->Reset();
                chipvecptr->getChipPedestalSigma()->Reset();
        }
    }
}


void pedestalsFileLoader::fillChipPedestals(QString chip, int chipChannel, float pedMean, float pedSigma)
{
    for(int i=0; i<mainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(mainWindow->chamberTree->child(i))->childCount();iChip++)    {
            if((mainWindow->chamberTree->child(i))->child(iChip)->text(0).compare(chip)==0)  {
                QVariant chipVar = mainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());

                chipvecptr->getChipPedestalMean()->Fill(chipChannel,pedMean);
                chipvecptr->getChipPedestalSigma()->Fill(chipChannel,pedSigma);

            }

        }
    }
}

void pedestalsFileLoader::drawPedestalsHistos()
{
    int canvasIndex =1;
    for(int i=0; i<mainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(mainWindow->chamberTree->child(i))->childCount();iChip++)    {
            if((mainWindow->chamberTree->child(i))->child(iChip)->text(0).contains("APV", Qt::CaseInsensitive)
                    && (mainWindow->chamberTree->child(i))->child(iChip)->checkState(0)==2)  {
                QVariant chipVar = mainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
                chipvecptr->getChipPedestalMean()->SetMarkerStyle(34);
                chipvecptr->getChipPedestalMean()->GetYaxis()->SetRangeUser(
                            chipvecptr->getChipPedestalMean()->GetMean(2)-3*chipvecptr->getChipPedestalMean()->GetRMS(2),
                            chipvecptr->getChipPedestalMean()->GetMean(2)+3*chipvecptr->getChipPedestalMean()->GetRMS(2)
                            );

                chipvecptr->getChipPedestalSigma()->SetMarkerStyle(28);/*
                chipvecptr->getChipPedestalSigma()->GetYaxis()->SetRangeUser(
                            chipvecptr->getChipPedestalSigma()->GetBinCenter(chipvecptr->getChipPedestalSigma()->FindFirstBinAbove(0.0000001,2))-20,
                            chipvecptr->getChipPedestalSigma()->GetBinCenter(chipvecptr->getChipPedestalSigma()->FindLastBinAbove(0.000001,2))+20
                            );*/
                chipvecptr->getChipPedestalSigma()->GetYaxis()->SetRangeUser(-200.,+200.);
//                qDebug("HERE2%%%");
//                chipvecptr->getChipPedestalSigma()->GetYaxis()->SetRangeUser(
//                            chipvecptr->getChipPedestalSigma()->GetMean(2)-2*chipvecptr->getChipPedestalSigma()->GetRMS(2),
//                            chipvecptr->getChipPedestalSigma()->GetMean(2)+2*chipvecptr->getChipPedestalSigma()->GetRMS(2)
//                            );
                mainWindow->pedestalsFrame->draw2dHistogramInCanvas(chipvecptr->getChipPedestalMean(),canvasIndex);
                canvasIndex++;
                mainWindow->pedestalsFrame->draw2dHistogramInCanvas(chipvecptr->getChipPedestalSigma(),canvasIndex);
                canvasIndex++;
            }

        }
    }

}
