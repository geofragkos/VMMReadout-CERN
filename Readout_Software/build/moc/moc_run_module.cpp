/****************************************************************************
** Meta object code from reading C++ file 'run_module.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/run_module.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'run_module.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RunModule_t {
    QByteArrayData data[9];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RunModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RunModule_t qt_meta_stringdata_RunModule = {
    {
QT_MOC_LITERAL(0, 0, 9), // "RunModule"
QT_MOC_LITERAL(1, 10, 6), // "EndRun"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 10), // "checkLinks"
QT_MOC_LITERAL(4, 29, 13), // "s6resetStatus"
QT_MOC_LITERAL(5, 43, 3), // "Run"
QT_MOC_LITERAL(6, 47, 9), // "readEvent"
QT_MOC_LITERAL(7, 57, 9), // "sendPulse"
QT_MOC_LITERAL(8, 67, 9) // "finishRun"

    },
    "RunModule\0EndRun\0\0checkLinks\0s6resetStatus\0"
    "Run\0readEvent\0sendPulse\0finishRun"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RunModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    1,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   54,    2, 0x0a /* Public */,
       6,    0,   55,    2, 0x0a /* Public */,
       7,    0,   56,    2, 0x0a /* Public */,
       8,    0,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RunModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RunModule *_t = static_cast<RunModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EndRun(); break;
        case 1: _t->checkLinks(); break;
        case 2: _t->s6resetStatus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->Run(); break;
        case 4: _t->readEvent(); break;
        case 5: _t->sendPulse(); break;
        case 6: _t->finishRun(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RunModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunModule::EndRun)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RunModule::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunModule::checkLinks)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RunModule::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunModule::s6resetStatus)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject RunModule::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RunModule.data,
      qt_meta_data_RunModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RunModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RunModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RunModule.stringdata0))
        return static_cast<void*>(const_cast< RunModule*>(this));
    return QObject::qt_metacast(_clname);
}

int RunModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void RunModule::EndRun()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void RunModule::checkLinks()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void RunModule::s6resetStatus(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
