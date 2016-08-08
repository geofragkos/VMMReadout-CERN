//
//  frame.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "frame.h"
#include "canvas.h"
#include <QPushButton>
#include <QLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QWidget>
#include <QDebug>
#include "mainwindow.h"

#include <stdlib.h>

#include <TCanvas.h>
#include <TRandom.h>
#include <TVirtualX.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TFormula.h>
#include <TF1.h>
#include <TH1.h>
#include <TFrame.h>
#include <TTimer.h>


frame::frame(QWidget *parent) : QWidget(parent),
    frameType(), frameStatisticsHistos(0), frameEventHistos(0), frameCanvas(), frameTab()
{    

   //frameTab = parent;
    frameCanvas = new QRootCanvas(parent);
    frameCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
 //   frameCanvas->setUpdatesEnabled(kTRUE);
 //   frameLayout = new QVBoxLayout();
 //   frameLayout->addWidget(frameCanvas);
 //   this->setLayout(frameLayout);
    //frameCanvas->resize(parent->sizeHint());
    //attachFrameToTab();
    //rndm = new TRandom();
//   canvas = new QRootCanvas(this);

   //h1f = new TH1D("h1f","Test random numbers Gaus",200,-10,10);
   //h1f->SetFillColor(45);
   //h2f = new TH1D("h2f","Test random numbers Landau",200,-10,10);
   //h2f->SetFillColor(53);

   //divideCanvases(1,2);
//   clickedGaus(1);

 //  GausFit_clicked(1);
 //  clickedLandau(2);
   // LandauFit_clicked(2);
//   QVBoxLayout *l = new QVBoxLayout(this);
//   l->addWidget(canvas);
   //prevents program from crashing when clicking the x button
   fRootTimer = new QTimer(this);
   QObject::connect( fRootTimer, SIGNAL(timeout()), this, SLOT(handle_root_events()) );
   //fRootTimer->start( 20 );
}

//--------------------------------------------------------------------------------

std::vector <TH1D*> frame::getFrameStatisticsHistos()
{
    return frameStatisticsHistos;
}

std::vector <TH2D*> frame::getFrameEventHistos()
{
    return frameEventHistos;
}


QRootCanvas *frame::getFrameCanvas()
{
    return frameCanvas;
}

QTabWidget *frame::getFrameTab()
{
    return frameTab;
}

void frame::handle_root_events()
{
   //call the inner loop of ROOT
   gSystem->ProcessEvents();
}

void frame::divideCanvases(int height, int width)
{
    frameCanvas->getCanvas()->Clear();
    frameCanvas->getCanvas()->Divide(height, width);
    frameCanvas->setUpdatesEnabled(kTRUE);
    //frameCanvas->getCanvas()->GetFrame()->SetFillColor(42);
    //frameCanvas->getCanvas()->GetFrame()->SetBorderMode(-1);
    //frameCanvas->getCanvas()->GetFrame()->SetBorderSize(0);
    //frameCanvas->getCanvas()->Modified();
    //frameCanvas->getCanvas()->Update();

}

void frame::attachFrameToTab(QTabWidget *tab)
{
    QVBoxLayout *l = new QVBoxLayout(tab);
    l->addWidget(frameCanvas);
}

void frame::draw1dHistogramInCanvas(TH1D *histoToDraw, int canvasIndex)
{
    if(canvasIndex !=0/* && histoToDraw->GetEntries()!=0*/)    {
        frameCanvas->getCanvas()->cd(canvasIndex);
        gStyle->SetOptStat(0);
        frameCanvas->getCanvas()->cd(canvasIndex)->SetGrid(0,0);
        histoToDraw->Draw();
        frameCanvas->getCanvas()->Update();
    }
    else std::cout<<"Canvas index : "<<canvasIndex<<std::endl;
}

void frame::draw2dHistogramInCanvas(TH2D *histoToDraw, int canvasIndex)
{
    if(canvasIndex !=0/* && histoToDraw->GetEntries()!=0*/)    {
        frameCanvas->getCanvas()->cd(canvasIndex);
        frameCanvas->getCanvas()->SetGrid(1,1);
        histoToDraw->Draw();
        frameCanvas->getCanvas()->Update();
    }
    else std::cout<<"Canvas index : "<<canvasIndex<<std::endl;
}
