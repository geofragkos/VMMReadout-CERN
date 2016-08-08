#ifndef CHIP_H
#define CHIP_H

#include <iostream>
#include <string>

//boost
#include <boost/property_tree/ptree.hpp>

class Chip {
    public :
        Chip();
        bool loadChip(const boost::property_tree::ptree::value_type pt);

        std::string getName() { return m_name; }
        std::string getType() { return m_type; }
        std::string getId() { return m_id; }

        void print();

    private :
        std::string m_id;
        std::string m_type;
        std::string m_name;
}; // class

#endif
