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
    QByteArrayData data[43];
    char stringdata0[954];
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
QT_MOC_LITERAL(3, 33, 34), // "Update_Prompt_Message_Timer_H..."
QT_MOC_LITERAL(4, 68, 35), // "Slow_Scroll_Music_Send_Timer_..."
QT_MOC_LITERAL(5, 104, 28), // "Communicate_Msg_QT_Go_Handle"
QT_MOC_LITERAL(6, 133, 18), // "Alarm_Timer_Handle"
QT_MOC_LITERAL(7, 152, 30), // "Motor_Shake_Slot_Timer_Handler"
QT_MOC_LITERAL(8, 183, 28), // "Hardware_Check_Update_Handle"
QT_MOC_LITERAL(9, 212, 24), // "Setup_Touch_Check_Handle"
QT_MOC_LITERAL(10, 237, 12), // "Test_Handler"
QT_MOC_LITERAL(11, 250, 24), // "Slow_Scroll_Timer_Handle"
QT_MOC_LITERAL(12, 275, 26), // "Release_TimeOut_Update_GUI"
QT_MOC_LITERAL(13, 302, 35), // "Release_Slow_Dir_TimeOut_Upda..."
QT_MOC_LITERAL(14, 338, 32), // "Release_Slow_Dir_Near_Update_GUI"
QT_MOC_LITERAL(15, 371, 7), // "Cal_Crc"
QT_MOC_LITERAL(16, 379, 14), // "unsigned char*"
QT_MOC_LITERAL(17, 394, 4), // "Data"
QT_MOC_LITERAL(18, 399, 3), // "Len"
QT_MOC_LITERAL(19, 403, 14), // "removeListSame"
QT_MOC_LITERAL(20, 418, 12), // "QStringList*"
QT_MOC_LITERAL(21, 431, 4), // "list"
QT_MOC_LITERAL(22, 436, 31), // "Scroll_Quick_TimeOut_Update_GUI"
QT_MOC_LITERAL(23, 468, 24), // "Handle_Touch_Value_Event"
QT_MOC_LITERAL(24, 493, 23), // "Receive_Diff_Data_Total"
QT_MOC_LITERAL(25, 517, 28), // "dev_factory_init_status_task"
QT_MOC_LITERAL(26, 546, 24), // "dev_site_config_net_task"
QT_MOC_LITERAL(27, 571, 6), // "status"
QT_MOC_LITERAL(28, 578, 20), // "dev_configh_net_task"
QT_MOC_LITERAL(29, 599, 21), // "dev_factory_bind_task"
QT_MOC_LITERAL(30, 621, 20), // "dev_work_status_task"
QT_MOC_LITERAL(31, 642, 19), // "dev_update_ota_task"
QT_MOC_LITERAL(32, 662, 36), // "dev_factory_init_status_touch..."
QT_MOC_LITERAL(33, 699, 28), // "dev_configh_net_touch_handle"
QT_MOC_LITERAL(34, 728, 29), // "dev_factory_bind_touch_handle"
QT_MOC_LITERAL(35, 758, 29), // "dev_work_status_touch_handele"
QT_MOC_LITERAL(36, 788, 22), // "Disp_Net_Status_Handle"
QT_MOC_LITERAL(37, 811, 27), // "dev_update_ota_touch_handle"
QT_MOC_LITERAL(38, 839, 27), // "factory_hardware_check_task"
QT_MOC_LITERAL(39, 867, 32), // "dev_site_config_net_touch_handle"
QT_MOC_LITERAL(40, 900, 35), // "factory_hardware_check_touch_..."
QT_MOC_LITERAL(41, 936, 8), // "qtReboot"
QT_MOC_LITERAL(42, 945, 8) // "play_wav"

    },
    "Widget\0Touch_Allow_Send_Handler\0\0"
    "Update_Prompt_Message_Timer_Handle\0"
    "Slow_Scroll_Music_Send_Timer_Handle\0"
    "Communicate_Msg_QT_Go_Handle\0"
    "Alarm_Timer_Handle\0Motor_Shake_Slot_Timer_Handler\0"
    "Hardware_Check_Update_Handle\0"
    "Setup_Touch_Check_Handle\0Test_Handler\0"
    "Slow_Scroll_Timer_Handle\0"
    "Release_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_TimeOut_Update_GUI\0"
    "Release_Slow_Dir_Near_Update_GUI\0"
    "Cal_Crc\0unsigned char*\0Data\0Len\0"
    "removeListSame\0QStringList*\0list\0"
    "Scroll_Quick_TimeOut_Update_GUI\0"
    "Handle_Touch_Value_Event\0"
    "Receive_Diff_Data_Total\0"
    "dev_factory_init_status_task\0"
    "dev_site_config_net_task\0status\0"
    "dev_configh_net_task\0dev_factory_bind_task\0"
    "dev_work_status_task\0dev_update_ota_task\0"
    "dev_factory_init_status_touch_handle\0"
    "dev_configh_net_touch_handle\0"
    "dev_factory_bind_touch_handle\0"
    "dev_work_status_touch_handele\0"
    "Disp_Net_Status_Handle\0"
    "dev_update_ota_touch_handle\0"
    "factory_hardware_check_task\0"
    "dev_site_config_net_touch_handle\0"
    "factory_hardware_check_touch_handle\0"
    "qtReboot\0play_wav"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  184,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  185,    2, 0x08 /* Private */,
       4,    0,  186,    2, 0x08 /* Private */,
       5,    0,  187,    2, 0x08 /* Private */,
       6,    0,  188,    2, 0x08 /* Private */,
       7,    0,  189,    2, 0x08 /* Private */,
       8,    0,  190,    2, 0x08 /* Private */,
       9,    0,  191,    2, 0x08 /* Private */,
      10,    0,  192,    2, 0x08 /* Private */,
      11,    0,  193,    2, 0x08 /* Private */,
      12,    0,  194,    2, 0x08 /* Private */,
      13,    0,  195,    2, 0x08 /* Private */,
      14,    0,  196,    2, 0x08 /* Private */,
      15,    2,  197,    2, 0x08 /* Private */,
      19,    1,  202,    2, 0x08 /* Private */,
      22,    0,  205,    2, 0x08 /* Private */,
      23,    1,  206,    2, 0x08 /* Private */,
      25,    1,  209,    2, 0x08 /* Private */,
      26,    1,  212,    2, 0x08 /* Private */,
      28,    0,  215,    2, 0x08 /* Private */,
      29,    2,  216,    2, 0x08 /* Private */,
      30,    0,  221,    2, 0x08 /* Private */,
      31,    0,  222,    2, 0x08 /* Private */,
      32,    1,  223,    2, 0x08 /* Private */,
      33,    1,  226,    2, 0x08 /* Private */,
      34,    1,  229,    2, 0x08 /* Private */,
      35,    1,  232,    2, 0x08 /* Private */,
      36,    0,  235,    2, 0x08 /* Private */,
      37,    1,  236,    2, 0x08 /* Private */,
      38,    1,  239,    2, 0x08 /* Private */,
      39,    1,  242,    2, 0x08 /* Private */,
      40,    1,  245,    2, 0x08 /* Private */,
      41,    0,  248,    2, 0x08 /* Private */,
      42,    0,  249,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::UChar, 0x80000000 | 16, QMetaType::UChar,   17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort,   24,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Touch_Allow_Send_Handler(); break;
        case 1: _t->Update_Prompt_Message_Timer_Handle(); break;
        case 2: _t->Slow_Scroll_Music_Send_Timer_Handle(); break;
        case 3: _t->Communicate_Msg_QT_Go_Handle(); break;
        case 4: _t->Alarm_Timer_Handle(); break;
        case 5: _t->Motor_Shake_Slot_Timer_Handler(); break;
        case 6: _t->Hardware_Check_Update_Handle(); break;
        case 7: _t->Setup_Touch_Check_Handle(); break;
        case 8: _t->Test_Handler(); break;
        case 9: _t->Slow_Scroll_Timer_Handle(); break;
        case 10: _t->Release_TimeOut_Update_GUI(); break;
        case 11: _t->Release_Slow_Dir_TimeOut_Update_GUI(); break;
        case 12: _t->Release_Slow_Dir_Near_Update_GUI(); break;
        case 13: { unsigned char _r = _t->Cal_Crc((*reinterpret_cast< unsigned char*(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< unsigned char*>(_a[0]) = _r; }  break;
        case 14: _t->removeListSame((*reinterpret_cast< QStringList*(*)>(_a[1]))); break;
        case 15: _t->Scroll_Quick_TimeOut_Update_GUI(); break;
        case 16: _t->Handle_Touch_Value_Event((*reinterpret_cast< unsigned short(*)>(_a[1]))); break;
        case 17: _t->dev_factory_init_status_task((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->dev_site_config_net_task((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->dev_configh_net_task(); break;
        case 20: _t->dev_factory_bind_task((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->dev_work_status_task(); break;
        case 22: _t->dev_update_ota_task(); break;
        case 23: _t->dev_factory_init_status_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->dev_configh_net_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->dev_factory_bind_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: _t->dev_work_status_touch_handele((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->Disp_Net_Status_Handle(); break;
        case 28: _t->dev_update_ota_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->factory_hardware_check_task((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->dev_site_config_net_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: _t->factory_hardware_check_touch_handle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: _t->qtReboot(); break;
        case 33: _t->play_wav(); break;
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
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void Widget::Touch_Allow_Send_Handler()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
