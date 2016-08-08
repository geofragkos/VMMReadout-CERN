//
//  DetReadout.cpp
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetReadout.h"

#include <iostream>

using namespace online::display;

CDetReadout::CDetReadout()
{
   
}

CDetReadout::CDetReadout(DetBaseWeakPtr detector,
            DetBaseWeakPtr parent,
            size_t idnumber,
            const std::string& myname,
            const Coord3& size,
            const Coord3& position,
            const Coord3& rotation) :
    CDetBase(detector, parent, idnumber, myname, size, position, rotation),
    m_readout_statistics_histo( new TH1D( std::string(std::string("Statistics_") + getMyChamberName()+ name() ).c_str(),
                                          std::string(std::string("Statistics_") + getMyChamberName()+ name() ).c_str(),
                                          620,0,620) ),
    m_readout_charge_statistics_histo(new TH1D( std::string("Charge_"+ getMyChamberName()+name() ).c_str(),
                                                std::string("Charge_"+ getMyChamberName()+name() ).c_str(),
                                                400,0,2000) ),
    m_readout_PDO_statistics_histo(new TH1D( std::string("PDO "+ getMyChamberName()+name() ).c_str(),
                                                std::string("PDO_"+ getMyChamberName()+name() ).c_str(),
                                                400,0,2000) ),
    m_readout_TDO_statistics_histo(new TH1D( std::string("TDO "+ getMyChamberName()+name() ).c_str(),
                                                std::string("TDO_"+ getMyChamberName()+name() ).c_str(),
                                                400,0,2000) ),
    m_readout_time_statistics_histo(new TH1D( std::string("Time_"+ getMyChamberName()+name() ).c_str(),
                                              std::string("Time_"+ getMyChamberName()+name() ).c_str(),
                                              200,0,800) ),
    m_readout_event_charge1d(new TH1D(std::string("eventQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      std::string("eventQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      620,0,620)),
    m_readout_event_PDO1d(new TH1D(std::string("eventPDOQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      std::string("eventPDOQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      620,0,620)),
    m_readout_event_TDO1d(new TH1D(std::string("eventTDOQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      std::string("eventTDOQ1d_"+ getMyChamberName()+name() ).c_str(),
                                      620,0,620)),
    m_readout_event_charge(new TH2D( std::string("eventQ_"+ getMyChamberName()+name() ).c_str(),
                                     std::string("eventQ_"+ getMyChamberName()+name() ).c_str(),
                                     620,0,620,400,0,2000) ),
    m_readout_event_time(new TH2D( std::string("eventT_"+ getMyChamberName()+name() ).c_str(),
                                   std::string("eventT_"+ getMyChamberName()+name() ).c_str(),
                                   620,0,620,200,0,800) ),
    m_readout_statistics_advanced_q(0),
    m_readout_statistics_advanced_qmax(0),
    m_strip_range(), m_pitch(0.0)
{

    //m_readout_statistics_histo = new TH1D(histoName,histoName,640,0,620);
    m_readout_statistics_histo->SetLineColor(38);
    m_readout_statistics_histo->SetFillColor(38);
    m_readout_statistics_histo->SetTitleSize(0.04);
    m_readout_statistics_histo->GetXaxis()->SetLabelSize(0.04);

    //m_readout_charge_statistics_histo = new TH1D(chargehistoName,chargehistoName,300,0,3000);
    m_readout_charge_statistics_histo->SetLineColor(46);
    m_readout_charge_statistics_histo->SetFillColor(46);
    m_readout_charge_statistics_histo->GetXaxis()->SetLabelSize(0.04);
    m_readout_charge_statistics_histo->SetTitleSize(0.06);

    //m_readout_PDO_statistics_histo = new TH1D(PDOhistoName,PDOhistoName,300,0,3000);
    m_readout_PDO_statistics_histo->SetLineColor(46);
    m_readout_PDO_statistics_histo->SetFillColor(46);
    m_readout_PDO_statistics_histo->GetXaxis()->SetLabelSize(0.04);
    m_readout_PDO_statistics_histo->SetTitleSize(0.06);

    //m_readout_TDO_statistics_histo = new TH1D(TDOhistoName,TDOhistoName,300,0,3000);
    m_readout_TDO_statistics_histo->SetLineColor(46);
    m_readout_TDO_statistics_histo->SetFillColor(46);
    m_readout_TDO_statistics_histo->GetXaxis()->SetLabelSize(0.04);
    m_readout_TDO_statistics_histo->SetTitleSize(0.06);

    //m_readout_time_statistics_histo = new TH1D(timehistoName,timehistoName,30,0,600);
    m_readout_time_statistics_histo->SetLineColor(8);
    m_readout_time_statistics_histo->SetFillColor(8);
    m_readout_time_statistics_histo->GetXaxis()->SetLabelSize(0.04);
    m_readout_time_statistics_histo->SetTitleSize(0.06);

   // m_readout_event_charge = new TH2D(qEventName, qEventName,640,0,640,1000,0,2000);
    m_readout_event_charge->SetMarkerStyle(27);
    m_readout_event_charge->SetLineColor(33);
    m_readout_event_charge->SetFillColor(33);
    m_readout_event_charge->GetXaxis()->SetLabelSize(0.04);
    m_readout_event_charge->SetTitleSize(0.06);

    //m_readout_event_charge1d->SetMarkerStyle(27);
    m_readout_event_charge1d->SetLineColor(33);
    m_readout_event_charge1d->SetFillColor(33);
    m_readout_event_charge1d->GetXaxis()->SetLabelSize(0.04);
    m_readout_event_charge1d->SetTitleSize(0.06);

    //m_readout_event_PDO1d->SetMarkerStyle(27);
    m_readout_event_PDO1d->SetLineColor(33);
    m_readout_event_PDO1d->SetFillColor(33);
    m_readout_event_PDO1d->GetXaxis()->SetLabelSize(0.04);
    m_readout_event_PDO1d->SetTitleSize(0.06);

    //m_readout_event_TDO1d->SetMarkerStyle(27);
    m_readout_event_TDO1d->SetLineColor(33);
    m_readout_event_TDO1d->SetFillColor(33);
    m_readout_event_TDO1d->GetXaxis()->SetLabelSize(0.04);
    m_readout_event_TDO1d->SetTitleSize(0.06);



    //m_readout_event_time = new TH2D(tEventName, tEventName,640,0,640,300,0,600);
    m_readout_event_time->SetMarkerStyle(7);
    m_readout_event_time->GetXaxis()->SetLabelSize(0.04);
    m_readout_event_time->SetTitleSize(0.06);
    //m_readout_event_time->SetMarkerSize(10);

    //m_readout_statistics_advanced_q = new TH2D(advancedQName, advancedQName,640,0,640,1000,0,2000);
    //m_readout_statistics_advanced_qmax = new TH2D(advancedQmaxName, advancedQmaxName,640,0,640,1000,0,2000);
    // std::cout << "new chip " << m_chip_id.get_string() << " "  << name << std::endl;

    readoutStatisticsHistos.push_back(m_readout_statistics_histo);
    readoutStatisticsHistos.push_back(m_readout_charge_statistics_histo);
    readoutStatisticsHistos.push_back(m_readout_time_statistics_histo);
    readoutStatisticsHistos.push_back(m_readout_PDO_statistics_histo);
    readoutStatisticsHistos.push_back(m_readout_TDO_statistics_histo);

    readoutEventHistos.push_back(m_readout_event_charge);
//    readoutEventHistos.push_back(m_readout_event_PDO1d);
    readoutEventHistos.push_back(m_readout_event_time);

    readout1dHistos.push_back(m_readout_statistics_histo);
    readout1dHistos.push_back(m_readout_charge_statistics_histo);
    readout1dHistos.push_back(m_readout_time_statistics_histo);
    readout1dHistos.push_back(m_readout_event_PDO1d);
    readout1dHistos.push_back(m_readout_event_TDO1d);

    readout2dHistos.push_back(m_readout_event_charge);
    readout2dHistos.push_back(m_readout_event_time);
    //readout2dHistos.push_back(m_readout_statistics_advanced_q);
    //readout2dHistos.push_back(m_readout_statistics_advanced_qmax);


}

CDetReadout::CDetReadout(DetBaseWeakPtr detector, DetBaseWeakPtr parent) :
    CDetBase(detector, parent)
{


}

CDetReadout::~CDetReadout()
{
   delete m_readout_statistics_histo;
}

void CDetReadout::setStrips(std::pair<int,int> strip_range, double pitch)
{
    m_strip_range=strip_range;
    m_pitch = pitch;
    m_readout_statistics_histo->SetBins(strip_range.second-strip_range.first, strip_range.first,strip_range.second+1);
    //m_readout_charge_statistics_histo->SetBins(strip_range.second-strip_range.first,strip_range.first,strip_range.second);
//    /m_readout_time_statistics_histo->SetBins(strip_range.second-strip_range.first,strip_range.first,strip_range.second);
    m_readout_event_charge1d->SetBins(strip_range.second-strip_range.first+1,strip_range.first,strip_range.second+1);
    m_readout_event_PDO1d->SetBins(strip_range.second-strip_range.first+1,strip_range.first,strip_range.second+1);
    m_readout_event_TDO1d->SetBins(strip_range.second-strip_range.first+1,strip_range.first,strip_range.second+1);
    m_readout_event_time->SetBins(strip_range.second-strip_range.first,strip_range.first,strip_range.second+1,400,0,800);
}

void CDetReadout::print() const
{
   std::cout << "(*) Readout ";
   CDetBase::print();
}

std::string CDetReadout::getMyReadoutName()
{
    return m_name;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TH1D* CDetReadout::getStatisticsHisto()
{
    return m_readout_statistics_histo;
}

TH1D* CDetReadout::getChargeStatisticsHisto()
{
    return m_readout_charge_statistics_histo;
}

TH1D* CDetReadout::getPDOStatisticsHisto()
{
    return m_readout_PDO_statistics_histo;
}

TH1D* CDetReadout::getTDOStatisticsHisto()
{
    return m_readout_TDO_statistics_histo;
}

TH1D* CDetReadout::getTimeStatisticsHisto()
{
    return m_readout_time_statistics_histo;
}

TH2D* CDetReadout::getChargeEventHisto()
{
    return m_readout_event_charge;
}

TH1D* CDetReadout::getChargeEventHisto1d()
{
    return m_readout_event_charge1d;
}

TH1D* CDetReadout::getPDOEventHisto1d()
{
    return m_readout_event_PDO1d;
}

TH1D* CDetReadout::getTDOEventHisto1d()
{
    return m_readout_event_TDO1d;
}

TH2D* CDetReadout::getTimeEventHisto()
{
    return m_readout_event_time;
}

TH2D* CDetReadout::getQAdvnacedStatisticsHisto()
{
    return m_readout_statistics_advanced_q;
}

TH2D* CDetReadout::getQmaxAdvnacedStatisticsHisto()
{
    return m_readout_statistics_advanced_qmax;
}


std::vector <TH1D*> CDetReadout::getReadoutStatisticsHistos()
{
    return readoutStatisticsHistos;
}

std::vector <TH2D*> CDetReadout::getReadoutEventHistos()
{
    return readoutEventHistos;
}

void CDetReadout::fill_histogram(std::vector <float> *fill_values_x, std::vector <float> *fill_values_y)
{
    for(size_t iHisto=0; iHisto<fill_values_x->size(); iHisto++)
    {
        m_readout_raw_histo->SetBinContent(fill_values_x->at(iHisto), fill_values_y->at(iHisto));
    }
}

//void CDetReadout::clear_histograms()
//{
//    m_chip_raw_histo->Reset();
//}
