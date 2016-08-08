// qt
#include <QtCore/QCoreApplication>
#include <QFileInfo> // FileExists
#include <QtCore>

// vmmrun
#include "run_module.h"

// std/stl
#include <iostream>
using namespace std;

void help()
{
    qDebug() << " ---------------------------------------------------------------- ";
    qDebug() << " vmmrun usage : ";
    qDebug() << "   $ ./vmmrun -i (--input-xml) <path-to-xml-file> [options]";
    qDebug() << " options :";
    qDebug() << "    --daq              : (required) input daq configuration xml";
    qDebug() << "    --config           : (required) input vmm configuration xml";
    qDebug() << "    -o / --output-name : (optional) provide an output filename";
    qDebug() << "                         to be used instead of the one defined";
    qDebug() << "                         in the input-xml file";
    qDebug() << "    --test-mode        : (optional) toggle test mode on";
    qDebug() << "    -d / --dbg         : (optional) toggle debug mode on";
    qDebug() << " example :";
    qDebug() << "   $ ./vmmrun -i config.xml -o testrun.txt --test-mode"; 
    qDebug() << " ---------------------------------------------------------------- ";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QString inputDAQFile("");
    QString inputConfigFile("");
    bool dbg = false;
    bool testMode = false;
    bool useCustomName = false;
    bool writeEvent = false;
    QString customName("");

    // ---------------------------------------
    // take in command line
    int optin(1);
    while(optin < argc) {
        string cmd = argv[optin];
        if(cmd == "--daq") { inputDAQFile = argv[++optin]; }
        else if(cmd == "--config") { inputConfigFile = argv[++optin]; }
        else if(cmd == "-o" || cmd == "--output-name") { useCustomName = true; customName = argv[++optin]; } 
        else if(cmd == "--test-mode") { testMode = true; }
        else if(cmd == "-d" || cmd == "--dbg") { dbg = true; }
        else if(cmd == "-h" || cmd == "--help") { help(); exit(0); }
        else if(cmd == "-w" || cmd == "--writeEvent") { writeEvent = true; }
        else {
            qDebug() << "Incorrect command-line options! Printing expected usage below.";
            help();
            exit(1);
        }
        optin++;
    } // while optin
    if(inputDAQFile == "") {
        qDebug() << "You did not provide an input DAQ configuration file. Use the \"--daq\" command line option to specify this.";
        help();
        exit(1);
    }
    else if(inputConfigFile == "") {
        qDebug() << "You did not provide an input VMM configuration file. Use the \"--config\" command line option to specify this.";
        help();
        exit(1);
    }
    else {
        QFileInfo checkDAQFile(inputDAQFile);
        if(!(checkDAQFile.exists() && checkDAQFile.isFile())) {
            qDebug() << "The provided input DAQ configuration file (" << inputDAQFile << ") is not found. Please check that the path, etc... are correct.";
            help();
            exit(1);
        }
        QFileInfo checkConf(inputConfigFile);
        if(!(checkConf.exists() && checkConf.isFile())) {
            qDebug() << "The provided input vmm configuration file (" << inputConfigFile << ") is not found. Please check that the path, etc... are correct.";
            help();
            exit(1);
        }
    }

    // ---------------------------------------
    // begin DAQ
    RunDAQ daq;
    // if the user provided an output name, set this
    if(useCustomName) {
        if(customName == "") { qDebug() << "User-provided output name is \"\". Aborting."; help(); abort(); }
        daq.SetOutputFileName(customName);
    }
    daq.SetDebugMode(dbg);
    daq.SetTestMode(testMode);
    daq.SetWriteEvent(writeEvent);
    // process the input configuration file
    daq.ReadRFile(inputConfigFile);
    // process the input DAQ configuration file
    daq.getDAQConfig(inputDAQFile);
    // set up the acquisition mode
    daq.PrepareRun();

    QObject::connect(&daq, SIGNAL(ExitLoop()), &app, SLOT(quit()));
    QMetaObject::invokeMethod(&daq, "Run", Qt::QueuedConnection); //RunDAQ::Run() called as soon as application enters main event loop

    return app.exec();
}
