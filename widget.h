#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMutex>
#include <QList>
#include <QQueue>
#include "led.h"
#include "music.h"
#include "audiothread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>



enum  Display_Status
{
    Display_Status_Init,
    Display_Status_Succeed,
    Display_Status_Failed,
    Display_Status_Multi_Disk_Exist,
    Display_Status_No_Menu_Information,
    Display_Status_Failed_Read_Label
};

class Widget_Message : public QWidget
{
    Q_OBJECT

public:
    Widget_Message(QWidget *parent = 0);
    ~Widget_Message();
    void paintEvent(QPaintEvent *);
    enum  Display_Status Display_Status_Current;
    enum  Display_Status Display_Status_Previous;
private:
    QFont itemFont;         //菜单字体样式
};



enum Send_Message_Sort
{
    Send_Message_Write,
    Send_Message_Stop,
    Send_Message_Write_Finish,
    Send_Message_Stop_Finish,
    Recv_Message_Write_Return,
    Recv_Message_Stop_Return,
    Send_Message_Response_Succeed,
    Send_Message_Response_Failed,
    Send_Message_Response_Finish
};


struct mymesg
{
    long int mtype;
    unsigned char mtext[10];
};

extern QMutex mutex;
extern volatile unsigned char Current_Read_Press_Value;

class Widget : public QWidget
{
    Q_OBJECT

public:

    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *); //重绘函数
    void paintItems(QPainter *, QStringList*, int index, QFont*); //绘制文字

private:
    volatile unsigned char High_Scroll_Flage;
    volatile int Add_Step_By_Step;  //变化数
    int Is_Speed_Low_Scroll;        //慢滑动标志位

    int Last_Update_GUI;            //上电和最终初始化GUI 标志位

    int totalItemNum;       //总的item数量
    int selectItemIndex;    //当前选中item索引
    QStringList menuList;   //菜单内容

    int showItemNum;        //显示的菜单个数
    int baseFontSize;       //中心字体大小
    int itemIntervalH;      //菜单垂直间隔
    int itemRealH;          //菜单实际间隔（菜单字符宽度+间隔）

    unsigned char Receive_Length;



    int Select_Show_Menu_Spell_Index;     //当前选中拼音首字母索引
    int Total_Show_Menu_Spell_Num;        //菜单拼音首字母个数
    QStringList Menu_Spell_Display_List;  //显示菜单拼音首字母
    QStringList Menu_Spell_List;          //菜单拼音首字母

    QFont itemFont;         //菜单字体样式

    Widget_Message* p_Widget_Meaasge;
    unsigned char Update_Message_Tip;
    unsigned char Move_Prompt_Message_Times;
    enum  Display_Status Rcv_Display_Status_Current;

    unsigned short Test_Times;
    int maxScrollTime;   //默认滑动最大值，时间时间1S
    int timeCount;      //滑动计时（1秒）

    int Touchpad_Flage; //

    int Released_Flage; //

    volatile unsigned char Quick_Scroll_Period;
    unsigned char Quick_Scroll_Times;

    unsigned char Quick_Add_Step;

    unsigned char Alarm_Flage;
    unsigned char Scroll_Flage;
    unsigned char Scroll_Flage_Old;

    unsigned int Scroll_Times;     //滑动次数
    unsigned char Scroll_Dir;                    //滑动方向 //滑动方向  1：向上滑动  2：向下滑动d

    unsigned char Release_Slow_Dir_Update_Times; // 松手滑动次数
    unsigned char Release_Quick_Dir_Update_Times; //松手滑动次数
    unsigned char Release_Slow_Update_Times;


    volatile unsigned char Quick_Scroll_Update_Flage;
    volatile unsigned char Widget_Page_Switch;

    unsigned char Quick_Alarm_Times_Flage;
    unsigned char Total_Page;
    volatile unsigned char Previous_Page;
    volatile unsigned char Current_Page;
    volatile unsigned char Scroll_Quick_Flage;
    unsigned char Release_Quick_TimeOut_Flage;

    key_t key_Rcv;
    int id_Rcv;
    struct mymesg Rcv_msg;

    key_t key_Snd;
    int id_Snd;
    struct mymesg Snd_msg;

    unsigned short Menu_Price;
    unsigned int Menu_ID;

    enum Send_Message_Sort Send_Message_Type;
    unsigned int Send_Message_Times;


    QString String_Display;
    QTimer *Slow_Scroll_Timer;   //慢滑定时器
    QTimer *Release_Timeout;
    QTimer *Release_Slow_Dir_Timeout;

    QTimer *Quick_Scroll_Update_Timer;  //快滑定时器
    QTimer *Release_Quick_Dir_Timeout;

    QTimer *Quick_Allow_Timer;

    QTimer *Quick_Scroll_Slot_Timer;

    QTimer *Motor_Shake_Slot_Timer;

    QTimer *Communicate_Msg_QT_Go_Timer;

    QTimer *Test_Timer;
    QTimer *Alarm_Timer;
    QLabel *Label_Button_Text;

    QTimer *Update_Prompt_Message_Timer;

    QStringList Button_Determine_list;
    QStringList Button_Cancel_list;
    QPushButton *Button_Determine;//确定按钮
    QPushButton *Button_Cancel;   //取消按钮


    QSlider *Slider_p;            //滑动条

    Led Shock_Motor;
    Music * p_Music_Alarm;
    AudioThread audio;            //

private slots:

    void Update_Prompt_Message_Timer_Handle();


    void Communicate_Msg_QT_Go_Handle();

    void Alarm_Timer_Handle();

    void Motor_Shake_Slot_Timer_Handler();

    void Quick_Scroll_Update_Timer_Slot_Handle();
    void Test_Handler();

    void Slow_Scroll_Timer_Handle();
    void Release_TimeOut_Update_GUI();
    void Release_Slow_Dir_TimeOut_Update_GUI();
    void Release_Slow_Dir_Near_Update_GUI();

    unsigned char Cal_Crc(unsigned char *Data, unsigned char Len);

    //void Quick_Scroll_Update_Timer_Handle();
    void Scroll_Quick_TimeOut_Update_GUI();
    //void Quick_Scroll_Update_Timer_Flage();
    void Handle_Touch_Value_Event(unsigned short Receive_Diff_Data_Total);



signals:

    void Touch_Allow_Send_Handler(void);

    void Quick_Scroll_Slot(unsigned short  Receive_Diff_Data_Total);     //声明一个信号，不需要实现 快滑叠加信号
};


#endif // WIDGET_H
