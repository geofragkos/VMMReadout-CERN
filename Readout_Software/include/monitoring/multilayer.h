#ifndef MULTILAYER_H
#define MULTILAYER_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>

//mmdaq
#include "layer.h"
#include "coordinates.h"


class MultiLayer {
    public :
        MultiLayer();
        bool loadLayer(const boost::property_tree::ptree::value_type pt);

        std::string getName() { return m_name; }
        std::string getId() { return m_id; }

    private :
        std::string m_name;
        std::string m_id;
        Coordinates* position;
        Coordinates* rotation;

        Layer** m_layerArray;
}; // class

#endif
