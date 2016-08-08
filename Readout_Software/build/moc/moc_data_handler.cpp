/****************************************************************************
** Meta object code from reading C++ file 'data_handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/data_handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'data_handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DataHandler_t {
    QByteArrayData data[23];
    char stringdata0[354];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataHandler_t qt_meta_stringdata_DataHandler = {
    {
QT_MOC_LITERAL(0, 0, 11), // "DataHandler"
QT_MOC_LITERAL(1, 12, 13), // "checkDAQCount"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 11), // "setRunDirOK"
QT_MOC_LITERAL(4, 39, 21), // "eventCountStopReached"
QT_MOC_LITERAL(5, 61, 13), // "testSharedMem"
QT_MOC_LITERAL(6, 75, 9), // "readEvent"
QT_MOC_LITERAL(7, 85, 6), // "EndRun"
QT_MOC_LITERAL(8, 92, 21), // "writeAndCloseDataFile"
QT_MOC_LITERAL(9, 114, 15), // "set_monitorData"
QT_MOC_LITERAL(10, 130, 17), // "clearSharedMemory"
QT_MOC_LITERAL(11, 148, 16), // "setUseChannelMap"
QT_MOC_LITERAL(12, 165, 21), // "loadELxChannelMapping"
QT_MOC_LITERAL(13, 187, 14), // "setWriteNtuple"
QT_MOC_LITERAL(14, 202, 11), // "setIgnore16"
QT_MOC_LITERAL(15, 214, 17), // "setCalibrationRun"
QT_MOC_LITERAL(16, 232, 21), // "setCalibrationChannel"
QT_MOC_LITERAL(17, 254, 22), // "updateCalibrationState"
QT_MOC_LITERAL(18, 277, 16), // "setupOutputFiles"
QT_MOC_LITERAL(19, 294, 16), // "setupOutputTrees"
QT_MOC_LITERAL(20, 311, 16), // "connectDAQSocket"
QT_MOC_LITERAL(21, 328, 14), // "closeDAQSocket"
QT_MOC_LITERAL(22, 343, 10) // "daqHanging"

    },
    "DataHandler\0checkDAQCount\0\0setRunDirOK\0"
    "eventCountStopReached\0testSharedMem\0"
    "readEvent\0EndRun\0writeAndCloseDataFile\0"
    "set_monitorData\0clearSharedMemory\0"
    "setUseChannelMap\0loadELxChannelMapping\0"
    "setWriteNtuple\0setIgnore16\0setCalibrationRun\0"
    "setCalibrationChannel\0updateCalibrationState\0"
    "setupOutputFiles\0setupOutputTrees\0"
    "connectDAQSocket\0closeDAQSocket\0"
    "daqHanging"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  119,    2, 0x06 /* Public */,
       3,    1,  122,    2, 0x06 /* Public */,
       4,    0,  125,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  126,    2, 0x0a /* Public */,
       6,    0,  127,    2, 0x0a /* Public */,
       7,    0,  128,    2, 0x0a /* Public */,
       8,    0,  129,    2, 0x0a /* Public */,
       9,    1,  130,    2, 0x0a /* Public */,
      10,    0,  133,    2, 0x0a /* Public */,
      11,    1,  134,    2, 0x0a /* Public */,
      12,    1,  137,    2, 0x0a /* Public */,
      13,    1,  140,    2, 0x0a /* Public */,
      14,    1,  143,    2, 0x0a /* Public */,
      15,    1,  146,    2, 0x0a /* Public */,
      16,    1,  149,    2, 0x0a /* Public */,
      17,    5,  152,    2, 0x0a /* Public */,
      18,    2,  163,    2, 0x0a /* Public */,
      19,    0,  168,    2, 0x0a /* Public */,
      20,    0,  169,    2, 0x0a /* Public */,
      21,    0,  170,    2, 0x0a /* Public */,
      22,    0,  171,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,    2,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DataHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DataHandler *_t = static_cast<DataHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkDAQCount((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setRunDirOK((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->eventCountStopReached(); break;
        case 3: _t->testSharedMem(); break;
        case 4: _t->readEvent(); break;
        case 5: _t->EndRun(); break;
        case 6: _t->writeAndCloseDataFile(); break;
        case 7: _t->set_monitorData((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->clearSharedMemory(); break;
        case 9: _t->setUseChannelMap((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->loadELxChannelMapping((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->setWriteNtuple((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->setIgnore16((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->setCalibrationRun((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->setCalibrationChannel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->updateCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 16: { bool _r = _t->setupOutputFiles((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 17: _t->setupOutputTrees(); break;
        case 18: _t->connectDAQSocket(); break;
        case 19: _t->closeDAQSocket(); break;
        case 20: _t->daqHanging(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DataHandler::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataHandler::checkDAQCount)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DataHandler::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataHandler::setRunDirOK)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DataHandler::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataHandler::eventCountStopReached)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject DataHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DataHandler.data,
      qt_meta_data_DataHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DataHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DataHandler.stringdata0))
        return static_cast<void*>(const_cast< DataHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int DataHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void DataHandler::checkDAQCount(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataHandler::setRunDirOK(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DataHandler::eventCountStopReached()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
