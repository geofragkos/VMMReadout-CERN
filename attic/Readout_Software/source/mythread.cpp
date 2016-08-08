#include "mythread.h"
#include "constants.h"

MyThread::MyThread(int interval, QObject* parent)
    : QThread(parent)
    , i(0)
    , inc(-1)
    , intvl(interval)
    , timer(0)
{
    ;
}

MyThread::~MyThread()
{
    if(timer != 0)
        delete timer;
}

void MyThread::run(void)
{
    if(timer == 0)
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(count()));
    }
    timer->start(intvl);
    exec();
}

void MyThread::count(void)
{
    if((i >= 100) or ( i <= 0))
        inc = -inc;

    i += inc;
    emit valueChanged(i);
}
