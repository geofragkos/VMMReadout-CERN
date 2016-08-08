#-------------------------------------------
#
# vmmrun
#
# daniel.joseph.antrim@cern.ch
# March 2016
#
#-------------------------------------------

sourcepath     ="../src"
includepath    = "../include"
configpath     = "../include"

QT += core
CONFIG += console
CONFIG += declarative_debug

TARGET = vmmrun
TEMPLATE = app

INCLUDEPATH     += $$includepath/
DEPENDPATH      += $$includepath/

OBJECTS_DIR     += ./objects/
MOC_DIR         += ./moc/
RCC_DIR         += ./rcc/
UI_DIR          += ./ui/

SOURCES         += $$sourcepath/vmmrun.cpp\
                   $$sourcepath/run_module.cpp

HEADERS         += $$includepath/run_module.h

