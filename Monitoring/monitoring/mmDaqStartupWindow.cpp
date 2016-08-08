//
//  mmDaqStartupWindow.cpp
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#include "mmDaqStartupWindow.h"

using namespace online::display;

mmDaqStartupWindow::mmDaqStartupWindow(QWidget *parent) :
    QWidget(parent)

{
    settingWindowLayout = new  QVBoxLayout(this);
    settingWindowLayout->addLayout(mmDaqStartupWindow::setUpConfigFileSelection());

}

mmDaqStartupWindow::~mmDaqStartupWindow()
{

}

void mmDaqStartupWindow::transmitConfigFileNameSignal()
{
    qDebug()<<"transmiting config file signal";
    qDebug()<<configFileFullPath;

    //emit configFileNameIs(configFileName);
    emit configFilePathIs(configFileFullPath);
}


QVBoxLayout* mmDaqStartupWindow::setUpConfigFileSelection()
{
    QVBoxLayout* configLayout = new QVBoxLayout();
    configSectionLabel = new QLabel(QString("Configuration File Selection")) ;

    configFile = new QLineEdit("");
    configFile->setReadOnly(true);

    QHBoxLayout* configButtonsLayout = new QHBoxLayout();
    loadConfigFile = new QPushButton("Load Config");
    loadConfigFile->setEnabled(0);
    browseConfigFile = new QPushButton("Select Config");
    configButtonsLayout->addWidget(browseConfigFile);
    configButtonsLayout->addWidget(loadConfigFile);

    configLayout->addWidget(configSectionLabel);
    configLayout->addWidget(configFile);
    configLayout->addLayout(configButtonsLayout);

    connect(this->browseConfigFile,SIGNAL(clicked()),this,SLOT(searchForConfigFile()));
    connect(this->loadConfigFile,SIGNAL(clicked()),this,SLOT(transmitConfigFileNameSignal()));

    return configLayout;

}

void mmDaqStartupWindow::searchForConfigFile()
{
    //quint64 sizeOfFile=0;
    QDir *currentDir = new QDir(QDir::current());
    currentDir->cdUp();
    QString serverPath = currentDir->path();
    QString configPath = serverPath.append("/mmdaq3-server/config");
// //   QList<bool> channelChecks;
// //   channelChecks.clear();
    configFileFullPath = QFileDialog::getOpenFileName(this,tr("Select Configuration File"),configPath,tr("Document files (*.xml)"),0);
    if(!configFileFullPath.isNull())   {
        QFileInfo pathInfo( configFileFullPath );
        configFileName = pathInfo.fileName();
    }
    configFile->setText(configFileName);
    loadConfigFile->setEnabled(1);
}


