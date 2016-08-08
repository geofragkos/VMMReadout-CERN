#-----------------------------------
#
# QT project - vmmrun
#
# daniel.joseph.antrim@cern.ch
# October 2015
#
# ---------------------------------


sourcepath_="../source"
headerpath_="../include"
configpath_="../include"

QT += core
QT += network
QT += xmlpatterns
QT += xml
CONFIG += console
CONFIG += declarative_debug

TARGET = vmmrun
TEMPLATE = app

INCLUDEPATH += $(ROOTSYS)/include
LIBS += -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lNet \
            -lHist -lGraf -lGraf3d -lGpad -lTree \
            -lRint -lPostscript -lMatrix -lPhysics \
            -lGui -lRGL -lMathCore
LIBS += -L./objects -lMylib


INCLUDEPATH += $$headerpath_/
DEPENDPATH  += $$headerpath_/

OBJECTS_DIR += ./objects/
MOC_DIR += ./moc/
RCC_DIR += ./rcc/
UI_DIR += ./ui/

LIBS += ./objects/configuration_module.o

SOURCES +=  $$sourcepath_/vmmrun.cpp\
            $$sourcepath_/run_module.cpp\
            $$sourcepath_/dataprocessor.cpp

HEADERS += $$headerpath_/run_module.h\
           $$headerpath_/configuration_module.h\
           $$headerpath_/dataprocessor.h
