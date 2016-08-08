#include "fec.h"

//boost
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////
// FEC
////////////////////////////////////////////////////////////////////////
Fec::Fec() :
    m_dcs_file(""),
    m_name(""),
    m_ip(""),
    m_id("")
{
}

bool Fec::loadFec(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    int n_chip_counter = 0;
    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            if(v.first == "chip") n_chip_counter++;
        } // foreach
    } //try
    catch(std::exception &e) {
        cout << "Fec::loadFec    ERROR creating FEC" << endl;
        ok = false;
    }

    if(ok) {
        m_chipArraySize = n_chip_counter;
        m_chipArray = new Chip*[n_chip_counter];

        int n_chip = 0;

        try {
            BOOST_FOREACH(ptree::value_type v, pt.second) {
                ////////////////////////////////////////////
                // dcs file
                ////////////////////////////////////////////
                if(v.first == "dcs_file") {
                    m_dcs_file = v.second.data();
                    trim(m_dcs_file);
                }
                ////////////////////////////////////////////
                // name
                ////////////////////////////////////////////
                else if(v.first == "name") {
                    m_name = v.second.data();
                    trim(m_name);
                }
                ////////////////////////////////////////////
                // IP/ID
                ////////////////////////////////////////////
                else if(v.first == "ip") {
                    m_ip = v.second.data();
                    trim(m_ip);
                }
                else if(v.first == "<xmlattr>") {
                    m_id = v.second.get<string>("id");
                    trim(m_id);
                }
                else if(v.first == "<xmlcomment>") {
                    continue;
                }
                ////////////////////////////////////////////
                // load the chips
                ////////////////////////////////////////////
                else if(v.first == "chip") {
                    #warning previous code starts at n_chip++ ??
                    m_chipArray[n_chip] = new Chip();
                    if(!m_chipArray[n_chip]->loadChip(v)) ok = false;
                    n_chip++;
                }

                else {
                    cout << "Fec::loadFec    WARNING Unknown key: " << v.first << endl;
                    ok = false;
                }
            } // foreach
        } //try
        catch(std::exception &e) {
            cout << "Fec::loadFec    ERROR: " << e.what() << endl;
            ok = false;
        }
    }//ok

    if(ok) {
        for(int ichip = 0; ichip < m_chipArraySize; ichip++){
            m_chipArray[ichip]->print();
        }
    } 

    return ok;

}

void Fec::print()
{
    cout << "-----------------------------------" << endl;
    cout << "  Print FEC " << endl;
    cout << "      dcs file    : " << m_dcs_file << endl;
    cout << "      name        : " << m_name << endl;
    cout << "      IP          : " << m_ip << endl;
    cout << "      ID          : " << m_id << endl;
    cout << " - - - - - - - - - - - - - - - - - " << endl;
    cout << "  FEC Printing loaded chip array " << endl;
    for(int ichip = 0; ichip < m_chipArraySize; ichip++){
        m_chipArray[ichip]->print();
    }
}
