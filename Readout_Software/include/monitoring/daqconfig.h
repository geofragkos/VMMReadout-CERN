#ifndef DAQCONFIG_H
#define DAQCONFIG_H

#include <iostream>
#include <string>

//mmdaq
#include "srsconfig.h"
#include "detectorconfig.h"


class DaqConfig {
    public :
        DaqConfig();
        void loadXml(std::string filename);

        SRSConfig& getSrsConfig() { return *m_srs; }
        DetectorConfig& getDetectorConfig() { return *m_detector; }


    private :
        bool readXMLfile(const std::string filename);

        std::string m_daqConfigXml;
        std::string m_srsConfigXml;
        std::string m_detectorConfigXml;

        SRSConfig* m_srs;
        DetectorConfig* m_detector;

}; // class

#endif
