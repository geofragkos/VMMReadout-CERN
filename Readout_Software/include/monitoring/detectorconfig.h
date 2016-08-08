#ifndef DETECTORCONFIG_H
#define DETECTORCONFIG_H

#include <iostream>
#include <string>

//mmdaq
#include "chamber.h"
#include "coordinates.h"


class DetectorConfig {
    public :
        DetectorConfig();
        void loadXml(std::string filename);

        int getChamberArraySize() { return n_chamberArraySize; }

        std::string getConfigXml() { return m_configXml; }


        int n_chamberArraySize;
        #warning make this_ a vector and implement a getter
        Chamber** m_chamberArray;

    private :

        bool readXMLfile(const std::string filename);

        std::string m_configXml;

        Coordinates* position;
        Coordinates* rotation;



}; // class

#endif
