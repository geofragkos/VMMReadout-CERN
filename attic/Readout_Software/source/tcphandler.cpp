#include "tcphandler.h"
#include "mainwindow.h"


//tcpHandler::tcpHandler(QObject *parent) :
//    QObject(parent)
//{
//}
void tcpHandler::initialize(){

//    communicationAliveFPGA = 0;
//    FPGAPort=65000;
//    SetFPGAIP();
//    PingFPGA();
//    socketTCP = new QTcpSocket();
//    socketTCP->connectToHost(ips_tr,FPGAPort, QTcpSocket::ReadWrite);
//    connect(socketTCP, SIGNAL(error(QAbstractSocket::SocketError)),
//            this, SLOT(displayError(QAbstractSocket::SocketError)));
//    connect(socketTCP, SIGNAL(readyRead()), this, SLOT(readTCP()));
//    if (socketTCP->waitForConnected(2000)){
//        //connect( socket, SIGNAL(readyRead()), this, SLOT(dataPending()) );
//        ui->connectionLabel_tr->setText("      Alive");
//        ui->connectionLabel_tr->setStyleSheet("background-color: green");
//    }else{
//        ui->connectionLabel_tr->setText("   Problem");
//        ui->connectionLabel_tr->setStyleSheet("background-color: gray");
//    }
}

//_________________________________________________________________________________________________
void tcpHandler::SetFPGAIP(){
}
//_________________________________________________________________________________________________
void tcpHandler::PingFPGA(){
//    int exitCode = QProcess::execute("ping", QStringList() << "-t1" << ips_tr);
//    if (0 == exitCode) {
//        communicationAliveFPGA=true;
//    } else {
//        communicationAliveFPGA=false;
//    }
}
