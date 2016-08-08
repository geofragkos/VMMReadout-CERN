#-------------------------------------------------
#
# Project created by QtCreator 2012-06-18T11:31:24
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += widgets
QT += xml

includepath_="../include"
sourcepath_= "../source"
 
CONFIG+=declarative_debug

TARGET = vmmdcs
TEMPLATE = app

INCLUDEPATH += $(ROOTSYS)/include
win32:LIBS += -L$(ROOTSYS)/lib -llibCint -llibRIO -llibNet \
       -llibHist -llibGraf -llibGraf3d -llibGpad -llibTree \
       -llibRint -llibPostscript -llibMatrix -llibPhysics \
       -llibGui -llibRGL -llibMathCore
else:LIBS += -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lNet \
       -lHist -lGraf -lGraf3d -lGpad -lTree \
       -lRint -lPostscript -lMatrix -lPhysics \
       -lGui -lRGL -lMathCore

INCLUDEPATH += $$includepath_/
DEPENDPATH  += $$includepath_/

OBJECTS_DIR += ./objects/
MOC_DIR += ./moc/
RCC_DIR += ./rcc/
UI_DIR += ./ui/

LIBS += -L./objects -lMylib
LIBS += ./objects/configuration_module.o

SOURCES += $$sourcepath_/main.cpp\
           $$sourcepath_/mainwindow.cpp \
           $$sourcepath_/sleeper.cpp \
           $$sourcepath_/mythread.cpp \
           $$sourcepath_/tcphandlerthread.cpp \
           $$sourcepath_/tcphandler.cpp \
           $$sourcepath_/run_module.cpp \
           $$sourcepath_/dataprocessor.cpp

HEADERS  += $$includepath_/mainwindow.h \
            $$includepath_/constants.h \
            $$includepath_/sleeper.h \
            $$includepath_/mythread.h \
            $$includepath_/tcphandlerthread.h \
            $$includepath_/tcphandler.h \
            $$includepath_/rootSpecific.h \
		    $$includepath_/configuration_module.h \
            $$includepath_/run_module.h \
            $$includepath_/dataprocessor.h
    #mydict.h

FORMS    += $$sourcepath_/mainwindow.ui

RESOURCES += \
    $$sourcepath_/icons.qrc \
    $$sourcepath_/calibration_data.qrc
