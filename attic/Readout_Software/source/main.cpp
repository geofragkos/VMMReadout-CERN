#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QFont Font;
    Font.setFamily("Arial");
    Font.setPixelSize(10);
    QApplication::setFont(Font);

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("VMM2 - SRS DCS");

    w.show();
    
    return a.exec();
}
