//
//  DetReadout.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetReadout_h
#define online_display_DetReadout_h

#include "DetBase.h"
#include <TCanvas.h>
#include <TRandom.h>
#include <TVirtualX.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFormula.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TFrame.h>
#include <TTimer.h>


namespace online {
   namespace display {
   
      class CDetReadout : public CDetBase
   {
      
   public:
      CDetReadout();
      CDetReadout(DetBaseWeakPtr detector, DetBaseWeakPtr parent);

      CDetReadout(DetBaseWeakPtr detector,
                  DetBaseWeakPtr parent,
                  size_t idnumber,
                  const std::string& name,
                  const Coord3& size,
                  const Coord3& position,
                  const Coord3& rotation);

      virtual ~CDetReadout();
      virtual void configure(CDaqServerConfig* ) {};
      virtual void print() const;
      std::string getMyReadoutName();




      TH1D* getStatisticsHisto();
      TH1D* getChargeStatisticsHisto();
      TH1D* getPDOStatisticsHisto();
      TH1D* getTDOStatisticsHisto();
      TH1D* getTimeStatisticsHisto();

      TH2D* getChargeEventHisto();
      TH1D* getChargeEventHisto1d();
      TH1D* getPDOEventHisto1d();
      TH1D* getTDOEventHisto1d();
      TH2D* getTimeEventHisto();


      TH2D* getQAdvnacedStatisticsHisto();
      TH2D* getQmaxAdvnacedStatisticsHisto();

      std::vector <TH1D*> getReadoutStatisticsHistos();
      std::vector <TH2D*> getReadoutEventHistos();
      void fill_histogram(std::vector <float> *fill_values_x, std::vector <float> *fill_values_y);
      void clear_histogram();

      void setStrips(std::pair<int,int> strip_range, double pitch);

      TH1D* m_readout_raw_histo;
      TH1D* m_readout_statistics_histo;
      TH1D* m_readout_charge_statistics_histo;
      TH1D* m_readout_PDO_statistics_histo;
      TH1D* m_readout_TDO_statistics_histo;
      TH1D* m_readout_time_statistics_histo;

      TH1D *m_readout_event_charge1d;
      TH1D *m_readout_event_PDO1d;
      TH1D *m_readout_event_TDO1d;
      TH2D *m_readout_event_charge;
      TH2D *m_readout_event_time;

      TH2D *m_readout_statistics_advanced_q;
      TH2D *m_readout_statistics_advanced_qmax;

      std::vector <TH1D*> readoutStatisticsHistos;
      std::vector <TH2D*> readoutEventHistos;

      std::vector <TH1D*> readout1dHistos;
      std::vector <TH2D*> readout2dHistos;

      std::pair<int,int> m_strip_range;
      double m_pitch;
   };
      
   }
} // namespace

#endif
