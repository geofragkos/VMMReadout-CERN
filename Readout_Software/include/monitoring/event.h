#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>

class Event {
    public :
        Event(std::string chamber,
                std::string multilayer,
                std::string layer,
                std::string readout,
                std::string strip,
                std::string fec,
                std::string chip,
                std::string channel);

        std::string constructEvent(int event, int charge, int time, int charge2, int time2);
        //void constructEvent(int event, int charge, int time, int charge2, int time2);
        std::string msg();

        std::string getEventNumber() { return m_eventNumberStr; }
        std::string getChamberName() { return m_chamberStr; }
        std::string getMultiLayer() { return m_multilayerStr; }
        std::string getLayer() { return m_layerStr; }
        std::string getReadout() { return m_readoutStr; }
        std::string getStrip() { return m_stripStr; }
        std::string getFecName() { return m_fecStr; }
        std::string getChipName() { return m_chipStr; }
        std::string getChannel() { return m_channelStr; }

    private :
        int n_eventCounter;

        std::string m_eventNumberStr;
        std::string m_chamberStr;
        std::string m_multilayerStr;
        std::string m_layerStr;
        std::string m_readoutStr;
        std::string m_stripStr;
        std::string m_fecStr;
        std::string m_chipStr;
        std::string m_channelStr;
        std::string m_chargeStr;
        std::string m_timeStr;
        std::string m_pdoStr;
        std::string m_tdoStr;

}; // class


#endif
