#ifndef READOUT_H
#define READOUT_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>
#include "coordinates.h"

class Readout {
    public :
        Readout();
        bool loadReadout(const boost::property_tree::ptree::value_type pt);

    private :
        std::string m_name;
        std::string m_id;
        Coordinates *position;
        Coordinates *rotation;
        int m_minstrip;
        int m_maxstrip;
        std::string m_pitch;


}; // class

#endif
