#include "chamber.h"

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

////////////////////////////////////////////////////////////////////////
// CHAMBER
////////////////////////////////////////////////////////////////////////
Chamber::Chamber() :
    m_chamberConfigFile(""),
    m_name(""),
    m_drift_gap(""),
    m_voltage_drift(-1),
    m_voltage_mesh(-1),
    m_voltage_strips(-1),
    position(NULL),
    rotation(NULL)
{
} 

bool Chamber::loadChamber(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    int n_connector_count = 0;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            if(v.first == "connector") n_connector_count++;
        } // foreach
    } // try
    catch(std::exception& e) {
        cout << "Chamber::loadChamber    ERROR loading chamber: " << e.what() << endl;
        ok = false;
    }

    if(ok) {
        m_connectorArray = new Connector*[n_connector_count];
        m_connectorArraySize = n_connector_count;

        int n_connector = 0;

        try {
            BOOST_FOREACH(const ptree::value_type &v, pt.second) {

                ////////////////////////////////////////////
                // chamber spec file
                ////////////////////////////////////////////
                if(v.first == "config_file") {
                    m_chamberConfigFile = v.second.data();
                    m_chamberSpecs = new ChamberSpecs();
                    m_chamberSpecs->loadSpecs(m_chamberConfigFile);
                }
                ////////////////////////////////////////////
                // drift gap (mm)
                ////////////////////////////////////////////
                else if(v.first == "drift_gap") {
                    m_drift_gap = v.second.data();
                    trim(m_drift_gap);
                }
                ////////////////////////////////////////////
                // voltages
                ////////////////////////////////////////////
                else if(v.first == "voltage") {
                    m_voltage_drift = v.second.get<int>("drift");
                    m_voltage_mesh = v.second.get<int>("mesh");
                    m_voltage_strips = v.second.get<int>("strips");
                }
                ////////////////////////////////////////////
                // position/rotation
                ////////////////////////////////////////////
                else if(v.first == "position") {
                    position = new Coordinates(v.second.get<double>("x"),
                                                v.second.get<double>("y"),
                                                v.second.get<double>("z"));
                }
                else if(v.first == "rotation") {
                    rotation = new Coordinates(v.second.get<double>("x"),
                                                v.second.get<double>("y"),
                                                v.second.get<double>("z"));
                }
                ////////////////////////////////////////////
                // connector for this chamber
                ////////////////////////////////////////////
                else if(v.first == "connector") {
                    m_connectorArray[n_connector] = new Connector();
                    if(!m_connectorArray[n_connector]->loadConnector(v)) ok = false;
                    n_connector++; 
                }
                ////////////////////////////////////////////
                // name
                ////////////////////////////////////////////
                else if(v.first == "<xmlattr>") {
                    m_name = v.second.get<string>("name");
                }
                else if(v.first == "<xmlcomment>"){
                    continue;
                }

                else {
                    cout << "Chamber::loadChamber    WARNING Unknown key: " << v.first << endl;
                    ok = false;
                }
            } //foreach
        } //try
        catch(std::exception& e) {
            cout << "Chamber::loadChamber    ERROR parsing chamber file: " << e.what() << endl;
            ok = false;
        }
    } // ok

    return ok;
}

string Chamber::getChipNameFromConnectorName(string connectorName)
{
    string out = "";
    for(int icon = 0; icon < m_connectorArraySize; icon++) {
        if(!connectorName.compare(m_connectorArray[icon]->getName())) {
            out = m_connectorArray[icon]->getChipName();
        }
    } //icon
    return out;
}

Connector& Chamber::getConnector(string connectorName)
{
    Connector* c = NULL;
    for(int i = 0; i < m_connectorArraySize; i++) {
        if(!connectorName.compare(m_connectorArray[i]->getName())) {
            c = m_connectorArray[i];
        }
    } // i
    return *c;
}

void Chamber::print()
{
    cout << "-----------------------------------" << endl;
    cout << " CHAMBER " << endl;
    cout << "   > config file       : " << getConfigFile() << endl;
    cout << "   > name              : " << getName() << endl;
    cout << "   > drift gap         : " << driftGap() << endl;
    cout << "   > voltage drift     : " << voltageDrift() << endl;
    cout << "   > voltage mesh      : " << voltageMesh() << endl;
    cout << "   > voltage strips    : " << voltageStrips() << endl;

    getChamberSpecs().print();
    for(int icon = 0; icon < m_connectorArraySize; icon++) {
        m_connectorArray[icon]->print();
    }
    

}
