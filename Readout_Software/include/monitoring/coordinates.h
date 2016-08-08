#ifndef COORDINATES_H
#define COORDINATES_H

#include <iostream>
#include <string>
//#include "debug.h"

using namespace std;

class Coordinates {
    public:
        Coordinates(double x, double y, double z);
     //   void printCortinates();
        double getX() { return x; }
        double getY() { return y; }
        double getZ() { return z; }
     //   void printObjectItems();

    private:
        double x;
        double y;
        double z;

};

#endif
