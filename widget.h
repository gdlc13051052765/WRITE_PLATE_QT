#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMutex>
#include <QList>
#include <QQueue>



extern QMutex mutex;
extern volatile unsigned char Current_Read_Press_Value;

class Widget : public QWidget
{
    Q_OBJECT

public:

    explicit Widget(QWidget *parent = 0);
    ~Widget();

    QTimer *Gui_Upate_Cycle_Timer;
    void Quick_Scroll_Once_Handle();
    //void Scroll_Quick_TimeOut_Update_GUI();
    //void Handle_Touch_Value_Event(void);
protected:
    void paintEvent(QPaintEvent *); //重绘函数
    void paintItems(QPainter *, QStringList*, int index, QFont*); //绘制文字

private:
    volatile unsigned char High_Scroll_Flage;
    volatile int Add_Step_By_Step;        //变化数
    //int Add_Step_By_Step_Flage;  //变化方向
    int Is_Speed_Low_Scroll;     //慢滑动标志位

    int Last_Update_GUI;         //上电和最终初始化GUI 标志位


    int totalItemNum;       //总的item数量
    int selectItemIndex;    //当前选中item索引
    QStringList menuList;   //菜单内容

    int showItemNum;        //显示的菜单个数
    int baseFontSize;       //中心字体大小
    int itemIntervalH;      //菜单垂直间隔
    int itemRealH;          //菜单实际间隔（菜单字符宽度+间隔）

    QFont itemFont;         //菜单字体样式


    int maxScrollTime;   //默认滑动最大值，时间时间1S
    int timeCount;      //滑动计时（1秒）

    int Touchpad_Flage; //

    int Released_Flage; //

    volatile unsigned char Quick_Scroll_Period;
    unsigned char Quick_Scroll_Times;

    unsigned int Scroll_Times;     //滑动次数
    unsigned char Scroll_Dir;                    //滑动方向 //滑动方向  1：向上滑动  2：向下滑动d

    unsigned char Release_Slow_Dir_Update_Times; // 松手滑动次数
    unsigned char Release_Quick_Dir_Update_Times; //松手滑动次数
    unsigned char Release_Slow_Update_Times;

    volatile unsigned char Quick_Allow_Flage;
    volatile unsigned char Quick_Scroll_Update_Flage;
    QTimer *Slow_Scroll_Timer;   //慢滑定时器
    QTimer *Release_Timeout;
    QTimer *Release_Slow_Dir_Timeout;

    QTimer *Quick_Scroll_Update_Timer;  //快滑定时器
    QTimer *Release_Quick_Dir_Timeout;

    QTimer *Quick_Allow_Timer;

    QTimer *Quick_Scroll_Slot_Timer;

    QLabel *Label_Button_Text;
    QPushButton *Button_Determine;//确定按钮
    QSlider *Slider_p;            //滑动条


private slots:

    void Quick_Scroll_Update_Timer_Slot_Handle();


    void Slow_Scroll_Timer_Handle();
    void Release_TimeOut_Update_GUI();
    void Release_Slow_Dir_TimeOut_Update_GUI();

    void Quick_Scroll_Update_Timer_Handle();
    void Scroll_Quick_TimeOut_Update_GUI();
    void Quick_Scroll_Update_Timer_Flage();
    void Handle_Touch_Value_Event(unsigned short Receive_Diff_Data_Total);

signals:
    void Quick_Scroll_Slot(unsigned short  Receive_Diff_Data_Total);     //声明一个信号，不需要实现


};


#endif // WIDGET_H
