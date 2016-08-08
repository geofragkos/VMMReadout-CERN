#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <iostream>
#include <string>
#include <tuple>
#include <vector>


//mmdaq
#include "srsconfig.h"

//boost
#include <boost/property_tree/ptree.hpp>

class Connector {
    public :
        Connector();
        bool loadConnector(const boost::property_tree::ptree::value_type pt);

        void print();

        bool loadMapFile(std::string filename);
        bool readMapFile(std::string filename);

        std::string getChipName() { return m_chipName; }
        std::string getName() { return m_name; }
        std::string getId() { return m_id; }
        std::string getMapFileName() { return m_map_filename; }

        #warning the data here is different for VMM than with APV
        std::tuple<int, int, int> getChannel(int chamberStrip, int chip_number);
        std::tuple<int, int, int> getChannelMMFE8(int chamberStrip);
        std::tuple<int, int, int> getPin(int pin); // pinstr, chipstr, chanstr
        std::vector<std::tuple<int,int,int>> data; // pin, chip, chan

    private :
        std::string m_name;
        std::string m_id;
        std::string m_chipName;
        std::string m_map_filename;

}; // class

#endif
