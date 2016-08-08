#include "chip.h"

//boost
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////
// CHIP
////////////////////////////////////////////////////////////////////////
Chip::Chip() :
    m_id(""),
    m_type(""),
    m_name("")
{
}

bool Chip::loadChip(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    try {
        BOOST_FOREACH(ptree::value_type v, pt.second) {
            ////////////////////////////////////////////
            // name
            ////////////////////////////////////////////
            if(v.first == "name") {
                m_name = v.second.data();
                trim(m_name);
            }
            ////////////////////////////////////////////
            // type
            ////////////////////////////////////////////
            else if(v.first == "type") {
                m_type = v.second.data();
                trim(m_name);
            }
            ////////////////////////////////////////////
            // id/type
            ////////////////////////////////////////////
            else if(v.first == "<xmlattr>") {
                m_id = v.second.get<string>("id");
                trim(m_id);
                m_type = v.second.get<string>("type",m_type);
                trim(m_type);
            }

            else if(v.first == "<xmlcomment>") {
                continue;
            }

            else {
                cout << "Chip::loadChip    WARNING Unknown key: " << v.first << endl;
                ok = false;
            }
        } //foreach
    } //try
    catch(std::exception& e) {
        cout << "Chip::loadChip    ERROR: " << e.what() << endl;
        ok = false;
    }

    return ok;

}

void Chip::print()
{
    cout << "-----------------------------------" << endl;
    cout << "   Print Chip " << endl;
    cout << "      id    : " << m_id << endl;
    cout << "      type  : " << m_type << endl;
    cout << "      name  : " << m_name << endl;
    cout << "-----------------------------------" << endl;

}
