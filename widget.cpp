#include "widget.h"
#include <sys/time.h>
#include <QtGui>
#include <QPainter>
#include <QBrush>
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QTime>
#include <QTextCodec>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QSound>
#include <sys/ioctl.h>
#include "led.h"
#include "sqlite3.h"
#include "dispNetStatus.h"
#include "iic_pthread.h"
#include <sys/unistd.h>
#include "i2c-dev.h"
#include <stdint.h>
#include "audiothread.h"
#include <QProcess>
#include <QDir>
#include <QAudioFormat>  
#include <QAudioOutput> 
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <alsa/asoundlib.h>
#include <math.h>
#include <inttypes.h>
#include <QImage>

#define   ALARM_SLOW_ENABLE
#define   MOTOR_SHAKE_MS                    100
#define   SLOW_SCROLL_RELEASED_UPDATE_MS    50
#define   SPEED_SORT          1     //高速低速分界线
#define   FRAME               7     //FRAME
#define   FONT_STEP_VALUE     2

//#define DRAW_DEBUG //调试是否画框

//#define   ALARM_ENABLE   //打开喇叭
// #define   ALARM
//#define   RELEASED_QUICK
//#define   SEND_MSG_ENABLE

struct timeval tpstart,tpend;
float timeuse;

// static int  Address_Font[15][7] = {
// /*向下翻页*/
// 93,   168,    260,   340,   404,    451,    0,    //        0
// 82,   156,    248,   332,   400,    450,    0,    //        1
// 71,   144,    236,   324,   396,    449,    0,    //        2
// 60,   132,    224,   316,   392,    448,    0,    //        3    只显示6个
// 56,   124,    213,   305,   381,    438,    476,  //        4
// 52,   116,    202,   294,   370,    428,    468,  //        5
// 48,   108,    191,   283,   359,    418,    460,  //        6
// 44,   104,    180,   272,   348,    408,    452,  //        中间    7
// /*向上翻页*/
// 32,   93,   168,    260,   340,   404,    451,      //        8
// 20,   82,   156,    248,   332,   400,    450,      //        9
// 8,    71,   144,    236,   324,   396,    449,      //       10
// 0,    60,   132,    224,   316,   392,    448,      //        11 只显示6个
// 0,    56,   124,    213,   305,   381,    438,      //        12
// 0,    52,   116,    202,   294,   370,    428,      //        13
// 0,    48,   108,    191,   283,   359,    418,      //        14
// };

static int  Address_Font[15][7] = {
/*向下翻页*/
93,   168,   260,   340,   404,   451,   0,    //        0
82,   156,   248,   332,   400,   450,   0,    //        1
71,   144,   236,   324,   396,   449,   0,    //        2
60,   132,   224,   316,   392,   448,   0,    //        3    只显示6个
56,   124,   213,   305,   381,   438,   476,  //        4
52,   116,   202,   294,   370,   428,   468,  //        5
48,   108,   191,   283,   359,   418,   460,  //        6
34,   68,    122,   196,   280,   351,   410,  //        中间    7
/*向上翻页*/      
34,   68,   122,   196,   280,   353,   406,      //        8
24,   58,   112,   186,   270,   343,   406,      //       9
14,   48,   102,   176,   262,   333,   396,      //        10 
0,    36,   94,    168,   254,   325,   388,      //        11只显示6个
0,    30,   88,    162,   248,   319,   382,      //        12
0,    24,   82,    156,   242,   313,   376,      //        13
0,    20,   78,    152,   238,   309,   372,      //        14
};

static int  Address_Spell[7] = {130, 170, 210, 250, 290, 330, 370};
//static int  Virtual_Font[7] = {22, 38, 54, 70, 54, 38, 22};
static int  Virtual_Font[7] = {25, 40, 55, 70, 55, 40, 25};
//static int  Virtual_Font[7] = {20, 28, 32, 54, 32, 28, 20};
static int  Virtual_Disp_Start_Add[7] = {34, 68, 122, 196, 280, 351, 410};//虚化显示画框的起始纵坐标
static int  Virtual_Disp_Width[7] = {40, 72, 100, 120, 100, 72, 40};//虚化显示的高度
static bool left_btn_press = false;
static bool right_btn_press = false;

static bool is_show_left_btn  = false;
static bool is_show_right_btn = false;

static bool left_btn_dblClick  = false;
static bool right_btn_dblClick = false;

QString pMenudbName = "menu_lev1";//选中的菜单数据库名字
QString pselectMenuName;//选中的菜单名字
QString pPreSelectMenuName;//选中的菜单的前菜单名字用于最上面的显示
Sqlite3 mSqliteClass;//sqlite类
configMsgSt mDevMsg;//设备信息
IIC_MyThread touchClass;//touch类
int NetConfigStatus = 0;//网络配置状态机
menuSt mMenu;//菜单信息

bool ConnectNetStatus = false;//网络连接状态
int updateMenuStatus = -1;//菜单更新状态
int updateOtaStatus = -1;//固件更新状态
int updateProcess = 0;//进度条更新进度
int devStatusBak = -1;//设备状态备份
int hardwareCheckCmd = -1;//工厂硬件检测指令
int hardwareCheckCustom = -1;//工厂硬件检测自定义字节
int blueCheckResult = -1;//蓝牙检测结果
static bool touch_check_start = false;//touch检测
static int rgbTest = 0;
long long int p_menu_ver = 0;//菜单版本

QString dishName = "";//选中的菜品名称
QString dispProcess = "";//显示搜索进度

QString ipAddr = "";//ip地址
QString macAddr = "";//mac地址
QString ssidStr = "";//ssid名字
QString linuxVer = "";//linux版本

bool backUpFlag = false;//启动启动失败，运行的备份程序
bool touchLock = false;//按键锁

int selectItemIndexBak = 0;//菜品标号备份
bool quickSlowMoveFlag = false;//快慢滑动标记


void Widget:: play_wav()
{
    //while(1)
    {
        if(!play->isRunning())
        {   
            play->start();
        }  
    }
}


Widget::Widget(QWidget *parent): QWidget(parent)
{
    qDebug() <<"QT version 0.0.1";
    if(backUpFlag == false)
     mSqliteClass.Sqlite_update_process_db_run();//进程数据库写入运行成功标记位

    mMenu.nextmenu = "menu_lev1";
    key_Rcv = ftok("/tmp", 88);
    //创建一个权限为0666（所有用户可读可写，具体查询linux权限相关内容）的消息队列，并返回一个消息队列ID，如果key值已经存在有消息队列了，则直接返回一个消息队列ID。
    id_Rcv = msgget(key_Rcv, IPC_CREAT|0666);
    if(id_Rcv == -1) {
        printf("create Rcv_msg error \n");
    }
    key_Snd = ftok("/tmp", 66);
    id_Snd = msgget(key_Snd, 0666|IPC_CREAT);
    if(id_Snd == -1) {
        printf("create Snd_msg error \n");
    }
    Snd_msg.mtype = 66;

    Communicate_Msg_QT_Go_Timer = new QTimer(this);          //  MSG消息队列定时器任务     
    connect(Communicate_Msg_QT_Go_Timer, SIGNAL(timeout()), this, SLOT(Communicate_Msg_QT_Go_Handle()));
    Communicate_Msg_QT_Go_Timer->start(1000);

    Disp_Net_Status_Timer = new QTimer(this);          //  定时显示网络状态定时器任务     
    connect(Disp_Net_Status_Timer, SIGNAL(timeout()), this, SLOT(Disp_Net_Status_Handle()));
    Disp_Net_Status_Timer->start(1000);

    Hardware_Check_Update_Timer = new QTimer(this);          //  工厂硬件检测定时更新界面   
    connect(Hardware_Check_Update_Timer, SIGNAL(timeout()), this, SLOT(Hardware_Check_Update_Handle()));
    //Hardware_Check_Update_Timer->start(1000);

    Setup_Touch_Check_Timer = new QTimer(this);          //  设置界面按键检测
    connect(Setup_Touch_Check_Timer, SIGNAL(timeout()), this, SLOT(Setup_Touch_Check_Handle()));
    Setup_Touch_Check_Timer->start(300);

    Slow_Scroll_Music_Times_Stored = 0;
    Slow_Scroll_Music_Times_Start_Flage = 1;
    Slow_Scroll_Music_Send_Timer = new QTimer(this);          // 慢滑喇叭定时器      
    connect(Slow_Scroll_Music_Send_Timer, SIGNAL(timeout()), this, SLOT(Slow_Scroll_Music_Send_Timer_Handle()));

    Scroll_Times_Quick_Last = 0;   // 快滑 减速处理
    Scroll_Times_Quick_Last_Period = 0;

    Total_Page = 3;
    Current_Page = 0;
    Previous_Page = 0;
    Widget_Page_Switch = 0;
    Quick_Add_Step = 1;
    Scroll_Quick_Flage = 0;
    Quick_Alarm_Times_Flage = 0;

    Scroll_Flage = 0;
    Scroll_Flage_Old = 0;
    Button_Determine_list.append("color:white");                         // 前景色
    Button_Determine_list.append("background-color:rgb(167,203,74)");    // 背景色
    Button_Determine_list.append("border-style:outset");                 // 边框风格
    Button_Determine_list.append("border-width:0px");                    // 边框宽度
    Button_Determine_list.append("border-color:rgb(167,203,74)");        // 边框颜色
    Button_Determine_list.append("border-radius:10px");                  // 边框倒角

    Button_Determine = new QPushButton(this);
    Button_Determine->setStyleSheet((Button_Determine_list.join(';')));
    Button_Determine->move(718,451);
    Button_Determine->resize(75, 20);
    Button_Determine->hide();//确定按键

    Button_Cancel_list.append("color:white");                         // 前景色
    Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
    Button_Cancel_list.append("border-style:outset");                 // 边框风格
    Button_Cancel_list.append("border-width:0px");                    // 边框宽度
    Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
    Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

    Button_Cancel = new QPushButton(this);
    Button_Cancel->setStyleSheet(Button_Cancel_list.join(';'));
    Button_Cancel->move(718,9);
    Button_Cancel->resize(75, 20);
    Button_Cancel->hide(); //取消按键

    Scroll_Times = 0;   //滑动次数
    Scroll_Dir = 0;     //慢滑方向

    Release_Slow_Dir_Update_Times = 0; // 松手滑动次数
    Release_Quick_Dir_Update_Times = 0; //松手滑动次数
    Release_Slow_Update_Times = 0;
    String_Display = "";
    Release_Quick_TimeOut_Flage = 0;

    Last_Update_GUI = 1;
    Slow_Alarm_Flage = 1;
    Touchpad_Flage = 0;

    Quick_Scroll_Period = 0;
    Scroll_Times = 0;
    //p_Music_Alarm = new Music();
    Slider_p =new QSlider(this);
    Slider_p->setOrientation(Qt::Vertical);
    Slider_p->setGeometry (752, 21, 6, 373);
    Slider_p->setMinimum(0);  // 最小值
    Slider_p->setMaximum(0);  // 最大值

    //margin-left:-3pxmargin-left 就是设置标签的左外边距
    Slider_p->setStyleSheet("QSlider::groove:vertical{ height: 373px; width: 4px;  border-radius:2px;  left: 20px;        right: 20px;         background: #707070;}\
                             QSlider::handle:vertical{ height: 39px;  width: 6px;  border-radius:3px;  margin-left:-1px;  margin-right:-1px;   background: #A7CB4A;}");

    if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
    {
      Slider_p->setValue(totalItemNum-1-selectItemIndex);
    }
    Slider_p->hide();

    Setup_Slider_p =new QSlider(this);
    Setup_Slider_p->setOrientation(Qt::Vertical);
    Setup_Slider_p->setGeometry (752, 90, 6, 300);
    // Setup_Slider_p->move(752,80);
    Setup_Slider_p->setMinimum(0);  // 最小值
    Setup_Slider_p->setMaximum((5)*5.4);  // 最大值
    Setup_Slider_p->setSingleStep(5);  // 步长
    //margin-left:-3pxmargin-left 就是设置标签的左外边距
    Setup_Slider_p->setStyleSheet("QSlider::groove:vertical{ height: 373px; width: 4px;  border-radius:2px;  left: 20px;        right: 20px;         background: #707070;}\
                            QSlider::handle:vertical{ height: 39px;  width: 6px;  border-radius:3px;  margin-left:-1px;  margin-right:-1px;   background: #A7CB4A;}");

    Setup_Slider_p->hide();

    Menu_ID = 0;
    baseFontSize = 70;  //中心字符大小
    //设置文字为粗体
    itemFont.setBold(true);               //封装的setWeight函数
    //设置文字为斜体
    //itemFont.setItalic(true);           //封装的setStyle函数
    //设置文字大小
    itemFont.setPointSize(baseFontSize);  //中心字符大小
    //设置文字倾斜
    //itemFont.setStyle(QFont::StyleItalic);
    //设置文字粗细//enum Weight 存在5个值
    itemFont.setWeight(QFont::Light);

    QFontMetrics fm(itemFont);
    qDebug() << fm.height()<<"16619851335";           //获取文字高度  105  327
    qDebug() << fm.maxWidth()<<"16619851335";         //获取文字宽度

    //itemRealH = fm.height()*20/19 + itemIntervalH;  //计算实际间隔 //菜单实际间隔（菜单字符宽度+间隔）
    itemRealH = 0;

    Update_Message_Tip = 1;
    p_Widget_Meaasge = new Widget_Message(this);
    p_Widget_Meaasge->resize(800, 480);
    p_Widget_Meaasge->move(0, 0);
    p_Widget_Meaasge->hide();
    p_Widget_Meaasge->Son_Button_Cancel->hide();
    p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键

    //创建显示网络状态
    p_Widget_Disp_Net = new DispNetStatus(this);
    p_Widget_Disp_Net->resize(800, 480);
    p_Widget_Disp_Net->move(0, 0);
    p_Widget_Disp_Net->hide();

    //Page0_selectItemIndex = totalItemNum/2;
    selectItemIndex_Page0 = -1;    //当前Page0选中item索引
    selectItemIndex_Page1 = -1;    //当前Page1选中item索引

    Motor_Shake_Slot_Timer = new QTimer(this); //震动电机  OK
    connect(Motor_Shake_Slot_Timer, SIGNAL(timeout()), this, SLOT(Motor_Shake_Slot_Timer_Handler()));

    Release_Quick_Dir_Timeout = new QTimer(this); //快滑松手  快滑滑动松手根据前一刻方向 来滑最后一格
    connect(Release_Quick_Dir_Timeout, SIGNAL(timeout()), this, SLOT(Scroll_Quick_TimeOut_Update_GUI()));

    Test_Timer = new QTimer(this);
    connect(Test_Timer, SIGNAL(timeout()), this, SLOT(Test_Handler()));
    

    Release_Timeout = new QTimer(this); //慢滑松手 靠近最近的那个 OK
    connect(Release_Timeout, SIGNAL(timeout()), this, SLOT(Release_TimeOut_Update_GUI()));

    Release_Slow_Dir_Timeout = new QTimer(this); //慢滑松手  根据方向  来决定滑动  OK
    connect(Release_Slow_Dir_Timeout, SIGNAL(timeout()), this, SLOT(Release_Slow_Dir_TimeOut_Update_GUI()));

    Slow_Scroll_Timer = new QTimer(this); //慢滑滑动 处理函数   OK
    connect(Slow_Scroll_Timer, SIGNAL(timeout()), this, SLOT(Slow_Scroll_Timer_Handle()));

    Alarm_Timer = new QTimer(this); //闹钟 处理函数  OK
    connect(Alarm_Timer, SIGNAL(timeout()), this, SLOT(Alarm_Timer_Handle()));
    

    Update_Prompt_Message_Timer = new QTimer(this); //处理提示界面慢慢往下滑动  OK
    connect(Update_Prompt_Message_Timer, SIGNAL(timeout()), this, SLOT(Update_Prompt_Message_Timer_Handle()));

    qDebug() << fm.height()<<"16619851335+";  //获取文字高度  105   327
    qDebug() << fm.maxWidth()<<"16619851335+";//获取文字宽度

    mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
    printf("设备状态 = %d\n",mDevMsg.devStatus );
    printf("设备ID = %d\n", mDevMsg.devId );
    qDebug()<<"设备sn ="<<mDevMsg.sn ;
    qDebug()<<"菜单版本 ="<<mDevMsg.menu_ver;
    qDebug()<<"硬件版本 ="<<mDevMsg.har_ver;
    qDebug()<<"软件版本 ="<<mDevMsg.soft_ver;
    qDebug()<<"固件更新 ="<<mDevMsg.otaResult ;
    if(mDevMsg.otaResult == "ok") {
        qDebug()<<"固件更新成功";
        devStatusBak = mDevMsg.devStatus;
        mDevMsg.devStatus = UPDATE_OTA_STATUS;//设备状态进入固件更新状态
        updateProcess = 100;//进度条进度
        updateOtaStatus = Update_Ota_Process;
        mSqliteClass.Sqlite_update_ota_result();
        NetConfigStatus = Config_Net_Status_Ok;//网络配置成功
        Disp_Net_Status_Timer->stop();
        update();
        Send_Message_Type = Send_Reboot_Ota_golang_App;
    } 
    else if(mDevMsg.otaResult == "fail") {
        qDebug()<<"固件更新失败";
        devStatusBak = mDevMsg.devStatus;
        mDevMsg.devStatus = UPDATE_OTA_STATUS;//设备状态进入固件更新状态
        updateProcess = 100;//进度条进度
        updateOtaStatus = Update_Ota_Fail;
        mSqliteClass.Sqlite_update_ota_result();
        NetConfigStatus = Config_Net_Status_Ok;//网络配置成功
        Disp_Net_Status_Timer->stop();
        update();
    }
    else {
        qDebug()<<"固件无需更新";
    }
    p_menu_ver = mDevMsg.menu_ver;

    qDebug()<<"语音播放。。。。";
    play = new Play(settings);
    play->open_and_print_file_params("/home/meican/test.wav");
    play->SetWavFileName("/home/meican/Alarm.wav");
    play_wav();
}

Widget::~Widget()
{

}

void Widget::Update_Prompt_Message_Timer_Handle()
{
    Move_Prompt_Message_Times++;
    qDebug()<<"Update_Prompt_Message_Timer_Handle";
    if(Move_Prompt_Message_Times <= 4)
    {
        qDebug()<<"p_Widget_Meaasge->show-----------------------";
        if(p_Widget_Meaasge->Display_Status_Current != p_Widget_Meaasge->Display_Status_Previous) {
            qDebug()<<"!=";
            Move_Prompt_Message_Times = 0;
            //p_Widget_Meaasge->update();
            p_Widget_Meaasge->Display_Status_Previous = p_Widget_Meaasge->Display_Status_Current;
        }

        p_Widget_Meaasge->move(0, 0 + Move_Prompt_Message_Times*60);
    } else {  
        if(Update_Prompt_Message_Timer->isActive() == true) {
            Move_Prompt_Message_Times = 0;
            Update_Prompt_Message_Timer->stop();
        }
    }
}

#define    SLOW_SCROLL__MUSIC_PERIOD    30
/*
*   慢滑声音次数响动处理函数
*/
void Widget::Slow_Scroll_Music_Send_Timer_Handle()
{
    if(Slow_Scroll_Music_Times_Stored > 0) {
        Slow_Scroll_Music_Times_Stored--;
        qDebug()<<"Send audio.play()";
       // audio.play();
    }
}

void Widget::Test_Handler()
{
    //audio.play();
}
/**********************************************************/
void Widget::Alarm_Timer_Handle()
{
    //audio.play();
}

void Widget::Motor_Shake_Slot_Timer_Handler()
{
    if(Motor_Shake_Slot_Timer->isActive() == true)
    {
        Motor_Shake_Slot_Timer->stop();
    }
    Shock_Motor.Led_Off();
}

unsigned char Widget::Cal_Crc(unsigned char *Data, unsigned char Len)
{
    unsigned char Crc = 0, i = 0;

    for(i=0; i<Len; i++) {
        Crc = Crc ^ Data[i];
    }
    return Crc;
}

//QStringList 滤重
void Widget::removeListSame(QStringList *list)
{
    for (int i = 0; i < list->count(); i++) {
        for (int k = i + 1; k <  list->count(); k++) {
            if ( list->at(i) ==  list->at(k)) {
                list->removeAt(k);
                k--;
            }
        }
    }
}

/*==================================================================================
* 函 数 名： Disp_Net_Status_Handle
* 参    数：
* 功能描述:  定时显示网络状态定时器回调函数
* 返 回 值： 显示网络状态
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
bool netDispReverse = false;
void Widget::Disp_Net_Status_Handle()
{
    if (netDispReverse == false) {
        netDispReverse = true;
        if( Current_Page == 2) {
            p_Widget_Disp_Net->set_disp_content("网络中断",true);
        } else {
            p_Widget_Disp_Net->set_disp_content("网络中断",false);
        } 
        p_Widget_Disp_Net->show();
    } else {
        netDispReverse = false;
        p_Widget_Disp_Net->hide();
    }    
}

/*==================================================================================
* 函 数 名： Disp_Net_Status_Handle
* 参    数：
* 功能描述:  硬件检测定时器回调函数
* 返 回 值： 
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::Hardware_Check_Update_Handle()
{
    if (mDevMsg.devStatus = FACTORY_CHECK_STATUS) {
        update();
    }    
}

/*==================================================================================
* 函 数 名： Setup_Touch_Check_Handle
* 参    数：
* 功能描述:  定时检测是否触发设置按键
* 返 回 值： 
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::Setup_Touch_Check_Handle()
{
    int i = 0;
    
    if(Setup_Touch_Value_list.contains(255) == true) //判断指定数据是否在列表中 在 返回true
    {
        Setup_Touch_Value_list.removeLast();    //删除最后一个数据
        Setup_Touch_Time_list.removeLast();     //删除最后一个数据
        QList<int> otherList = Setup_Touch_Value_list; //list 复制
        qSort(otherList.begin(), otherList.end());  //排序-从小到大

        if(Setup_Touch_Value_list.empty() == true)  return;

        if(otherList.constLast() > 90 & //touch起始值在最下面
            (Setup_Touch_Value_list.at(0) - Setup_Touch_Value_list.at(Setup_Touch_Value_list.count()-1) > 20) &//滑动距离
            (Setup_Touch_Time_list.at(Setup_Touch_Value_list.count()-1) - Setup_Touch_Time_list.at(0) < 100 ) &
            otherList.at(0) <30) //滑动时间
        {
            qDebug() << "设置按键触发";
            devStatusBak = mDevMsg.devStatus;//备份当前设备状态，设置界面退出后返回当前设备状态
            mDevMsg.devStatus = SETUP_STATUS;//设备状态
            Widget_Page_Switch = 0;
            Current_Page = 1;
            selectItemIndex = 2;
            //Setup_Touch_Check_Timer.stop();
            update();
        }
        qDebug() << "Setup_Touch_Time_list  = " << Setup_Touch_Time_list;
        qDebug() << "Setup_Touch_Value_list = " << Setup_Touch_Value_list;
        qDebug() << "otherList = " << otherList;
        Setup_Touch_Value_list.clear();
        Setup_Touch_Time_list.clear();
    }
}

/*==================================================================================
* 函 数 名： Communicate_Msg_QT_Go_Handle
* 参    数：
* 功能描述:  定时查询跟golang之间消息队列定时器回调函数
* 返 回 值： 
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::qtReboot()
{
    // QString program = QApplication::applicationFilePath();
    // QStringList arguments = QApplication::arguments();
    // QString workingDirectory = QDir::currentPath();
    // QProcess::startDetached(program, arguments, workingDirectory);
    // QApplication::exit();

    qApp->exit(777);//调用exit()，退出应用程序。
}


void Widget::Communicate_Msg_QT_Go_Handle()
{
    int postionStart = 0, i = 0;
    int postionEnd = 0;
  
    QString strTemp = "";
    QByteArray result_;

    if(Send_Message_Type == Send_Message_Write_Finish) {
        Send_Message_Times++;
        if(Send_Message_Times >= 20) {
            Send_Message_Type = Send_Message_Write;
            Send_Message_Times = 0;
            qDebug()<<"send Write again";// add your code
        }  
    }
    if(Send_Message_Type == Send_Message_Stop_Finish) {
        Send_Message_Times++;
        if(Send_Message_Times >= 20) {
            Send_Message_Type = Send_Message_Stop;
            Send_Message_Times = 0;
            qDebug()<<"send stop again";// add your code
        }
    }
    if(Send_Message_Type == Send_Message_Write) {
        Snd_msg.mtext[0] = 0x06;
        Snd_msg.mtext[1] = 0x01;
        Snd_msg.mtext[2] = Menu_ID;
        Snd_msg.mtext[3] = Menu_ID>>8;
        Snd_msg.mtext[4] = Menu_ID>>16;
        Snd_msg.mtext[5] = Menu_ID>>24;
        Snd_msg.mtext[6] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n"; 
        }
        Send_Message_Type = Send_Message_Write_Finish;
        Send_Message_Times = 0;
        qDebug()<<"send Write";
    }
    if(Send_Message_Type == Send_Message_Stop) { 
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0x02;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type = Send_Message_Stop_Finish;
        Send_Message_Times = 0;
        qDebug()<<"send stop ";
    }
    if(Send_Message_Type == Send_Config_Net) { //发送配网指令
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0x03;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type  = Send_Config_Net_Finsh;
        Send_Message_Times = 0;
        qDebug()<<"Send_Config_Net send stop ";
    }
    if(Send_Message_Type == Send_Unbind_Cmd_To_Golang) { //发送解绑指令到golang后台
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0x05;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type  = Send_Config_Net_Finsh;
        Send_Message_Times = 0;
        qDebug()<<"Send_Config_Net send stop ";
    }
    if(Send_Message_Type == Send_Reboot_Ota_golang_App) { //发送ota重启指令到golang后台
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0x06;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type  = Send_Config_Net_Finsh;
        Send_Message_Times = 0;
        qDebug()<<"Send_Reboot_Ota_golang_App send stop ";
    } 

    if(Send_Message_Type == Send_Reboot_Golang_App) { //发送重启指令到golang后台
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0x07;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type  = Send_Config_Net_Finsh;
        Send_Message_Times = 0;
        qDebug()<<"Send_Rebootgolang_App send stop ";
    } 

    if(Send_Message_Type == Send_Display_Disapper_Response) {
        Snd_msg.mtext[0] = 0x03;
        Snd_msg.mtext[1] = 0xA2;
        Snd_msg.mtext[2] = 0x00;
        Snd_msg.mtext[3] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        Send_Message_Type = Send_Response_Finish;
        Send_Message_Times = 0;
    }
    if(Send_Message_Type == Send_Status_Message_Response) {
        Snd_msg.mtext[0] = 0x02;
        Snd_msg.mtext[1] = 0xA1;
        Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
            qDebug()<<"Send_Status Response \n";  
        }
        Send_Message_Type  = Send_Response_Finish;
        Send_Message_Times = 0;
        qDebug()<<"Send_Status Response \n";      
    }
    if(Send_Message_Type == Send_Change_Dev_Status_Response) {//应答设备状态
        // Snd_msg.mtext[0] = 0x02;
        // Snd_msg.mtext[1] = 0xA5;
        // Snd_msg.mtext[2] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        // if(msgsnd(id_Snd, (void *)&Snd_msg,  10,  IPC_NOWAIT) == -1) {
        //     qDebug()<<"Send_Status Response \n";  
        // }
        // Send_Message_Type  = Send_Response_Finish;
        // Send_Message_Times = 0;
        // qDebug()<<"Change dev Status Response \n";    
    }
    if(msgrcv(id_Rcv, (void *)&Rcv_msg, 10, 88, IPC_NOWAIT) == -1) {
 
    } else {
        if(Rcv_msg.mtext[0] > 0) {
            qDebug()<<"receive >= 0";// 
            for(int i=0; i<Rcv_msg.mtext[0]; i++) {
                qDebug()<<" "<<Rcv_msg.mtext[i];
            }
            qDebug()<<"\r\n";
        } else {
            qDebug()<<"receive == 0";// 
        }
        Receive_Length = Rcv_msg.mtext[0];
        if(Cal_Crc(Rcv_msg.mtext, Receive_Length) == Rcv_msg.mtext[Receive_Length]) {
            if((Rcv_msg.mtext[1] == 0x01)&&(Rcv_msg.mtext[2] == 0x00))//设置成功------------------
            {
                Send_Message_Type  = Send_Response_Finish;
                Rcv_Message_Status = Rcv_Message_Write_Return;
                Send_Message_Times = 0;
                qDebug()<<"set succeed";// 
            }
            else if((Rcv_msg.mtext[1] == 0x01)&&(Rcv_msg.mtext[2] == 0x01))//设置失败-----------------
            {
                Send_Message_Type  = Send_Message_Write;
                Send_Message_Times = 0;
                qDebug()<<"set failued";// 
            }

            if((Rcv_msg.mtext[1] == 0x02)&&(Rcv_msg.mtext[2] == 0x00))//停止读写成功------------------
            {
                Send_Message_Type  = Send_Response_Finish;
                Send_Message_Times = 0;
                //Communicate_Msg_QT_Go_Timer->stop();
                p_Widget_Meaasge->hide();
                qDebug()<<"stop succeed";// 
            }
            else if((Rcv_msg.mtext[1] == 0x02)&&(Rcv_msg.mtext[2] == 0x01))//停止读写失败------------------
            {
                Send_Message_Type  = Send_Message_Stop;
                Send_Message_Times = 10;
                qDebug()<<"stop failed";// 
            }
            else if((Rcv_msg.mtext[1] == 0xA2)&&(Rcv_msg.mtext[2] == 0x00))//盘子拿走
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Disappear;
                p_Widget_Meaasge->hide();
                qDebug()<<"Disk go ahead";// 
                //Send_Message_Type = Send_Display_Disapper_Response;
                Send_Message_Type = Send_Response_Finish;

                touchLock = false;//解锁
            }
            else if((Rcv_msg.mtext[1] == 0xA1)&&(Rcv_msg.mtext[2] == 0x00))//写盘成功
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Succeed;
                p_Widget_Meaasge->show();
                qDebug()<<"write succeed---------------------";// 
               // Send_Message_Type = Send_Status_Message_Response;
                Send_Message_Type = Send_Response_Finish;

                touchLock = true;//按键加锁
            }
            else if((Rcv_msg.mtext[1] == 0xA1)&&(Rcv_msg.mtext[2] == 0x01))//写盘失败
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Failed;
                p_Widget_Meaasge->show();
                Move_Prompt_Message_Times = 0;
               // Send_Message_Type = Send_Status_Message_Response;
                Send_Message_Type = Send_Response_Finish;
                qDebug()<<"write failed";// 

                touchLock = true;//按键加锁
            }
            else if((Rcv_msg.mtext[1] == 0xA1)&&(Rcv_msg.mtext[2] == 0x02))//多盘存在
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Multi_Disk_Exist;
                p_Widget_Meaasge->show();
                //Send_Message_Type = Send_Status_Message_Response;
                Send_Message_Type = Send_Response_Finish;
                qDebug()<<"mutuile disk exist";// 

                touchLock = true;//按键加锁
            }
            else if((Rcv_msg.mtext[1] == 0xA1)&&(Rcv_msg.mtext[2] == 0x03))  //无菜单信息
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_No_Menu_Information;
                p_Widget_Meaasge->show();
                Send_Message_Type = Send_Status_Message_Response;
                qDebug()<<"no menu ";// 
            }
            else if((Rcv_msg.mtext[1] == 0xA1)&&(Rcv_msg.mtext[2] == 0x04))  //读标签失败
            {
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Failed_Read_Label;
                p_Widget_Meaasge->show();
               // Send_Message_Type = Send_Status_Message_Response;
                Send_Message_Type = Send_Response_Finish;
                qDebug()<<"read remark failed";// 

                touchLock = true;//按键加锁
            }

            if((Rcv_msg.mtext[1] == 0xA3)&&(Rcv_msg.mtext[2] == 0x00))  //配网成功
            {
                Send_Message_Type = Send_Config_Net_Finsh;
                qDebug()<<"qt 网络配置成功";// 
                NetConfigStatus = Config_Net_Status_Ok;//网络配置成功
                // if(mDevMsg.devStatus == SITE_CONFIG_NET_STATUS) {
                // if(devStatusBak != SETUP_STATUS) 
                //     mDevMsg.devStatus = FACTORY_BIND_OK_STATUS;
                // else 
                //     mDevMsg.devStatus = devStatusBak;   
                // }

                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                if(mDevMsg.devStatus == SITE_BIND_OK_STATUS)
                {
                    Current_Page = 0;
                    selectItemIndex = 0;
                    Widget_Page_Switch = 0;
                    p_Widget_Meaasge->hide();
                    p_Widget_Meaasge->Son_Button_Cancel->hide();
                    p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
                }
                qDebug() << "devStatusBak = " << devStatusBak << mDevMsg.devStatus;
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA3)&&(Rcv_msg.mtext[2] == 0x01))  //IOT联网成功
            {
                if(mDevMsg.devStatus == FACTORY_CHECK_STATUS) {
                    update();
                    return;
                }
                    
                NetConfigStatus = Config_Iot_Connect_Ok;
                ConnectNetStatus = true;//IOT连接成功 设置联网成功标记
                qDebug()<<"QT IOT 连接成功";//
                Disp_Net_Status_Timer->stop();
                p_Widget_Disp_Net->hide();
                // if(mDevMsg.devStatus == SITE_CONFIG_NET_STATUS) {
                //     if(devStatusBak != SETUP_STATUS) 
                //         mDevMsg.devStatus = FACTORY_BIND_OK_STATUS;
                //     else 
                //     {
                //         mDevMsg.devStatus = devStatusBak;  
                //     }          
                // }
                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                if(mDevMsg.devStatus == SITE_BIND_OK_STATUS)
                {
                    Current_Page = 0;
                    selectItemIndex = 0;
                    Widget_Page_Switch = 0;
                    p_Widget_Meaasge->hide();
                    p_Widget_Meaasge->Son_Button_Cancel->hide();
                    p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
                }
                qDebug() << "devStatusBak = " << devStatusBak << mDevMsg.devStatus;
                update();
                return;
            }
            else if((Rcv_msg.mtext[1] == 0xA3)&&(Rcv_msg.mtext[2] == 0x02))  //配网失败
            {
                Send_Message_Type = Send_Config_Net_Finsh;
                qDebug()<<"qt 网络配置失败";//
                ConnectNetStatus = false;//联网失败
                Disp_Net_Status_Timer->start(1000);
                if( Current_Page == 2) {
                    p_Widget_Disp_Net->set_disp_content("网络中断",true);
                } else {
                    p_Widget_Disp_Net->set_disp_content("网络中断",true);
                }    
                update();
            }

            if(Rcv_msg.mtext[1] == 0xA5)//更改设备状态
            {
                mDevMsg.devStatus = Rcv_msg.mtext[2];
                //devStatusBak = mDevMsg.devStatus;
                Send_Message_Type = Send_Change_Dev_Status_Response;
                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                printf("更改后设备状态 = %d\n", mDevMsg.devStatus);
                printf("更改后设备ID   = %d\n", mDevMsg.devId);
                qDebug()<<"更改后设备SN ="<<mDevMsg.sn;
                p_Widget_Meaasge->hide();
                Setup_Touch_Value_list.clear();
                Setup_Touch_Time_list.clear();
                update();
            }

            //菜单更新命令
            if((Rcv_msg.mtext[1] == 0xA6)&&(Rcv_msg.mtext[2] == 0x00))  //开始获取菜单
            {
                p_Widget_Disp_Net->hide();
                p_Widget_Meaasge->hide();
                updateMenuStatus = Update_Menu_Start;
                Disp_Net_Status_Timer->stop();
                updateProcess = 0;//进度条进度==0
                if(devStatusBak != -1)
                    mDevMsg.devStatus = devStatusBak;
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA6)&&(Rcv_msg.mtext[2] == 0x01))//菜单获取成功
            {
                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                printf("设备状态 = %d\n",mDevMsg.devStatus );
                printf("设备ID = %d\n", mDevMsg.devId );
                qDebug()<<"设备sn ="<<mDevMsg.sn ;
                qDebug()<<"菜单版本 ="<<mDevMsg.menu_ver;
                p_menu_ver = mDevMsg.menu_ver;
                
                updateMenuStatus = Update_Menu_Ok;
                qDebug() <<"qt 菜单更新成功";
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA6)&&(Rcv_msg.mtext[2] == 0x02))//菜单获取失败
            {
                updateMenuStatus = Update_Menu_Fail;
                qDebug() << "更新菜单失败";
                Disp_Net_Status_Timer->start(1000);
                p_Widget_Disp_Net->set_disp_content("更新菜单失败，请再次更新",true);   
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA6)&&(Rcv_msg.mtext[2] == 0x03))//菜单更新中
            {
                updateMenuStatus = Update_Menu_Process;
                updateProcess = Rcv_msg.mtext[3];//更新进度
                qDebug() <<"菜单更新进度 "<<Rcv_msg.mtext[3];
                update();
            }

            //固件更新命令
            if((Rcv_msg.mtext[1] == 0xA7)&&(Rcv_msg.mtext[2] == 0x00))  //开始固件更新
            {
                p_Widget_Disp_Net->hide();
                p_Widget_Meaasge->hide();
                updateOtaStatus = Update_Ota_Start;
                Disp_Net_Status_Timer->stop();
                devStatusBak = mDevMsg.devStatus;//备份设备状态
                mDevMsg.devStatus = UPDATE_OTA_STATUS;//设备状态进入固件更新状态
                updateProcess = 0;//进度条进度==0    
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA7)&&(Rcv_msg.mtext[2] == 0x01))//固件更新成功
            {
                updateOtaStatus = Update_Ota_Ok;
                qDebug() <<"qt 固件更新成功";
               // qtReboot();
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA7)&&(Rcv_msg.mtext[2] == 0x02))//固件更新失败
            {
                updateOtaStatus = Update_Ota_Fail;
                qDebug() << "固件更新失败";
                Disp_Net_Status_Timer->start(1000); 
                update();
            }
            else if((Rcv_msg.mtext[1] == 0xA7)&&(Rcv_msg.mtext[2] == 0x03))//固件更新中
            {
                updateOtaStatus = Update_Ota_Process;
                updateProcess = Rcv_msg.mtext[3];//更新进度
                qDebug() <<"固件更新进度 "<<Rcv_msg.mtext[3];
                update();
            }

            //工厂检测
            if((Rcv_msg.mtext[1] == 0xFF))  //工厂检测
            {
                if( mDevMsg.devStatus != FACTORY_CHECK_STATUS ) {
                    devStatusBak = mDevMsg.devStatus;//备份设备状态检测完成后回到之前状态
                    mDevMsg.devStatus = FACTORY_CHECK_STATUS;
                }
                
                hardwareCheckCmd = Rcv_msg.mtext[2];
                hardwareCheckCustom = Rcv_msg.mtext[3];//工厂检测自定义字节
                qDebug() << "工厂检测指令";
                qDebug() << "hardwareCheckCmd == " <<hardwareCheckCmd;
                dispProcess = "";
                firstFlag = false;
                Button_Cancel->hide();  
                Button_Determine->hide(); 
                Touch_Value_list.clear();
                touch_check_start = false;
                Setup_Touch_Check_Timer->stop();

                rgbTest = 0;
                if(hardwareCheckCmd == WIFI_CHECK) {
                    NetConfigStatus = Config_Net_Status_Idle;
                }    
                if(hardwareCheckCmd == BLUE_CHECK) {
                    blueCheckResult =  hardwareCheckCustom;//蓝牙检测结果
                    tempList.clear();
                } 
                if(hardwareCheckCmd == TOUCH_CHECK) {//touch检测配置touch芯片
                    set_touch_config_param();
                }
                update(); 
                Hardware_Check_Update_Timer->start(1000);
            }
            //update(); 
        }
    }
}

#define MXA_FIFTER_NUM      5
#define MAX_FIFTER_TIM      100  //MS
#define MAX_SPEED_THRESHOLD 20
#define MAX_SLOW_THREAHOLD  3
static int fifter_count = 0;
/*==================================================================================
* 函 数 名： dev_factory_init_status_task
* 参    数：
* 功能描述:  工厂装配完成状态
* 返 回 值：
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_factory_init_status_task(int status)
{
    int Current_FontSize = 28;
    qDebug()<<"设备状态 =" << mDevMsg.devStatus ;
    int w = this->width();      //宽
    int h = this->height();     //高
    QPainter p(this);
    QPen pen;

    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    pen.setWidth(1);
    pen.setColor(QColor (57, 57, 57, 255));
    p.setPen(pen);

    QBrush brush(QColor (57, 57, 57, 255));
    p.setBrush(brush);
    p.drawRect(0, 0, w-90-1, h);

    pen.setWidth(1);
    pen.setColor(QColor (0, 0, 0, 255));
    p.setPen(pen);
    QBrush brush1(QColor (0, 0, 0, 255));
    p.setBrush(brush1);
    p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);

    switch (status)
    {
        case Config_Net_Status_Idle:
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 440, "确定");
            p.drawText(716, 68,  "返回");

            Current_FontSize = 40;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(40, 250, "按确定按键进行配网模式");

            Button_Cancel->show();   
            Button_Determine->show();
        break;

        case Config_Net_Status_Process:
            qDebug()<<"Config_Net_Status_Process";
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68, "返回");

            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(200, 250, "配网过程中");

            Button_Cancel->show();   
            Button_Determine->hide();
        break;

        case Config_Net_Status_Ok:
            Button_Cancel_list.append("color:white");                         // 前景色
            Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
            Button_Cancel_list.append("border-style:outset");                 // 边框风格
            Button_Cancel_list.append("border-width:0px");                    // 边框宽度
            Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
            Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            if(mDevMsg.devStatus == SITE_CONFIG_NET_STATUS) {
                p.drawText(250,  250, "联网成功");  
            } else {
                p.drawText(250,  250, "联网成功");
            }

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68,  "返回");
            qDebug()<< "联网成功";
            Button_Cancel->show();    
        break;

        case Config_Net_Status_Fail:
            Button_Cancel_list.append("color:white");                         // 前景色
            Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
            Button_Cancel_list.append("border-style:outset");                 // 边框风格
            Button_Cancel_list.append("border-width:0px");                    // 边框宽度
            Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
            Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(250,  250, "联网失败");

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68,  "返回");
            qDebug()<< "联网失败";
            Button_Cancel->show();  
        break;
    }
}

/*==================================================================================
* 函 数 名： dev_configh_net_task
* 参    数：
* 功能描述:  设备初始化状态运行任务
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_configh_net_task()
{
    QPainter p(this);
    QPixmap pix;
    QPen pen;

    //显示uuid二维码图片
    pix.load("/home/meican/mct_uuid.png");
    //设置背景颜色
    QBrush brush(QColor (57, 57, 57));
    p.setBrush(brush);
    p.drawRect(rect());
    p.drawPixmap(200, 50, 400, 330, pix);

    //画下面绿色的长条
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    QBrush brush2(QColor (167, 203, 74, 255));
    p.setBrush(brush2);
    QRectF rectangle(50, 450, 700, 4);
    // prectangle.setWidth(10);
    rectangle.setHeight(10);
    p.drawRoundedRect(rectangle, 5, 5); 

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);
    int Current_FontSize = 28;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(716, 68,  "返回");
    Button_Determine->hide();
    Button_Cancel->show(); 
}

/*==================================================================================
* 函 数 名： dev_factory_bind_task
* 参    数：
* 功能描述:  工厂绑定完成显示sn条形码，6位code码
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_factory_bind_task(QString snnum , int v2id)
{
    QPainter p(this);
    QPen pen;
    QPixmap pix;

    Slider_p->hide();
    p_Widget_Disp_Net->set_disp_content("网络中断",true);
    //设置背景颜色
    QBrush brush(QColor (57, 57, 57));
    p.setBrush(brush);
    p.drawRect(rect());

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);
    int Current_FontSize = 28;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    //p.drawText(716, 68,  "返回");
    Button_Determine->hide();
    Button_Cancel->hide(); 

    Current_FontSize = 20;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    //p.drawText(240, 45,  "按返回键进入配网绑定模式");

    //创建中间白框
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    QBrush brush1(QColor (255, 255, 255));
    p.setBrush(brush1);
    QRectF rectangle(220, 70, 375, 190);
    rectangle.setHeight(320);
    p.drawRoundedRect(rectangle, 10, 10); 

    //显示条形码
    pix.load("/home/meican/mct_sn.png");
    p.drawPixmap(222,95,372,60,pix);
    //写入sn码
    QFont Font ("SimHei", 12, 40);//第一个属性是字体(微软雅黑),第二个是字体大小,第三个是加粗(权重是10)
    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setFont(Font);
    QString snstr = "Serial Number: ";
    p.drawText(268, 182, snstr);
    QFont Font1 ("Microsoft YaHei", 12, 75);//第一个属性是字体(微软雅黑),第二个是字体大小,第三个是加粗(权重是75)
    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setFont(Font1);
    p.drawText(400, 182, snnum);

    //创建中间白框里面的黑框
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    QBrush brush2(QColor(0, 0, 0));
    p.setBrush(brush2);
    QRectF rectangle1(235, 200, 345, 200);
    rectangle1.setHeight(170);
    p.drawRoundedRect(rectangle1, 10, 10); 

    //画下面绿色的长条
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    QBrush brush3(QColor(167, 203, 74));
    p.setBrush(brush3);
    QRectF rectangle3(50, 450, 700, 4);
    rectangle3.setHeight(10);
    p.drawRoundedRect(rectangle3, 5, 5); 

    //黑框中间显示6位码
    QFont Font2("PMingLiU", 45, 0);//第一个属性是字体(微软雅黑),第二个是字体大小,第三个是加粗(权重是75)
    p.setPen(QPen(QColor(167, 203, 74), 1));//字体颜色 = 绿色
    p.setFont(Font2);
    QString v2idStr = QString::number(v2id);//int转QString
    v2idStr.insert(3, ".");//字符串中间插入 "."
    p.setTransform(QTransform(1, 0, 0, 2, 0, 0));//(),(),(),(高度拉伸),(),()
    // itemFont.setItalic(5);
    // p.setFont(itemFont);
    p.drawText(275, 165, v2idStr);
}

/*==================================================================================
* 函 数 名： dev_work_status_task
* 参    数：
* 功能描述:  设备正常工作状态
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_work_status_task()
{
    int w = this->width();      //宽
    int h = this->height();     //高
    int Current_FontSize = 0;  //中心字符大小
    int curPos = 0;
    volatile int Write_Post = 0;

    qDebug() << "设备状态 = " << mDevMsg.devStatus;
    Setup_Slider_p->hide();
    p_Widget_Meaasge->Son_Button_Cancel->hide();
    p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
    if(mDevMsg.devStatus != SETUP_STATUS)
    {
        if(updateMenuStatus == -1 ) 
        {
            mSqliteClass.Sqlite_read_grade_from_menudb(pMenudbName);
            menuList = mSqliteClass.Sqlite_read_context_from_menudb(pMenudbName);
            if (!menuList.length())//无菜单
            {
                QPainter p(this);
                QPen pen;
                p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
                pen.setWidth(1);
                pen.setColor(QColor (57, 57, 57, 255));
                p.setPen(pen);

                QBrush brush(QColor (57, 57, 57, 255));
                p.setBrush(brush);
                p.drawRect(0, 0, w-90-1, h);

                pen.setWidth(1);
                pen.setColor(QColor (0, 0, 0, 255));
                p.setPen(pen);
                QBrush brush1(QColor (0, 0, 0, 255));
                p.setBrush(brush1);
                p.drawRect(w-90, 0, 90-1, h);    //右半边矩形
                
                p.setBrush(brush);               //假滑条的棍
                QRectF rectangle1(750, 20, 8, 440);
                p.drawRoundedRect(rectangle1, 8, 8);
                p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
                QBrush brush3(QColor (167, 203, 74, 255));
                p.setBrush(brush3);
                QRectF rectangle2(746, 100, 16, 80);
                p.drawRoundedRect(rectangle2, 8, 8);

                pen.setColor(QColor(255, 255, 255));
                p.setPen(pen);
                Current_FontSize = 50;
                itemFont.setPointSize(Current_FontSize);
                p.setFont(itemFont);
                p.drawText(250, 250, "无菜单");
                
                pMenudbName = "menu_lev1";
                Widget_Page_Switch = 0;
            } else {
                updateMenuStatus = Update_Menu_Work_Status ;
            }
        }
        else if(updateMenuStatus == Update_Menu_Start || updateMenuStatus == Update_Menu_Process)
        {
            QPainter p(this);
            QPen pen;
            Button_Determine->hide();//确定按键显示
            Button_Cancel->hide();
            Slider_p->hide();//隐藏滑条
    
            //设置背景颜色
            QBrush brush(QColor (57, 57, 57));
            p.setPen(pen);
            p.setBrush(brush);
            p.drawRect(0, 0, w, h);

            //下面的进度条
            QBrush brush4(QColor(79, 79, 79));
            p.setBrush(brush4);
            QRectF rectangle3(50, 450, 700, 4);
            rectangle3.setHeight(10);
            p.drawRoundedRect(rectangle3, 5, 5); 

            QBrush brush5(QColor(167, 203, 74));
            p.setBrush(brush5);
            QRectF rectangle4(50, 450, (updateProcess*700)/100, 4);
            rectangle4.setHeight(10);
            p.drawRoundedRect(rectangle4, 5, 5); 

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(200, 200, "正在更新菜单");
            p.drawText(260, 260,   "请勿断电");
        }
        else if(updateMenuStatus == Update_Menu_Ok)
        {
            QPainter p(this);
            QPen pen;
            //设置背景颜色
            QBrush brush(QColor (57, 57, 57));
            p.setBrush(brush);
            p.drawRect(0, 0, w, h);

            pen.setColor(QColor(167, 203, 74));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(220, 250, "菜单更新成功");

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 440, "确定");
            Button_Determine->show();//确定按键显示
        }
        else if (updateMenuStatus == Update_Menu_Fail)
        {
            QPainter p(this);
            QPen pen;
            //设置背景颜色
            QBrush brush(QColor (250, 06, 30));
            p.setBrush(brush);
            p.drawRect(0, 0, w, h);

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(220, 250, "菜单更新失败");

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 440, "确定");
            Button_Determine->show();//确定按键显示
        }   
        if(updateMenuStatus !=Update_Menu_Work_Status )
            return;
        //进入正常选菜单工作状态
        //从菜单数据库读取菜单等级
        mMenu.grade = mSqliteClass.Sqlite_read_grade_from_menudb(pMenudbName);
        if(mMenu.grade<=0)
        {
            QPainter p(this);
            QPen pen;
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
            pen.setWidth(1);
            pen.setColor(QColor (57, 57, 57, 255));
            p.setPen(pen);

            QBrush brush(QColor (57, 57, 57, 255));
            p.setBrush(brush);
            p.drawRect(0, 0, w-90-1, h);

            pen.setWidth(1);
            pen.setColor(QColor (0, 0, 0, 255));
            p.setPen(pen);
            QBrush brush1(QColor (0, 0, 0, 255));
            p.setBrush(brush1);
            p.drawRect(w-90, 0, 90-1, h);    //右半边矩形
            
            p.setBrush(brush);               //假滑条的棍
            QRectF rectangle1(750, 20, 8, 440);
            p.drawRoundedRect(rectangle1, 8, 8);
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
            QBrush brush3(QColor (167, 203, 74, 255));
            p.setBrush(brush3);
            QRectF rectangle2(746, 100, 16, 80);
            p.drawRoundedRect(rectangle2, 8, 8);

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 50;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(250, 250, "空菜单");
            Button_Determine->hide();
            Button_Cancel->hide();
            return;
        }
            
    }
    if(aplaySelectItemIndex != selectItemIndex) {
        aplaySelectItemIndex = selectItemIndex;
        
        play_wav();
    }
        
    qDebug()<<"确定selectItemIndex = "<<selectItemIndex;
    qDebug()<<"Current_Page = "<<Current_Page;
    if(updateMenuStatus == Update_Menu_Start) return;
    if(Current_Page == 0)
    {
        QPainter p(this);
        if(Widget_Page_Switch == 0)
        {
            Widget_Page_Switch = 1;
            menuList.clear();
            Menu_Spell_List.clear();
            Menu_Spell_Display_List.clear();
            //主菜单数据库
            menuList = mSqliteClass.Sqlite_read_context_from_menudb(pMenudbName);
            
            //menuList<<"周一早餐"<<"周一午餐"<<"周二早餐"<<"周二午餐"<<"周三早餐"<<"周三午餐" <<"周四早餐" <<"周四午餐"<< "周五早餐"<<"周五晚餐";
            totalItemNum = menuList.length();
            if(!selectItemIndex) {//初始界面如果是回退界面回来的用上次选中的菜单
                if(selectItemIndex_Page0 == -1) {
                    selectItemIndex = totalItemNum/2;
                    selectItemIndexBak = selectItemIndex;
                } else {
                    selectItemIndex = selectItemIndex_Page0;
                    selectItemIndexBak = selectItemIndex;
                }
            }
            if (totalItemNum <= selectItemIndex) {
                selectItemIndex = 0;
                selectItemIndexBak = selectItemIndex;
            }
            showItemNum  = 7;   //界面显示菜单个数,最好为奇数
            baseFontSize = 70;  //中心字符大小
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;

            Scroll_Times = 0;   //滑动次数
            Scroll_Dir   = 0;   //慢滑方向
            Release_Slow_Dir_Update_Times  = 0; //松手滑动次数
            Release_Quick_Dir_Update_Times = 0; //松手滑动次数
            Release_Slow_Update_Times = 0;

            if(mDevMsg.devStatus != SETUP_STATUS)
                Button_Cancel->hide();   //取消按键隐藏
            else
                Button_Cancel->show();
            Button_Determine->show();//确定按键显示

            Slider_p->setMinimum(0);               // 最小值
            Slider_p->setMaximum(totalItemNum-1);  // 最大值
            Slider_p->show();          //滑条
        }
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        if(((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1)) {
            Slider_p->setValue(totalItemNum-1-selectItemIndex);
        }
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 440, "确定");
        pselectMenuName = menuList.at(selectItemIndex);//选中的菜单名字
    }
    else if(Current_Page == 1) {
        if(Widget_Page_Switch == 0) {
            Widget_Page_Switch = 1;
            menuList.clear();
            Menu_Spell_List.clear();
            Menu_Spell_Display_List.clear();

            if(mDevMsg.devStatus != SETUP_STATUS) {
                menuList = mSqliteClass.Sqlite_read_context_from_menudb(pMenudbName);
                qDebug()<<"选中菜单名字 = "<<pMenudbName;
                //从数据库选取上级菜单的名字显示在上面
                pPreSelectMenuName = mSqliteClass.Sqlite_read_nextmenu_context_from_menudb(pMenudbName.left(pMenudbName.length() -2 ) ,pMenudbName);
                qDebug()<<"顶上菜单名字 = "<<pPreSelectMenuName;
                Menu_Spell_List = mSqliteClass.Sqlite_read_spell_from_menudb(pMenudbName);
                Menu_Spell_Display_List = Menu_Spell_List;
                removeListSame(&Menu_Spell_Display_List);
            } else {//设置界面
                menuList<<"出厂设置"<<"更改网络"<<"菜单版本"<<"网络属性"<<"设备信息";
                pPreSelectMenuName = "系统界面";
                totalItemNum = menuList.length();
            }
            qDebug()<< "menuList = "<<menuList;
            // menuList<<"白菜花"<<"白菜粉条"<<"白菜蛋花"<<"白菜肉片"<<"白菜黄瓜"<<"白菜萝卜"<<"菜花粉条"<<"菜花黄瓜"<<"菜花肉片"
            //         <<"菜花"
            //         <<"蛋花粉条"
      
            //qDebug() <<"滤重首字母"<< Menu_Spell_Display_List;
            if(mDevMsg.devStatus != SETUP_STATUS) 
            {
                totalItemNum = menuList.length();
                if(selectItemIndex_Page1  == -1) {
                    selectItemIndex = totalItemNum/2;
                    selectItemIndexBak = selectItemIndex;
                } else {
                    selectItemIndex = selectItemIndex_Page1;
                    selectItemIndexBak = selectItemIndex;
                }
                if (totalItemNum <= selectItemIndex) {
                    selectItemIndex = 0;
                    selectItemIndexBak = selectItemIndex;
                }
            }
            
            Total_Show_Menu_Spell_Num = Menu_Spell_Display_List.length();
            showItemNum = 7;    //界面显示菜单个数,最好为奇数
            baseFontSize = 70;  //中心字符大小
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;

            Scroll_Times = 0;   //滑动次数
            Scroll_Dir = 0;     //慢滑方向
            Release_Slow_Dir_Update_Times = 0; // 松手滑动次数
            Release_Quick_Dir_Update_Times = 0; //松手滑动次数
            Release_Slow_Update_Times = 0;

            Slider_p->hide();
            Button_Determine->show();
            Button_Cancel->show();
        }
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(57, 57, 57));
        p.setPen(pen);

        QBrush brush(QColor(57, 57, 57));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor(0, 0, 0));
        p.setPen(pen);
        QBrush brush1(QColor(0, 0, 0));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(5, 32, 690, 4);
        p.drawRoundedRect(rectangle, 2, 2);        //上面长条
  
        if(mDevMsg.devStatus != SETUP_STATUS) {//
            p.setBrush(brush);                         //假滑条的棍
            QRectF rectangle1(750, 100, 16, 280);
            p.drawRoundedRect(rectangle1, 8, 8);
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
            QBrush brush3(QColor (167, 203, 74, 255));
            p.setBrush(brush3);
            p.drawEllipse(742, 225, 30, 30);           //假滑条的圆
        } else { //设置界面处理滑条进度条   
            if((selectItemIndex+1) == totalItemNum) Setup_Slider_p->setValue((totalItemNum-selectItemIndex)*3.5);
            else Setup_Slider_p->setValue((totalItemNum-selectItemIndex)*4.9);
            Setup_Slider_p->show();
        }
            
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 440, "确定");
        p.drawText(716, 68,  "返回");
        // p.drawText(320, 30, "周二午餐");//mMenu
        //上面显示上级菜单名字
        //p.drawText(320-pPreSelectMenuName.length()*10, 30, pPreSelectMenuName);
        p.drawText(0,0,this->width()-60,30,Qt::AlignCenter,pPreSelectMenuName);
        pselectMenuName = menuList.at(selectItemIndex);//选中的菜单名字
        qDebug()<<"pselectMenuName line 1361 = "<<pselectMenuName;
    }
    if(Current_Page == 2 && mDevMsg.devStatus != SETUP_STATUS)
    {   
        if(Update_Message_Tip == 1)
        {
            Button_Determine->hide();
            //Button_Cancel->show();
            QPainter p(this);
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

            QBrush brush(QColor(57, 57, 57));
            p.setBrush(brush);
            p.drawRect(0, 0, w, h);

            QBrush brush1(QColor(167, 203, 74));
            p.setBrush(brush1);
            //QRectF rectangle(10, 35, 45 + pPreSelectMenuName.length()*20, 6);
            QRectF rectangle(10, 35, 15+pPreSelectMenuName.length()*27, 6);
            p.drawRoundedRect(rectangle, 4, 3); //上面长条

            QPen pen;
            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68, "返回");

            Current_FontSize = 22;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            //p.drawText(10,  30,  "周二午餐");
            p.drawText(10, 30, pPreSelectMenuName);//上面框显示上级菜单名字

            //显示选中的要写的菜品的名字
            dishName = String_Display;
            qDebug()<<"选中的字符串长度 = " << String_Display.length()/8;
            qDebug()<<"屏幕宽度 = " << this->width()<< this->height();
            //String_Display = "中华人民共和国北京市朝阳区将台路中华人民共和国北京市朝阳区将台路";
            switch(String_Display.length()/9)
            {
                case 0:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    p.drawText(0,0,this->width(),this->height(),Qt::AlignCenter,String_Display);
                break;
                case 1:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,140,this->width(),100,Qt::AlignCenter,String_Display.left(8));
                    p.drawText(0,240,this->width(),100,Qt::AlignCenter,String_Display.mid(8));
                break;
                case 2:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,40,this->width(),200,Qt::AlignCenter,String_Display.mid(0,8));
                    p.drawText(0,140,this->width(),200,Qt::AlignCenter,String_Display.mid(8,8));
                    p.drawText(0,240,this->width(),200,Qt::AlignCenter,String_Display.mid(16,8));
                break;
                case 3:
                    Current_FontSize = 60;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,40,this->width(),100,Qt::AlignCenter,String_Display.mid(0,8));
                    p.drawText(0,140,this->width(),100,Qt::AlignCenter,String_Display.mid(8,8));
                    p.drawText(0,240,this->width(),100,Qt::AlignCenter,String_Display.mid(16,8));
                    p.drawText(0,340,this->width(),100,Qt::AlignCenter,String_Display.mid(24,8));
                break;
            }
        }
        if(Widget_Page_Switch == 0)
        {
            Widget_Page_Switch = 1;
            String_Display.clear();
            String_Display  = menuList.at(selectItemIndex);
            pselectMenuName = menuList.at(selectItemIndex);//选中的菜单名字

            menuList.clear();
            Menu_Spell_List.clear();
            Menu_Spell_Display_List.clear();

            Slider_p->hide();
            Button_Determine->show();
            Button_Cancel->show();

            totalItemNum = 0;
            selectItemIndex = 0;

            QPainter p(this);
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

            QBrush brush(QColor(57, 57, 57));
            p.setBrush(brush);
            p.drawRect(0, 0, w, h);

            QBrush brush1(QColor(167, 203, 74));
            p.setBrush(brush1);
            QRectF rectangle(10, 35, 15+pPreSelectMenuName.length()*27, 6);
            p.drawRoundedRect(rectangle, 4, 3); //上面长条

            QPen pen;
            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68, "返回");

            Current_FontSize = 22;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            //p.drawText(10,  30,  "周二午餐");
            p.drawText(10, 30, pPreSelectMenuName);//上面框显示上级菜单名字

            //String_Display = "中华人民共和国北京市朝阳区将台路中华人民共和国北京市朝阳区将台路";
            //显示选中的要写的菜品的名字 
            switch(String_Display.length()/9)
            {
                case 0:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    p.drawText(0,0,this->width(),this->height(),Qt::AlignCenter,String_Display);
                break;
                case 1:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,140,this->width(),100,Qt::AlignCenter,String_Display.left(8));
                    p.drawText(0,240,this->width(),100,Qt::AlignCenter,String_Display.mid(8));
                break;
                case 2:
                    Current_FontSize = 80;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,40,this->width(),200,Qt::AlignCenter,String_Display.mid(0,8));
                    p.drawText(0,140,this->width(),200,Qt::AlignCenter,String_Display.mid(8,8));
                    p.drawText(0,240,this->width(),200,Qt::AlignCenter,String_Display.mid(16,8));
                break;
                case 3:
                    Current_FontSize = 60;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    //根据显示字符长度修改显示位置
                    p.drawText(0,40,this->width(),100,Qt::AlignCenter,String_Display.mid(0,8));
                    p.drawText(0,140,this->width(),100,Qt::AlignCenter,String_Display.mid(8,8));
                    p.drawText(0,240,this->width(),100,Qt::AlignCenter,String_Display.mid(16,8));
                    p.drawText(0,340,this->width(),100,Qt::AlignCenter,String_Display.mid(24,8));
                break;
            }
        }
    }
    //0  1 界面刷新
    if((Current_Page == 0)||(Current_Page == 1))
    {
        if(Current_Page == 0)
        {
            if(((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
                Slider_p->setValue(totalItemNum-1-selectItemIndex);
        }
        if(Current_Page == 1)
        {
            QPainter p(this);
            QPen pen;
            pen.setColor(QColor(212, 212, 212));
            p.setPen(pen);
            itemFont.setPointSize(20);
            p.setFont(itemFont);

            if(mDevMsg.devStatus != SETUP_STATUS) {
                Select_Show_Menu_Spell_Index = Menu_Spell_Display_List.indexOf(Menu_Spell_List.at(selectItemIndex));
                for(int ii=0; ii<showItemNum; ii++)
                {          
                    curPos = Select_Show_Menu_Spell_Index + (ii-(showItemNum/2));
                    if((curPos < 0)||(curPos >= (Total_Show_Menu_Spell_Num)))    //边沿检测
                        continue; //continue 语句只结束本次循环,而不是终止整个循环
                    //p.drawText(750, Address_Spell[ii], Menu_Spell_Display_List[curPos]);
                    p.drawText(742, Address_Spell[ii]-38, 30, 50, Qt::AlignCenter, Menu_Spell_Display_List[curPos]);
                }
            }
        }
        if((Add_Step_By_Step==FRAME))
        {
            showItemNum = 7;
            QPainter p(this);
            QPen pen;
            qDebug()<<"Add_Step_By_Step == FRAME";

            //绘制内容  界面显示菜单个数,最好为奇数  这里是7
            for(int i=0; i<showItemNum; i++)
            {
                int  offsetValue = 0;
                //qDebug()<<"draw last" <<showItemNum<<baseFontSize;
                curPos = selectItemIndex + (i-(showItemNum/2));
                if((curPos < 0)||(curPos >= (totalItemNum)))    //边沿检测
                {
                    continue; //continue 语句只结束本次循环,而不是终止整个循环
                }
                if(i == (showItemNum/2))
                {
                    pen.setColor(QColor(255,255,255));
                    p.setPen(pen);
                }
                else  if( (i == ((showItemNum/2) +1))|| (i == ((showItemNum/2) -1)) )
                {
                    pen.setColor(QColor(135,135,135));
                    p.setPen(pen);
                }
                else  if( (i == ((showItemNum/2) +2))|| (i == ((showItemNum/2) -2)) )
                {
                    pen.setColor(QColor(119,119,119));
                    p.setPen(pen);
                }
                else  if( (i == ((showItemNum/2) +3))|| (i == ((showItemNum/2) -3)) )
                {
                    pen.setColor(QColor(79,79,79));
                    p.setPen(pen);
                }

                //计算字体大小
                Current_FontSize = baseFontSize - abs((i-(showItemNum/2)))*15;
                //qDebug()<<"字体大小 = "<<Current_FontSize;
                itemFont.setPixelSize(Current_FontSize);
                p.setFont(itemFont);
                if(Current_Page == 0)
                {
                    offsetValue = 0;
                }
                if(Current_Page == 1)
                {
                    offsetValue = 20;
                }

                if(i == 0)
                {
                    const QRect rectangle0 = QRect(24, 34+offsetValue, 680, 30);
                    #ifdef DRAW_DEBUG
                    p.drawRect(24, 34+offsetValue, 680, 30);
                    #endif
                    
                    p.drawText(rectangle0, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 1)
                {
                    const QRect rectangle1 = QRect(24, 68+offsetValue, 680, 50);
                    #ifdef DRAW_DEBUG
                     p.drawRect(24, 68+offsetValue, 680, 50);
                    #endif
                   
                    p.drawText(rectangle1, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 2)
                {
                    const QRect rectangle2 = QRect(24, 122+offsetValue, 680, 65);
                    #ifdef DRAW_DEBUG
                     p.drawRect(24, 122+offsetValue, 680, 65);
                    #endif
                    
                    p.drawText(rectangle2, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 3)
                {
                    const QRect rectangle3 = QRect(24, 196+offsetValue, 680, 75);
                    #ifdef DRAW_DEBUG
                     p.drawRect(24, 196+offsetValue, 680, 75);
                    #endif
                    
                    p.drawText(rectangle3, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 4)
                {
                    const QRect rectangle4 = QRect(24, 280+offsetValue, 680, 65);
                    #ifdef DRAW_DEBUG
                     p.drawRect(24, 280+offsetValue, 680, 65);
                    #endif
                    
                    p.drawText(rectangle4, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 5)
                {
                    const QRect rectangle5 = QRect(24, 354+offsetValue, 680, 50);
                    #ifdef DRAW_DEBUG
                     p.drawRect(24, 354+offsetValue, 680, 50);
                    #endif
                    
                    p.drawText(rectangle5, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
                if(i == 6)
                {
                    const QRect rectangle6 = QRect(24, 410+offsetValue, 680, 30);
                    #ifdef DRAW_DEBUG
                    p.drawRect(24, 410+offsetValue, 680, 30);
                    #endif
                    
                    p.drawText(rectangle6, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                }
            }
            return;
        } else {
            if((Add_Step_By_Step>FRAME)&&(Add_Step_By_Step<=(FRAME*2)))//  8   9  10   11  12  13  14
            {
                qDebug()<<"上滑Add_Step_By_Step = "<<Add_Step_By_Step;
                QPainter p(this);
                QPen pen;
                int j = Add_Step_By_Step-FRAME;
                if(Add_Step_By_Step > ((FRAME*2) - ((FRAME-1)/2)))//  12 13 14
                {
                    // if((selectItemIndex + 4) <= (totalItemNum-1))    //边沿检测  //
                    // {
                    //     qDebug()<<"上滑边缘检测";
                    //     //计算字体大小  
                    //     Current_FontSize = 14 + ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*2);  
                    //     itemFont.setPointSize(Current_FontSize);
                    //     p.setFont(itemFont);
                    //     pen.setColor(QColor(77+5*j, 77+5*j, 77+5*j));
                    //     p.setPen(pen);
                    //     if(Current_Page == 0) {
                    //         Write_Post = 476 - ((Add_Step_By_Step - ((FRAME*2) - ((FRAME-1)/2)))*8);
                    //     }
                    //     if(Current_Page == 1) {
                    //         Write_Post = 476 - ((Add_Step_By_Step - ((FRAME*2) - ((FRAME-1)/2)))*8)+20;
                    //     }
                    //     if((Write_Post > 0)&&(Write_Post < 480)) {
                    //         p.drawText(10, Write_Post, menuList[selectItemIndex + 4]);
                    //     }       
                    // }
                }
                for(int i=0; i<showItemNum; i++)
                {
                    curPos = selectItemIndex + (i-(showItemNum/2));
                    if((curPos < 0)||(curPos >= (totalItemNum)))    //边沿检测
                        continue; //
                    int addOffert = 20;
                    if(Current_Page == 0)
                    {
                        addOffert = 0;
                    }
                    if(Current_Page == 1)
                    {
                        addOffert = 20;
                    }
                    if(i == 0) {   
                        if(Add_Step_By_Step < ((FRAME*2)- ((FRAME-1)/2)))  //   8   9   10
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[0] - FONT_STEP_VALUE*j;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(79-5*j, 79-5*j, 79-5*j));
                            p.setPen(pen);
                            if(Current_Page == 0) {
                                Write_Post = Address_Font[7][i];
                            }
                            if(Current_Page == 1) {
                                Write_Post = Address_Font[7][i]+addOffert;
                            }
                            if((Write_Post > 0)&&(Write_Post < 480)) {
                                //p.drawText(10, Write_Post, menuList[selectItemIndex - 3]);
                                const QRect rectangle0 = QRect(24, Write_Post- (10-abs(15/7))*j, 680, 30-FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                p.drawRect(24, 34+addOffert, 680, 30);
                                #endif
                                
                                p.drawText(rectangle0, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                    }
                    else if(i == 1) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[1] - FONT_STEP_VALUE*j;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(119-5*j, 119-5*j, 119-5*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                            Write_Post = Address_Font[7][i];
                        if(Current_Page == 1)
                            Write_Post = Address_Font[7][i]+addOffert;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            const QRect rectangle1 = QRect(24, Write_Post-abs(32/7)*j, 680, 50-FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                p.drawRect(24, 68+addOffert, 680, 50);
                            #endif
                            
                            p.drawText(rectangle1, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        }
                        //p.drawText(10, Write_Post, menuList[curPos]);
                    }
                    else if(i == 2) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[2] - FONT_STEP_VALUE*j;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(136-3*j, 136-3*j, 136-3*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                            Write_Post = Address_Font[7][i];
                        if(Current_Page == 1)
                            Write_Post = Address_Font[7][i]+addOffert;
                        if((Write_Post > 0)&&(Write_Post < 480))
                        {
                            const QRect rectangle2 = QRect(24, Write_Post-abs(54/7)*j, 680, 65-FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                p.drawRect(24, 122+addOffert, 680, 65);
                            #endif
                            
                            p.drawText(rectangle2, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        } 
                        //p.drawText(10, Write_Post, menuList[curPos]);       
                    }
                    else if(i == 3) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[3] - FONT_STEP_VALUE*j;
                        qDebug()<<"下滑字体大小 i=3 "<< Current_FontSize;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);//朝下滑  6 
                        Last_Update_GUI = 0;
                        pen.setColor(QColor(252-15*j, 252-15*j, 252-15*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                            Write_Post = Address_Font[7][i];
                        if(Current_Page == 1)
                            Write_Post = Address_Font[7][i]+addOffert;
                        if((Write_Post >0)&&(Write_Post <480)) 
                        {
                            const QRect rectangle3 = QRect(24, Write_Post-abs(74/7)*j, 680, 75-FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                 p.drawRect(24, 196+addOffert, 680, 75);
                            #endif
                           
                            p.drawText(rectangle3, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        }
                        //p.drawText(10, Write_Post, menuList[curPos]);
                    }
                    else if(i == 4) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[4] + FONT_STEP_VALUE*j ;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(136+15*j, 136+15*j, 136+15*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                            Write_Post = Address_Font[7][i];
                        if(Current_Page == 1)
                            Write_Post = Address_Font[7][i]+addOffert;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            const QRect rectangle4 = QRect(24, Write_Post-abs(84/7)*j, 680, 65+FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                p.drawRect(24, 280+addOffert, 680, 65);
                            #endif
                            
                            p.drawText(rectangle4, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        }
                        //p.drawText(10, Write_Post, menuList[curPos]);
                    }
                    else if(i == 5) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[5] + FONT_STEP_VALUE*j;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(116+3*j, 116+3*j, 116+3*j));
                        p.setPen(pen);

                        if(Current_Page == 0) {
                            Write_Post = Address_Font[7][i];
                        } 
                        if(Current_Page == 1) {
                            Write_Post = Address_Font[7][i]+addOffert;
                        }
                        if((Write_Post >0)&&(Write_Post <480)) {
                            //p.drawText(10, Write_Post, menuList[curPos]);
                            const QRect rectangle5 = QRect(24, Write_Post-abs(74/7)*j, 680, 50+FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                p.drawRect(24, 354+addOffert, 680, 50);
                            #endif
                           
                            p.drawText(rectangle5, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        }
                    }
                    else if(i == 6) {
                        //计算字体大小
                        Current_FontSize = Virtual_Font[6] + FONT_STEP_VALUE*j;
                        itemFont.setPixelSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(76+5*j, 76+5*j, 76+5*j));
                        p.setPen(pen);                        
                        if(Current_Page == 0) {
                            Write_Post = Address_Font[7][i];
                        }   
                        if(Current_Page == 1) {
                            Write_Post = Address_Font[7][i]+addOffert;
                        }
                        if((Write_Post > 0)&&(Write_Post < 480)) {
                            //p.drawText(10, Write_Post, menuList[curPos]);
                            const QRect rectangle6 = QRect(24, Write_Post-abs(54/7)*j, 680, 30+FONT_STEP_VALUE*j);
                            #ifdef DRAW_DEBUG
                                p.drawRect(24, 410+addOffert, 680, 30);
                            #endif
                            
                            p.drawText(rectangle6, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        }
                    }
                }
            }              
            else if((Add_Step_By_Step<FRAME) && (Add_Step_By_Step>=0))// int j = Add_Step_By_Step 6  5   4   3   2   1  0
            {
                QPainter p(this);
                QPen pen;
                int j = FRAME - Add_Step_By_Step;
                {
                    if(Add_Step_By_Step < ((FRAME-1)/2))  // 显示6 行之后继续下拉上头新增了一行  0    1    2
                    {
                        // if((selectItemIndex - 4) >= 0)    //边沿检测
                        // {
                        //     Current_FontSize = 14+(((FRAME-1)/2)- Add_Step_By_Step) *2;  //baseFontSize - abs((i-(showItemNum/2)))*16;
                        //     itemFont.setPointSize(Current_FontSize);
                        //     p.setFont(itemFont);
                        //     pen.setColor(QColor(77+5*j,77+5*j,77+5*j));
                        //     p.setPen(pen);

                        //     if(Current_Page == 0) {
                        //         Write_Post = Address_Font[7][i];
                        //     }
                        //     if(Current_Page == 1) {
                        //         Write_Post = Address_Font[7][i]+20;
                        //     }
                        //     if((Write_Post >0)&&(Write_Post <480)) {
                        //        // p.drawText(10, Write_Post, menuList[selectItemIndex - 4]);
                        //         const QRect rectangle0 = QRect(24, Write_Post+(44-abs(20/7))*j, 680, 30-FONT_STEP_VALUE*j);
                        //         p.drawRect(24, Write_Post, 680, 30);
                        //         p.drawText(rectangle0, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                        //     }   
                        // }
                    }
                    //绘制内容 界面显示菜单个数,最好为奇数  这里是7
                    for(int i=0; i<showItemNum; i++)
                    {
                        curPos = selectItemIndex + (i-(showItemNum/2));
                        if((curPos < 0)||(curPos >= (totalItemNum))) //边沿检测
                            continue; //
                        // Current_FontSize = baseFontSize - abs((i-(showItemNum/2)))*15;
                        // itemFont.setPointSize(Current_FontSize);
                        int addOffert = 20;
                        if(Current_Page == 0)
                        {
                            addOffert = 0;
                        }
                        if(Current_Page == 1)
                        {
                            addOffert = 20;
                        }
                        if(i == 0)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[0] + FONT_STEP_VALUE*j;  
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(79+5*j,79+5*j,79+5*j));
                            p.setPen(pen);
                            if(Current_Page == 0) {
                                Write_Post = Address_Font[7][i];
                            } 
                            if(Current_Page == 1) {
                                Write_Post = Address_Font[7][i]+addOffert;
                            } 
                            if((Write_Post >0)&&(Write_Post <480)) {
                                //p.drawText(10, Write_Post, menuList[curPos]); 
                                const QRect rectangle0 = QRect(24, Write_Post+abs(32/7)*j, 680, 30+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                 p.drawRect(24, 44, 680, 30);
                                #endif
                               
                                p.drawText(rectangle0, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }      
                        }
                        else if(i == 1)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[1] + FONT_STEP_VALUE*j;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);

                            pen.setColor(QColor(119+2*j,119+2*j,119+2*j));
                            p.setPen(pen);
                            if(Current_Page == 0) {
                                Write_Post = Address_Font[7][i];
                            }
                            if(Current_Page == 1) {
                                Write_Post = Address_Font[7][i]+addOffert;
                            }
                            if((Write_Post >0)&&(Write_Post <480)) {
                               // p.drawText(10, Write_Post, menuList[curPos]);
                                const QRect rectangle1 = QRect(24, Write_Post+abs(54/7)*j, 680, 50+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                 p.drawRect(24, 78, 680, 50);
                                #endif
                                
                                p.drawText(rectangle1, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                        else if(i == 2)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[2] + FONT_STEP_VALUE*j ;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(135+15*j,135+15*j,135+15*j));
                            p.setPen(pen);
                            if(Current_Page == 0) {
                                Write_Post = Address_Font[7][i];
                            } 
                            if(Current_Page == 1) {
                                Write_Post = Address_Font[7][i]+addOffert;
                            }       
                            if((Write_Post >0)&&(Write_Post <480)) {
                                //p.drawText(10, Write_Post, menuList[curPos]);
                                const QRect rectangle2 = QRect(24, Write_Post+abs(74/7)*j, 680, 65+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                 p.drawRect(24, 132, 680, 65);
                                #endif
                                
                                p.drawText(rectangle2, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                        else if(i == 3)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[3] - FONT_STEP_VALUE*j;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(255-15*j,255-15*j,255-15*j));
                            p.setPen(pen);
                            if(Current_Page == 0)
                                Write_Post = Address_Font[7][i];
                            if(Current_Page == 1)
                                Write_Post = Address_Font[7][i]+addOffert;
                            if((Write_Post >0)&&(Write_Post < 480)){
                               // p.drawText(10, Write_Post, menuList[curPos]);
                                const QRect rectangle3 = QRect(24, Write_Post+abs(84/7)*j, 680, 75+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                 p.drawRect(24, 206, 680, 75);
                                #endif
                               
                                p.drawText(rectangle3, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                        else if(i == 4)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[4] - FONT_STEP_VALUE*j ;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(135-2*j,135-2*j,135-2*j));
                            p.setPen(pen);
                            if(Current_Page == 0)
                                Write_Post = Address_Font[7][i];
                            if(Current_Page == 1)
                                Write_Post = Address_Font[7][i]+addOffert;
                            if((Write_Post >0)&&(Write_Post <480)) {
                                //p.drawText(10, Write_Post, menuList[curPos]);
                                const QRect rectangle4 = QRect(24, Write_Post+abs(74/7)*j, 680, 65+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                 p.drawRect(24, 290, 680, 65);
                                #endif
                                
                                p.drawText(rectangle4, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                        else if(i == 5)
                        {
                            //计算字体大小
                            Current_FontSize = Virtual_Font[5] - FONT_STEP_VALUE*j ;
                            itemFont.setPixelSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(119-5*j,119-5*j,119-5*j));
                            p.setPen(pen);                        
                            if(Current_Page == 0)
                                Write_Post = Address_Font[7][i];
                            if(Current_Page == 1)
                                Write_Post = Address_Font[7][i]+addOffert;
                            if((Write_Post >0)&&(Write_Post <480)) {
                                //p.drawText(10, Write_Post, menuList[curPos]);
                                const QRect rectangle5 = QRect(24, Write_Post+abs(54/7)*j, 680, 50+FONT_STEP_VALUE*j);
                                #ifdef DRAW_DEBUG
                                p.drawRect(24, 364, 680, 50);
                                #endif
                                
                                p.drawText(rectangle5, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                            }
                        }
                        else if(i == 6)
                        {
                            if(Add_Step_By_Step > ((FRAME-1)/2)) {
                                //计算字体大小
                                Current_FontSize = Virtual_Font[6] - FONT_STEP_VALUE*j ;
                                itemFont.setPixelSize(Current_FontSize);
                                p.setFont(itemFont);
                                pen.setColor(QColor(77-5*j,77-5*j,77-5*j));
                                p.setPen(pen);

                                if(Current_Page == 0)
                                    Write_Post = Address_Font[7][i];
                                if(Current_Page == 1)
                                    Write_Post = Address_Font[7][i]+addOffert;
                                if((Write_Post >0)&&(Write_Post <480)) {
                                    //p.drawText(10, Write_Post, menuList[curPos]);
                                    const QRect rectangle6 = QRect(24, Write_Post+abs(32/7)*j, 680, 50+FONT_STEP_VALUE*j);
                                    #ifdef DRAW_DEBUG
                                    p.drawRect(24, 410, 680, 30);
                                    #endif
                                    
                                    p.drawText(rectangle6, Qt::AlignLeft | Qt::AlignVCenter, menuList[curPos]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*==================================================================================
* 函 数 名： dev_update_ota_task
* 参    数：
* 功能描述:  工厂绑定完成显示sn条形码，6位code码
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_update_ota_task(void)
{
    int w = this->width();      //宽
    int h = this->height();     //高
    int Current_FontSize;

    if(updateOtaStatus == Update_Ota_Start || updateOtaStatus == Update_Ota_Process)
    {
        QPainter p(this);
        QPen pen;
        Button_Determine->hide();//确定按键显示
        Button_Cancel->hide();
        Slider_p->hide();//隐藏滑条
  
        //设置背景颜色
        QBrush brush(QColor(57, 57, 57));
        p.setPen(pen);
        p.setBrush(brush);
        p.drawRect(0, 0, w, h);

        //下面的进度条
        QBrush brush4(QColor(79, 79, 79));
        p.setBrush(brush4);
        QRectF rectangle3(50, 450, 700, 4);
        rectangle3.setHeight(10);
        p.drawRoundedRect(rectangle3, 5, 5); 

        QBrush brush5(QColor(167, 203, 74));
        p.setBrush(brush5);
        QRectF rectangle4(50, 450, (updateProcess*700)/100, 4);
        rectangle4.setHeight(10);
        p.drawRoundedRect(rectangle4, 5, 5); 

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 45;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(200, 200, "正在更新程序");
        p.drawText(260, 260,   "请勿断电");
    }
    else if(updateOtaStatus == Update_Ota_Ok)
    {
        QPainter p(this);
        QPen pen;
        //设置背景颜色
        QBrush brush(QColor(57, 57, 57));
        p.setBrush(brush);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(167, 203, 74));
        p.setPen(pen);
        Current_FontSize = 45;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(220, 250, "程序更新成功");

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 440, "确定");
        Button_Determine->show();//确定按键显示
    }
    else if (updateOtaStatus == Update_Ota_Fail)
    {
        QPainter p(this);
        QPen pen;
        //设置背景颜色
        QBrush brush(QColor(250, 06, 30));
        p.setBrush(brush);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 45;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(220, 250, "程序更新失败");

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 440, "确定");
        Button_Determine->show();//确定按键显示
    } 
}

/*==================================================================================
* 函 数 名： dev_site_config_net_task
* 参    数：
* 功能描述:  现场配置网络
* 返 回 值：
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_site_config_net_task(int status)
{
    int Current_FontSize = 28;
    qDebug()<<"设备状态 =" << mDevMsg.devStatus ;
    int w = this->width();      //宽
    int h = this->height();     //高
    QPainter p(this);
    QPen pen;

    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    pen.setWidth(1);
    pen.setColor(QColor (57, 57, 57));
    p.setPen(pen);

    QBrush brush(QColor (57, 57, 57));
    p.setBrush(brush);
    p.drawRect(0, 0, w-90-1, h);

    pen.setWidth(1);
    pen.setColor(QColor (0, 0, 0));
    p.setPen(pen);
    QBrush brush1(QColor (0, 0, 0));
    p.setBrush(brush1);
    p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);

    switch (status)
    {
        case Config_Net_Status_Idle:
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 440, "确定");
            p.drawText(716, 68,  "返回");

            Current_FontSize = 40;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(40, 250, "按确定按键进行配网模式");

            Button_Cancel->show();   
            Button_Determine->show();
        break;

        case Config_Net_Status_Process:
            qDebug()<<"Config_Net_Status_Process";
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68, "返回");

            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(200, 250, "配网过程中");

            Button_Cancel->show();   
            Button_Determine->hide();
        break;

        case Config_Net_Status_Ok:
            Button_Cancel_list.append("color:white");                         // 前景色
            Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
            Button_Cancel_list.append("border-style:outset");                 // 边框风格
            Button_Cancel_list.append("border-width:0px");                    // 边框宽度
            Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
            Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(250,  250, "联网成功");

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68,  "返回");
            qDebug()<< "联网成功";
            Button_Cancel->show();  
            update();
          
        break;

        case Config_Net_Status_Fail:
            Button_Cancel_list.append("color:white");                         // 前景色
            Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
            Button_Cancel_list.append("border-style:outset");                 // 边框风格
            Button_Cancel_list.append("border-width:0px");                    // 边框宽度
            Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
            Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

            pen.setColor(QColor(255, 255, 255));
            p.setPen(pen);
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(250,  250, "联网失败");

            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(716, 68,  "返回");
            qDebug()<< "联网失败";
            Button_Cancel->show();  
        break;
    }
}

/*==================================================================================
* 函 数 名： factory_check_task
* 参    数：
* 功能描述:  工厂硬件检测
* 返 回 值：
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::factory_hardware_check_task(int status)
{
    int postionStart = 0 ,i = 0;
    int postionEnd = 0;
    int Current_FontSize = 28;
    int redNum, greenNum, blueNum;

    int w = this->width();//宽
    int h = this->height();//高
    QPainter p(this);
    QPen pen;
    QByteArray result_;

    p_Widget_Disp_Net->hide();
    Slider_p->hide();//隐藏滑条
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    pen.setWidth(1);
    pen.setColor(QColor(57, 57, 57));
    p.setPen(pen);

    QBrush brush(QColor(57, 57, 57));
    p.setBrush(brush);
    p.drawRect(0, 0, w-90-1, h);

    pen.setWidth(1);
    pen.setColor(QColor(0, 0, 0));
    p.setPen(pen);
    QBrush brush1(QColor(0, 0, 0));
    p.setBrush(brush1);
    p.drawRect(w-90, 0, 90-1, h);//右半边矩形

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);

    Current_FontSize = 28;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(716, 68,  "返回");
    p.drawText(716, 440, "确定");
    // Button_Cancel->hide();  
    // Button_Determine->hide(); 
    
    pen.setColor(QColor(167, 203, 74));
    p.setPen(pen);
    p.drawText(250, 400, "成功按确定键");
    pen.setColor(QColor(250, 06, 30));
    p.setPen(pen);
    p.drawText(250, 100, "失败按返回键"); 

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);
    Current_FontSize = 45;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);

    switch (status)
    { 
        case SCREEN_CHECK://屏幕测试
            qDebug() << "屏幕测试";
            qDebug() << "rgbTest = " << rgbTest;
            switch (rgbTest)
            {
                case 0: 
                case 1: 
                    p.drawText(250, 250, "屏幕检测");
                break;

                case 2://RED COLOR
                case 3: 
                    redNum = 255; greenNum = 0; blueNum = 0;
                break;

                case 4://GREEN COLOR
                case 5: 
                    redNum = 0; greenNum = 255; blueNum = 0;
                break;

                case 6://BLUE COLOR
                case 7: 
                    redNum = 0; greenNum = 0; blueNum = 255;
                break;
            }
            if(rgbTest > 1) 
            {
                QBrush brush(QColor(redNum, greenNum, blueNum));
                p.setBrush(brush);
                p.drawRect(0, 0, w, h);
            } 
            if(rgbTest >= 7) rgbTest = 0;
            else rgbTest ++;
        break;

        case TOUCH_CHECK://触摸测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "触摸测试";
            p.drawText(250, 220, "触摸测试");
            //触摸值滤重
            removeListSame(&Touch_Value_list );
            qDebug() << "Touch_Value_list = " << Touch_Value_list;
            for(i=0; i<Touch_Value_list.count(); i++ )
            {
                if(Touch_Value_list[i] == "255") {//手指拿开测试结束
                    touch_check_start = true;
                    break;
                }
            }
            if(touch_check_start == false) {
                dispProcess += ".";
                p.drawText(170, 300, dispProcess);
                if(dispProcess.length() > 30) {
                    dispProcess = ".";
                }     
            } else {
                qDebug() << "touch list 长度 = " << Touch_Value_list.count();
                if(Touch_Value_list.count() > 90) {//touch 值个数大于90认定测试成功
                    pen.setColor(QColor(167, 203, 74));
                    p.setPen(pen);
                    p.drawText(170, 300, "触摸检测成功");
                } else {
                    pen.setColor(QColor(250, 06, 30));
                    p.setPen(pen);
                    p.drawText(170, 300, "触摸检测失败");
                }    
            }
            Current_FontSize = 28;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(200, 150,  "手指连续从上滑到下");
            Current_FontSize = 45;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
        break;

        case WIFI_CHECK://WIFI 测试  
            Button_Cancel->show();  
            Button_Determine->show();   
            qDebug() << "WIFI测试";
            p.drawText(230, 220, "WIFI测试");
            if(NetConfigStatus == Config_Net_Status_Ok) {
                Current_FontSize = 14;
                itemFont.setPointSize(Current_FontSize);
                pen.setColor(QColor(167, 203, 74));
                p.setPen(pen);
                p.drawText(170, 300, "WIFI连接成功");
            } else {
                dispProcess += ".";
                p.drawText(170, 300, dispProcess);
            }
        break;

        case BLUE_CHECK://蓝牙测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "蓝牙测试";
            p.drawText(230, 220, "蓝牙测试");

            if(blueCheckResult == 0) {
                dispProcess += ".";
                p.drawText(170, 300, dispProcess);
                if(dispProcess.length() > 30) {
                    dispProcess = ".";
                }
            }
            else if(blueCheckResult == 1) {//蓝牙检测成功
                Current_FontSize = 14;
                itemFont.setPointSize(Current_FontSize);
                pen.setColor(QColor(167, 203, 74));
                p.setPen(pen);
                p.drawText(170, 300, "蓝牙检测成功");
            } 
            else if(blueCheckResult == 2) {//蓝牙检测失败
                Current_FontSize = 14;
                itemFont.setPointSize(Current_FontSize);
                pen.setColor(QColor(250, 06, 30));
                p.setPen(pen);
                p.drawText(170, 300, "蓝牙检测失败");
            }
        break;

        case BEEP_CHECK://蜂鸣器测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "蜂鸣器测试";
            p.drawText(250, 250, "蜂鸣器测试");
            dispProcess += ".";
            p.drawText(170, 350, dispProcess);
            if(dispProcess.length() > 30) {
                dispProcess = ".";
            }
        break;

        case RFID_CHECK://读盘测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "读盘测试";
            Current_FontSize = 40;
            itemFont.setPointSize(Current_FontSize);
            p.setFont(itemFont);
            p.drawText(250, 200, "读盘测试");
            p.drawText(200, 280, "标签个数: ");
            p.drawText(500, 280,  QString::number(hardwareCheckCustom, 10));

            dispProcess += ".";
            p.drawText(170, 350, dispProcess);
            if(dispProcess.length() > 30) {
                dispProcess = ".";
            }
        break;

        case MOTOR_CHECK://电机测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "电机测试";
            p.drawText(250, 250, "电机测试");
            dispProcess += ".";
            p.drawText(170, 350, dispProcess);
            if(dispProcess.length() > 30) {
                dispProcess = ".";
            }
        break;

        case VOICE_CHECK://语音测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "语音测试";
            p.drawText(250, 250, "语音测试");
            dispProcess += ".";
            p.drawText(170, 350, dispProcess);
            if(dispProcess.length() > 30) {
                dispProcess = ".";
            }
        break;   

        case LED_CHECK://灯带测试
            Button_Cancel->show();  
            Button_Determine->show();
            qDebug() << "灯带测试";
            p.drawText(250, 250, "灯带测试");
            dispProcess += ".";
            p.drawText(170, 350, dispProcess);
            if(dispProcess.length() > 30) {
                dispProcess = ".";
            }
        break;   
    }
}

/*==================================================================================
* 函 数 名： paintEvent
* 参    数：
* 功能描述:  界面刷新，各个任务的处理
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::paintEvent(QPaintEvent *)
{
    //设备的各个状态做相应的操作
    switch (mDevMsg.devStatus)  
    {
        case INIT_STATUS://装配完成状态
            p_Widget_Meaasge->Son_Button_Cancel->hide();
            p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            dev_factory_init_status_task(NetConfigStatus);
        break;

        case WAIT_TOPIC_STATUS://工厂完成配网获取完uuid 二维码显示uuid
            p_Widget_Meaasge->Son_Button_Cancel->hide();
            p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            dev_configh_net_task();    
        break;

        case FACTORY_BIND_OK_STATUS://工厂完成sn码跟uuid匹配绑定显示6位数字码跟条形码
        case LOCAL_UNBIND_STATUS://解绑状态，回到绑定状态
            p_Widget_Meaasge->Son_Button_Cancel->hide();
            p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            dev_factory_bind_task(mDevMsg.sn, mDevMsg.devId);
        break;

        case SITE_BIND_OK_STATUS://设备正常工作状态
        case SETUP_STATUS://设置界面
            dev_work_status_task();
        break;

        case UPDATE_OTA_STATUS://固件更新状态
            dev_update_ota_task();
        break;

        case SITE_CONFIG_NET_STATUS://现场配置网络
            dev_site_config_net_task(NetConfigStatus);
        break;

        case FACTORY_CHECK_STATUS://工厂硬件检测
            factory_hardware_check_task(hardwareCheckCmd);
        break;
    } 
}

unsigned char Quick_Add_Step_Times = 0;
unsigned char Quick_Out_Of_Bound_Flage = 0;
//快滑松手  //快滑滑动松手根据前一刻方向 来滑最后一格   ***************************
void Widget::Scroll_Quick_TimeOut_Update_GUI()
{
    //#ifdef RELEASED_QUICK
    qDebug()<<"Scroll_Quick_TimeOut_Update_GUI >"<<Release_Quick_TimeOut_Flage<<" "<<Add_Step_By_Step<<"Scroll_Dir = "<<Scroll_Dir;
    //#endif

    if(Scroll_Dir == 0x01)
    {
        Add_Step_By_Step--;  //下拉菜单
        Last_Update_GUI = 0;

        if((Add_Step_By_Step <= 3)&&(Slow_Alarm_Flage == 1)) {
            Slow_Alarm_Flage = 0;
        }
        if(Add_Step_By_Step <=-1)  //
        {
            if(selectItemIndex > 0) {
                selectItemIndex--;
                Slow_Alarm_Flage = 1;
                Release_Quick_TimeOut_Flage = 1;
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex == 0) {
            Quick_Out_Of_Bound_Flage = 1;
            Release_Quick_TimeOut_Flage = 1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
            Scroll_Times_Quick_Last = 0;
            Quick_Add_Step = 1;
        }
    }
    else if(Scroll_Dir == 0x02) {
        Add_Step_By_Step++; //上滑菜单
        Last_Update_GUI = 0;

        if((Add_Step_By_Step >= 11) && (Slow_Alarm_Flage == 1)) {
            Slow_Alarm_Flage = 0;
        }
        if(Add_Step_By_Step >=(2*FRAME +1))// 4  5  6
        {
            if(selectItemIndex < (totalItemNum-1)) {
                selectItemIndex++;
                selectItemIndexBak = selectItemIndex;
                Slow_Alarm_Flage = 1;
                Release_Quick_TimeOut_Flage = 1;
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex == (totalItemNum-1)) {
            Quick_Out_Of_Bound_Flage = 1;
            Release_Quick_TimeOut_Flage = 1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
            Scroll_Times_Quick_Last = 0;
            Quick_Add_Step = 1;
        }
    }
    if((Add_Step_By_Step == FRAME)&&(Scroll_Times_Quick_Last == 0)) {
        Quick_Out_Of_Bound_Flage = 0;
        Last_Update_GUI = 1;
        Release_Quick_Dir_Update_Times = 0;
        Scroll_Times_Quick_Last = 0;

        if(Release_Quick_Dir_Timeout->isActive() == true) {
            Release_Quick_Dir_Timeout->stop();
        }
        emit Touch_Allow_Send_Handler();//快滑期间确定取消失效  快滑结束确定取消  生效
        update();
    } else {
        if(Scroll_Times_Quick_Last > 0) {
            #ifdef RELEASED_QUICK
            qDebug()<<"Release_Quick_Dir_Update_Times = "<<Release_Quick_Dir_Update_Times;
            #endif
            if((Quick_Add_Step>=2)&&(Quick_Add_Step<=4)) {
                Quick_Add_Step_Times++;
                if(Quick_Add_Step_Times == 4) {
                    Quick_Add_Step_Times = 0;
                    Quick_Add_Step--;
                    qDebug()<<"Quick_Add_Step--" <<Scroll_Times_Quick_Last<<Quick_Add_Step;
                }
                Release_Quick_Dir_Update_Times = 0;
                Release_Quick_Dir_Timeout->start(Scroll_Times_Quick_Last_Period);
            }
            else if(Quick_Add_Step == 1) {
                qDebug()<<"Release_Quick_Dir_Update_Times+"<<Scroll_Times_Quick_Last<<Add_Step_By_Step;
                Release_Quick_Dir_Update_Times++;
                Release_Quick_Dir_Timeout->start(Scroll_Times_Quick_Last_Period + 10*Release_Quick_Dir_Update_Times);
            }
            Scroll_Times_Quick_Last--;
        }
        update();
    }
}

/***********************************************慢滑松手  靠近最近的那个*********************************/
void Widget::Release_TimeOut_Update_GUI()
{
    qDebug()<<"Slow_Near--"<<Add_Step_By_Step;
    Release_Slow_Update_Times++;
    
    if((Add_Step_By_Step >= (FRAME/2)) && (Add_Step_By_Step < FRAME)) {//6  5  4  3
        Add_Step_By_Step++;
    }
    else if((Add_Step_By_Step <= (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step > FRAME)) {//8  9  10  11  靠近
        Add_Step_By_Step--;
    }

    if(Add_Step_By_Step == FRAME) {
        Add_Step_By_Step = 7;
        Release_Slow_Update_Times = 0;
        Last_Update_GUI = 1;
        if(Release_Timeout->isActive() == true)   
            Release_Timeout->stop();
        qDebug()<<"Add_Step_By_Step == "<<Add_Step_By_Step;
        Slow_Alarm_Flage = 1; //打开声音播放  Slow_Alarm_Flage 慢滑声音响动标志
        update();
    } else {
        update();
        if(Release_Timeout->isActive() == false)
            Release_Timeout->start(40 + Release_Slow_Update_Times*10);
    }
}
/********************************慢滑松手  根据方向来决定滑动 **************************************************/
void Widget::Release_Slow_Dir_TimeOut_Update_GUI()
{
    qDebug()<<"Slow_Dir -- Dir";
    if(Add_Step_By_Step == FRAME)
    {
        Alarm_Flage = 1; // 慢滑声音  标志
        Release_Slow_Dir_Update_Times = 0;
        if(Release_Slow_Dir_Timeout->isActive() == true)
            Release_Slow_Dir_Timeout->stop();
        return;
    }

    Release_Slow_Dir_Update_Times++;
    if(Scroll_Dir == 1)    //0 1 2
    {
        Add_Step_By_Step--;  //朝下滑 6  //FRAME
        Last_Update_GUI = 0;

        if(Add_Step_By_Step <=-1)//6  5  4  3  2  1  0
        {
            if(selectItemIndex > 0) {
                selectItemIndex--;
                Slow_Alarm_Flage = 1;
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex <= 0)//最后一条不可以滑动
        {
            Alarm_Flage = 1;    // 慢滑声音  标志
            selectItemIndex = 0;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        Slow_Alarm_Flage = 1; //慢滑声音响动标志
    }
    else if(Scroll_Dir == 2)   // 12 13 14
    {
        Add_Step_By_Step++;    //上滑菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step >= (2*FRAME+1))//8 9 10 11 12 13 14
        {
            if(selectItemIndex < (totalItemNum-1) && selectItemIndex<(selectItemIndexBak+2)) {
                selectItemIndex++;
                selectItemIndexBak = selectItemIndex;
                Slow_Alarm_Flage = 1;//慢滑声音响动标志
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex >= (totalItemNum-1)) {//最后一条不可以滑动
            selectItemIndex = totalItemNum-1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI  = 1;
            Slow_Alarm_Flage = 1;//慢滑声音响动标志
        }
    }

    if(Add_Step_By_Step == FRAME) {
        // 慢滑  铃声响动
        Last_Update_GUI = 1;
        Alarm_Flage = 1; // 慢滑声音  标志
        Release_Slow_Dir_Update_Times = 0;

        if(Release_Slow_Dir_Timeout->isActive() == true) {
             Release_Slow_Dir_Timeout->stop();
        }
        update();
    } else {
        Last_Update_GUI = 0;
        update();
        if(Release_Slow_Dir_Timeout->isActive()== false) {
            Release_Slow_Dir_Timeout->start(40 + Release_Slow_Dir_Update_Times*10);
        }
    }
}

//**********************慢滑松手   根据方向  位置   来决定滑动******************************************/
void Widget::Release_Slow_Dir_Near_Update_GUI()
{
    if((Add_Step_By_Step >= (FRAME/2)) && (Add_Step_By_Step < FRAME)) {//6  5  4  3 靠近
        Release_TimeOut_Update_GUI();//根据靠近来决定选择哪个条目
    }// 7/2
    else if((Add_Step_By_Step < (FRAME/2)) && (Add_Step_By_Step >= 0)) {//2  1  0 方向
        Release_Slow_Dir_TimeOut_Update_GUI();//根据方向来决定选择哪个条目
    }// 7 + 7/2 +1
    else if((Add_Step_By_Step <= (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step > FRAME)) {//8  9  10  11  靠近
        Release_TimeOut_Update_GUI();//根据靠近来决定选择哪个条目
    }// 7 + 7/2 +1
    else if((Add_Step_By_Step > (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step < (2*FRAME + 1))) {//12  13  14 方向
        Release_Slow_Dir_TimeOut_Update_GUI();//根据方向来决定选择哪个条目
    }
}

//********************************************************************************/
void Widget::Slow_Scroll_Timer_Handle()
{
    qDebug() << "Slow_Scroll_Timer_Handle ";
    if((selectItemIndex == 0)&&(Scroll_Dir == 1))//最后一条不可以滑动
    {
        if(Slow_Scroll_Timer->isActive() == true) {
            Slow_Scroll_Timer->stop();
        }
        if(Alarm_Timer->isActive() == true) {
           Alarm_Timer->stop();
        }
        return;
    }
    else if((selectItemIndex == (totalItemNum-1))&&(Scroll_Dir == 2))//最后一条不可以滑动
    {
        if(Slow_Scroll_Timer->isActive() == true) {
            Slow_Scroll_Timer->stop();
        }
        if(Alarm_Timer->isActive() == true) {
           Alarm_Timer->stop();
        }
        return;
    }

    if(Scroll_Dir == 1)//下滑
    {
        //慢滑精细选择时控制当前菜品不能出最下面的边界
        if((selectItemIndex < (selectItemIndexBak-2)) && quickSlowMoveFlag == false)
        {
            if(Slow_Scroll_Timer->isActive() == true) {
                Slow_Scroll_Timer->stop();
            }
            if(Alarm_Timer->isActive() == true) {
                Alarm_Timer->stop();
            }
            return;
        } 
        Add_Step_By_Step = Add_Step_By_Step - Quick_Add_Step;
        Last_Update_GUI = 0;
     
        if((Add_Step_By_Step == 5)&&((Slow_Alarm_Flage == 1)||(Quick_Alarm_Times_Flage == 1)))
        {
            if(Quick_Alarm_Times_Flage == 1) {
            
            }
            if(Slow_Alarm_Flage == 1) {
                Slow_Alarm_Flage = 0;
                #ifdef  ALARM_SLOW_ENABLE // 慢滑 铃声响动
                #endif
            }
        }
        if(Add_Step_By_Step <= -1)  //   6   5   4    3    2   1   0
        {
            if(selectItemIndex > 0) {
                selectItemIndex--;
                Slow_Alarm_Flage = 1;
                Quick_Alarm_Times_Flage = 1;

                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex == 0) 
        {  //最后一条不可以滑动
            Slow_Alarm_Flage = 1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        update();
    }
    else if(Scroll_Dir == 2)
    {
        //慢滑精细选择时控制当前菜品不能出最上面的边界
        if((selectItemIndex > (selectItemIndexBak+2)) && quickSlowMoveFlag == false)
        {
            if(Slow_Scroll_Timer->isActive() == true) {
                Slow_Scroll_Timer->stop();
            }
            if(Alarm_Timer->isActive() == true) {
                Alarm_Timer->stop();
            }
            return;
        } 

        //Add_Step_By_Step++;       //上滑菜单
        Add_Step_By_Step = Add_Step_By_Step + Quick_Add_Step;
        Last_Update_GUI = 0;

        if((Add_Step_By_Step == 10) && ((Slow_Alarm_Flage == 1)||(Quick_Alarm_Times_Flage == 1)))
        {
            if(Quick_Alarm_Times_Flage == 1) {
                Quick_Alarm_Times_Flage = 0;
                #ifdef  ALARM_ENABLE
                qDebug()<<" "<<Add_Step_By_Step;
                #endif
            }
            if(Slow_Alarm_Flage == 1) {
                Slow_Alarm_Flage = 0;
                #ifdef  ALARM_SLOW_ENABLE//慢滑  铃声响动
                #endif
            }
        }
        if(Add_Step_By_Step >= (2*FRAME+1))//8 9 10 11 12 13 14
        {  
            if(selectItemIndex < (totalItemNum-1) ) {
                selectItemIndex++;
                Slow_Alarm_Flage = 1;
                Quick_Alarm_Times_Flage = 1;

                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;     
            }
        }
        if(selectItemIndex == (totalItemNum-1)) 
        {//最后一条不可以滑动
            Slow_Alarm_Flage = 1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        update();
    }
    Scroll_Times--;
    if(Scroll_Times > 0) {
        Slow_Scroll_Timer->start(Quick_Scroll_Period);
    }
    if(Scroll_Times <= 0)
    {
        if(Slow_Scroll_Timer->isActive() == true) {
            Slow_Scroll_Timer->stop();
            Scroll_Times = 0;
        }
        if(Scroll_Quick_Flage == 1)  //
        {
            #ifdef  ALARM_ENABLE
            if(Alarm_Timer->isActive() == true)
            {
               Alarm_Timer->stop();
               //audio.play();     //  1  次
            }
            #endif
            Scroll_Quick_Flage = 0;
            Release_Quick_TimeOut_Flage = 0;
            Release_Quick_Dir_Update_Times = 1;

            Last_Update_GUI = 1;
            Release_Quick_Dir_Timeout->start(Scroll_Times_Quick_Last_Period); //50ms
        }
    }
}

/*==================================================================================
* 函 数 名： dev_factory_init_status_touch_handle
* 参    数：
* 功能描述:  设备初始化状态运行任务
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_factory_init_status_touch_handle(int Receive_Diff_Data_Total)
{
    //非写菜界面，确认键有效
    if (Receive_Diff_Data_Total == 0x4000) {//确定按键    
        qDebug() << "确定按键";         
        Send_Message_Type = Send_Config_Net;//发送配网指令到golang
        Send_Message_Times = 0;
        Communicate_Msg_QT_Go_Timer->start(200);//200 ms
        NetConfigStatus = Config_Net_Status_Process;//主屏显示网络配置中
        update();  
    } else if (Receive_Diff_Data_Total == 0x8000) {//取消按键
        qDebug() << "取消按键";
        if (NetConfigStatus == Config_Net_Status_Process) {
            NetConfigStatus = Config_Net_Status_Idle;
            Send_Message_Type = Send_Config_Net;//发送配网指令到golang
            Send_Message_Times = 0;
            Communicate_Msg_QT_Go_Timer->start(200);//200 ms
        } 
        else if (NetConfigStatus == Config_Net_Status_Ok) {
            NetConfigStatus = Config_Net_Status_Idle;
        }
        p_Widget_Meaasge->hide();
        update();
    }
}

/*==================================================================================
* 函 数 名： dev_configh_net_touch_handle
* 参    数：
* 功能描述:  设备初始化状态运行任务
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_configh_net_touch_handle(int Receive_Diff_Data_Total)
{

}

/*==================================================================================
* 函 数 名： dev_factory_bind_touch_handle
* 参    数：
* 功能描述:  设备初始化状态运行任务
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_factory_bind_touch_handle(int Receive_Diff_Data_Total)
{
    //非写菜界面，确认键有效
    // if (Receive_Diff_Data_Total == 0x4000) {//确定按键    
    //     qDebug() << "确定按键";         
    //     Send_Message_Type = Send_Config_Net;//发送配网指令到golang
    //     Send_Message_Times = 0;
    //     Communicate_Msg_QT_Go_Timer->start(200);//200 ms
    //     NetConfigStatus = Config_Net_Status_Process;//主屏显示网络配置中
    //     update();  
    // } else 
    if(Receive_Diff_Data_Total == 0x8000) 
    {//取消按键
        Setup_Touch_Value_list.clear();
        Setup_Touch_Time_list.clear();
        // NetConfigStatus = Config_Net_Status_Idle;
        // qDebug() << "取消按键";
        // mDevMsg.devStatus = SITE_CONFIG_NET_STATUS;//设备进入配网状态
        // p_Widget_Meaasge->hide();
        // update();
    }
}

/*==================================================================================
* 函 数 名： dev_site_config_net_touch_handle
* 参    数：
* 功能描述:  现场配置网络状态按键处理
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_site_config_net_touch_handle(int Receive_Diff_Data_Total)
{
    if (Receive_Diff_Data_Total == 0x4000) {//确定按键    
        qDebug() << "确定按键";         
        Send_Message_Type = Send_Config_Net;//发送配网指令到golang
        Send_Message_Times = 0;
        Communicate_Msg_QT_Go_Timer->start(200);//200 ms
        NetConfigStatus = Config_Net_Status_Process;//主屏显示网络配置中
        update();  
    } else if (Receive_Diff_Data_Total == 0x8000) {//取消按键
        qDebug() << "现场配网取消按键  设备状态备份 "<< devStatusBak;
        p_Widget_Meaasge->hide();
        
        if( NetConfigStatus == Config_Net_Status_Process)
        { 
            Send_Message_Type = Send_Reboot_Golang_App;
            Send_Message_Times = 0;
            Communicate_Msg_QT_Go_Timer->start(200);//200 ms

            mDevMsg.devStatus = SITE_CONFIG_NET_STATUS;
            NetConfigStatus = Config_Net_Status_Idle;
        } else {
             mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
            
            // mDevMsg.devStatus = devStatusBak ;
            // if(mDevMsg.devStatus == SITE_BIND_OK_STATUS)
            // {
            //     Current_Page = 0;
            //     selectItemIndex = 0;
            //     Widget_Page_Switch = 0;
            //     p_Widget_Meaasge->hide();
            //     p_Widget_Meaasge->Son_Button_Cancel->hide();
            //     p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            // }
            // if(mDevMsg.devStatus == FACTORY_BIND_OK_STATUS)
            // {

            // } else 
            {
                qDebug() << "回到设置界面。。。。。。。。。。。。。。";
                mDevMsg.devStatus = SETUP_STATUS;
                Widget_Page_Switch = 0;
                Current_Page = 1;
                selectItemIndex = 1;
            }
          
        }
        update();
    }
}

/*==================================================================================
* 函 数 名： dev_update_ota_touch_handle
* 参    数：
* 功能描述:  固件更新状态touch回调
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_update_ota_touch_handle(int Receive_Diff_Data_Total)
{
    //固件更新按键回调
    if (Receive_Diff_Data_Total == 0x4000) {//确定按键    
        qDebug() << "固件更新确定按键" << devStatusBak;        
        mDevMsg.devStatus = devStatusBak;//固件更新过程中按确认键后回到之前的设备状态
        update(); 
        // QProcess p;
        // //执行开始，括号里的字符串是执行的命令
        // p.start("reboot");
        // //设置命令执行过后，多少时间后来获取执行结果,单位毫秒
        // p.waitForBytesWritten(); 
        // if(!p.waitForFinished(1)) {
        //     p.kill(); 
        //     p.waitForFinished(1); 
        // }
        
    } else if (Receive_Diff_Data_Total == 0x8000) {//取消按键
        qDebug() << "固件更新取消按键";
        mDevMsg.devStatus = devStatusBak;//固件更新过程中按确认键后回到之前的设备状态
        update();
    }
}

/*==================================================================================
* 函 数 名： dev_work_status_touch_handele
* 参    数：
* 功能描述:  设备初始化状态运行任务
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::dev_work_status_touch_handele(int Receive_Diff_Data_Total)
{
    unsigned short Receive_Diff_Data = 0;
    unsigned char  Data_Flage = 0;
    unsigned char  Dir_Old = 0;
    int postionStart, postionEnd;

    if(Receive_Diff_Data_Total == 0x4000 ||Receive_Diff_Data_Total == 0x8000)
    {
        play_wav();
    }
    //写盘状态
    if(updateMenuStatus == Update_Menu_Ok)//菜单更新成功
    {
        if(Receive_Diff_Data_Total == 0x4000) {//确定按键
           Receive_Diff_Data_Total = 0;
           updateMenuStatus = Update_Menu_Work_Status;//进入工作状态
           Current_Page = 0;
           Quick_Add_Step = 1;
           Widget_Page_Switch = 0;
           selectItemIndex_Page0 = -1;
           pMenudbName = "menu_lev1";
           selectItemIndex = 0;
           update();
           Previous_Page = Current_Page;
           return;
        }
    }
    if(mDevMsg.devStatus != SETUP_STATUS) 
    {
        if(updateMenuStatus != Update_Menu_Work_Status)
            return;
    } 
    //设置界面二次确认键处理
    if((Receive_Diff_Data_Total == 0x4000) && Current_Page == 2 && mDevMsg.devStatus == SETUP_STATUS) {
        if(pselectMenuName == "更改网络") {
            //NetConfigStatus = Config_Net_Status_Idle;
            NetConfigStatus = Config_Net_Status_Process;//主屏显示网络配置中
            qDebug() << "确定按键..........";
            devStatusBak = mDevMsg.devStatus;
            mDevMsg.devStatus = SITE_CONFIG_NET_STATUS;//设备进入配网状态

            Send_Message_Type = Send_Config_Net;//发送配网指令到golang
            Send_Message_Times = 0;
            Communicate_Msg_QT_Go_Timer->start(200);//200 ms
           

            p_Widget_Meaasge->hide();
            Current_Page = 0;
            Widget_Page_Switch = 0;
            p_Widget_Meaasge->Son_Button_Cancel->hide();
            p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            update();
            return;
        }
        else if(pselectMenuName == "出厂设置") {
            devStatusBak = -1;
            mDevMsg.devStatus = FACTORY_BIND_OK_STATUS;//恢复出厂状态
            p_Widget_Meaasge->hide();
            Current_Page = 0;
            Widget_Page_Switch = 0;
            p_Widget_Meaasge->Son_Button_Cancel->hide();
            p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            Send_Message_Type = Send_Unbind_Cmd_To_Golang;//消息队列发送解绑指令
            updateMenuStatus = -1;
            update();
            return;
        }
    }
    //非写菜界面，确认键有效
    if((Receive_Diff_Data_Total == 0x4000)&&((Current_Page == 0)||(Current_Page == 1))) //确定  第0页  第1页
    {   
        if(mDevMsg.devStatus == SETUP_STATUS) 
        {//设置界面
            Current_Page = 2;
            Button_Cancel->show();
            if(pselectMenuName == "出厂设置") 
            {
                qDebug() << "出厂设置选择";
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Restore_Factory;
                p_Widget_Meaasge->show();
            }
            else if(pselectMenuName == "更改网络")
            { 
                qDebug() << "更改网络选择";
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Change_Net; 
                p_Widget_Meaasge->show();
            }
            else if(pselectMenuName == "菜单版本")
            {
                qDebug() << "菜单版本选择";
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Menu_Ver;
                p_Widget_Meaasge->show();
            }
            else if(pselectMenuName == "网络属性")
            {
                qDebug() << "网络属性选择";
                QProcess process;
                //执行开始，括号里的字符串是执行的命令
                process.start("ifconfig wlan0");
                //设置命令执行过后，多少时间后来获取执行结果,单位毫秒
                process.waitForFinished(2000);
                QByteArray output = process.readAllStandardOutput();
                //声明字符串用来接收执行的结果
                QString result_ = output;
                qDebug() << "网络信息 = " << result_;
                
                qDebug() << result_;
                postionStart = result_.indexOf("inet ");
                if(postionStart < 0) {
                    qDebug() << "查询失败";
                    p_Widget_Meaasge->Display_Status_Current = Display_Status_Net_Msg;
                    p_Widget_Meaasge->show();
                    return;
                }
                postionEnd = result_.indexOf("netmask");
                qDebug() << "ip 起始位置" << postionStart << postionEnd;
                ipAddr = result_.mid(postionStart+5, postionEnd - postionStart - 5);
                qDebug() << "ip 名称" << ipAddr;

                postionStart = result_.indexOf("ether");
                postionEnd = result_.indexOf("txqueuelen");
                qDebug() << "mac 起始位置" << postionStart << postionEnd;
                macAddr = result_.mid(postionStart+6, postionEnd - postionStart - 6);
                qDebug() << "mac 名称" << macAddr;
                
                //查询wifi 信息
                //执行开始，括号里的字符串是执行的命令
                process.start("cat /etc/wpa_supplicant.conf");
                //设置命令执行过后，多少时间后来获取执行结果,单位毫秒
                process.waitForFinished(2000);
                output = process.readAllStandardOutput();
                //声明字符串用来接收执行的结果
                result_ = output;
                qDebug() << "WIFI信息 = " << result_;
                
                qDebug() << result_;
                postionStart = result_.indexOf("ssid");
                if(postionStart < 0) {
                    qDebug() << "查询失败";
                    p_Widget_Meaasge->Display_Status_Current = Display_Status_Net_Msg;
                    p_Widget_Meaasge->show();
                    return;
                }
                
                postionEnd = result_.indexOf("\" \n");
                if(postionEnd = -1) {
                    postionEnd = result_.indexOf("\"\n");
                }
                qDebug() << "ssid 起始位置" << postionStart << postionEnd;
                ssidStr = result_.mid(postionStart+6, postionEnd - postionStart -6);
                qDebug() << "ssid 名称" << ssidStr;

                p_Widget_Meaasge->Display_Status_Current = Display_Status_Net_Msg;
                p_Widget_Meaasge->show();
            }
            else if(pselectMenuName == "设备信息")
            {
                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                mDevMsg.devStatus = SETUP_STATUS;
                printf("设备状态 = %d\n",mDevMsg.devStatus );
                printf("设备ID = %d\n", mDevMsg.devId );
                qDebug()<<"设备sn ="<<mDevMsg.sn ;
                qDebug()<<"菜单版本 ="<<mDevMsg.menu_ver;
                qDebug()<<"硬件版本 ="<<mDevMsg.har_ver;
                qDebug()<<"软件版本 ="<<mDevMsg.soft_ver;
                p_menu_ver = mDevMsg.menu_ver;
                qDebug() << "设备信息选择";
                QProcess process;
                //执行开始，括号里的字符串是执行的命令
                process.start("cat /proc/version");
                //设置命令执行过后，多少时间后来获取执行结果,单位毫秒
                process.waitForFinished(2000);
                QByteArray output = process.readAllStandardOutput();
                //声明字符串用来接收执行的结果
                QString result_ = output;
                qDebug() << "linux版本信息 = " << result_;
                
                qDebug() << result_;
                postionStart = result_.indexOf("version");
                if(postionStart < 0) {
                    qDebug() << "查询失败";
                    p_Widget_Meaasge->Display_Status_Current = Display_Status_Dev_Msg;
                    p_Widget_Meaasge->show();
                    return;
                }
                postionEnd = result_.indexOf("(");
                qDebug() << "linux版本 起始位置" << postionStart << postionEnd;
                linuxVer = result_.mid(postionStart+8, postionEnd - postionStart - 8);
                qDebug() << "linux版本 = " << linuxVer;
                p_Widget_Meaasge->Display_Status_Current = Display_Status_Dev_Msg;
                p_Widget_Meaasge->show();
            }
            devStatusBak = SETUP_STATUS;
            
        }  else {
            mMenu = mSqliteClass.Sqlite_read_menu_from_menudb(pMenudbName, pselectMenuName); //从数据库读取选择的菜单信息
            if(mMenu.nextmenu !="") {
                pMenudbName = mMenu.nextmenu;//当前选中的数据库名称
            }    
            qDebug()<<"选取的菜单信息 = "<<mMenu.id <<mMenu.grade <<mMenu.context <<mMenu.dish_id <<mMenu.nextmenu;
            if(Current_Page < (Total_Page-1) & mDevMsg.devStatus != SETUP_STATUS ) {
                Current_Page++;
                if(mMenu.nextmenu !="") {
                    Current_Page = 1;
                } else {
                    Menu_ID = mMenu.dish_id;
                    qDebug() <<"菜品 id = "<<Menu_ID;
                }
            }
            if(Current_Page >= (Total_Page-1)) {
                Current_Page = Total_Page-1;
            }
            //if(Current_Page != Previous_Page) 
            {
                Quick_Add_Step = 1;
                Widget_Page_Switch = 0;
                if(Current_Page == 1) {//从第一页 到 第二页
                    selectItemIndex_Page0 = selectItemIndex;
                }
                if((Current_Page == 2)&&(Previous_Page == 1)) {//从第二页到第三页
                    selectItemIndex_Page1 = selectItemIndex;
                    Send_Message_Type  = Send_Message_Write;
                    Send_Message_Times = 0;
                    Communicate_Msg_QT_Go_Timer->start(200);
                }
                update();
                Previous_Page = Current_Page;
            }
        }         
       
    }
 
    else if((Receive_Diff_Data_Total == 0x8000)&&((Current_Page == 1)||(Current_Page == 2))) //取消按键
    {
        if(touchLock == true) {update(); return;}

        if(mDevMsg.devStatus == SETUP_STATUS) {//设置界面
            qDebug() << "取消按键触发..............";
            if(Current_Page == 2) {
                Current_Page = 1;
                Widget_Page_Switch = 0;
                p_Widget_Meaasge->hide();
                p_Widget_Meaasge->Son_Button_Cancel->hide();
                p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
            } else {
                //mDevMsg.devStatus = devStatusBak;
                mDevMsg = mSqliteClass.Sqlite_read_msg_from_configdb();//配置数据库读取设备信息
                printf("设备状态 = %d\n",mDevMsg.devStatus );
                Current_Page = 0;
                Widget_Page_Switch = 0;
                p_Widget_Meaasge->hide();
                p_Widget_Meaasge->Son_Button_Cancel->hide();
                p_Widget_Meaasge->Son_Button_Determine->hide(); //确认按键
                Setup_Slider_p->hide();
            }
            update();
            return;
        } else {
            mMenu = mSqliteClass.Sqlite_read_menu_from_menudb(pMenudbName, pselectMenuName); //从数据库读取选择的菜单信息
            qDebug()<<"取消菜单 = "<<mMenu.premenu <<mMenu.nextmenu <<mMenu.context;
            qDebug()<<"当前页 = "<<Current_Page;
            qDebug()<<"\r\n";
            if (Current_Page > 0) {
                qDebug()<<"mMenu.premenu = "<<mMenu.premenu;
                if(mMenu.premenu != "") {
                    if(Current_Page == 2) {
                        Current_Page = 1;//不是写菜品页，都默认为中间页
                    } else {
                        Current_Page = 1;//不是写菜品页，都默认为中间页
                        //根据当前选中的菜单回退时保持上一级菜单的选中记录id
                        selectItemIndex = mSqliteClass.Sqlite_read_premenu_id_from_menudb(mMenu.premenu, pMenudbName)-1;
                        selectItemIndex_Page1 = selectItemIndex;
                        qDebug()<<"取消selectItemIndex = "<<selectItemIndex;
                        pMenudbName = mMenu.premenu;//当前选中的数据库名称
                        if(mMenu.premenu == "menu_lev1") {
                            Current_Page = 0;
                        }
                    }
                } else {
                    Current_Page = 0;
                    pMenudbName = "menu_lev1";
                }  
            }
            if (Current_Page <= 0) {
                Current_Page = 0;
                pMenudbName = "menu_lev1";
            } 
            //if(Current_Page != Previous_Page) 
            {   //换页
                audio.play();
                Quick_Add_Step = 1;
                Widget_Page_Switch = 0;
                if((Current_Page == 1)&&(Previous_Page == 2)) {//从第二页到第一页
                    Send_Message_Type = Send_Message_Stop;
                    Communicate_Msg_QT_Go_Timer->start(1000);
                }
                update();
                Previous_Page = Current_Page;
            }
        }
    }
    else if((Receive_Diff_Data_Total > 0x00)&&(Receive_Diff_Data_Total < 0x2000)&&((Current_Page == 0)||(Current_Page == 1)))
    {
        if((Receive_Diff_Data_Total>>12) == 1) {//负
            Receive_Diff_Data_Total = Receive_Diff_Data_Total - 0x1000;
            Data_Flage = 1;
        }
        else if((Receive_Diff_Data_Total>>12) == 0) {//正
            Receive_Diff_Data_Total = Receive_Diff_Data_Total - 0x0000;
            Data_Flage = 0;
        }
        //慢滑界面刷新
        if((Receive_Diff_Data_Total>>8) == 1) {// 按下
            // 滑动值越来越大
            Receive_Diff_Data = Receive_Diff_Data_Total - 0x100;
            if(Data_Flage == 0) {//方向确定
                Scroll_Dir = 1;
                Data_Flage = 0;
            }
            else if(Data_Flage == 1) {
                Scroll_Dir = 2;
                Data_Flage = 0;
            }

            Receive_Diff_Data = 1;
            Quick_Add_Step = 1;

            if(Dir_Old ==  Scroll_Dir) {
                Scroll_Times = Receive_Diff_Data;
            }
            else if(Dir_Old != Scroll_Dir) {
                Scroll_Times = Receive_Diff_Data;
            }
            Scroll_Quick_Flage = 0;
            Slow_Alarm_Flage = 1;
            Quick_Alarm_Times_Flage = 0;

            Quick_Scroll_Period = 50;
            quickSlowMoveFlag = false;//慢滑标记
            Slow_Scroll_Timer_Handle();//800ms 就可以执行完
        }
        //快滑界面刷新
        if((Receive_Diff_Data_Total>>8) == 4) {//松手
            Receive_Diff_Data = Receive_Diff_Data_Total - 0x400;
            qDebug()<<"按键松开.................selectItemIndexBak = "<<selectItemIndexBak;
            selectItemIndexBak = selectItemIndex;
            if(Data_Flage == 0) {//方向确定
                Scroll_Dir = 1;
                Data_Flage = 0;
            }
            else if(Data_Flage == 1) {
                Scroll_Dir = 2;
                Data_Flage = 0;
            }

            if(Slow_Scroll_Music_Send_Timer->isActive() == true) {
                Slow_Scroll_Music_Send_Timer->stop();
            }
            Slow_Scroll_Music_Times_Stored = 0;
            Slow_Scroll_Music_Times_Start_Flage = 1;

            if(Receive_Diff_Data == 0xFF) {
                Scroll_Quick_Flage = 0;
                Quick_Add_Step = 1;
                if(Add_Step_By_Step != FRAME) {
                    Release_Slow_Dir_Near_Update_GUI();
                }
                else if(Add_Step_By_Step == FRAME) {
                    update();
                }
            }
            else if(Receive_Diff_Data < 0xFF) {
                Receive_Diff_Data = Receive_Diff_Data;

                if(Receive_Diff_Data >= 10) {// 17 测试滑 最快
                    Receive_Diff_Data = 10;
                }
                if(Receive_Diff_Data == 2) {
                    Quick_Add_Step = 1;
                    Quick_Scroll_Period = 40;
                    Scroll_Times =  500/Quick_Scroll_Period;
                    qDebug()<<"Scroll_Times = "<<Scroll_Times;
                    Scroll_Times += (8 - Scroll_Times%8 -4);
                    qDebug()<<"Scroll_Times1+= "<<Scroll_Times%8;
                    Scroll_Times_Quick_Last = 4;
                    Scroll_Times_Quick_Last_Period = 20;
                } else {
                    Quick_Scroll_Period = 40;
                    Scroll_Times = 400/Quick_Scroll_Period;// 30次

                    qDebug()<<"Scroll_Times2+= "<<Scroll_Times%8;
                    Scroll_Times_Quick_Last_Period = 20;
                }

                if((Receive_Diff_Data == 10)||(Receive_Diff_Data == 9)) {
                    Quick_Add_Step = 3;// 16 12---缓解步骤  4->3  3->2  2->1 4--缓解步骤
                    Scroll_Times_Quick_Last = 16-8;
                    Scroll_Times += (8 - Scroll_Times%8);
                    qDebug()<<"Decress_Times= "<< 4;
                }
                if((Receive_Diff_Data == 8)||(Receive_Diff_Data == 7)) {
                    Scroll_Times_Quick_Last = 12-6;// 12  8---缓解步骤  3->2  2->1 4--缓解步骤
                    Scroll_Times += (8 - Scroll_Times%8 - 4);
                    Quick_Add_Step = 2;
                    qDebug()<<"Decress_Times= "<< 3;
                }
                if((Receive_Diff_Data == 6)||(Receive_Diff_Data == 5)) {
                    Scroll_Times_Quick_Last = 8-4;// 8  4---缓解步骤 2->1  4--缓解时间
                    Scroll_Times += (8 - Scroll_Times%8);
                    Quick_Add_Step = 2;
                    qDebug()<<"Decress_Times= "<< 2;
                }
                if((Receive_Diff_Data == 4)||(Receive_Diff_Data == 3)) {
                    Scroll_Times_Quick_Last = 2;// 8 4---缓解步骤 2->1   4--缓解时间
                    Scroll_Times = (Scroll_Times<<1)/3;
                    Scroll_Times += (8 - Scroll_Times%8);
                    Quick_Add_Step = 2;
                    qDebug()<<"Decress_Times= "<< 2;
                }
                if(Receive_Diff_Data == 2) {// 4 4---缓解时间
                    Quick_Add_Step = 1;
                    Scroll_Times_Quick_Last = 1;
                    Scroll_Times += (8 - Scroll_Times%8 - 4);
                    qDebug()<<"Decress_Times= "<< 1;
                }
                Scroll_Quick_Flage = 1;
                if((Quick_Add_Step == 1)&&(Scroll_Times <= 48)) {//让喇叭单次响动
                    Scroll_Flage = 2;
                    if(Scroll_Flage != Scroll_Flage_Old) {
                        Quick_Alarm_Times_Flage = 1;
                    }
                } else {//让喇叭一直响动
                    Scroll_Quick_Flage = 1;
                    Quick_Alarm_Times_Flage = 0;
                    //Alarm_Timer->start(Quick_Scroll_Period*10);
                }
                Add_Step_By_Step = 7;
                Release_Quick_TimeOut_Flage = 0;
                quickSlowMoveFlag = true;//快滑标记
                Slow_Scroll_Timer_Handle();//800ms 就可以执行完
            }
        }
    }
}

/*==================================================================================
* 函 数 名： factory_hardware_check_touch_handle
* 参    数：
* 功能描述:  工厂硬件检测touch回调
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget::factory_hardware_check_touch_handle(int Receive_Diff_Data_Total)
{
    int value = 0;
    value = touchClass.get_touch_value();
    Touch_Value_list <<  QString::number(value);
    qDebug() << "触摸按键值 = " << value;
 
    if (Receive_Diff_Data_Total == 0x4000) {//确定按键    
        qDebug() << "确定按键"; 
        Hardware_Check_Update_Timer->stop();  
        Setup_Touch_Check_Timer->start(300);     
        mDevMsg.devStatus = devStatusBak;
        //消息队列发送检测结果到golang
        Snd_msg.mtext[0] = 0x04;
        Snd_msg.mtext[1] = 0xFF;
        Snd_msg.mtext[2] = hardwareCheckCmd;
        Snd_msg.mtext[3] = 0x01;
        Snd_msg.mtext[4] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg, 10, IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        update();  
    } 
    else if (Receive_Diff_Data_Total == 0x8000) {//取消按键
        qDebug() << "取消按键";
        Setup_Touch_Check_Timer->start(300); 
        mDevMsg.devStatus = devStatusBak;//
        Hardware_Check_Update_Timer->stop();
        //消息队列发送检测结果到golang
        Snd_msg.mtext[0] = 0x04;
        Snd_msg.mtext[1] = 0xFF;
        Snd_msg.mtext[2] = hardwareCheckCmd;
        Snd_msg.mtext[3] = 0x00;
        Snd_msg.mtext[4] = Cal_Crc(Snd_msg.mtext, Snd_msg.mtext[0]);

        if(msgsnd(id_Snd, (void *)&Snd_msg, 10, IPC_NOWAIT) == -1) {
            qDebug()<<"send msg error \n";  
        }
        update();
    }
   
}

void Widget::Handle_Touch_Value_Event(unsigned short Receive_Diff_Data_Total)
{
    unsigned short Receive_Diff_Data = 0;
    unsigned char  Data_Flage = 0;
    unsigned char  Dir_Old = 0;
    int value = 0;

   //audio.play();
    
    //获取touch值判断是否触发设置按键
    if(mDevMsg.devStatus != SETUP_STATUS & mDevMsg.devStatus != FACTORY_CHECK_STATUS & Current_Page == 0 ) 
    {
        //if(Setup_Touch_Value_list.count() > 10) {Setup_Touch_Value_list.clear(); Setup_Touch_Time_list.clear();}

        value = touchClass.get_touch_value();
        // if(Setup_Touch_Value_list.count() > 0) {//非空
        //     if(Setup_Touch_Value_list.at(Setup_Touch_Value_list.count()-1) != 255) {
        //         Setup_Touch_Time_list << QDateTime::currentMSecsSinceEpoch();//获取时间戳ms
        //         Setup_Touch_Value_list << value;
        //     }      
        // } else 
        {
            Setup_Touch_Time_list << QDateTime::currentMSecsSinceEpoch();
            Setup_Touch_Value_list << value;
        }
    }
    //touchClass.aplay_touch_voice();
    if(Receive_Diff_Data_Total == 0x2000)
    {
        Quick_Out_Of_Bound_Flage = 0;
        Scroll_Times_Quick_Last = 0;

        Scroll_Times_Quick_Last = 0;   // 快滑 减速处理
        Scroll_Times_Quick_Last_Period = 0;

        if(Alarm_Timer->isActive() == true) {// 闹钟
           Alarm_Timer->stop();
        }

        if(Slow_Scroll_Timer->isActive() == true) {//关掉所有 刷新 定时器 慢滑定时器
            Slow_Scroll_Timer->stop();  
        }
        Scroll_Times = 0;

        if(Release_Timeout->isActive() == true) {//慢滑 松手 定时器 靠近最近的那个
            Release_Timeout->stop();
        }
        Release_Slow_Update_Times = 0;

        if(Release_Slow_Dir_Timeout->isActive() == true) {//慢滑 松手 定时器 根据方向来决定选择哪个条目
            Release_Slow_Dir_Timeout->stop();
        }
        Release_Slow_Dir_Update_Times = 0;

        if(Release_Quick_Dir_Timeout->isActive() == true) {//快滑 松手 定时器
            Release_Quick_Dir_Timeout->stop();
            Release_Quick_Dir_Update_Times = 1;
        }
        Scroll_Quick_Flage = 0;
    }

    //设备的各个状态做相应的操作
    switch (mDevMsg.devStatus)   
    {
        case INIT_STATUS://装配完成状态
            dev_factory_init_status_touch_handle(Receive_Diff_Data_Total);
        break;

        case WAIT_TOPIC_STATUS://工厂完成配网获取完uuid 二维码显示uuid
            dev_configh_net_touch_handle(Receive_Diff_Data_Total);    
        break;

        case FACTORY_BIND_OK_STATUS://工厂完成sn码跟uuid匹配绑定显示6位数字码跟条形码
        case LOCAL_UNBIND_STATUS://解绑状态，回到绑定状态
            dev_factory_bind_touch_handle(Receive_Diff_Data_Total);
        break;

        case SITE_BIND_OK_STATUS://设备正常工作状态
        case SETUP_STATUS://设置界面
            dev_work_status_touch_handele(Receive_Diff_Data_Total);
        break;

        case UPDATE_OTA_STATUS://固件更新
            dev_update_ota_touch_handle(Receive_Diff_Data_Total);
        break;

        case SITE_CONFIG_NET_STATUS://现场配置网络
            dev_site_config_net_touch_handle(Receive_Diff_Data_Total);
        break;

        case FACTORY_CHECK_STATUS://工厂硬件检测
            factory_hardware_check_touch_handle(Receive_Diff_Data_Total);
        break;
    }
}

Widget_Message::Widget_Message(QWidget *parent): QWidget(parent)
{
    Display_Status_Current  = Display_Status_Succeed;
    Display_Status_Previous = Display_Status_Succeed;
    qDebug()<<"Widget_Message 窗口初始化 ";
    Son_Button_Cancel_list.append("color:white");                         // 前景色
    Son_Button_Cancel_list.append("background-color:rgb(255,255,255)");   // 背景色
    Son_Button_Cancel_list.append("border-style:outset");                 // 边框风格
    Son_Button_Cancel_list.append("border-width:0px");                    // 边框宽度
    Son_Button_Cancel_list.append("border-color:rgb(167,203,74)");        // 边框颜色
    Son_Button_Cancel_list.append("border-radius:10px");                  // 边框倒角

    Son_Button_Cancel = new QPushButton(this);
    Son_Button_Cancel->setStyleSheet(Son_Button_Cancel_list.join(';'));
    Son_Button_Cancel->move(718,9);
    Son_Button_Cancel->resize(75, 20);
    Son_Button_Cancel->hide(); //取消按键

    Son_Button_Determine_list.append("color:white");                         // 前景色
    Son_Button_Determine_list.append("background-color:rgb(167,203,74)");   // 背景色
    Son_Button_Determine_list.append("border-style:outset");                 // 边框风格
    Son_Button_Determine_list.append("border-width:0px");                    // 边框宽度
    Son_Button_Determine_list.append("border-color:rgb(167,203,74)");        // 边框颜色
    Son_Button_Determine_list.append("border-radius:10px");                  // 边框倒角

    Son_Button_Determine = new QPushButton(this);
    Son_Button_Determine->setStyleSheet(Son_Button_Determine_list.join(';'));
    Son_Button_Determine->move(718,451);
    Son_Button_Determine->resize(75, 20);
    Son_Button_Determine->hide(); //确认按键
}

Widget_Message::~Widget_Message()
{

}

/*==================================================================================
* 函 数 名： disp_string_center_black_back
* 参    数：
* 功能描述:  黑色背景中心显示字符串
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void Widget_Message::paintEvent(QPaintEvent *)
{
    int postionEnd, postionStart;
    int w = this->width();      //宽
    int h = this->height();     //高
    int Current_FontSize;
    QPainter p(this);
    QPen pen;

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);

    itemFont.setPointSize(80);
    p.setFont(itemFont);

    qDebug()<<"写状态信息"<<Display_Status_Current;
    if(Display_Status_Current == Display_Status_Succeed)
    {
        QBrush brush1(QColor(167, 203, 74, 255));
        pen.setColor(QColor(167, 203, 74, 255));
        p.setPen(pen);
        p.setBrush(brush1);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);//
        //p.drawText(150, 280, dishName);
        p.drawText(180, 280, "写盘成功");
        //写盘成功效果显示
        // if(dishName.length() < 8) {
        //     Current_FontSize = 80;
        //     itemFont.setPointSize(Current_FontSize);
        //     p.setFont(itemFont);
        //     //根据显示字符长度修改显示位置
        //     p.drawText(360-dishName.length()*50, 240+Current_FontSize/2, dishName);
        // } else {
        //     Current_FontSize = 70;
        //     itemFont.setPointSize(Current_FontSize);
        //     p.setFont(itemFont);
        //     //根据显示字符长度修改显示位置
        //     p.drawText(410-dishName.length()*50, 240+Current_FontSize/2, dishName);
        // }
        qDebug()<<"写盘成功";
    }
    else if(Display_Status_Current == Display_Status_Failed)
    {
        QBrush brush1(QColor(250, 06, 30, 255));
        pen.setColor(QColor(250, 06, 30, 255));
        p.setPen(pen);
        p.setBrush(brush1);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(180, 280, "写盘失败");
        qDebug()<<"写盘失败";
    }
    else if(Display_Status_Current == Display_Status_Multi_Disk_Exist)
    {
        QBrush brush1(QColor(250, 06, 30, 255));
        pen.setColor(QColor(250, 06, 30, 255));
        p.setPen(pen);
        p.setBrush(brush1);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(180, 280, "多盘存在");
        qDebug()<<"多盘存在";
    }
    else if(Display_Status_Current == Display_Status_No_Menu_Information)
    {
        QBrush brush1(QColor(250, 06, 30, 255));
        pen.setColor(QColor(250, 06, 30, 255));
        p.setPen(pen);
        p.setBrush(brush1);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(130, 280, "无菜单信息");
        qDebug()<<"无菜单信息";
    }
    else if(Display_Status_Current == Display_Status_Failed_Read_Label)
    {
        QBrush brush1(QColor(250, 06, 30, 255));
        pen.setColor(QColor(250, 06, 30, 255));
        p.setPen(pen);
        p.setBrush(brush1);
        p.drawRect(0, 0, w, h);

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(130, 280, "读标签失败");
        qDebug()<<"读标签失败";
    }

    //显示菜单版本
    else if(Display_Status_Current == Display_Status_Menu_Ver)
    {
        Son_Button_Cancel->show();
        Son_Button_Determine->hide(); //确认按键
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(10, 50, 170, 6);
        p.drawRoundedRect(rectangle, 4, 3); //上面长条

        Current_FontSize = 30;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(10, 40, "菜单版本");

        //QDateTime time = QDateTime::fromTime_t(p_menu_ver + 28800); //时间戳非毫秒级
        QDateTime time = QDateTime::fromMSecsSinceEpoch(p_menu_ver+28800000); //时间戳-毫秒级
        QString strStartTime = time.toString("yyyy-MM-dd hh:mm:ss");
        p.drawText(30, 150, "上次更新菜单时间：");
        p.drawText(30, 200, strStartTime);

        p.drawText(30, 300, "菜单版本：");
        p.drawText(30, 350, QString::number(p_menu_ver, 10));

        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 68, "返回");
    }
    //显示设备信息 
    else if(Display_Status_Current == Display_Status_Dev_Msg)
    {
        Son_Button_Cancel->show();
        Son_Button_Determine->hide(); //确认按键
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(10, 50, 170, 6);
        p.drawRoundedRect(rectangle, 4, 3); //上面长条

        Current_FontSize = 30;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(10, 40, "设备信息");

        p.drawText(30, 120, "产品序列号:");  p.drawText(270, 120, mDevMsg.sn);
        QString v2idStr = QString::number(mDevMsg.devId, 10);
        v2idStr.insert(3, ".");//字符串中间插入 "."
        p.drawText(30, 190, "六位随机码:");  p.drawText(270, 190, v2idStr);
        p.drawText(30, 260, "LINUX版本:");  p.drawText(270, 260, linuxVer);
        if(backUpFlag == true){
            p.drawText(30, 330, "应用版本:");    p.drawText(230, 330, "备份程序");
        } else {
            p.drawText(30, 330, "应用版本:");    p.drawText(230, 330, mDevMsg.soft_ver);
        }
        
        p.drawText(30, 400, "硬件版本:");    p.drawText(230, 400, mDevMsg.har_ver);

        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 68, "返回");
    }
    //显示网络信息 
    else if(Display_Status_Current == Display_Status_Net_Msg)
    {
        Son_Button_Cancel->show();
        Son_Button_Determine->hide(); //确认按键
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);       //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(10, 50, 170, 6);
        p.drawRoundedRect(rectangle, 4, 3); //上面长条

        Current_FontSize = 30;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(10, 40, "网络信息");
     
        p.drawText(30, 120, "热点名字:");  p.drawText(220, 120, ssidStr);
        p.drawText(30, 190, "ip地址:");  p.drawText(220, 190, ipAddr);
        p.drawText(30, 260, "MAC:");  p.drawText(220, 260, macAddr);

        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 68, "返回");
    }
    //设置界面重新配网
    else if(Display_Status_Current == Display_Status_Change_Net)//
    {
        Son_Button_Cancel->show();
        Son_Button_Determine->show(); //确认按键
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(10, 50, 170, 6);
        p.drawRoundedRect(rectangle, 4, 3); //上面长条

        Current_FontSize = 30;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(10, 40, "更改网络");
        Current_FontSize = 35;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(100, 250, "按确认键进入配置模式");

        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 68, "返回");
        p.drawText(716, 440, "确定");

        // qDebug()<<"Config_Net_Status_Process";
        // Current_FontSize = 28;
        // itemFont.setPointSize(Current_FontSize);
        // p.setFont(itemFont);
        // p.drawText(716, 68, "返回");

        // Current_FontSize = 45;
        // itemFont.setPointSize(Current_FontSize);
        // p.setFont(itemFont);
        // p.drawText(200, 250, "配网过程中");

    }
    //恢复出厂设置
    else if(Display_Status_Current == Display_Status_Restore_Factory)
    {
        Son_Button_Cancel->show();
        Son_Button_Determine->show(); //确认按键
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        pen.setWidth(1);
        pen.setColor(QColor (57, 57, 57, 255));
        p.setPen(pen);

        QBrush brush(QColor (57, 57, 57, 255));
        p.setBrush(brush);
        p.drawRect(0, 0, w-90-1, h);

        pen.setWidth(1);
        pen.setColor(QColor (0, 0, 0, 255));
        p.setPen(pen);
        QBrush brush1(QColor (0, 0, 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90-1, h);              //右半边矩形

        QBrush brush2(QColor(167, 203, 74));
        p.setBrush(brush2);
        QRectF rectangle(10, 50, 170, 6);
        p.drawRoundedRect(rectangle, 4, 3); //上面长条

        Current_FontSize = 30;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        p.drawText(10, 40, "出厂设置");
        Current_FontSize = 35;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(100, 250, "按确认键恢复到出厂设置");

        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716, 68, "返回");
        p.drawText(716, 440, "确定");
    }
}
