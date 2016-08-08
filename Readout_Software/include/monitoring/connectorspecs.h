#ifndef CONNECTORSPECS_H
#define CONNECTORSPECS_H

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

//boost
#include <boost/property_tree/ptree.hpp>

class ConnectorSpecs {

    public :
        ConnectorSpecs();
        bool loadSpecs(const boost::property_tree::ptree::value_type pt);
        bool loadMapFile(std::string filename);
        bool readMapFile(std::string filename);

        std::string getName() { return m_name; }
        std::string getId() { return m_id; }
        std::string getChipName() { return m_chipName; }
        std::string getMapFileName() { return m_map_filename; }

        std::vector<std::tuple<int, int, int, std::string, int> > data; // pin, multilayer, layer, readout, strip

        void print();

    private :
        std::string m_name;
        std::string m_id;
        std::string m_chipName;
        std::string m_map_filename;

}; // class

#endif
