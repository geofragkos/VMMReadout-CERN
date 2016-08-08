/****************************************************************************
** Meta object code from reading C++ file 'calibration_module.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/calibration_module.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calibration_module.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CalibModule_t {
    QByteArrayData data[13];
    char stringdata0[228];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalibModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalibModule_t qt_meta_stringdata_CalibModule = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CalibModule"
QT_MOC_LITERAL(1, 12, 20), // "calibrationLoopState"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 22), // "setPDOCalibrationState"
QT_MOC_LITERAL(4, 57, 22), // "setTDOCalibrationState"
QT_MOC_LITERAL(5, 80, 11), // "setChannels"
QT_MOC_LITERAL(6, 92, 22), // "setupCalibrationConfig"
QT_MOC_LITERAL(7, 115, 19), // "setCalibrationACQon"
QT_MOC_LITERAL(8, 135, 20), // "setCalibrationACQoff"
QT_MOC_LITERAL(9, 156, 17), // "endCalibrationRun"
QT_MOC_LITERAL(10, 174, 16), // "calibACQcomplete"
QT_MOC_LITERAL(11, 191, 16), // "advanceCalibLoop"
QT_MOC_LITERAL(12, 208, 19) // "stopCalibrationLoop"

    },
    "CalibModule\0calibrationLoopState\0\0"
    "setPDOCalibrationState\0setTDOCalibrationState\0"
    "setChannels\0setupCalibrationConfig\0"
    "setCalibrationACQon\0setCalibrationACQoff\0"
    "endCalibrationRun\0calibACQcomplete\0"
    "advanceCalibLoop\0stopCalibrationLoop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalibModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       3,    3,   72,    2, 0x06 /* Public */,
       4,    4,   79,    2, 0x06 /* Public */,
       5,    1,   88,    2, 0x06 /* Public */,
       6,    0,   91,    2, 0x06 /* Public */,
       7,    1,   92,    2, 0x06 /* Public */,
       8,    0,   95,    2, 0x06 /* Public */,
       9,    0,   96,    2, 0x06 /* Public */,
      10,    0,   97,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   98,    2, 0x0a /* Public */,
      12,    0,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CalibModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CalibModule *_t = static_cast<CalibModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->calibrationLoopState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setPDOCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->setTDOCalibrationState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 3: _t->setChannels((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setupCalibrationConfig(); break;
        case 5: _t->setCalibrationACQon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->setCalibrationACQoff(); break;
        case 7: _t->endCalibrationRun(); break;
        case 8: _t->calibACQcomplete(); break;
        case 9: _t->advanceCalibLoop(); break;
        case 10: _t->stopCalibrationLoop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CalibModule::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::calibrationLoopState)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setPDOCalibrationState)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)(int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setTDOCalibrationState)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setChannels)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setupCalibrationConfig)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setCalibrationACQon)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::setCalibrationACQoff)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::endCalibrationRun)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (CalibModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CalibModule::calibACQcomplete)) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject CalibModule::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CalibModule.data,
      qt_meta_data_CalibModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CalibModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalibModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CalibModule.stringdata0))
        return static_cast<void*>(const_cast< CalibModule*>(this));
    return QObject::qt_metacast(_clname);
}

int CalibModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CalibModule::calibrationLoopState(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CalibModule::setPDOCalibrationState(int _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CalibModule::setTDOCalibrationState(int _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CalibModule::setChannels(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CalibModule::setupCalibrationConfig()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void CalibModule::setCalibrationACQon(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CalibModule::setCalibrationACQoff()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void CalibModule::endCalibrationRun()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void CalibModule::calibACQcomplete()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
