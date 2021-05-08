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
struct qt_meta_stringdata_Widget_Message_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_Message_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_Message_t qt_meta_stringdata_Widget_Message = {
    {
QT_MOC_LITERAL(0, 0, 14) // "Widget_Message"

    },
    "Widget_Message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget_Message[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Widget_Message::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Widget_Message::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Widget_Message.data,
      qt_meta_data_Widget_Message,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widget_Message::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget_Message::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widget_Message.stringdata0))
        return static_cast<void*>(const_cast< Widget_Message*>(this));
    return QWidget::qt_metacast(_clname);
}

int Widget_Message::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[21];
    char stringdata0[450];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 24), // "Touch_Allow_Send_Handler"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 17), // "Quick_Scroll_Slot"
QT_MOC_LITERAL(4, 51, 23), // "Receive_Diff_Data_Total"
QT_MOC_LITERAL(5, 75, 34), // "Update_Prompt_Message_Timer_H..."
QT_MOC_LITERAL(6, 110, 28), // "Communicate_Msg_QT_Go_Handle"
QT_MOC_LITERAL(7, 139, 18), // "Alarm_Timer_Handle"
QT_MOC_LITERAL(8, 158, 30), // "Motor_Shake_Slot_Timer_Handler"
QT_MOC_LITERAL(9, 189, 37), // "Quick_Scroll_Update_Timer_Slo..."
QT_MOC_LITERAL(10, 227, 12), // "Test_Handler"
QT_MOC_LITERAL(11, 240, 24), // "Slow_Scroll_Timer_Handle"
QT_MOC_LITERAL(12, 265, 26), // "Release_TimeOut_Update_GUI"
QT_MOC_LITERAL(13, 292, 35), // "Release_Slow_Dir_TimeOut_Upda..."
QT_MOC_LITERAL(14, 328, 32), // "Release_Slow_Dir_Near_Update_GUI"
QT_MOC_LITERAL(15, 361, 7), // "Cal_Crc"
QT_MOC_LITERAL(16, 369, 14), // "unsigned char*"
QT_MOC_LITERAL(17, 384, 4), // "Data"
QT_MOC_LITERAL(18, 389, 3), // "Len"
QT_MOC_LITERAL(19, 393, 31), // "Scroll_Quick_TimeOut_Update_GUI"
QT_MOC_LITERAL(20, 425, 24) // "Handle_Touch_Value_Event"

    },
    "Widget\0Touch_Allow_Send_Handler\0\0"
    "Quick_Scroll_Slot\0Receive_Diff_Data_Total\0"
    "Update_Prompt_Message_Timer_Handle\0"
    "Communicate_Msg_QT_Go_Handle\0"
    "Alarm_Timer_Handle\0Motor_Shake_Slot_Timer_Handler\0"
    "Quick_Scroll_Update_Timer_Slot_Handle\0"
    "Test_Handler\0Slow_Scroll_Timer_Handle\0"
    "Release_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_Near_Update_GUI\0"
    "Cal_Crc\0unsigned char*\0Data\0Len\0"
    "Scroll_Quick_TimeOut_Update_GUI\0"
    "Handle_Touch_Value_Event"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    1,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   93,    2, 0x08 /* Private */,
       6,    0,   94,    2, 0x08 /* Private */,
       7,    0,   95,    2, 0x08 /* Private */,
       8,    0,   96,    2, 0x08 /* Private */,
       9,    0,   97,    2, 0x08 /* Private */,
      10,    0,   98,    2, 0x08 /* Private */,
      11,    0,   99,    2, 0x08 /* Private */,
      12,    0,  100,    2, 0x08 /* Private */,
      13,    0,  101,    2, 0x08 /* Private */,
      14,    0,  102,    2, 0x08 /* Private */,
      15,    2,  103,    2, 0x08 /* Private */,
      19,    0,  108,    2, 0x08 /* Private */,
      20,    1,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort,    4,

 // slots: parameters
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
    QMetaType::UChar, 0x80000000 | 16, QMetaType::UChar,   17,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort,    4,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Touch_Allow_Send_Handler(); break;
        case 1: _t->Quick_Scroll_Slot((*reinterpret_cast< unsigned short(*)>(_a[1]))); break;
        case 2: _t->Update_Prompt_Message_Timer_Handle(); break;
        case 3: _t->Communicate_Msg_QT_Go_Handle(); break;
        case 4: _t->Alarm_Timer_Handle(); break;
        case 5: _t->Motor_Shake_Slot_Timer_Handler(); break;
        case 6: _t->Quick_Scroll_Update_Timer_Slot_Handle(); break;
        case 7: _t->Test_Handler(); break;
        case 8: _t->Slow_Scroll_Timer_Handle(); break;
        case 9: _t->Release_TimeOut_Update_GUI(); break;
        case 10: _t->Release_Slow_Dir_TimeOut_Update_GUI(); break;
        case 11: _t->Release_Slow_Dir_Near_Update_GUI(); break;
        case 12: { unsigned char _r = _t->Cal_Crc((*reinterpret_cast< unsigned char*(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< unsigned char*>(_a[0]) = _r; }  break;
        case 13: _t->Scroll_Quick_TimeOut_Update_GUI(); break;
        case 14: _t->Handle_Touch_Value_Event((*reinterpret_cast< unsigned short(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Widget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Widget::Touch_Allow_Send_Handler)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Widget::*_t)(unsigned short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Widget::Quick_Scroll_Slot)) {
                *result = 1;
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Widget::Touch_Allow_Send_Handler()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Widget::Quick_Scroll_Slot(unsigned short _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
