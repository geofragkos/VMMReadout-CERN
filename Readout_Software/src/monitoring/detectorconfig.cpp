//mmdaq
#include "detectorconfig.h"

//boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////
// DetectorConfig
////////////////////////////////////////////////////////////////////////
DetectorConfig::DetectorConfig() :
    m_configXml(""),
    n_chamberArraySize(0),
    m_chamberArray(NULL),
    position(NULL),
    rotation(NULL)
{
}

void DetectorConfig::loadXml(string filename)
{
    // already should have checked file and should be providing full filepath
    if(!readXMLfile(filename)) {
        cout << "DetectorConfig::loadXml    Exitting." << endl;
        exit(1);
    }

    cout << "DetectorConfig::loadXml    INFO Detector configuration loaded from file: " << filename << endl;

    m_configXml = filename;
}

bool DetectorConfig::readXMLfile(const string filename)
{
    cout << "IN DetectorConfig::readXMLfile" << endl;
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost::property_tree::xml_parser;
    ptree pt;

    read_xml(filename, pt, trim_whitespace | no_comments);

    int n_chamber_counter = 0;
    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.get_child("detector")) {
            if(v.first == "chamber") {
                cout << "IN FOREACH" << endl;
                n_chamber_counter++;
            }
        }
    }
    catch(std::exception &e)
    {
        stringstream sx;
        sx << "DetectorConfig::readXMLfile    ERROR: " << e.what() << endl;
        cout << sx.str() << endl;
        ok = false;
    }

    if(ok) {
        n_chamberArraySize = n_chamber_counter;
        m_chamberArray = new Chamber*[n_chamber_counter];

        int n_chamber = 0;

        BOOST_FOREACH(const ptree::value_type &v, pt.get_child("detector")) {
            //////////////////////////////////////////////
            // load the chamber
            //////////////////////////////////////////////
            if(v.first == "chamber") {
                try {
                    #warning parent path not needed with environment var
                    //m_chamberArray[n_chamber] = new Chamber(v /*,parent_path*/);
                    m_chamberArray[n_chamber] = new Chamber();
                    if(!m_chamberArray[n_chamber]->loadChamber(v)) ok = false;
                }
                catch(std::exception &e) {
                    cout << "DetectorConfig::readXMLfile    ERROR creating chamber["<<n_chamber<<"]" << endl;
                    ok = false;
                    throw;
                }
                n_chamber++;
            } // chamber

            //////////////////////////////////////////////
            // comment line
            //////////////////////////////////////////////
            else if(v.first == "comment") {
                try {
                    cout << "DetectorConfig::readXMLfile    " << v.first << endl;
                }
                catch(std::exception &e) {
                    cout << "DetectorConfig::readXMLfile    ERROR reading " << v.first << " node" << endl;
                }
            } // comment

            //////////////////////////////////////////////
            // coordinates
            //////////////////////////////////////////////
            else if(v.first == "position") {
                try {
                    position = new Coordinates(v.second.get<double>("x"),
                                                v.second.get<double>("y"),
                                                v.second.get<double>("z"));
                }
                catch(std::exception &e) {
                    cout << "DetectorConfig::readXMLfile    ERROR reading position node" << endl;
                    ok = false;
                    throw;
                }
            } // position
            else if(v.first == "rotation") {
                try {
                    rotation = new Coordinates(v.second.get<double>("x"),
                                                v.second.get<double>("y"),
                                                v.second.get<double>("z"));
                }
                catch(std::exception &e) {
                    cout << "DetectorConfig::readXMLfile    ERROR reading rotation node" << endl;
                    ok = false;
                    throw;
                }
            } // rotation
            else if(v.first == "<xmlattr>") {
                continue;
            }
            else if(v.first == "<xmlcomment>") {
                continue;
            }

            else {
                cout << "DetectorConfig::readXMLfile    WARNING Unknown key: " << v.first
                     << " in detector config file: " << m_configXml << endl;
                ok = false;
            }
        } //foreach

    } // ok

    return ok;

}
