#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "testsharedmem.h"
#include "generalheader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPushButton *sendData;

private:
    Ui::MainWindow *ui;
    testSharedMemTool *m_testSharedMemTool;

public slots :
    void sendDataToSharedMemory();

};

#endif // MAINWINDOW_H
