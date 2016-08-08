#ifndef CHAMBER_SPECS_H
#define CHAMBER_SPECS_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>

//mmdaq
#include "multilayer.h"
#include "connectorspecs.h"
#include "coordinates.h"

class ChamberSpecs {
    public :
        ChamberSpecs();
        bool loadSpecs(std::string filename);

        int getConnectorArraySize() { return m_connectorArraySize; }

        void print();

        ConnectorSpecs** m_connectorArray;
        int m_connectorArraySize;

    private :

        bool readXMLfile(const std::string filename);

        std::string m_configXml;

        std::string m_name;
        std::string m_id;
        MultiLayer *m_multilayer;

}; //class

#endif
