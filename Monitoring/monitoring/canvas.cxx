//
//  canvas.cxx
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include <QPushButton>
#include <QLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QWidget>
#include <QString>

#include <stdlib.h>

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
#include "canvas.h"
//#include "main_window.h"


//--------------------------------------------------------------------------------

QRootCanvas::QRootCanvas(QWidget *parent) :
             QWidget(parent, 0), fCanvas(0), fNeedResize(kTRUE)
{
   //setMinimumSize(1000, 850);//original (1000,800)
   //setMinimumSize(parent->width(),parent->height());//original (600,500)

   setUpdatesEnabled( kTRUE );
   setMouseTracking(kTRUE);
//   setMouseTracking(kFALSE);

   //setFocusPolicy( TabFocus );
   //setCursor( Qt::crossCursor );

   setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

   int wid = gVirtualX->AddWindow((ULong_t)winId(),parent->width(),parent->height());
   fCanvas = new TCanvas(Form("c%d",std::rand() % 100 + 1),width(),height(),wid);
   fCanvas->Show();
}

void QRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
//   if (fCanvas) {
//      if (e->buttons() & Qt::LeftButton) {
//         fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
//      } else if (e->buttons() & Qt::MidButton) {
//         fCanvas->HandleInput(kButton2Motion, e->x(), e->y());
//      } else if (e->buttons() & Qt::RightButton) {
//         fCanvas->HandleInput(kButton3Motion, e->x(), e->y());
//      } else {
//         fCanvas->HandleInput(kMouseMotion, e->x(), e->y());
//      }
//   }
}

void QRootCanvas::mousePressEvent( QMouseEvent *e )
{
//   if (fCanvas) {
//      switch (e->button()) {
//         case Qt::LeftButton :
//            fCanvas->HandleInput(kButton1Down, e->x(), e->y());
//            break;
//         case Qt::MidButton :
//            fCanvas->HandleInput(kButton2Down, e->x(), e->y());
//            break;
//         case Qt::RightButton :
//            // does not work properly on Linux...
//            fCanvas->HandleInput(kButton3Down, e->x(), e->y());
//          //#ifdef WIN32
////            fCanvas    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
////s->HandleInput(kButton3Down, e->x(), e->y());
////#endif
//            break;
//         default:
//            break;
//      }
//   }
}

void QRootCanvas::mouseReleaseEvent( QMouseEvent *e )
{
//   if (fCanvas) {
//      switch (e->button()) {
//         case Qt::LeftButton :
//            fCanvas->HandleInput(kButton1Up, e->x(), e->y());
//            break;
//         case Qt::MidButton :
//            fCanvas->HandleInput(kButton2Up, e->x(), e->y());
//            break;
//         case Qt::RightButton :
//            // does not work properly on Linux...
//#ifdef WIN32
//            fCanvas->HandleInput(kButton3Up, e->x(), e->y());
//#endif
//            break;
//         default:
//            break;
//      }
//   }



}

void QRootCanvas::resizeEvent( QResizeEvent * )
{

//       // Call QWidget resize and inform the ROOT Canvas.

////       QWidget::resizeEvent( e );
////       fNeedResize=kTRUE;


   if (fCanvas && fNeedResize == kTRUE) {
      fCanvas->Resize();
      fCanvas->Update();
   }
}

void QRootCanvas::paintEvent( QPaintEvent * )
{
//   if (fCanvas) {
//      fCanvas->Resize();
//      //fCanvas->Update();
//   }
}


//void QRootCanvas::setTitle(QString title)
//{


//}
