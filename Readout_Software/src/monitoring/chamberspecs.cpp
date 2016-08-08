#include "chamberspecs.h"

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

//std/stl
#include <stdlib.h> //getenv

using namespace std;

////////////////////////////////////////////////////////////////////////
// CHAMBERSPECS
////////////////////////////////////////////////////////////////////////
ChamberSpecs::ChamberSpecs() :
    m_configXml(""),
    m_name(""),
    m_id(""),
    m_multilayer(NULL)
{
}

bool ChamberSpecs::loadSpecs(string filename)
{
    bool ok = true;

    char* mmdaqpath;
    mmdaqpath = getenv("MMDAQDIR");
    if(mmdaqpath==NULL) {
        cout << "ChamberSpecs::loadSpecs    ERROR Environment \"MMDAQDIR\" not found!" << endl;
        cout << "ChamberSpecs::loadSpecs       -> Set this with \"set_daqenv.sh\" script." << endl;
        exit(1);
    }
    string path(mmdaqpath);

    string spacer = "";
    if(!boost::algorithm::ends_with(path, "/")) {
        spacer = "/";
    }
    path = path + spacer;
    string full_filename = path + "readout_configuration/" + filename;
    bool exists = std::ifstream(full_filename).good();
    if(!exists) {
        cout << "ChamberSpecs::loadSpecs    ERROR Unable to find spec file: \"" << filename << "\"!" << endl;
        cout << "ChamberSpecs::loadSpecs    ERROR Looking in full path: " << full_filename << "." << endl;
        exit(1);
    }
    cout << "ChamberSpecs::loadSpecs    INFO Found input spec file: " << full_filename << endl;

    m_configXml = full_filename;

    if(!readXMLfile(full_filename)) {
        cout << "ChamberSpecs::loadSpecs    ERROR Unable to read spec config" << endl;
        ok = false;
    }

    return ok;
}

bool ChamberSpecs::readXMLfile(string filename)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    ptree pt;
    read_xml(filename, pt);

    int n_connector_count = 0;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.get_child("chamber")) {
            if(v.first=="connector") n_connector_count++;
        }

    } //try
    catch(std::exception &e) {
        cout << "ChamberSpecs::readXMLfile    ERROR reading config xml: " << e.what() << endl;
        ok = false;
    }

    if(ok) {
        m_connectorArraySize = n_connector_count;
        m_connectorArray = new ConnectorSpecs*[n_connector_count];

        int n_con = 0;

        try {
            BOOST_FOREACH(const ptree::value_type &v, pt.get_child("chamber")) {
                ////////////////////////////////////////////
                // name
                ////////////////////////////////////////////
                if(v.first == "name") {
                    m_name = v.second.data();
                    trim(m_name);
                }
                ////////////////////////////////////////////
                // connector
                ////////////////////////////////////////////
                else if(v.first == "connector") {
                    m_connectorArray[n_con] = new ConnectorSpecs();
                    if(!m_connectorArray[n_con]->loadSpecs(v)) ok = false;
                    n_con++;
                }
                ////////////////////////////////////////////
                // multilayer
                ////////////////////////////////////////////
                else if(v.first == "multilayer") {
                    m_multilayer = new MultiLayer();
                    if(!m_multilayer->loadLayer(v)) ok = false;
                }
                ////////////////////////////////////////////
                // id
                ////////////////////////////////////////////
                else if(v.first == "<xmlattr>") {
                    m_id = v.second.get<string>("id");
                    trim(m_id);
                }

                else if(v.first == "<xmlcomment>") {
                    continue;
                } 

                else {
                    cout << "ChamberSpecs::readXMLfile    WARNING Unknown key: " << v.first << endl;
                    ok = false;
                }
            }//foreach

        } // try
        catch(std::exception &e) {
            cout << "ChamberSpecs::readXMLfile    ERROR reading config xml [2]: " << e.what() << endl;
            ok = false;
        }
    } // ok

    return ok;

}

void ChamberSpecs::print()
{
    cout << "-----------------------------------" << endl;
    cout << "  CHAMBERSPECS " << endl;
    cout << "     > config xml      : " << m_configXml << endl;
    cout << "     > name            : " << m_name << endl;
    cout << "     > id              : " << m_id << endl;
    for(int i = 0; i < m_connectorArraySize; i++) {
        m_connectorArray[i]->print();
    }
}

