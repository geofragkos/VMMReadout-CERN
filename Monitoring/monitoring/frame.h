//
//  frame.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QTabWidget>
#include <QList>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <canvas.h>

#include <QLayout>
#include <vector>

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QPushButton;
class QTimer;
class TCanvas;


class frame : public QWidget
{
    Q_OBJECT

public:
    frame(QWidget *parent = 0);
    virtual ~frame() {}

    std::string frameType;
    std::vector <TH1D*> frameStatisticsHistos;
    std::vector <TH2D*> frameEventHistos;

    QRootCanvas    *frameCanvas;
    QTabWidget *frameTab;
    QTimer *fRootTimer;
    QVBoxLayout *frameLayout;
    std::vector <TH1D*> getFrameStatisticsHistos();
    std::vector <TH2D*> getFrameEventHistos();

    QRootCanvas *getFrameCanvas();
    QTabWidget *getFrameTab();
    void attachFrameToTab(QTabWidget* tab);

    void draw1dHistogramInCanvas(TH1D *histoToDraw, int canvasIndex);
    void draw2dHistogramInCanvas(TH2D *histoToDraw, int canvasIndex);

public slots:
   void handle_root_events();
   virtual void divideCanvases(int height, int width);

private:



protected:

};

#endif // FRAME_H
