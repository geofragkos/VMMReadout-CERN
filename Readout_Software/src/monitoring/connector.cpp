#include "connector.h"

//std/stl
#include <fstream>
#include <stdlib.h> //getenv
#include <sstream>

//boost
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;


bool dataCompare(const std::tuple<int, int, int> &lhs, const std::tuple<int, int, int> &rhs) {
    return get<0>(lhs) < get<0>(rhs);
}

////////////////////////////////////////////////////////////////////////
// CONNECTOR
////////////////////////////////////////////////////////////////////////
Connector::Connector() :
    m_name(""),
    m_id(""),
    m_chipName(""),
    m_map_filename("")
{
}

bool Connector::loadConnector(const boost::property_tree::ptree::value_type pt)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost;

    try {
        BOOST_FOREACH(const ptree::value_type &v, pt.second) {
            ////////////////////////////////////////////
            // chip
            ////////////////////////////////////////////
            if(v.first =="chip") {
                m_chipName = v.second.get<string>("name");
                trim(m_chipName);
            }
            ////////////////////////////////////////////
            // map file
            ////////////////////////////////////////////
            else if(v.first == "map_file") {
                m_map_filename = v.second.data();
                trim(m_map_filename);
                if(!loadMapFile(m_map_filename)) ok = false;
            }
            ////////////////////////////////////////////
            // name
            ////////////////////////////////////////////
            else if(v.first == "<xmlattr>") {
                m_name = v.second.get<string>("name", "Nan");
                trim(m_name);
            }
            else if(v.first == "<xmlcomment>") {
                continue;
            }

            else {
                cout << "Connector::loadConnector    WARNING Unknown key: " << v.first << endl;
                ok = false;
            }
        } //foreach

    } //try
    catch(std::exception &e) {
        cout << "Connector::loadConnector    ERROR Unable to load connector: " << e.what() << endl;
        ok = false;
    }

    return ok;
}

bool Connector::loadMapFile(string filename)
{
    bool ok = true;

    char* mmdaqpath;
    mmdaqpath = getenv("MMDAQDIR");
    if(mmdaqpath==NULL) {
        cout << "Connector::loadMapFile    ERROR Environment \"MMDAQDIR\" not found!" << endl;
        cout << "Connector::loadMapFile       -> Set this with \"set_daqenv.sh\" script." << endl;
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
        cout << "Connector::loadMapFile    ERROR Unable to find map file: \"" << filename << "\"!" << endl;
        cout << "Connector::loadMapFile    ERROR Looking in full path: " << full_filename << "." << endl;
        exit(1);
    }
    cout << "Connector::loadMapFile    INFO Found input map file: " << full_filename << endl;

    if(!readMapFile(full_filename)) {
        cout << "Connector::loadMapFile    ERROR Unablei to read map file" << endl;
        ok = false;
    }

    return ok;

}

bool Connector::readMapFile(string filename)
{
    bool ok = true;

    ifstream infile(filename.c_str());
    string line;

    int line_counter = 0;

    // assuming tap separated columns:
    // | vmmchannel | chip number (0 or 1) | T chamber strip |
    while(getline(infile, line)){
        line_counter++;
        boost::trim(line);
        if(boost::starts_with(line, "#") || line.empty()) { continue; } // avoid comments and empty lines
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(", \t");
        tokenizer tokens(line, sep);

        string chanstr;
        string chipnumstr;
        string stripstr;

        enum { state_chan, state_chip, state_strip } parse_state;
        parse_state = state_chan;

        for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
            // grab the first column info
            if(parse_state == state_chan) {
                chanstr = boost::trim_copy(*tok_iter);
                parse_state = state_chip;
            }
            // grab the second column info
            else if(parse_state == state_chip) {
                chipnumstr = boost::trim_copy(*tok_iter);
                parse_state = state_strip;
            }
            // grab the third column info
            else if(parse_state == state_strip) {
                stripstr = boost::trim_copy(*tok_iter);
            }
        } // tok_iter
        if(parse_state != state_strip) {
            ok = false;
            stringstream sx;
            sx << "Connector::readMapFile    ERROR reading map file '" << filename << "' at line " << line_counter << endl;
            throw std::runtime_error(sx.str().c_str());
        }
        //cout << "readMapFile: " << chanstr << "  " << chipnumstr << "  " << stripstr << endl;
        data.push_back( make_tuple(stoi(chanstr), stoi(chipnumstr), stoi(stripstr)) );
    } // while  version2

/*
    while(getline(infile, line)) {
        line_counter++;
        boost::trim(line);
        if(boost::starts_with(line, "#") || line.empty()) { continue; }
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(", \t");
        tokenizer tokens(line, sep);
        tokenizer::iterator tok_iter = tokens.begin();

        #warning for VMM the columns are different data than for APV
        string pinstr;
        string even_chanstr;
        string odd_chanstr;

        enum {state_pin, state_even_chan, state_odd_chan} parse_state;
        //enum {state_pin, state_chip, state_channel} parse_state;
        parse_state = state_pin;

        for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
            if(parse_state == state_pin) {
                pinstr = boost::trim_copy(*tok_iter);
                //parse_state = state_chip;
                parse_state = state_even_chan;
            }
            else if(parse_state == state_even_chan) {
                even_chanstr = boost::trim_copy(*tok_iter);
                parse_state = state_odd_chan;
            }
            else if(parse_state == state_odd_chan) {
                odd_chanstr = boost::trim_copy(*tok_iter);
            }
        } // tok_iter

        if(parse_state != state_odd_chan) {
            ok = false;
            stringstream sx;
            sx << "Connector::readMapFile    ERROR reading map file '" << filename << "' at line " << line_counter << endl;
            throw std::runtime_error(sx.str().c_str());
        }

        data.push_back( make_tuple(stoi(pinstr), stoi(even_chanstr), stoi(odd_chanstr)) );

    } // while
*/
    infile.close();
    sort(data.begin(), data.end(), dataCompare);

    return ok;
}
tuple<int, int, int> Connector::getChannel(int chamberStrip, int chip_number)
{
    // assuming that the VMM channel to chamber strip mapping is as follows:
    // space separated columns:
    // | vmm channel | chip number (0 or 1) | T chamber strip |
    int chip_number_on_mini2 = ( (chip_number%2==0) ? 0 : 1);
    for(tuple<int, int, int> t : data) {
        if(chip_number_on_mini2==get<1>(t) && chamberStrip==get<2>(t)) {
            return t;
        }
    }
    return make_tuple(-1,-1,-1);

}
tuple<int, int, int> Connector::getChannelMMFE8(int chamberstrip)
{
    for(tuple<int, int, int> t : data) {
        if(get<2>(t) == chamberstrip)
            return t;
    }
    return make_tuple(-1,-1,-1);
}
tuple<int, int, int> Connector::getPin(int pin)
{
    #warning SUBTRACTING 2 FROM CHAMBER MAP PIN --  CHECK THAT THIS IS CORRECT FOR MINI2
    int pin_to_check = pin-2;
    if(pin_to_check>=64) pin_to_check = pin_to_check - 64;
    for(tuple<int, int, int> t : data) {
        if(get<0>(t) == pin_to_check) {
            return t;
        }
    }
}

void Connector::print()
{
    cout << "-----------------------------------" << endl;
    cout << "  CONNECTOR " << endl;
    cout << "     > name        : " << getName() << endl;
    cout << "     > id          : " << getId() << endl;
    cout << "     > chip name   : " << getChipName() << endl;
    cout << "     > mape file   : " << getMapFileName() << endl;
    cout << "-----------------------------------" << endl;

}

