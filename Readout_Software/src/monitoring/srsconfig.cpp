#include "srsconfig.h"

//boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////
// SRSConfig
////////////////////////////////////////////////////////////////////////
SRSConfig::SRSConfig() :
    m_configXml(""),
    m_fecArraySize(0),
    m_fecArray(NULL)
{
}

void SRSConfig::loadXml(string filename)
{
    // already should have checked file and should be providing full filepath
    if(!readXMLfile(filename)) {
        cout << "SRSConfig::loadXml    Exitting." << endl;
        exit(1);
    }

    cout << "SRSConfig::loadXml    INFO SRS configuration loaded from file: " << filename << endl;

    m_configXml = filename;
}

bool SRSConfig::readXMLfile(const string filename)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost::property_tree::xml_parser;
    ptree pt;

    read_xml(filename, pt, trim_whitespace | no_comments);

    int fec_counter = 0;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.get_child("srs")) {
            if(v.first == "fec") {
                fec_counter++;
            }
        }
    }
    catch(std::exception &e)
    {
        stringstream sx;
        sx << "SRSConfig::readXMLfile    ERROR: " << e.what() << endl;
        cout << sx.str() << endl;
        ok = false;
    }

    m_fecArraySize = fec_counter;
    m_fecArray = new Fec*[fec_counter];
    int n_fec = 0;

    if(ok) {
        BOOST_FOREACH(const ptree::value_type &v, pt.get_child("srs")) {
            if(v.first == "fec") {
                try {
                    cout << "SRSConfig::readXMLfile    at fec[" << n_fec << "] " << endl;
                    m_fecArray[n_fec] = new Fec();
                    if(!m_fecArray[n_fec]->loadFec(v)) ok = false;
                    #warning previous version sets f++ in array index??
                }
                catch(std::exception &e) {
                    cout << "SRSConfig::readXMLfile    ERROR creating FEC[" << n_fec << "]" << endl;
                    throw;
                }
                n_fec++;
            } // fec
            // previous version had more children, e.g. 'chip_hybrid_maps'
            //else if {

            //}
            else {
                cout << "SRSConfig::readXMLfile    WARNING Unknown key: " << v.first << " in SRS config file: "
                     << m_configXml << endl;
                ok = false;
            }
        } // srs child loop

    }

    for(int ifec = 0; ifec < fec_counter; ifec++) {
        m_fecArray[ifec]->print();
    }

    return ok;
}
string SRSConfig::getFecContainsChipName(string chipname)
{
    string out = "";
    for(int i = 0; i < m_fecArraySize; i++) {
        for(int j = 0; j < m_fecArray[i]->m_chipArraySize; j++) {
            if(!m_fecArray[i]->m_chipArray[j]->getName().compare(chipname)) {
                out = m_fecArray[i]->getId();
            }
        } // j
    } // i
    return out;
}
string SRSConfig::getChipNameFromIp(int vmm_id, string ip)
{
    // provide a vmmid (expecting [1...8]) and the IP from the UDP packet
    // and get the full chip name, e.g. VMM.1.2, given the input of 
    // 2 and 198.162.0.2

    string out_name = "";
    for(int i = 0; i < m_fecArraySize; i++) {
        if(!(ip==m_fecArray[i]->getIp())) continue;
        for(int j = 0; j < m_fecArray[i]->m_chipArraySize; j++) {
            string chipname = m_fecArray[i]->m_chipArray[j]->getName();
            vector<string> chip_name_split;
            boost::split(chip_name_split, chipname, boost::is_any_of("."));
            int chip_number_j = stoi(chip_name_split.back());
            if(chip_number_j == vmm_id) out_name = chipname;
        } // j
    }//i
    return out_name;
}


void SRSConfig::print()
{
    cout << "-------------------------------------------" << endl;
    cout << "  Print SRS " << endl;
    cout << "    > srs config file: " << m_configXml << endl;
    cout <<"     > # fecs loaded  : " << m_fecArraySize << endl;
    cout << "- - - - - - - - - - - - - - - - - - - - - -" << endl;
    for(int ifec = 0; ifec < m_fecArraySize; ifec++) {
        m_fecArray[ifec]->print();
    }
}

