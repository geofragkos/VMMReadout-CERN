//
//  mmDaqSettingsWindow.h
//  online_display
//
//  Created by Konstantinos Ntekas on 06/2012.
//  Copyright (c) 2012 CERN - PH/UAT. All rights reserved.
//


#ifndef MMDAQSETTINGSWINDOW_H
#define MMDAQSETTINGSWINDOW_H

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

class mmDaqSettingsWindow : public QWidget //do not forget public to be able to use it's QWidget functions from outside
{
    Q_OBJECT

public:
    mmDaqSettingsWindow(QWidget *parent=0);
    QVBoxLayout *settingWindowLayout;

    QHBoxLayout * fileSelectionLayout;
    QLabel * pedestalSectionLabel;
    QLineEdit* pedestalsFile;
    QPushButton* loadPedFile;
    QPushButton* browsePedFile;
    QString pedestalsFileName;
    QString pedestalsFileFullPath;


    QLabel * configSectionLabel;
    QLineEdit* configFile;
    QPushButton* loadConfigFile;
    QPushButton* browseConfigFile;
    QString configFileName;
    QString configFileFullPath;

    QLabel * varSettingsLabel;
    QLabel * zeroSuppressionLabel;
    QDoubleSpinBox *zeroSuppressionBox;

public slots:
//    void transmitBrowsePedestalsClickedSignal();
//    void transmitLoadPedestalsClickedSignal();
    //void openPedestalsFile();
    void loadPedestalsFile();

    //void transmitBrowseConfigClickedSignal();
    //void transmitLoadConfigClickedSignal();
    //void openConfigFileSlot();
    //void loadConfigFileSlot();

    void searchForConfigFile();
    void transmitConfigFileNameSignal();
    void searchForPedestalsFile();
    void transmitPedestalsFileNameSignal();

private:
    QVBoxLayout* setUpPedestalsFileSelection();
    QVBoxLayout* setUpConfigFileSelection();
    QHBoxLayout *setUpZeroSuppressionSettingsSelection();
    //QGroupBox *setUpVariousSettingsSelection();

private slots:

signals:
    void browsePedestalsClicked();
    void loadPedestalsClicked();
    void pedestalsFilePathIs(const QString &);
    void pedestalsFileNameIs(const QString &);

    void rebrowseConfigClicked();
    void reloadConfigClicked();
    void reconfigFilePathIs(const QString &);
    void reconfigFileNameIs(const QString &);
};

#endif // MMDAQSETTINGSWINDOW_H
