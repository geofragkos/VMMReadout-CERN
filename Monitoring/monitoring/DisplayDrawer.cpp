//
//  DisplayDrawer.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "DisplayDrawer.h"
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

#include <QMetaType>


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

using namespace online::display;


int jjjj= 0;

DisplayDrawer::DisplayDrawer(
        MainWindow *mainWindow
        ) :
    service(new online::display::CAsioService(1)), monitoringMainWindow(), drawCondition(), drawMutex(),
    mem_rawData(0), mem_stripData(0), mem_eventNumber(0), mem_eventNumber_str()
{
    online::display::CAsioService::MainLoopFunction f = boost::bind(&DisplayDrawer::drawSharedData, this);
    //    if(false)//aikoulou debug
    service->post(f);
    monitoringMainWindow = mainWindow;
    //connect(this,SIGNAL(runNumberIs(QString)),monitoringMainWindow->runNumberLabel_update,SLOT(setText(QString)),Qt::QueuedConnection);
    //    connect(monitoringMainWindow->mainTabs, SIGNAL(currentChanged(int)),this,SLOT(changeActiveTab(int)));//aikoulou: was commented. leave it so ;)
    //aikoulou: comment outin. don't want to reset when stopping.
    connect(monitoringMainWindow,SIGNAL(stopDaqClicked()),this,SLOT(reset_slot()));
}


DisplayDrawer::~DisplayDrawer()
{
    service->stopping(true);
    service->stop();
}

void DisplayDrawer::NotifyFill(QVector<std::pair<QString, QVector<int> > > rawData, std::vector <std::string> stripData, int eventNumber)
{
    mem_rawData = rawData; //na figoun ta raw TODO
    //dantrim origianl edo
    mem_stripData = stripData;


   // //aikoulou+ntekas: adjust to slot connection.
   // mem_stripData.clear();
   // for(int i=0;i<stripData.size();i++)
   // {
   //     //        QString str = QString::fromUtf8(stripData.at(i).c_str());

   //     mem_stripData.push_back(QString::fromUtf8(stripData.at(i).c_str()));
   // }

    //    mem_stripData = stripData;
    mem_eventNumber = eventNumber;

    fillCondition.wakeOne();
}

void DisplayDrawer::NotifyDraw()
{
    drawCondition.wakeOne();
}

void DisplayDrawer::drawSharedData()
{
    while(!service->stopping()) {
        QMutexLocker locker(&drawMutex);
        if(fillCondition.wait(&drawMutex,1000))   {
            fillHistograms();
            mem_eventNumber_str.setNum(mem_eventNumber);
            monitoringMainWindow->eventCounterLabel_update->setText(mem_eventNumber_str);
            drawHistograms();
        }
        else    {
            continue;
        }
    }
}

void DisplayDrawer::changeActiveTab(int tabIndex)
{
    if(monitoringMainWindow->mainTabs->currentIndex()==0)   {
        drawEventHistos();
        //monitoringMainWindow->eventDisplayFrame->show();
    }
    else if(monitoringMainWindow->mainTabs->currentIndex()==1)  {
        drawStatisticsHistos();
        //monitoringMainWindow->statisticsFrame->frameCanvas->show();
        //monitoringMainWindow->statisticsFrame->frameCanvas->getCanvas()->Update();
    }
    //    else if(monitoringMainWindow->mainTabs->currentIndex()==2)
    //        monitoringMainWindow->statisticsFrame->frameCanvas->show();
    //    else if(monitoringMainWindow->mainTabs->currentIndex()==3)
    //        monitoringMainWindow->statisticsFrame->frameCanvas->show();
    //    else if(monitoringMainWindow->mainTabs->currentIndex()==4)
    //        monitoringMainWindow->statisticsFrame->frameCanvas->show();
    else if(monitoringMainWindow->mainTabs->currentIndex()==5)  {
        drawApvRawHistos();
        //monitoringMainWindow->apvRawFrame->frameCanvas->show();
        //monitoringMainWindow->apvRawFrame->frameCanvas->getCanvas()->Update();
    }
    //    else if(monitoringMainWindow->mainTabs->currentIndex()==6)  {
    //monitoringMainWindow->statisticsFrame->frameCanvas->show();
    //monitoringMainWindow->statisticsFrame->frameCanvas->getCanvas()->Update();
    //    }
    //    else if(monitoringMainWindow->mainTabs->currentIndex()==8)  {
    //        drawPedestalsHistos();
    //        //monitoringMainWindow->pedestalsFrame->frameCanvas->show();
    //        //monitoringMainWindow->pedestalsFrame->frameCanvas->getCanvas()->Update();
    //    }
    //activeTabIndex=tabIndex;
}

void DisplayDrawer::fillHistograms()
{
    //    apvRawHistoFiller(); // TODO remove
    readoutHistoFiller();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++Histogram Fillers+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void DisplayDrawer::apvRawHistoFiller()
{
    resetApvRawHistos_slot();
    for(int iLine = 0; iLine<mem_rawData.size(); iLine++)
    {
        fill_chip_raw(mem_rawData.at(iLine).first,mem_rawData.at(iLine).second);
    }
}

void DisplayDrawer::readoutHistoFiller()
{
    if(!service->stopping())
        resetEventHistos_slot();//clearing event display histograms for new event
    for(int iLine = 0; iLine<mem_stripData.size(); iLine++)
    {
        handleBufferedEvent(mem_stripData.at(iLine));
    }
}

void DisplayDrawer::fill_chip_raw(QString name, QVector<int> rawVector)
{
    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
            if((monitoringMainWindow->chamberTree->child(i))->child(iChip)->text(0).compare(name)==0)  {
                QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
                //chipvecptr->getRawHisto()->Reset();
                for(int iVec=0; iVec<rawVector.size(); iVec++)
                {
                    chipvecptr->getRawHisto()->GetXaxis()->SetRangeUser(0,rawVector.size());
                    chipvecptr->getRawHisto()->GetYaxis()->SetRangeUser(
                                rawVector.at(std::min_element(rawVector.begin(),rawVector.end())-rawVector.begin())-100,
                                rawVector.at(std::max_element(rawVector.begin(),rawVector.end())-rawVector.begin())+100);
                    chipvecptr->getRawHisto()->SetBinContent(iVec,rawVector.at(iVec));
                }

            }

        }
    }
}



//Data buffer reader (SLOT)
int DisplayDrawer::handleBufferedEvent(QString line_qstr)
{
    std::string line_str = line_qstr.toStdString();
    int column=0;
    int processedEventNumber=-1000;
    CSrsChipId *currentChipId;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ");
    tokenizer tokens(line_str, sep);
    tokenizer::iterator tok_iter = tokens.begin();

    //aikoulou: debug
    //    std::cout << "<--\n";
    //containers for reading .bin file elements
    //elements' names
    std::string pinstr;
    std::string eventNumstr; //counter
    std::string channelstr;
    std::string chipstr;
    std::string fecstr; // 1 i 2
    std::string chiptypestr; //den xrismopieite
    std::string chamberstr; //chamber name from config
    std::string multilayerstr; //
    std::string layerstr;
    std::string readoutstr;
    std::string stripstr;
    //elements' data
    std::string chargestr; // 0- 2000 fortio
    std::string timestr;
    std::string chargeavgstr;
    std::string maxchargestr;

    std::string tdo_str;
    std::string pdo_str;




    float charge=0;
    int time=0;
    int fec=0;
    int chip=0;
    int channel=0;

    column=0;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
        column++;
        pinstr = boost::trim_copy(*tok_iter);
        //std::cout<<pinstr<<std::endl;
        if(column == 1) {
            //std::cout<<"event # : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            eventNumstr = pinstr;
        }
        else if(column == 2) {
            //std::cout<<"chamber : "<<pinstr<<std::endl;
            chamberstr = pinstr;
        }
        else if(column == 3) {
            //std::cout<<"multilayer : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            multilayerstr = pinstr;
        }
        else if(column == 4) {
            //std::cout<<"layer : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            layerstr = pinstr;
        }
        else if(column == 5) {
            //std::cout<<"readout : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            readoutstr = pinstr;
        }
        else if(column == 6) {
            //std::cout<<"strip : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            stripstr = pinstr;
        }
        else if(column == 7) {
            //std::cout<<"fec : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            fecstr = pinstr;
        }
        else if(column == 8) {
            //std::cout<<"chip : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            chipstr = pinstr;
        }
        else if(column == 9) {
            //std::cout<<"channel : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            channelstr = pinstr;
        }
        else if(column == 10) {
            //std::cout<<"charge : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            chargestr = pinstr;
        }
        else if(column == 11) {
            //            std::cout<<"time : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            timestr = pinstr;
        }
        else if(column == 12) {
            //std::cout<<"charge : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            pdo_str = pinstr;
        }
        else if(column == 13) {
            //std::cout<<"charge : "<<pinstr<<std::endl;
            //getChannelReadout(pinstr);
            tdo_str = pinstr;
        }
        else {
            std::cout<<"Data format not recognised..."<<std::endl;
            std::cout<< "column: " << column << std::endl;
        }
    }
    if(/*atoi(eventNumstr.c_str())==processedEventNumber ||*/ chamberstr=="unmapped")
    {return 0;}
    else {
        processedEventNumber == atoi(eventNumstr.c_str());
        //std::cout<<"got "<<chamberstr<<std::endl;
    }
    //currentChipId = new CSrsChipId(atoi(fecstr.c_str()),atoi(chipstr.c_str()));
    charge = atof(chargestr.c_str());
    time = atof(timestr.c_str());
    fec = atoi(fecstr.c_str());
    chip = atoi(chipstr.c_str());
    channel = atoi(channelstr.c_str());

    int tdo=atof(tdo_str.c_str());
    int pdo=atof(pdo_str.c_str());
//    tdo=50;
//    pdo=100;


    //conversion of counts in volts for vmm1 chips data

    //TO CHANGE: FEC NUMBER 3 FOR VMM1 CURRENTLY
    if(fec==3)   {
        charge*=3.3/16384;
        time*=3.3/16384;
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! use atof(strinf.c_str()) to convert string to double for filling values ion the histograms
    //std::cout<<"++++++++++++++++++++++++++++++++++++++++++Filling readout++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    //++++++++++++++++++++++++filling readout histograms (accessing them from class objects)+++++++++++++++++++++
    fillReadoutHistos(chamberstr,readoutstr,atoi(stripstr.c_str()),charge,time, pdo,tdo);
    //aikoulou: comment out on 12/5/2016. the functions are just comments.
    //    fillChipHistos(fec, chip, channel, charge, time);
    return 1;

    //    jjjj++;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++Histogram Fillers+++++++++++++++++++++++++++++++++++++++++++++++++++++
//Get 1d/2d readout histogram to fill - Selecting from the histograms lists of each frame using a std::string
//histoType selection key

void DisplayDrawer::fillReadoutHistos(std::string chamberName, std::string readoutName, int strip, float charge, float time, int pdo, int tdo)
{
    //    std::cout << "is here111--------------------------------------------------------";
    //aikoulou: debug
    //std::cout << "chamberName -->" << chamberName << "\nreadoutName -->" << readoutName <<
    //                 "\nstrip -->" << strip <<  "\ncharge -->" << charge << "\ntime -->" << time << "\n";

    for(size_t i=0; i<monitoringMainWindow->chamberTree->childCount(); ++i)
    {

        QVariant chamberVar = monitoringMainWindow->chamberTree->child(i)->data(0,Qt::UserRole);
        CDetBase *chambptr = reinterpret_cast<CDetBase *>(chamberVar.value<void*>());
        if(chambptr->getMyChamberName()==chamberName)   {
            Q_FOREACH(DetBasePtr mlayervecptr,chambptr->get_children()) {
                Q_FOREACH(DetBasePtr layervecptr, mlayervecptr->get_children()) {
                    Q_FOREACH(DetBasePtr baseptr, layervecptr->get_children()) {
                        boost::shared_ptr<CDetReadout> readout = boost::dynamic_pointer_cast<CDetReadout>(baseptr);
                        if(readout->name()==readoutName)    {
                            //filling statistics histograms
                            //aikoulou: debug
                            //                            std::cout << "chamberName ------------>" << chamberName << "\nreadoutName -->" << readoutName <<
                            //                                         "\nstrip -->" << strip <<  "\ncharge -->" << charge << "\ntime -->" << time << "\n";
                            readout->getStatisticsHisto()->Fill(strip);
                            //                                if(readout->getTimeStatisticsHisto()->GetBinContent(readout->getTimeStatisticsHisto()->GetMaximumBin())<time)
                            //                                    readout->getTimeStatisticsHisto()->SetBins(time+1*500,0,time+1);
                            //                                if(readout->getChargeStatisticsHisto()->GetBinContent(readout->getChargeStatisticsHisto()->GetMaximumBin())<charge)
                            //                                    readout->getTimeStatisticsHisto()->SetBins(charge+1*500,0,charge+1);
                            readout->getTimeStatisticsHisto()->Fill(time);
                            readout->getChargeStatisticsHisto()->Fill(charge);
                            readout->getPDOStatisticsHisto()->Fill(pdo);
                            readout->getTDOStatisticsHisto()->Fill(tdo);
                            //filling event display histograms
                            readout->getChargeEventHisto1d()->SetBinContent(strip,charge);
                            readout->getPDOEventHisto1d()->SetBinContent(strip,pdo);
                            readout->getTDOEventHisto1d()->SetBinContent(strip,tdo);
                            readout->getTimeEventHisto()->Fill(strip,time);
                        }
                    }

                }
            }
        }
    }

}

void DisplayDrawer::fillChipHistos(int fecNumber, int chipNumber, int channelNumber, float charge, float time)
{
    //    CSrsChipId *chipId(fecNumber, chipNumber);

    //    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    //    {
    //        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
    //            if((monitoringMainWindow->chamberTree->child(i))->child(iChip)->text(0).compare(name)==0)  {
    //                QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
    //                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
    //                chipvecptr->getRawHisto()->Reset();
    //                for(int iVec=0; iVec<rawVector.size(); iVec++)
    //                {
    //                    chipvecptr->getRawHisto()->GetXaxis()->SetRangeUser(0,rawVector.size());
    //                    chipvecptr->getRawHisto()->GetYaxis()->SetRangeUser(
    //                                rawVector.at(std::min_element(rawVector.begin(),rawVector.end())-rawVector.begin())-100,
    //                                rawVector.at(std::max_element(rawVector.begin(),rawVector.end())-rawVector.begin())+100);
    //                    chipvecptr->getRawHisto()->SetBinContent(iVec,rawVector.at(iVec));
    //                }

    //            }

    //        }
    //    }

}


TH2D *DisplayDrawer::fillChip2dHisto(std::string chipName, std::string histoType)
{
    //    Q_FOREACH(boost::shared_ptr<CSrsChip> chipvecptr, configuredChipvec) {
    //        //std::cout<<chipvecptr->get_chip_id().chip_id()<<std::endl;
    //        //std::cout<<"TH2D *CDaqServerConfig::fillChip2dHisto(std::string chipName, std::string histoType)"<<std::endl;
    //        //std::cout<<chipvecptr->name().c_str()<<" : "<<chipName.c_str()<<std::endl;
    //        //if(chipvecptr->name()==chipName.c_str())  {
    //            //if(histoType == "Apv Raw")  {
    //                //return chipvecptr->getRawHisto();
    //                //std::cout<<"Chip "<<chipvecptr->name()<<" found"<<std::endl;
    //                //std::cout<<"Filling "<<chipvecptr->getRawHisto()->GetTitle()<<std::endl;
    //            //}
    //        //}
    //    }

    return 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++Histogram drawers++++++++++++++++++++++++++++++++++++++++++++++
//draw histograms slot for displaying on active tab only
void DisplayDrawer::drawHistograms()
{
    //    qDebug("### @DRAWHISTS - TAB = %i", monitoringMainWindow->mainTabs->currentIndex());
    //std::cout<<"Draw signal Received"<<std::endl;
    if(monitoringMainWindow->mainTabs->currentIndex()==0)   {
        drawEventHistos();
        //        resetEventHistos_slot();
        //drawTimeEventHistos();
        //        monitoringMainWindow->eventDisplayFrame->frameCanvas->getCanvas()->Modified();
        //        monitoringMainWindow->eventDisplayFrame->frameCanvas->getCanvas()->Update();
    }
    else if(monitoringMainWindow->mainTabs->currentIndex()==1)   {
        drawStatisticsHistos();
        //drawChargeStatisticsHistos();
        //drawTimeStatisticsHistos();
        //monitoringMainWindow->statisticsFrame->frameCanvas->getCanvas()->Modified();
        //monitoringMainWindow->statisticsFrame->frameCanvas->getCanvas()->Update();
    }
    //else if(monitoringMainWindow->mainTabs->currentIndex()==2)  {
    //drawChipStatisticsHistos();
    //monitoringMainWindow->apvRawFrame->frameCanvas->getCanvas()->Modified();

    //}

    //    else if(monitoringMainWindow->mainTabs->currentIndex()==5)   {
    //        drawApvRawHistos();
    //monitoringMainWindow->apvRawFrame->frameCanvas->getCanvas()->Update();
    //    }

    //    else if(monitoringMainWindow->mainTabs->currentIndex()==8)  {
    //        drawPedestalsHistos();
    //    }
}

void DisplayDrawer::drawStatisticsHistos()
{
    //std::cout<<"Drawing statistics"<<std::endl;
    int canvasIndex =1;
    for(size_t i=0; i<monitoringMainWindow->chamberTree->childCount(); ++i)
    {
        if(monitoringMainWindow->chamberTree->child(i)->checkState(0)==2)   {
            //            reinterpret_cast<void*> (chambvecptr.get())
            //            * reinterpret_cast(boost::shared_ptr<A>*)(var);
            QVariant chamberVar = monitoringMainWindow->chamberTree->child(i)->data(0,Qt::UserRole);
            CDetBase *chambptr = reinterpret_cast<CDetBase *>(chamberVar.value<void*>());
            //boost::shared_ptr<CDetChamber> chamber = boost::dynamic_pointer_cast<CDetChamber>(*chambvecptr);
            Q_FOREACH(DetBasePtr mlayervecptr,chambptr->get_children()) {
                Q_FOREACH(DetBasePtr layervecptr, mlayervecptr->get_children()) {
                    Q_FOREACH(DetBasePtr baseptr, layervecptr->get_children()) {
                        boost::shared_ptr<CDetReadout> readout = boost::dynamic_pointer_cast<CDetReadout>(baseptr);
                        monitoringMainWindow->statisticsFrame->draw1dHistogramInCanvas(readout->getStatisticsHisto(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->statisticsFrame->draw1dHistogramInCanvas(readout->getChargeStatisticsHisto(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->statisticsFrame->draw1dHistogramInCanvas(readout->getTimeStatisticsHisto(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->statisticsFrame->draw1dHistogramInCanvas(readout->getPDOStatisticsHisto(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->statisticsFrame->draw1dHistogramInCanvas(readout->getTDOStatisticsHisto(),canvasIndex);
                        canvasIndex++;
                    }

                }
            }
        }

    }
}


void DisplayDrawer::drawEventHistos()
{
    int canvasIndex =1;
    for(size_t i=0; i<monitoringMainWindow->chamberTree->childCount(); ++i)
    {
        //        qDebug()<<" #######   monitoringMainWindow->chamberTree->child(i)->checkState(0) = "<<monitoringMainWindow->chamberTree->child(i)->checkState(0);
        if(monitoringMainWindow->chamberTree->child(i)->checkState(0)==2)   {
            QVariant chamberVar = monitoringMainWindow->chamberTree->child(i)->data(0,Qt::UserRole);
            CDetBase *chambptr = reinterpret_cast<CDetBase *>(chamberVar.value<void*>());
            //boost::shared_ptr<CDetChamber> chamber = boost::dynamic_pointer_cast<CDetChamber>(*chambvecptr);
            Q_FOREACH(DetBasePtr mlayervecptr,chambptr->get_children()) {
                Q_FOREACH(DetBasePtr layervecptr, mlayervecptr->get_children()) {
                    Q_FOREACH(DetBasePtr baseptr, layervecptr->get_children()) {
                        boost::shared_ptr<CDetReadout> readout = boost::dynamic_pointer_cast<CDetReadout>(baseptr);
                        monitoringMainWindow->eventDisplayFrame->draw1dHistogramInCanvas(readout->getChargeEventHisto1d(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->eventDisplayFrame->draw2dHistogramInCanvas(readout->getTimeEventHisto(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->eventDisplayFrame->draw1dHistogramInCanvas(readout->getPDOEventHisto1d(),canvasIndex);
                        canvasIndex++;
                        monitoringMainWindow->eventDisplayFrame->draw1dHistogramInCanvas(readout->getTDOEventHisto1d(),canvasIndex);
                        canvasIndex++;
                    }

                }
            }
        }

    }
}


void DisplayDrawer::drawApvRawHistos()
{
    int canvasIndex =1;
    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
            if((monitoringMainWindow->chamberTree->child(i))->child(iChip)->text(0).contains("APV", Qt::CaseInsensitive)
                    && (monitoringMainWindow->chamberTree->child(i))->child(iChip)->checkState(0)==2)  {
                QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
                monitoringMainWindow->apvRawFrame->draw1dHistogramInCanvas(chipvecptr->getRawHisto() ,canvasIndex);
                //std::cout<<chipvecptr->getRawHisto()->GetTitle()<<std::endl;
                canvasIndex++;

            }

        }
    }

}

void DisplayDrawer::drawChipStatisticsHistos()
{
    //    int canvasIndex =1;
    //    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    //    {
    //        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
    //            if((monitoringMainWindow->chamberTree->child(i))->child(iChip)->text(0).contains("APV", Qt::CaseInsensitive)
    //                    && (monitoringMainWindow->chamberTree->child(i))->child(iChip)->checkState(0)==2)  {
    //                QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
    //                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
    //                monitoringMainWindow->apvRawFrame->draw1dHistogramInCanvas(chipvecptr->getRawHisto() ,canvasIndex);
    //                //std::cout<<chipvecptr->getRawHisto()->GetTitle()<<std::endl;
    //                canvasIndex++;

    //            }

    //        }
    //    }
}
//not used currently, pedestal drawing is done on pedestalFileReader
void DisplayDrawer::drawPedestalsHistos()
{
    int canvasIndex =1;
    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
            if((monitoringMainWindow->chamberTree->child(i))->child(iChip)->text(0).contains("APV", Qt::CaseInsensitive)
                    && (monitoringMainWindow->chamberTree->child(i))->child(iChip)->checkState(0)==2)  {
                QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
                CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
                chipvecptr->getChipPedestalMean()->SetMarkerStyle(34);
                chipvecptr->getChipPedestalMean()->GetYaxis()->SetRangeUser(
                            chipvecptr->getChipPedestalMean()->GetMean(2)-3*chipvecptr->getChipPedestalMean()->GetRMS(2),
                            chipvecptr->getChipPedestalMean()->GetMean(2)+3*chipvecptr->getChipPedestalMean()->GetRMS(2)
                            );

                chipvecptr->getChipPedestalSigma()->SetMarkerStyle(28);
                chipvecptr->getChipPedestalSigma()->GetYaxis()->SetRangeUser(
                            chipvecptr->getChipPedestalSigma()->GetBinCenter(chipvecptr->getChipPedestalSigma()->FindFirstBinAbove(0.0000001,2))-20,
                            chipvecptr->getChipPedestalSigma()->GetBinCenter(chipvecptr->getChipPedestalSigma()->FindLastBinAbove(0.000001,2))+20
                            );
                //                chipvecptr->getChipPedestalSigma()->GetYaxis()->SetRangeUser(
                //                            chipvecptr->getChipPedestalSigma()->GetMean(2)-2*chipvecptr->getChipPedestalSigma()->GetRMS(2),
                //                            chipvecptr->getChipPedestalSigma()->GetMean(2)+2*chipvecptr->getChipPedestalSigma()->GetRMS(2)
                //                            );
                monitoringMainWindow->pedestalsFrame->draw2dHistogramInCanvas(chipvecptr->getChipPedestalMean(),canvasIndex);
                canvasIndex++;
                monitoringMainWindow->pedestalsFrame->draw2dHistogramInCanvas(chipvecptr->getChipPedestalSigma(),canvasIndex);
                canvasIndex++;
            }

        }
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++Histogram Reseters+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DisplayDrawer::reset_slot()
{
    resetEventHistos_slot();
    //aikoulou: trying to make the plots resizable when stopped
    //    resetStatisticsHistos_slot();
    resetApvRawHistos_slot();
}

void DisplayDrawer::resetStatisticsHistos_slot()
{
    for(size_t i=0; i<monitoringMainWindow->chamberTree->childCount(); ++i)
    {
        QVariant chamberVar = monitoringMainWindow->chamberTree->child(i)->data(0,Qt::UserRole);
        CDetBase *chambptr = reinterpret_cast<CDetBase *>(chamberVar.value<void*>());

        Q_FOREACH(DetBasePtr mlayervecptr,chambptr->get_children()) {
            Q_FOREACH(DetBasePtr layervecptr, mlayervecptr->get_children()) {
                Q_FOREACH(DetBasePtr baseptr, layervecptr->get_children()) {
                    boost::shared_ptr<CDetReadout> readout = boost::dynamic_pointer_cast<CDetReadout>(baseptr);
                    readout->getStatisticsHisto()->Reset();
                    readout->getChargeStatisticsHisto()->Reset();
                    readout->getTimeStatisticsHisto()->Reset();
                    readout->getPDOStatisticsHisto()->Reset();
                    readout->getTDOStatisticsHisto()->Reset();
                }

            }
        }
    }
}

void DisplayDrawer::resetEventHistos_slot()
{
    for(size_t i=0; i<monitoringMainWindow->chamberTree->childCount(); ++i)
    {
        QVariant chamberVar = monitoringMainWindow->chamberTree->child(i)->data(0,Qt::UserRole);
        CDetBase *chambptr = reinterpret_cast<CDetBase *>(chamberVar.value<void*>());

        Q_FOREACH(DetBasePtr mlayervecptr,chambptr->get_children()) {
            Q_FOREACH(DetBasePtr layervecptr, mlayervecptr->get_children()) {
                Q_FOREACH(DetBasePtr baseptr, layervecptr->get_children()) {
                    boost::shared_ptr<CDetReadout> readout = boost::dynamic_pointer_cast<CDetReadout>(baseptr);
                    readout->getChargeEventHisto1d()->Reset();
                    readout->getTimeEventHisto()->Reset();
                    readout->getPDOEventHisto1d()->Reset();
                    readout->getTDOEventHisto1d()->Reset();
                }

            }
        }
    }
}

void DisplayDrawer::resetApvRawHistos_slot()
{
    for(int i=0; i<monitoringMainWindow->chamberTree->childCount(); i++)
    {
        for(int iChip=0; iChip<(monitoringMainWindow->chamberTree->child(i))->childCount();iChip++)    {
            QVariant chipVar = monitoringMainWindow->chamberTree->child(i)->child(iChip)->data(0,Qt::UserRole);
            CSrsChip *chipvecptr = reinterpret_cast<CSrsChip* >(chipVar.value<void*>());
            chipvecptr->getRawHisto()->Reset();
        }
    }

}





