//
//  main.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qimage.h>
#include <qtimer.h>
#include <QFont>
//#include "canvas.h"

#include "frame.h"
#include "mainwindow.h"
#include "daqclient.h"
#include "DaqServerConfig.h"

#include <boost/asio.hpp>

#include <iostream>
#include <exception>

#include <stdlib.h>
#include <TApplication.h>
#include <TSystem.h>

//using namespace mmdaq1::client;
using namespace online::display;


int main(int argc, char** argv)
{


    TApplication rootapp("mmDaq Online Monitoring", &argc, argv);
    QApplication app(argc,argv);

    //++++++++++++++++++specify a new font+++++++++++++++++++++++++++++++++++
    //QFont newFont("Sans", 6, QFont::Bold, true);
    //set font of application
    //app.setFont(newFont);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    app.setWindowIcon(QIcon("media/display.png"));

    CDaqServerConfig *displayConfigurator = new CDaqServerConfig();
    displayConfigurator->displayStartupWindow();

    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );

    return app.exec();
}
