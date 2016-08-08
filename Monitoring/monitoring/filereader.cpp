//
//  filereader.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "filereader.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>


fileReader::fileReader(const char* filename) :
    fileName(filename),
    service(new online::display::CAsioService(1)), readMutex(), readCondition(), dataLine(), file(0)
{
    online::display::CAsioService::MainLoopFunction f = boost::bind(&fileReader::mainLoop, this);
    service->post(f);
//    readingTimer = new QTimer();
//    readingTimer->start(200);
    //connect(readingTimer, SIGNAL(timeout()), this, SLOT());
    fileReader::file = new std::ifstream();
    fileReader::file->open(fileName, std::ifstream::in);
    //file(fileName);
    //std::cout<<"Contruction OK"<<std::endl;

}


fileReader::~fileReader()
{
    service->stopping(true);
    service->stop();
}


void fileReader::setCondition()
{

}

std::string fileReader::getLine()
{
    return dataLine;
}

void fileReader::notify()
{
    readCondition.notify_one();
}


void fileReader::mainLoop()
{
    boost::mutex::scoped_lock lock(readMutex);
    dataLine = "";
    while(!service->stopping()/* && !(file->eof())*/)
    {
        //std::cout<<"Entering while loop"<<std::endl;

        //if(readCondition.timed_wait(lock, boost::posix_time::millisec(200)))
        //{
            //std::cout << "** CPublisherBase::main_loop() notified ! **" << std::endl;

        //}
        //dataLine = readLine();
        //if(readLine()!="")  {
            //emit newLine();

            if(readCondition.timed_wait(lock, boost::posix_time::millisec(1000)))
            {
                //std::cout << "** CPublisherBase::main_loop() notified ! **" << std::endl;

            }
            //dataLine = readLine();
            //if(readLine()!="")  {
                emit drawLine();
        //}
    }
}

std::string fileReader::readLine()
{
    //std::cout<<"Entering read line"<<std::endl;
    std::string line="";
        int lineCounter=0;

          if (fileReader::file->is_open())
          {
            lineCounter=0;
//            std::cout<<"Entering read line 1"<<std::endl;
            while(std::getline(*file,line) && !file->eof())
            {
 //               std::cout<<"Entering read line 2"<<std::endl;

                lineCounter++;
//                std::cout<<line<<std::endl;
                //std::cout<<"++++++++++++++++++++++++LINE "<<lineCounter<<"++++++++++++++++++++++++++"<<std::endl;
                //std::string line_str(line);
                boost::trim(line);
        //        if (boost::starts_with(line_str, "#") || line_str.empty()) {
        //                 continue;
        //        }
                return line;

            }
            fileReader::file->close();
         }
         else std::cout << "Unable to open file or No more lines to read..." << std::endl;

}
