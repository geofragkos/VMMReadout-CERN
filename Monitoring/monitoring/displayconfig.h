#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H


#include <string.h>
#include "QString"
#include <iostream>

class displayConfig
{
public:
    displayConfig();
    void read_config_memory(const std::string& memoryName);
};

#endif // DISPLAYCONFIG_H
