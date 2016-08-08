QT += core
QT += network
QT += xml

includepath_="../include"
sourcepath_ ="../source"

CONFIG += console
CONFIG+=declarative_debug

TARGET = vmmdata
TEMPLATE = app

INCLUDEPATH += $(ROOTSYS)/include
LIBS += -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lNet \
            -lHist -lGraf -lGraf3d -lGpad -lTree \
            -lRint -lPostscript -lMatrix -lPhysics \
            -lGui -lRGL -lMathCore

INCLUDEPATH += $$includepath_/
DEPENDPATH  += $$includepath_/

OBJECTS_DIR += ./objects/
MOC_DIR += ./moc/
RCC_DIR += ./rcc/
UI_DIR += ./ui/

SOURCES += $$sourcepath_/vmmdata.cpp\
           $$sourcepath_/dataprocessor.cpp

HEADERS += $$includepath_/dataprocessor.h

