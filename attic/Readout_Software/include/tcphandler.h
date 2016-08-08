#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QObject>
#include "constants.h"

class tcpHandler : public QObject
{
    Q_OBJECT

public:
    QTcpSocket *socketTCP;
    quint16 FPGAPort;
    bool communicationAliveFPGA;
    QString ips_tr;


public slots:
    void initialize();
    void SetFPGAIP();
    void PingFPGA();

};

#endif // TCPHANDLER_H
