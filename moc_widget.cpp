/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[11];
    char stringdata0[266];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 17), // "Quick_Scroll_Slot"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 23), // "Receive_Diff_Data_Total"
QT_MOC_LITERAL(4, 50, 37), // "Quick_Scroll_Update_Timer_Slo..."
QT_MOC_LITERAL(5, 88, 24), // "Slow_Scroll_Timer_Handle"
QT_MOC_LITERAL(6, 113, 26), // "Release_TimeOut_Update_GUI"
QT_MOC_LITERAL(7, 140, 35), // "Release_Slow_Dir_TimeOut_Upda..."
QT_MOC_LITERAL(8, 176, 32), // "Release_Slow_Dir_Near_Update_GUI"
QT_MOC_LITERAL(9, 209, 31), // "Scroll_Quick_TimeOut_Update_GUI"
QT_MOC_LITERAL(10, 241, 24) // "Handle_Touch_Value_Event"

    },
    "Widget\0Quick_Scroll_Slot\0\0"
    "Receive_Diff_Data_Total\0"
    "Quick_Scroll_Update_Timer_Slot_Handle\0"
    "Slow_Scroll_Timer_Handle\0"
    "Release_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_Near_Update_GUI\0"
    "Scroll_Quick_TimeOut_Update_GUI\0"
    "Handle_Touch_Value_Event"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   57,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,
       9,    0,   62,    2, 0x08 /* Private */,
      10,    1,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::UShort,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort,    3,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Quick_Scroll_Slot((*reinterpret_cast< unsigned short(*)>(_a[1]))); break;
        case 1: _t->Quick_Scroll_Update_Timer_Slot_Handle(); break;
        case 2: _t->Slow_Scroll_Timer_Handle(); break;
        case 3: _t->Release_TimeOut_Update_GUI(); break;
        case 4: _t->Release_Slow_Dir_TimeOut_Update_GUI(); break;
        case 5: _t->Release_Slow_Dir_Near_Update_GUI(); break;
        case 6: _t->Scroll_Quick_TimeOut_Update_GUI(); break;
        case 7: _t->Handle_Touch_Value_Event((*reinterpret_cast< unsigned short(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Widget::*_t)(unsigned short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Widget::Quick_Scroll_Slot)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Widget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Widget.data,
      qt_meta_data_Widget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(const_cast< Widget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Widget::Quick_Scroll_Slot(unsigned short _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE