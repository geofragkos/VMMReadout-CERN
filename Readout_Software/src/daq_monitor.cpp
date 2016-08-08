#include "daq_monitor.h"


// std/stl
#include <iostream>
#include <sstream>

// qt
#include <QThread>

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  DaqMonitor
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
DaqMonitor::DaqMonitor(QObject* parent) :
    QObject(parent),
    m_dbg(true),
    m_is_monitoring(false),
    n_interval_to_check(2),
    n_previous_counter(-1),
    n_stuck_count(0),
    n_live_counter( new int() ),
    m_timer(0),
    m_msg(0)
{
    (*n_live_counter) = 0;
}
// ------------------------------------------------------------------------ //
void DaqMonitor::LoadMessageHandler(MessageHandler& msg)
{
    m_msg = &msg;
}
// ------------------------------------------------------------------------ //
void DaqMonitor::setTimer()
{
    std::stringstream sx;
    if(dbg()) {
        sx << "DaqMonitor::setTimer()";
        msg()(sx,"DaqMonitor::setTimer"); sx.str("");
    }
    if(m_timer) delete m_timer;

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(checkCount()));
}
// ------------------------------------------------------------------------ //
void DaqMonitor::setCounter(boost::shared_ptr< int > counter)
{
    n_live_counter = counter;
    //std::cout << "DaqMonitor::setCounter   " <<  n_live_counter << "  " << (*n_live_counter) << std::endl;
}
// ------------------------------------------------------------------------ //
void DaqMonitor::startTimer()
{
    if(dbg()) {
        msg()("Timer started...", "DaqMonitor::startTimer");
    }
    if(!m_timer) {
        msg()("Timer is null!","DaqMonitor::startTimer");
        return;
    }
    m_timer->start(n_interval_to_check*1000); // timer is in milliseconds
}
// ------------------------------------------------------------------------ //
void DaqMonitor::stopTimer()
{
    if(dbg()) {
        msg()("Stopping timer...", "DaqMonitor::stopTimer");
    }
    if(!m_timer) {
        msg()("Timer is null!","DaqMonitor::stopTimer");
        return;
    }
    m_timer->stop();
}
// ------------------------------------------------------------------------ //
void DaqMonitor::checkCount()
{
    int count_to_check = (*n_live_counter);
    //std::cout << "DaqMonitor::checkCount [" << QThread::currentThreadId()
    //         << "]  counter_to_check: " << count_to_check
    //         << "   previous: " << n_previous_counter << std::endl;

    if(count_to_check == n_previous_counter)
        n_stuck_count++;
    else
        n_stuck_count = 0;

    if(n_stuck_count>=2) {
        std::cout << "DaqMonitor::checkCount [" << QThread::currentThreadId()
                << "]   DAQ HANG OBSERVED" << std::endl;

        emit daqHangObserved();
        //stopTimer();
    }
    n_previous_counter = count_to_check;
}
