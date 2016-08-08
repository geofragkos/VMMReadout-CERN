QT += core
QT += network
QT += xml

includepath_="../include"
sourcepath_ ="../source"

CONFIG += console
CONFIG+=declarative_debug

TARGET = vmmconfig
TEMPLATE = app

INCLUDEPATH += $$includepath_/
DEPENDPATH  += $$includepath_/

OBJECTS_DIR += ./objects/
MOC_DIR += ./moc/
RCC_DIR += ./rcc/
UI_DIR += ./ui/

SOURCES += $$sourcepath_/vmmconfig.cpp\
           $$sourcepath_/configuration_module.cpp


HEADERS  += $$includepath_/configuration_module.h

