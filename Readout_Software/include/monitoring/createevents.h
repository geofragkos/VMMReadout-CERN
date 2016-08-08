#ifndef CREATEEVENTS_H
#define CREATEEVENTS_H

#include <iostream>
#include <string>
#include <memory>
#include <map>

//boost
#include <boost/unordered_map.hpp>

//mmdaq
#include "daqconfig.h"
#include "event.h"

class Event;

typedef vector<Event*> ChamberReadout;
typedef vector<ChamberReadout> Chambers;

typedef map<int, Event*> map_channel;  // [VMMCHANNEL] = EVENT
typedef map<std::string, map_channel> map_chip; // [VMMID][VMMCHANNEL]
typedef map<std::string, map_chip> ip_map; // [IP][VMMID][VMMCHANNEL]

class CreateEvents {

    public :
        CreateEvents();
        void setDaq(DaqConfig* daq);
        void buildMaps();
        void buildMapsMMFE8();
        void createEvents();

        void setDebug(bool doit = true) { m_dbg = doit; }
        bool dbg() { return m_dbg; }

        DaqConfig& getDaq() { return *m_daq; }
        SRSConfig& getSrs() { return *m_srs; }
        DetectorConfig& getDetector() { return *m_detector; }

        map_chip m_chips_map;
        ip_map m_ip_map;

        std::string getEvent(int chip, int channel, int event, int charge, int time, int charge2, int time2);

    private :
        bool m_dbg;
        int n_eventCounter;
        DaqConfig* m_daq;
        SRSConfig* m_srs;
        DetectorConfig* m_detector;
    

}; // class


#endif
