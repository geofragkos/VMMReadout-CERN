#ifndef TCPHANDLERTHREAD_H
#define TCPHANDLERTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class tcpHandlerThread : public QThread
{
    Q_OBJECT

protected:
    void run() { exec(); }

};


#endif // TCPHANDLERTHREAD_H
