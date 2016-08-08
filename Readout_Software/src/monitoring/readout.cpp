#include "readout.h"

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>

////////////////////////////////////////////////////////////////////////
// READOUT
////////////////////////////////////////////////////////////////////////
Readout::Readout() :
    m_name(""),
    m_id(""),
    position(NULL),
    rotation(NULL),
    m_minstrip(-1),
    m_maxstrip(-1),
    m_pitch("")
{
}

bool Readout::loadReadout(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            ////////////////////////////////////////////
            // name
            ////////////////////////////////////////////
            if(v.first == "name") {
                m_name = v.second.data();
            }
            else if(v.first == "<xmlattr>") {
                m_name = v.second.get<string>("name", "NaN");
                m_id = v.second.get<string>("id", "NaNN");
            }
            else if(v.first == "<xmlcomment>") {
                continue;
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
            // strip range
            ////////////////////////////////////////////
            else if(v.first == "strip_range") {
                m_minstrip = v.second.get<int>("min");
                m_maxstrip = v.second.get<int>("max");
            }
            ////////////////////////////////////////////
            // strip pitch
            ////////////////////////////////////////////
            else if(v.first == "pitch") {
                m_pitch = v.second.data();
            }

            else {
                cout << "Readout::loadReadout    WARNING Unknown key: " << v.first << endl;
                ok = false;
            }
        } //foreach

    } //try
    catch(std::exception &e) {
        cout << "Readout::loadReadout    ERROR loading readout: " << e.what() << endl;
        ok = false;
    }

    return ok;
}
    
