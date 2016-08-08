#ifndef CHAMBER_H
#define CHAMBER_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>

//mmdaq
#include "connector.h"
#include "chamberspecs.h"
#include "coordinates.h"


class Chamber {
    public :
        Chamber();
        bool loadChamber(const boost::property_tree::ptree::value_type pt);

        std::string getChipNameFromConnectorName(std::string connectorName);
        Connector& getConnector(std::string connectorName);
        int connectorArraySize() { return m_connectorArraySize; }

        std::string getConfigFile() { return m_chamberConfigFile; }
        std::string getName() { return m_name; }
        std::string driftGap() { return m_drift_gap; }
        int voltageDrift() { return m_voltage_drift; }
        int voltageMesh() { return m_voltage_mesh; }
        int voltageStrips() { return m_voltage_strips; }

        void print();

        ChamberSpecs& getChamberSpecs() { return *m_chamberSpecs; }

        ChamberSpecs* m_chamberSpecs;

        Connector** m_connectorArray;
        int m_connectorArraySize;

    private :

        std::string m_chamberConfigFile;
        std::string m_name;
        std::string m_drift_gap;
        int m_voltage_drift;
        int m_voltage_mesh;
        int m_voltage_strips;
        Coordinates *position;
        Coordinates *rotation;

}; // class

#endif
