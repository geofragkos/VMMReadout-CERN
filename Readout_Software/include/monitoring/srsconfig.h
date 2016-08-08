#ifndef SRSCONFIG_H
#define SRSCONFIG_H

#include <iostream>
#include <string>

//mmdaq
#include "fec.h"


class SRSConfig {
    public :
        SRSConfig();
        void loadXml(std::string filename);
        int numberOfFecs() { return m_fecArraySize; }
        std::string getFecContainsChipName(std::string chipname);
        std::string getChipNameFromIp(int vmm_id, std::string ip);

        void print();

        std::string getConfigXml() { return m_configXml; }

        #warning why not make a vector of pointers?
        int m_fecArraySize;
        Fec **m_fecArray;

    private :
        bool readXMLfile(const std::string filename);

        std::string m_configXml;


}; // class

#endif
