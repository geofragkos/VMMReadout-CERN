/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[70];
    char stringdata0[1121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 8), // "checkFSM"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 6), // "EndRun"
QT_MOC_LITERAL(4, 28, 19), // "stopCalibrationLoop"
QT_MOC_LITERAL(5, 48, 17), // "monitorDataSignal"
QT_MOC_LITERAL(6, 66, 17), // "clearSharedMemory"
QT_MOC_LITERAL(7, 84, 16), // "setUseChannelMap"
QT_MOC_LITERAL(8, 101, 21), // "loadELxChannelMapping"
QT_MOC_LITERAL(9, 123, 14), // "setWriteNtuple"
QT_MOC_LITERAL(10, 138, 11), // "setIgnore16"
QT_MOC_LITERAL(11, 150, 17), // "setCalibrationRun"
QT_MOC_LITERAL(12, 168, 22), // "updateCalibrationState"
QT_MOC_LITERAL(13, 191, 16), // "setupOutputFiles"
QT_MOC_LITERAL(14, 208, 16), // "setupOutputTrees"
QT_MOC_LITERAL(15, 225, 16), // "checkReadyToRead"
QT_MOC_LITERAL(16, 242, 14), // "startDAQSocket"
QT_MOC_LITERAL(17, 257, 14), // "closeDAQSocket"
QT_MOC_LITERAL(18, 272, 10), // "testThread"
QT_MOC_LITERAL(19, 283, 13), // "testFunction2"
QT_MOC_LITERAL(20, 297, 12), // "testMultiARG"
QT_MOC_LITERAL(21, 310, 14), // "changeDACtoMVs"
QT_MOC_LITERAL(22, 325, 9), // "updateFSM"
QT_MOC_LITERAL(23, 335, 7), // "readLog"
QT_MOC_LITERAL(24, 343, 13), // "updateCounter"
QT_MOC_LITERAL(25, 357, 17), // "eventCountReached"
QT_MOC_LITERAL(26, 375, 25), // "prepareAndSendBoardConfig"
QT_MOC_LITERAL(27, 401, 24), // "prepareAndSendTDAQConfig"
QT_MOC_LITERAL(28, 426, 10), // "setRunMode"
QT_MOC_LITERAL(29, 437, 15), // "keepButtonState"
QT_MOC_LITERAL(30, 453, 10), // "setACQMode"
QT_MOC_LITERAL(31, 464, 21), // "selectOutputDirectory"
QT_MOC_LITERAL(32, 486, 25), // "loadConfigurationFromFile"
QT_MOC_LITERAL(33, 512, 24), // "writeConfigurationToFile"
QT_MOC_LITERAL(34, 537, 15), // "setNumberOfFecs"
QT_MOC_LITERAL(35, 553, 22), // "setAndSendEventHeaders"
QT_MOC_LITERAL(36, 576, 10), // "resetASICs"
QT_MOC_LITERAL(37, 587, 8), // "resetFEC"
QT_MOC_LITERAL(38, 596, 11), // "setHDMIMask"
QT_MOC_LITERAL(39, 608, 6), // "setART"
QT_MOC_LITERAL(40, 615, 15), // "checkLinkStatus"
QT_MOC_LITERAL(41, 631, 14), // "writeFECStatus"
QT_MOC_LITERAL(42, 646, 10), // "resetLinks"
QT_MOC_LITERAL(43, 657, 14), // "triggerHandler"
QT_MOC_LITERAL(44, 672, 11), // "setS6clocks"
QT_MOC_LITERAL(45, 684, 11), // "setS6Resets"
QT_MOC_LITERAL(46, 696, 17), // "set_s6resetStatus"
QT_MOC_LITERAL(47, 714, 11), // "configureTP"
QT_MOC_LITERAL(48, 726, 15), // "updateLogScreen"
QT_MOC_LITERAL(49, 742, 11), // "toggleDebug"
QT_MOC_LITERAL(50, 754, 15), // "badRunDirectory"
QT_MOC_LITERAL(51, 770, 11), // "setRunDirOK"
QT_MOC_LITERAL(52, 782, 18), // "updateChannelState"
QT_MOC_LITERAL(53, 801, 21), // "updateChannelVoltages"
QT_MOC_LITERAL(54, 823, 17), // "updateChannelADCs"
QT_MOC_LITERAL(55, 841, 18), // "updateTriggerCount"
QT_MOC_LITERAL(56, 860, 7), // "Connect"
QT_MOC_LITERAL(57, 868, 6), // "setFEB"
QT_MOC_LITERAL(58, 875, 20), // "propagateFEBSettings"
QT_MOC_LITERAL(59, 896, 21), // "propagateFEBSettings1"
QT_MOC_LITERAL(60, 918, 20), // "calibrationLoopState"
QT_MOC_LITERAL(61, 939, 22), // "setPDOCalibrationState"
QT_MOC_LITERAL(62, 962, 22), // "setTDOCalibrationState"
QT_MOC_LITERAL(63, 985, 17), // "changeDelayLabels"
QT_MOC_LITERAL(64, 1003, 19), // "setChannelsForCalib"
QT_MOC_LITERAL(65, 1023, 22), // "setupCalibrationConfig"
QT_MOC_LITERAL(66, 1046, 19), // "setCalibrationACQon"
QT_MOC_LITERAL(67, 1066, 20), // "setCalibrationACQoff"
QT_MOC_LITERAL(68, 1087, 17), // "endCalibrationRun"
QT_MOC_LITERAL(69, 1105, 15) // "setupMonitoring"

    },
    "MainWindow\0checkFSM\0\0EndRun\0"
    "stopCalibrationLoop\0monitorDataSignal\0"
    "clearSharedMemory\0setUseChannelMap\0"
    "loadELxChannelMapping\0setWriteNtuple\0"
    "setIgnore16\0setCalibrationRun\0"
    "updateCalibrationState\0setupOutputFiles\0"
    "setupOutputTrees\0checkReadyToRead\0"
    "startDAQSocket\0closeDAQSocket\0testThread\0"
    "testFunction2\0testMultiARG\0changeDACtoMVs\0"
    "updateFSM\0readLog\0updateCounter\0"
    "eventCountReached\0prepareAndSendBoardConfig\0"
    "prepareAndSendTDAQConfig\0setRunMode\0"
    "keepButtonState\0setACQMode\0"
    "selectOutputDirectory\0loadConfigurationFromFile\0"
    "writeConfigurationToFile\0setNumberOfFecs\0"
    "setAndSendEventHeaders\0resetASICs\0"
    "resetFEC\0setHDMIMask\0setART\0checkLinkStatus\0"
    "writeFECStatus\0resetLinks\0triggerHandler\0"
    "setS6clocks\0setS6Resets\0set_s6resetStatus\0"
    "configureTP\0updateLogScreen\0toggleDebug\0"
    "badRunDirectory\0setRunDirOK\0"
    "updateChannelState\0updateChannelVoltages\0"
    "updateChannelADCs\0updateTriggerCount\0"
    "Connect\0setFEB\0propagateFEBSettings\0"
    "propagateFEBSettings1\0calibrationLoopState\0"
    "setPDOCalibrationState\0setTDOCalibrationState\0"
    "changeDelayLabels\0setChannelsForCalib\0"
    "setupCalibrationConfig\0setCalibrationACQon\0"
    "setCalibrationACQoff\0endCalibrationRun\0"
    "setupMonitoring"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      68,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  354,    2, 0x06 /* Public */,
       3,    0,  355,    2, 0x06 /* Public */,
       4,    0,  356,    2, 0x06 /* Public */,
       5,    1,  357,    2, 0x06 /* Public */,
       6,    1,  360,    2, 0x06 /* Public */,
       7,    1,  363,    2, 0x06 /* Public */,
       8,    1,  366,    2, 0x06 /* Public */,
       9,    1,  369,    2, 0x06 /* Public */,
      10,    1,  372,    2, 0x06 /* Public */,
      11,    1,  375,    2, 0x06 /* Public */,
      12,    5,  378,    2, 0x06 /* Public */,
      13,    2,  389,    2, 0x06 /* Public */,
      14,    0,  394,    2, 0x06 /* Public */,
      15,    0,  395,    2, 0x06 /* Public */,
      16,    0,  396,    2, 0x06 /* Public */,
      17,    0,  397,    2, 0x06 /* Public */,
      18,    1,  398,    2, 0x06 /* Public */,
      19,    0,  401,    2, 0x06 /* Public */,
      20,    3,  402,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      21,    1,  409,    2, 0x0a /* Public */,
      22,    0,  412,    2, 0x0a /* Public */,
      23,    0,  413,    2, 0x0a /* Public */,
      24,    0,  414,    2, 0x0a /* Public */,
      25,    0,  415,    2, 0x0a /* Public */,
      26,    0,  416,    2, 0x0a /* Public */,
      27,    0,  417,    2, 0x0a /* Public */,
      28,    0,  418,    2, 0x0a /* Public */,
      29,    1,  419,    2, 0x0a /* Public */,
      30,    0,  422,    2, 0x0a /* Public */,
      31,    0,  423,    2, 0x0a /* Public */,
      32,    0,  424,    2, 0x0a /* Public */,
      33,    0,  425,    2, 0x0a /* Public */,
      34,    1,  426,    2, 0x0a /* Public */,
      35,    0,  429,    2, 0x0a /* Public */,
      36,    0,  430,    2, 0x0a /* Public */,
      37,    0,  431,    2, 0x0a /* Public */,
      38,    0,  432,    2, 0x0a /* Public */,
      39,    0,  433,    2, 0x0a /* Public */,
      40,    0,  434,    2, 0x0a /* Public */,
      41,    0,  435,    2, 0x0a /* Public */,
      42,    0,  436,    2, 0x0a /* Public */,
      43,    0,  437,    2, 0x0a /* Public */,
      44,    0,  438,    2, 0x0a /* Public */,
      45,    0,  439,    2, 0x0a /* Public */,
      46,    1,  440,    2, 0x0a /* Public */,
      47,    0,  443,    2, 0x0a /* Public */,
      48,    0,  444,    2, 0x0a /* Public */,
      49,    0,  445,    2, 0x0a /* Public */,
      50,    0,  446,    2, 0x0a /* Public */,
      51,    1,  447,    2, 0x0a /* Public */,
      52,    0,  450,    2, 0x0a /* Public */,
      53,    1,  451,    2, 0x0a /* Public */,
      54,    1,  454,    2, 0x0a /* Public */,
      55,    1,  457,    2, 0x0a /* Public */,
      56,    0,  460,    2, 0x0a /* Public */,
      57,    0,  461,    2, 0x0a /* Public */,
      58,    1,  462,    2, 0x0a /* Public */,
      59,    1,  465,    2, 0x0a /* Public */,
      60,    1,  468,    2, 0x0a /* Public */,
      61,    3,  471,    2, 0x0a /* Public */,
      62,    4,  478,    2, 0x0a /* Public */,
      63,    1,  487,    2, 0x0a /* Public */,
      64,    1,  490,    2, 0x0a /* Public */,
      65,    0,  493,    2, 0x0a /* Public */,
      66,    1,  494,    2, 0x0a /* Public */,
      67,    0,  497,    2, 0x0a /* Public */,
      68,    0,  498,    2, 0x0a /* Public */,
      69,    1,  499,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    2,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkFSM(); break;
        case 1: _t->EndRun(); break;
        case 2: _t->stopCalibrationLoop(); break;
        case 3: _t->monitorDataSignal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->clearSharedMemory((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setUseChannelMap((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->loadELxChannelMapping((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->setWriteNtuple((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setIgnore16((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setCalibrationRun((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->updateCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 11: _t->setupOutputFiles((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->setupOutputTrees(); break;
        case 13: _t->checkReadyToRead(); break;
        case 14: _t->startDAQSocket(); break;
        case 15: _t->closeDAQSocket(); break;
        case 16: _t->testThread((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->testFunction2(); break;
        case 18: _t->testMultiARG((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 19: _t->changeDACtoMVs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->updateFSM(); break;
        case 21: _t->readLog(); break;
        case 22: _t->updateCounter(); break;
        case 23: _t->eventCountReached(); break;
        case 24: _t->prepareAndSendBoardConfig(); break;
        case 25: _t->prepareAndSendTDAQConfig(); break;
        case 26: _t->setRunMode(); break;
        case 27: _t->keepButtonState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->setACQMode(); break;
        case 29: _t->selectOutputDirectory(); break;
        case 30: _t->loadConfigurationFromFile(); break;
        case 31: _t->writeConfigurationToFile(); break;
        case 32: _t->setNumberOfFecs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->setAndSendEventHeaders(); break;
        case 34: _t->resetASICs(); break;
        case 35: _t->resetFEC(); break;
        case 36: _t->setHDMIMask(); break;
        case 37: _t->setART(); break;
        case 38: _t->checkLinkStatus(); break;
        case 39: _t->writeFECStatus(); break;
        case 40: _t->resetLinks(); break;
        case 41: _t->triggerHandler(); break;
        case 42: _t->setS6clocks(); break;
        case 43: _t->setS6Resets(); break;
        case 44: _t->set_s6resetStatus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 45: _t->configureTP(); break;
        case 46: _t->updateLogScreen(); break;
        case 47: _t->toggleDebug(); break;
        case 48: _t->badRunDirectory(); break;
        case 49: _t->setRunDirOK((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 50: _t->updateChannelState(); break;
        case 51: _t->updateChannelVoltages((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 52: _t->updateChannelADCs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 53: _t->updateTriggerCount((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->Connect(); break;
        case 55: _t->setFEB(); break;
        case 56: _t->propagateFEBSettings((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->propagateFEBSettings1((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 58: _t->calibrationLoopState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->setPDOCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 60: _t->setTDOCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 61: _t->changeDelayLabels((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 62: _t->setChannelsForCalib((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 63: _t->setupCalibrationConfig(); break;
        case 64: _t->setCalibrationACQon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 65: _t->setCalibrationACQoff(); break;
        case 66: _t->endCalibrationRun(); break;
        case 67: _t->setupMonitoring((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::checkFSM)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::EndRun)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::stopCalibrationLoop)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::monitorDataSignal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::clearSharedMemory)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setUseChannelMap)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::loadELxChannelMapping)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setWriteNtuple)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setIgnore16)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setCalibrationRun)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(int , int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::updateCalibrationState)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setupOutputFiles)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::setupOutputTrees)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::checkReadyToRead)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::startDAQSocket)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::closeDAQSocket)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::testThread)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::testFunction2)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (MainWindow::*_t)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::testMultiARG)) {
                *result = 18;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 68)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 68;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 68)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 68;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::checkFSM()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void MainWindow::EndRun()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void MainWindow::stopCalibrationLoop()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void MainWindow::monitorDataSignal(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MainWindow::clearSharedMemory(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MainWindow::setUseChannelMap(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MainWindow::loadELxChannelMapping(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MainWindow::setWriteNtuple(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void MainWindow::setIgnore16(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void MainWindow::setCalibrationRun(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void MainWindow::updateCalibrationState(int _t1, int _t2, int _t3, int _t4, int _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void MainWindow::setupOutputFiles(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void MainWindow::setupOutputTrees()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void MainWindow::checkReadyToRead()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void MainWindow::startDAQSocket()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void MainWindow::closeDAQSocket()
{
    QMetaObject::activate(this, &staticMetaObject, 15, Q_NULLPTR);
}

// SIGNAL 16
void MainWindow::testThread(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void MainWindow::testFunction2()
{
    QMetaObject::activate(this, &staticMetaObject, 17, Q_NULLPTR);
}

// SIGNAL 18
void MainWindow::testMultiARG(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_END_MOC_NAMESPACE
