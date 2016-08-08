#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "constants.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int interval, QObject* parent = 0);
    ~MyThread();

signals:
    void valueChanged(int);
    void fillHisto(int);

private slots:
    void count(void);

protected:
    void run(void);

private:
    int i;
    int inc;
    int intvl;

    QTimer* timer;
};
#endif // MYTHREAD_H


