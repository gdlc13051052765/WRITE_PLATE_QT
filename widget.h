#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>
#include <QPushButton>
#include <QtGui>
#include <QPainter>
#include <QLabel>
#include <QSlider>
#include <QMutex>
#include <QList>
#include <QQueue>
#include "led.h"
#include "music.h"
#include "audiothread.h"
#include "aplay_pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "dispNetStatus.h"

#include "settings.h"

#include "play.h"

//菜单更新状态
enum  Update_Menu_Status
{
    Update_Menu_Start = 0,//菜单开始更新
    Update_Menu_Process,//菜单更新中
    Update_Menu_Ok,//菜单更新成功
    Update_Menu_Fail,//菜单更新失败
    Update_Menu_Work_Status,//更新完成按确认键后进入工作状态
};
//固件更新进度
enum  Update_Ota_Status
{
    Update_Ota_Start = 0,//固件开始更新
    Update_Ota_Process,//固件更新中
    Update_Ota_Ok,//固件更新成功
    Update_Ota_Fail,//固件更新失败
    Update_Ota_Work_Status,//固件更新完成按确认键后进入工作状态
};
//网络配置状态机
enum  Config_Net_Status
{
    Config_Net_Status_Idle,//网络配置空闲
    Config_Net_Status_Process,//网络配置过程中
    Config_Net_Status_Ok,//网络配置成功
    Config_Iot_Connect_Ok,//IOT联网成功
    Config_Net_Status_Fail//网络配置失败
};

enum  Display_Status
{
    Display_Status_Init,
    Display_Status_Succeed,
    Display_Status_Disappear,
    Display_Status_Failed,
    Display_Status_Multi_Disk_Exist,
    Display_Status_No_Menu_Information,
    Display_Status_Failed_Read_Label,
    Display_Status_Config_Net_OK,//网络配置成功
    Display_Status_Config_Net_Fail,//网络配置失败
    Display_Status_Menu_Ver,//显示菜单版本
    Display_Status_Dev_Msg,//设备信息
    Display_Status_Net_Msg,//显示网络信息
    Display_Status_Change_Net,//设置界面重新配网
    Display_Status_Restore_Factory//恢复出厂设置
};

class Widget_Message : public QWidget
{
    Q_OBJECT

    public:
        Widget_Message(QWidget *parent = 0);
        ~Widget_Message();
        void  paintEvent(QPaintEvent *);
        enum  Display_Status Display_Status_Current;
        enum  Display_Status Display_Status_Previous;
        QPushButton *Son_Button_Cancel;   //取消按钮
        QStringList Son_Button_Cancel_list;
        QPushButton *Son_Button_Determine;   //确定按钮
        QStringList Son_Button_Determine_list;
    private:
        QFont itemFont;         //菜单字体样式
        
};



enum Send_Message_Sort
{
    Send_Message_Write,
    Send_Message_Write_Finish,

    Send_Config_Net,//配置网络
    Send_Config_Net_Finsh,

    Send_Message_Stop,
    Send_Message_Stop_Finish,

    Send_Status_Message_Response,
    Send_Display_Disapper_Response,
    Send_Response_Finish,
    Send_Change_Dev_Status_Response,
    Send_Unbind_Cmd_To_Golang,//发送解绑指令到后台
    Send_Reboot_Ota_golang_App,//重启OTA后台程序
    Send_Reboot_Golang_App
};

enum Rcv_Message_Sort
{
    Rcv_Message_Write_Return,
    Rcv_Message_Stop_Finish,
    Rcv_Display_Status_Succeed,
    Rcv_Display_Status_Failed,
    Rcv_Display_Status_Multi_Disk_Exist,
    Rcv_Display_Status_No_Menu_Information,
    Rcv_Display_Status_Failed_Read_Label,
    Rcv_Display_Status_Disappear
};

//设备状态
enum Dev_Status_Sort
{
    INIT_STATUS           = 0 ,//生产状态
	WAIT_TOPIC_STATUS ,        //证书获取完成等待订阅
	FACTORY_BIND_OK_STATUS ,   //完成出厂绑定（解绑状态）
	SITE_BIND_OK_STATUS ,      //现场绑定完成，可以正常写盘拉菜单
	LOCAL_UNBIND_STATUS ,      //本地解绑完成，一直上传解绑状态到后台
    UPDATE_OTA_STATUS ,        //设备固件更新状态
    SITE_CONFIG_NET_STATUS ,   //现场配置网络状态
    FACTORY_CHECK_STATUS ,     //工厂硬件检测状态
    SETUP_STATUS ,//设置界面状态
};

//工厂检测命令
enum Factory_Check_Sort
{
    SCREEN_CHECK = 0xB0, //屏幕测试
    TOUCH_CHECK, //触摸测试
    WIFI_CHECK,  //触摸测试
    BLUE_CHECK,  //蓝牙测试
    BEEP_CHECK,  //蜂鸣器测试
    RFID_CHECK,  //读盘测试
    MOTOR_CHECK, //电机测试
    VOICE_CHECK,  //语音测试
    LED_CHECK  //灯带测试
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
    bool firstFlag = false;
    volatile unsigned char High_Scroll_Flage;
    volatile int Add_Step_By_Step;  //变化数
    int Is_Speed_Low_Scroll;        //慢滑动标志位

    int Last_Update_GUI;            //上电和最终初始化GUI 标志位

    int totalItemNum = 0;       //总的item数量
    int selectItemIndex = 0;    //当前选中item索引
    int aplaySelectItemIndex = 0;    //当前选中item索引

    int selectItemIndex_Page0;    //当前Page0选中item索引
    int selectItemIndex_Page1;    //当前Page1选中item索引

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
    QStringList tempList ;

    QFont itemFont;         //菜单字体样式

    Widget_Message* p_Widget_Meaasge;
    DispNetStatus* p_Widget_Disp_Net;
    
    unsigned char Update_Message_Tip;
    unsigned char Move_Prompt_Message_Times;

    enum  Rcv_Message_Sort Rcv_Message_Status;

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

    unsigned char Scroll_Times_Quick_Last;
    unsigned char Scroll_Times_Quick_Last_Period;

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

    //QTimer *Quick_Scroll_Slot_Timer;

    QTimer *Motor_Shake_Slot_Timer;

    QTimer *Communicate_Msg_QT_Go_Timer;
    QTimer *Disp_Net_Status_Timer;
    QTimer *Hardware_Check_Update_Timer;
    QTimer *Setup_Touch_Check_Timer;

    QTimer *Test_Timer;
    QTimer *Alarm_Timer;
    QLabel *Label_Button_Text;

    QTimer *Update_Prompt_Message_Timer;

    QStringList Button_Determine_list;
    QStringList Button_Cancel_list;
    QStringList Touch_Value_list;//touch值list
    QList<int> Setup_Touch_Value_list;//设置touch值list
    QList<long long int> Setup_Touch_Time_list;//设置touch触发时间list
    QPushButton *Button_Determine;//确定按钮
    QPushButton *Button_Cancel;   //取消按钮

    QPushButton *Button_Play_Alarm;   //播放按键音
    QProcess BlueProcess;//蓝牙测试命令行

    QSlider *Slider_p;            //滑动条
    QSlider *Setup_Slider_p;            //滑动条

    Led Shock_Motor;

    char Slow_Alarm_Flage;
    //Music * p_Music_Alarm;
    AudioThread audio;            //
    //SoundThread audio;

    QTimer *Slow_Scroll_Music_Send_Timer;

    volatile unsigned char Slow_Scroll_Music_Times_Start_Flage;
    volatile unsigned char Slow_Scroll_Music_Times_Stored;

    QMutex Draw_GUI_Mutex;
    QMediaPlayer *player ;

    QSound *sound;

    Play* play;
    SettingsData *settings;

private slots:

    void Update_Prompt_Message_Timer_Handle();
    void Slow_Scroll_Music_Send_Timer_Handle();

    void Communicate_Msg_QT_Go_Handle();

    void Alarm_Timer_Handle();

    void Motor_Shake_Slot_Timer_Handler();
    void Hardware_Check_Update_Handle();
    void Setup_Touch_Check_Handle();

    //void Quick_Scroll_Update_Timer_Slot_Handle();
    void Test_Handler();

    void Slow_Scroll_Timer_Handle();
    void Release_TimeOut_Update_GUI();
    void Release_Slow_Dir_TimeOut_Update_GUI();
    void Release_Slow_Dir_Near_Update_GUI();

    unsigned char Cal_Crc(unsigned char *Data, unsigned char Len);
    void removeListSame(QStringList *list);
    //void Quick_Scroll_Update_Timer_Handle();
    void Scroll_Quick_TimeOut_Update_GUI();
    //void Quick_Scroll_Update_Timer_Flage();

    void Handle_Touch_Value_Event(unsigned short Receive_Diff_Data_Total);
    void dev_factory_init_status_task(int);
    void dev_site_config_net_task(int status);
    void dev_configh_net_task();
    void dev_factory_bind_task(QString,int);
    void dev_work_status_task();
    void dev_update_ota_task();
    void dev_factory_init_status_touch_handle(int);
    void dev_configh_net_touch_handle(int);
    void dev_factory_bind_touch_handle(int);
    void dev_work_status_touch_handele(int);
    void Disp_Net_Status_Handle();
    void dev_update_ota_touch_handle(int);
    void factory_hardware_check_task(int status);
    void dev_site_config_net_touch_handle(int Receive_Diff_Data_Total);
    void factory_hardware_check_touch_handle(int Receive_Diff_Data_Total);
    void qtReboot();
    void play_wav();
    
signals:

    void Touch_Allow_Send_Handler(void);

    //void Quick_Scroll_Slot(unsigned short  Receive_Diff_Data_Total);     //声明一个信号，不需要实现 快滑叠加信号
};

int alsa_play(char *wav_name,unsigned int val,char chs);
#endif // WIDGET_H
