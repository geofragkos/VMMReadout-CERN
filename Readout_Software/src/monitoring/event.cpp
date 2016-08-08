#include "event.h"

//std/stl
#include <sstream>

using namespace std;

////////////////////////////////////////////////////////////////////////
// EVENT
////////////////////////////////////////////////////////////////////////
Event::Event(string chamber,
             string multilayer,
             string layer,
             string readout,
             string strip,
             string fec,
             string chip,
             string channel) :
    n_eventCounter(0),
    m_eventNumberStr(""),
    m_chargeStr(""),
    m_timeStr("")
{
    m_chamberStr = chamber;
    m_multilayerStr = multilayer;
    m_layerStr = layer;
    m_readoutStr = readout;
    m_stripStr = strip;
    m_fecStr = fec;
    m_chipStr = chip;
    m_channelStr = channel;
}

std::string Event::constructEvent(int event, int charge, int time, int charge2, int time2)
//void Event::constructEvent(int event, int charge, int time, int charge2, int time2)
{
   // cout << "Event::constructEvent   > event: " << event << "  charge: " << charge << "  time: " << time << "  ch2: " << charge2 <<  "  time2: " << time2 << endl;
    n_eventCounter = event;
    m_chargeStr = std::to_string(charge);
    m_timeStr = std::to_string(time);
    m_pdoStr = std::to_string(charge2);
    m_tdoStr = std::to_string(time2);

    std::string spacer = " ";
    stringstream ss;
    ss << std::to_string(n_eventCounter) << spacer
       << m_chamberStr                   << spacer
       << m_multilayerStr                << spacer
       << m_layerStr                     << spacer
       << m_readoutStr                   << spacer
       << m_stripStr                     << spacer
       << m_fecStr                       << spacer
       << m_chipStr                      << spacer
       << m_channelStr                   << spacer
       << m_chargeStr                    << spacer
       << m_timeStr                      << spacer
       << m_pdoStr                       << spacer
       << m_tdoStr;
    cout << "====> Event::constructEvent returns: " << ss.str() << endl;
    return ss.str();
}

string Event::msg()
{
    cout << std::to_string(n_eventCounter) << endl;
    stringstream ss;
    ss  << std::to_string(n_eventCounter)    << " "
        << m_chamberStr                 << " "
        << m_multilayerStr              << " "
        << m_layerStr                   << " "
        << m_readoutStr                 << " "
        << m_stripStr                   << " "
        << m_fecStr                     << " "
        << m_chipStr                    << " "
        << m_channelStr                 << " "
        << m_chargeStr                  << " "
        << m_timeStr                    << " "
        << m_pdoStr                     << " "
        << m_tdoStr;
    cout << "Event::msg() > " << ss.str() << endl;
    cout << "Event::msg() > " << ss.str() << endl;
    string x = "10 TL2 0 0 X 45 1 20 60 40 40 40 40";
    return x;
    //return ss.str();
}
