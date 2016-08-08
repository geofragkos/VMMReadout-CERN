#include "createevents.h"

//std/stl
#include <set>
#include <sstream>

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

////////////////////////////////////////////////////////////////////////
// CREATEVENTS
////////////////////////////////////////////////////////////////////////
CreateEvents::CreateEvents() :
    m_dbg(false),
    n_eventCounter(-1),
    m_daq(0),
    m_srs(0),
    m_detector(0)
{
    cout << "INITIALIZE CREATEVENTS" << endl;
}
void CreateEvents::setDaq(DaqConfig* daq)
{
    m_daq = daq;
    m_srs = &daq->getSrsConfig();
    m_detector = &daq->getDetectorConfig();
    n_eventCounter = -1;
}

// dantrim Jul20
void CreateEvents::buildMapsMMFE8()
{
    /*
        [IP][chip name][chip channel] -> Event object
    */
    vector<string> loaded_ips;

    int n_fec = getSrs().numberOfFecs();
    //std::cout << "number of fecs: " << n_fec << std::endl;

    for(int ifec = 0; ifec < n_fec; ifec++) {
        string fec_id = getSrs().m_fecArray[ifec]->getId();
        string fec_ip = getSrs().m_fecArray[ifec]->getIp();
        string fec_name = getSrs().m_fecArray[ifec]->name();


        //std::cout << "HIT IP: " << fec_ip << "   ID: " << fec_id << "   NAME: " << fec_name << std::endl;


        int n_chip = getSrs().m_fecArray[ifec]->m_chipArraySize;


        map_chip chips_map;
        for(int jchip = 0; jchip < n_chip; jchip++) {

            string chip_name = getSrs().m_fecArray[ifec]->m_chipArray[jchip]->getName();


            // now find the chamber --> connector --> map with the same chip name
            int n_chambers = getDetector().getChamberArraySize();
            for(int ichamber = 0; ichamber < n_chambers; ichamber++) {
                string chamber_name = getDetector().m_chamberArray[ichamber]->getName();

                int n_connectors = getDetector().m_chamberArray[ichamber]->connectorArraySize();
                for(int jconn = 0; jconn < n_connectors; jconn++) {
                    // chip connected at this connector
                    string chamber_chip_name = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getChipName();
                    if(!(chip_name == chamber_chip_name)) continue;

                    //std::cout << "chip key: " << chip_name << "  chip on chamber: " << chamber_chip_name << std::endl;
                    string connector_name = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getName();

                    map_channel channels;
                    int n_chamber_connectors = getDetector().m_chamberArray[ichamber]->m_chamberSpecs->getConnectorArraySize();


                    for(int kcsconn = 0; kcsconn < n_chamber_connectors; kcsconn++) {
                        string chamber_connector_name = getDetector().m_chamberArray[ichamber]->m_chamberSpecs->m_connectorArray[kcsconn]->getName();
                        if(!connector_name.compare(chamber_connector_name)) {

                            // tuple<panasonic pin, multilayer, layer, readout (X,Y), chamber strip>
                            for(auto chamberspecdata : getDetector().m_chamberArray[ichamber]->m_chamberSpecs->m_connectorArray[kcsconn]->data) {
                                //std::cout << "loop chambersepcdata" << std::endl;

                               // std::cout << get<0>((chamberspecdata)) << std::endl;
                                vector<string> chip_name_split;
                                boost::split(chip_name_split, chip_name, boost::is_any_of("."));
                                int chip_id = stoi(chip_name_split.back());
                                tuple<int, int, int> chip_tuple = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getChannelMMFE8(get<4>(chamberspecdata));
                                bool already_loaded = false;
                                for(auto loaded_channel : channels) {
                                    if(get<0>(chip_tuple) == loaded_channel.first) {
                                        if(loaded_channel.second->getChipName() == chip_name) {
                                            already_loaded = true;
                                        }
                                    }
                                }
                                for(int iip = 0; iip < loaded_ips.size(); iip++) {
                                    if(loaded_ips[iip] == fec_ip) already_loaded = true;
                                }
                                //std::cout << "chip tuple 0: " << get<0>(chip_tuple) << std::endl;
                                if(!already_loaded && !(get<0>(chip_tuple)<0)) {
                                    //std::cout << "FILLING CHANNELS" << std::endl;
                                    channels[get<0>(chip_tuple)] = new Event( chamber_name,
                                                                                to_string(get<1>(chamberspecdata)),
                                                                                to_string(get<2>(chamberspecdata)),
                                                                                get<3>(chamberspecdata),
                                                                                to_string(get<4>(chamberspecdata)),
                                                                                fec_name,
                                                                                chip_name,
                                                                                to_string(get<0>(chip_tuple)) );
                                }

                            }

                        } // connectors match
                    } // kcsconn
                    chips_map[chip_name] = channels;

                } // jconn
                
            } // ichamber

        } // jchip
        m_ip_map[fec_ip] = chips_map;
    }

    //for(auto ipmap : m_ip_map) {
    //    for(auto chipmap : m_ip_map[ipmap.first]) {
    //        for(auto chanmap : ((m_ip_map.find(ipmap.first)->second).find(chipmap.first)->second))
    //            std::cout << "[" << ipmap.first << "][" << chipmap.first << "] : " << chanmap.first << std::endl;
    //    } // chipmap
    //} // ip

}

// dantrim May 20 2016
// essentially a re-write of the 'createEvents' method but targetting VMM2-MINI2
// for which we have different mapping formats (currently)
void CreateEvents::buildMaps()
{
    /*
        This will build a map of two indices which point to an Event object
        whcih contains the full information about the chamber connection
        for a specific chip and channel:
            [chip name][chip channel] -> Event object
    */

    int n_chambers = getDetector().getChamberArraySize();
    // loop over all chambers in our detector setup
    for(int ichamber = 0; ichamber < n_chambers; ichamber++) {
        // loop over the connectors for this chamber 
        string chamber_name = getDetector().m_chamberArray[ichamber]->getName();
        int n_connectors = getDetector().m_chamberArray[ichamber]->connectorArraySize();
        for(int jconn = 0; jconn < n_connectors; jconn++){
            // name of chip attached at this connector (e.g. VMM2.1.00, ...)
            string chip_name = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getChipName();
            // name of this connector (e.g. C0,C1...)
            string connector_name = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getName();

            if(dbg()) {
                stringstream sx;
                sx << "CreateEvents::buildMaps    "
                   << "[Chip : Connector] :: "
                   << "[" << chip_name 
                   << " : " << connector_name;
                cout << sx.str() << endl;
            } // dbg

            //map_channel == map<int, Event*> (see include/Event.h)
            map_channel channels;
            int n_chamber_connectors = getDetector().m_chamberArray[ichamber]->m_chamberSpecs->getConnectorArraySize();
            for(int kcsconn = 0; kcsconn < n_chamber_connectors; kcsconn++){
                //make sure chip.connector and chamber.connector are being compared to
                //this ensures that we provide to the chip channel mapping chamber strips that are actually
                //connected to this readout electronics (e.g. mini2)
                string chamber_connector_name = getDetector().m_chamberArray[ichamber]->m_chamberSpecs->m_connectorArray[kcsconn]->getName();
                if(!connector_name.compare(chamber_connector_name)){
                    string fec_name = getSrs().getFecContainsChipName(chip_name);

                    //iterate over chamber specs
                    // <panasonic pin, multilayer, layer, readout (X,Y), chamber strip>
                    for(vector<tuple<int, int, int, string, int>>::iterator it_chamber = getDetector().m_chamberArray[ichamber]->m_chamberSpecs->m_connectorArray[kcsconn]->data.begin();
                            it_chamber != getDetector().m_chamberArray[ichamber]->m_chamberSpecs->m_connectorArray[kcsconn]->data.end(); ++it_chamber){

                        // get the chip number from the chip name
                        vector<string> chip_name_split;
                        boost::split(chip_name_split, chip_name, boost::is_any_of("."));
                        //for(int b = 0; b < chip_name_split.size(); b++) {
                        //    cout << b << " : " << chip_name_split[b] << endl;
                        //}
                        //this assumes name of chip is, e.g. VMM2.X.YY
                        //where X is the FEC and YY is the chip number
                        int chip_number = stoi(chip_name_split.back()); 
                        
                        // map the chamber specs to the chip spec
                        // provide the chip strip to get the corresponding vmm channel
                        // tuple: <vmmchannel, chip #, chamber strip>
                        // if the channel for this strip and chip number is not found, returns
                        // a tuple of -1, [-1,-1,-1]
                        tuple<int, int, int> chip_tuple = getDetector().m_chamberArray[ichamber]->m_connectorArray[jconn]->getChannel(get<4>(*it_chamber), chip_number);

                        bool already_loaded = false;
                        for(auto loaded_channel : channels) {
                            if(get<0>(chip_tuple) == loaded_channel.first) {
                                if(loaded_channel.second->getChipName() == chip_name) {
                                    already_loaded = true;
                                }
                            }
                        }
                        if(!already_loaded && !(get<0>(chip_tuple)<0)) {
                           // cout << "strip " << get<4>(*it_chamber) << " : " << get<0>(chip_tuple) << "  " << chip_number << endl;
                            // { vmm channel : Event* }
                            channels[get<0>(chip_tuple)] = new Event( chamber_name,
                                                                        to_string(get<1>(*it_chamber)),
                                                                        to_string(get<2>(*it_chamber)),
                                                                        get<3>(*it_chamber),
                                                                        to_string(get<4>(*it_chamber)),
                                                                        fec_name,
                                                                        chip_name,
                                                                        to_string(get<0>(chip_tuple)) );
                        }
                    } // it_chamber
                } // chip.connector and chamber.connector same
            } //kcsconn

            // mapping
            // { chip_name : { vmm-channel : Event* } }
            m_chips_map[chip_name] = channels;

        } //jconn
    } //ichamber

    //for(auto i : m_chips_map) {
    //    for(auto j : m_chips_map[i.first]) {
    //        cout << "chips_map[" << i.first << "] = " << j.first << "  strip: " << j.second->getStrip() << endl;
    //    }
    //}

}


// this method is the "createNewEvents3()" from the older DAQ code
void CreateEvents::createEvents()
{
    for(int i = 0; i < getDetector().getChamberArraySize(); i++) {
        string chamber_name = getDetector().m_chamberArray[i]->getName();
        set<string> readout;
        for(int j = 0; j < getDetector().m_chamberArray[i]->connectorArraySize(); j++) {
            //if(!getDetector().m_chamberArray[i]->getChipName().compare(4,1,"2")) break; // this is an APV thing?
            cout << "CreateEvents::createEvents   " << getDetector().m_chamberArray[i]->m_connectorArray[j]->getChipName() << " : " << getDetector().m_chamberArray[i]->m_connectorArray[j]->getName() << endl;

            string chip_name = getDetector().m_chamberArray[i]->m_connectorArray[j]->getChipName();
            string connector_name = getDetector().m_chamberArray[i]->m_connectorArray[j]->getName();
            string chamber_name = getDetector().m_chamberArray[i]->getName();
            //cout << __LINE__ << endl;

            map_channel channels;
            for(int k = 0; k < getDetector().m_chamberArray[i]->m_chamberSpecs->getConnectorArraySize(); k++) {
                if(!connector_name.compare(getDetector().m_chamberArray[i]->m_chamberSpecs->m_connectorArray[k]->getName())) {
                    //cout << __LINE__ << endl;
                    string fec_name = getSrs().getFecContainsChipName(getDetector().m_chamberArray[i]->m_connectorArray[j]->getChipName());
                    for(vector<tuple<int, int, int, string, int>>::iterator it = getDetector().m_chamberArray[i]->m_chamberSpecs->m_connectorArray[k]->data.begin();
                                it != getDetector().m_chamberArray[i]->m_chamberSpecs->m_connectorArray[k]->data.end(); ++it) {

                        // map the chamber pin to the connector/chip pint
                        tuple<int, int, int> myTuple = getDetector().m_chamberArray[i]->m_connectorArray[j]->getPin(get<0>(*it));

                        // { connector strip : Event() }
                        // channels 0-63 of VMM are odd # VMMs
                        if(boost::lexical_cast<int>(get<0>(*it)) < 64) {
                            channels[get<1>(myTuple)] = new Event( chamber_name, to_string(get<1>(*it)), to_string(get<2>(*it)), get<3>(*it), to_string(get<4>(*it)), fec_name,
                                                                        "NaN", to_string(get<1>(myTuple)));
                        }
                        // channels 64-127 are even # VMMs
                        else if(boost::lexical_cast<int>(get<0>(*it)) >=64) {
                            channels[get<2>(myTuple)] = new Event( chamber_name, to_string(get<1>(*it)), to_string(get<2>(*it)), get<3>(*it), to_string(get<4>(*it)), fec_name,
                                                                        "NaN", to_string(get<2>(myTuple)));
                            //cout << "yep  " <<  chamber_name << "  " << to_string(get<1>(*it)) << "  " << to_string(get<2>(*it)) << "  " << get<3>(*it) << "  " << to_string(get<4>(*it)) << "  " << fec_name << "  " << "NaN" << "  " << to_string(get<2>(myTuple)) << endl;
                        }
                        
                    } // it
                } // this connector is attached to this chamber element
            } // k
            // { chip name : { connector strip : Event() } }
            m_chips_map[chip_name] = channels;
        } // j
    }//i
}

string CreateEvents::getEvent(int chip, int channel, int event, int charge, int time, int charge2, int time2)
{
    if(chip >= 16)
        chip -= 16;
    string chip_str;
    if (chip < 10) {
        chip_str = "VMM2.1.0";
        chip_str += boost::lexical_cast<std::string>(chip);
    }
    else {
        chip_str = "VMM2.1.";
        chip_str += boost::lexical_cast<std::string>(chip);
    }

    //cout << "\n\nHardcoding chip name for mapping ==> fix!\n\n" << endl;
    //chip_str = "VMM2.1.00";
    cout << "------------------------------------------------------------------" << endl;
    cout << "CreateEvents::getEvent    event for [" << chip_str << "][" << channel << "]" << endl;
    cout << "------------------------------------------------------------------" << endl;

    //returns the formatted string to be sent to the shared memory and parsed
    //by the monitoring tools
    return ((m_chips_map.find(chip_str))->second).find(channel)->second->constructEvent(event, charge, time, charge2, time2);
    //((m_chips_map.find(chip_str))->second).find(channel)->second->constructEvent(event, charge, time, charge2, time2);

}
    
