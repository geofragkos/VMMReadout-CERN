#include "layer.h"

//boost
#include <boost/foreach.hpp>


////////////////////////////////////////////////////////////////////////
// LAYER
////////////////////////////////////////////////////////////////////////
Layer::Layer() :
    m_name(""),
    m_id(""),
    position(NULL),
    rotation(NULL),
    m_readoutArray(NULL)
{
}

bool Layer::loadLayer(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;

    int n_readout_counter = 0;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            if(v.first == "readout") n_readout_counter++;
        } //foreach

    } //try
    catch(std::exception &e){
        cout << "Layer::loadLayer    ERROR loading layer: " << e.what() << endl;
        ok = false;
    }

    if(ok) {
        m_readoutArray = new Readout*[n_readout_counter];

        int n_readout = 0;

        try {
            BOOST_FOREACH(const ptree::value_type &v, pt.second) {
                ////////////////////////////////////////////
                // readout
                ////////////////////////////////////////////
                if(v.first == "readout") {
                    m_readoutArray[n_readout] = new Readout();
                    if(!m_readoutArray[n_readout]->loadReadout(v)) ok = false;
                    n_readout++;
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
                // name
                ////////////////////////////////////////////
                else if(v.first == "name") {
                    m_name = v.second.data();
                }
                else if(v.first == "<xmlattr>") {
                    m_name = v.second.get<string>("name", "NaN");
                    m_id = v.second.get<string>("id", "NaNN");
                }
                else if(v.first =="<xmlcomment>") {
                    continue;
                }

                else {
                    cout << "Layer::loadLayer    WARNING Unknown key: " << v.first << endl;
                    ok = false;
                }
            } // foreach
        } // try
        catch(std::exception& e) {
            cout << "Layer::loadLayer    ERROR loading layer [2] : " << e.what() << endl;
            ok = false;
        }

    } // ok

    return ok;
}
