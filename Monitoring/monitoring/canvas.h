//
//  canvas.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QWidget>
#include <TF1.h>
#include <TH1.h>


class QAction;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QPushButton;
class QEvent;
class QTimer;
class TCanvas;

class QRootCanvas : public QWidget
{
   Q_OBJECT

public:
   QRootCanvas( QWidget *parent = 0);
   virtual ~QRootCanvas() {}

   TCanvas* getCanvas() { return fCanvas;}

protected:
   TCanvas        *fCanvas;
   bool fNeedResize;           // Resize flag


   virtual void    mouseMoveEvent( QMouseEvent *e );
   virtual void    mousePressEvent( QMouseEvent *e );
   virtual void    mouseReleaseEvent( QMouseEvent *e );
   virtual void    paintEvent( QPaintEvent *e );
   virtual void    resizeEvent( QResizeEvent *e );
};

#endif

