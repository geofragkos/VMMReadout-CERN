
#ifndef DAQ_MONITOR_H
#define DAQ_MONITOR_H

//qt
#include <QObject>
#include <QTimer>

//boost
#include <boost/shared_ptr.hpp>

//vmm
#include "message_handler.h"



class DaqMonitor : public QObject
{

    Q_OBJECT

    public :
        explicit DaqMonitor(QObject *parent = 0);
        virtual ~DaqMonitor(){};
        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }
        void setDebug(bool doit) { m_dbg = doit; }
        bool dbg() { return m_dbg; }


        void setInterval(int interval = 5) { n_interval_to_check = interval; }
        void setTimer();
        void setCounter(boost::shared_ptr< int > counter);

        bool isOn() { return m_is_monitoring; }

    private :
        bool m_dbg;
        bool m_is_monitoring;
        int n_interval_to_check;
        int n_previous_counter;
        int n_stuck_count;
        boost::shared_ptr< int > n_live_counter;

        QTimer* m_timer;

        MessageHandler *m_msg;

    signals :
        void daqHangObserved();

    public slots :
        void checkCount();
        void startTimer();
        void stopTimer();

}; //class

#endif
