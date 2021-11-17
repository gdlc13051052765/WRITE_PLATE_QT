/****************************************************************************
** Meta object code from reading C++ file 'aplay_pthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "aplay_pthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aplay_pthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SoundThread_t {
    QByteArrayData data[4];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SoundThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SoundThread_t qt_meta_stringdata_SoundThread = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SoundThread"
QT_MOC_LITERAL(1, 12, 6), // "doPlay"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4) // "play"

    },
    "SoundThread\0doPlay\0\0play"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SoundThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   25,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void SoundThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SoundThread *_t = static_cast<SoundThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doPlay(); break;
        case 1: _t->play(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SoundThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SoundThread::doPlay)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SoundThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SoundThread.data,
      qt_meta_data_SoundThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SoundThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SoundThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SoundThread.stringdata0))
        return static_cast<void*>(const_cast< SoundThread*>(this));
    return QObject::qt_metacast(_clname);
}

int SoundThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void SoundThread::doPlay()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
