//
//  filereader.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef FILEREADER_H
#define FILEREADER_H

#include "AsioService.h"

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QTimer>

//#include <QThread>
#ifndef Q_MOC_RUN

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#endif

class fileReader : public QObject
{
    Q_OBJECT

public:
    fileReader(const char *filename);
    ~fileReader();
    const char *fileName;
    online::display::CAsioService *service;
    boost::condition_variable readCondition;
    boost::mutex readMutex;
    std::string dataLine;
    QTimer *readingTimer;
    std::ifstream *file;


    void setCondition();
    std::string getLine();
    std::string readLine();
    void notify();
    void mainLoop();

signals:
    void newLine();
    void drawLine();
};

#endif // FILEREADER_H
