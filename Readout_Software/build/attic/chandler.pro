# ------------------------------------------- #
#
#  confighandler
#
# daniel.joseph.antrim@cern.ch
# March 2016
#
# ------------------------------------------- #

sourcepath      = "../src"
includepath     = "../include"
configpath      = "../include"
boostinclude    = "/usr/local/include"
boostlib        = "/usr/local/lib"


QT += core
CONFIG += console
CONFIG += declarative_debug

TARGET = chandler
TEMPLATE = app

INCLUDEPATH += $$includepath
INCLUDEPATH += $$boostinclude
DEPENDPATH  += $$includepath
DEPENDPATH  += $$boostinclude

CXXFLAGS += -std=c++11

OBJECTS_DIR      += ./objects/
MOC_DIR         += ./moc/
RCC_DIR         += ./rcc/
UI_DIR          += ./ui/

SOURCES         += $$sourcepath/chandler.cpp\
                   $$sourcepath/config_handler.cpp

HEADERS         += $$includepath/config_handler.h


