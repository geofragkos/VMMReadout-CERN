#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T21:05:15
#
#-------------------------------------------------

QT       += core gui

CONFIG +=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appWindow
TEMPLATE = app
LIBS += -L/usr/local/opt/boost/lib -lboost_thread-mt -lboost_filesystem -lboost_system

boostinclude="/Users/dantrim/boost_1_60_0/"
INCLUDEPATH+=$$boostinclude

SOURCES += main.cpp\
        mainwindow.cpp\
        testsharedmem.cpp

HEADERS  += mainwindow.h \
            testsharedmem.h\
            generalheader.h

FORMS    += mainwindow.ui
