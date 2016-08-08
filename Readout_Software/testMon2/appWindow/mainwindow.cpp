#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    sendData(0),
    m_testSharedMemTool(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sendData = new QPushButton("Send Data", this);
    sendData->move(10,10);
    sendData->show();

    m_testSharedMemTool = new testSharedMemTool();
    m_testSharedMemTool->initializeSharedMemory();

    connect(sendData, SIGNAL(clicked()), this, SLOT(sendDataToSharedMemory()));
}

void MainWindow::sendDataToSharedMemory()
{
    m_testSharedMemTool->sendData();
}

MainWindow::~MainWindow()
{
    delete ui;
}
