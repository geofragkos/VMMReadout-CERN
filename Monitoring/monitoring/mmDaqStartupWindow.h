//
//  mmDaqStartupWindow.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef MMDAQSTARTUPWINDOW_H
#define MMDAQSTARTUPWINDOW_H

#include <QTabWidget>
#include <QTreeWidget>
#include <QTimer>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QLayout>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QFileInfo>

namespace online {
   namespace display {

class mmDaqStartupWindow : public QWidget //do not forget public to be able to use it's QWidget functions from outside
{
    Q_OBJECT

public:
    mmDaqStartupWindow(QWidget *parent = 0);
    ~mmDaqStartupWindow();

    QVBoxLayout *settingWindowLayout;

    QHBoxLayout * fileSelectionLayout;
    QLabel * configSectionLabel;
    QLineEdit* configFile;
    QPushButton* loadConfigFile;
    QPushButton* browseConfigFile;
    QString configFileName;
    QString configFileFullPath;

public slots:
    QVBoxLayout* setUpConfigFileSelection();
    void searchForConfigFile();
    void transmitConfigFileNameSignal();

signals:
    void configFilePathIs(const QString &);
    void configFileNameIs(const QString &);


};

   }
}
#endif // MMDAQSTARTUPWINDOW_H
