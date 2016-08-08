#include "connectorspecs.h"

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

//std/stl
#include <stdlib.h> //getenv
#include <sstream>

using namespace std;

bool dataCompare (const tuple<int, int, int, string, int> &lhs,
                    const tuple<int, int, int, string, int> &rhs)
{
    return get<0>(lhs) < get<0>(rhs);
}

////////////////////////////////////////////////////////////////////////
// ConnectorSpecs
////////////////////////////////////////////////////////////////////////
ConnectorSpecs::ConnectorSpecs() :
    m_name(""),
    m_id(""),
    m_chipName(""),
    m_map_filename("")
{
}

bool ConnectorSpecs::loadSpecs(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            ////////////////////////////////////////////
            // map file
            ////////////////////////////////////////////
            if(v.first == "map_file") {
                m_map_filename = v.second.data();
                trim(m_map_filename);
                // build the mapping for this connector
                if(!loadMapFile(m_map_filename)) ok = false;
            }
            ////////////////////////////////////////////
            // name
            ////////////////////////////////////////////
            else if(v.first == "name") {
                m_name = v.second.data();
                trim(m_name);
            }
            else if(v.first == "<xmlattr>") {
                m_id = v.second.get<string>("id", "NaNN");
                trim(m_id);
            }
            else if(v.first == "<xmlcomment>") {    
                continue;
            }

            else {
                cout << "ConnectorSpecs::loadSpecs    WARNING Unknown key: " << v.first << endl;
                ok = false;
            }
        } // foreach

    } // try
    catch(std::exception &e) {
        cout << "ConnectorSpecs::loadSpecs    ERROR reading config xml: " << e.what() << endl;
        ok = false;
    }

    return ok;
}

bool ConnectorSpecs::loadMapFile(string filename)
{
    bool ok = true;

    char* mmdaqpath;
    mmdaqpath = getenv("MMDAQDIR");
    if(mmdaqpath==NULL) {
        cout << "ConnectorSpecs::loadMapFile    ERROR Environment \"MMDAQDIR\" not found!" << endl;
        cout << "ConnectorSpecs::loadMapFile       -> Set this with \"set_daqenv.sh\" script." << endl;
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
        cout << "ConnectorSpecs::loadSpecs    ERROR Unable to find spec file: \"" << filename << "\"!" << endl;
        cout << "ConnectorSpecs::loadSpecs    ERROR Looking in full path: " << full_filename << "." << endl;
        exit(1);
    }
    cout << "ConnectorSpecs::loadSpecs    INFO Found input spec file: " << full_filename << endl;

    if(!readMapFile(full_filename)) {
        cout << "ConnectorSpecs::loadSpecs    ERROR Unable to read spec file" << endl;
        ok = false;
    }

    return ok;
}

bool ConnectorSpecs::readMapFile(string filename)
{
    bool ok = true;

    ifstream infile(filename.c_str());
    string line;

    int line_counter = 0;

    while(getline(infile, line)) {
        line_counter++;

        boost::trim(line);
        if(boost::starts_with(line, "#") || line.empty()) continue;

        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(", \t");
        tokenizer tokens(line, sep);
        tokenizer::iterator tok_iter = tokens.begin();

        string pinstr;
        string mlystr;
        string laystr;
        string rdostr;
        string strstr;

        enum {state_pin, state_multi, state_layer, state_readout, state_strip } parse_state;

        parse_state = state_pin;

        for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
            // pin column
            if(parse_state == state_pin) {
                pinstr = boost::trim_copy(*tok_iter);
                parse_state = state_multi;
            }
            // multi-layer column
            else if(parse_state == state_multi) {
                mlystr = boost::trim_copy(*tok_iter);
                parse_state = state_layer;
            }
            // layer column
            else if(parse_state == state_layer) {
                laystr = boost::trim_copy(*tok_iter);
                parse_state = state_readout;
            }
            // readout column
            else if(parse_state == state_readout) {
                rdostr = boost::trim_copy(*tok_iter);
                parse_state = state_strip;
            }
            // strip column
            else if(parse_state == state_strip) {
                strstr = boost::trim_copy(*tok_iter);
            }
        } // tok_iter

        if(parse_state != state_strip) {
            ok = false;
            stringstream sx;
            sx << "Readout::readMapFile    ERROR reading map file '" << filename << "'at line " << line_counter << endl;
            throw std::runtime_error(sx.str().c_str());
        }

        data.push_back( make_tuple(stoi(pinstr), stoi(mlystr), stoi(laystr), rdostr, stoi(strstr)) );
    } // while

    infile.close();
    sort(data.begin(), data.end(), dataCompare);

    return ok;
}

void ConnectorSpecs::print()
{
    cout << "-----------------------------------" << endl;
    cout << "  CONNECTORSPECS " << endl;
    cout << "     > name            : " << getName() << endl;
    cout << "     > id              : " << getId() << endl;
    cout << "     > chip name       : " << getChipName() << endl;
    cout << "     > map file        : " << getMapFileName() << endl;
    cout << "-----------------------------------" << endl;
}

