#ifndef SLEEPER_H
#define SLEEPER_H
#include "constants.h"

class Sleeper : public QThread
{
public:
    static void sleep(unsigned long secs){QThread::sleep(secs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}

};

#endif // SLEEPER_H
