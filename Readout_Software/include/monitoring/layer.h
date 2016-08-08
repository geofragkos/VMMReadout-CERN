#ifndef LAYER_H
#define LAYER_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>

//mmdaq
#include "readout.h"
#include "coordinates.h"


class Layer {
    public :
        Layer();
        bool loadLayer(const boost::property_tree::ptree::value_type pt);

    private :
        std::string m_name;
        std::string m_id;
        Coordinates* position;
        Coordinates* rotation;

        Readout** m_readoutArray;

}; // class

#endif
