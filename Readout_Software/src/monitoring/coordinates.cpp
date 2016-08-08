#include "coordinates.h"

Coordinates::Coordinates(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

//void Coordinates::printCortinates(){
//    cout << "\\> x: " << x << endl;
//    cout << "\\> y: " << y << endl;
//    cout << "\\> z: " << z << endl;
//}
//
//void Coordinates::printObjectItems() {
//    DEBUG_DETECTOR_MSG( "Coordinates" );
//    PRINT_DETECTOR( x );
//    PRINT_DETECTOR( y );
//    PRINT_DETECTOR( z );
//}
