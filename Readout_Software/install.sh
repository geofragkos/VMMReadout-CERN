#!/bin bash

is_mac_=${1}

cd include
git clone -b master https://github.com/dantrim/boost_includes.git boost

rootcint -f aDict.cxx -c a.h LinkDef.h

mkdir ../build/objects
g++ -o ../build/objects/libMylib.so aDict.cxx `root-config --cflags --libs` -shared -fPIC

cd ../build
ln -s objects/libMylib.so .

if [ "${is_mac_}" == "--mac" ]
then
    qmake -spec macx-g++ -o Makefile vmmdcs.pro
    #qmake -spec macx-g++ -o Makefile vmmall.pro
else
    qmake -o Makefile vmmdcs.pro
    #qmake -o Makefile vmmall.pro
fi


#qmake -o Makefile vmmall.pro
