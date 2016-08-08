#include "multilayer.h"

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>


////////////////////////////////////////////////////////////////////////
// MULTILAYER
////////////////////////////////////////////////////////////////////////
MultiLayer::MultiLayer() :
    m_name(""),
    m_id(""),
    position(NULL),
    rotation(NULL),
    m_layerArray(NULL)
{
}

bool MultiLayer::loadLayer(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    int n_layer_counter = 0;

    using boost::property_tree::ptree;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            if(v.first == "layer") n_layer_counter++;
        } //foreach

    } //try
    catch(std::exception &e) {
        cout << "MultiLayer::loadLayer    ERROR loading layer from xml: " << e.what() << endl;
        ok = false;
    }

    if(ok) {
        m_layerArray = new Layer*[n_layer_counter];

        int n_layer = 0;

        try {
            BOOST_FOREACH(const ptree::value_type &v, pt.second) {
                ////////////////////////////////////////////
                // layer
                ////////////////////////////////////////////
                if(v.first == "layer") {
                    m_layerArray[n_layer] = new Layer();
                    if(!m_layerArray[n_layer]->loadLayer(v)) ok = false;
                    n_layer++;
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
                    position = new Coordinates(v.second.get<double>("x"),
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
                else if(v.first == "<xmlcomment>"){
                    continue;
                }

                else {
                    cout << "MultiLayer::loadLayer    WARNING Unknown key: " << v.first << endl;
                    ok = false;
                }
            } //foreach
        } // try
        catch(std::exception &e) {
            cout << "MultiLayer::loadLayer    ERROR loading layer from xml [2]: " << e.what() << endl;
            ok = false;
        }
    } // ok

    return ok;
}
