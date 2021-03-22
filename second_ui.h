#ifndef SECOND_UI_H
#define SECOND_UI_H


#include <QWidget>
#include <QTouchEvent>
#include <QPushButton>
#include <QLabel>
#include <QSlider>




class Second_Page_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Second_Page_Widget(QWidget *parent = 0);
    ~Second_Page_Widget();

protected:
    void paintEvent(QPaintEvent *); //重绘函数

    void paintItems(QPainter *, QStringList*, int index, QFont*); //绘制文字
    void scroll_quick(int speed, int dir); //快速滑动

    bool eventFilter(QObject *watched, QEvent *event);

private:


    volatile int Add_Step_By_Step;        //变化数
    //int Add_Step_By_Step_Flage;  //变化方向
    int Is_Speed_Low_Scroll;     //慢滑动标志位

    int Last_Update_GUI;         //上电和最终初始化GUI 标志位


    int totalItemNum;       //总的item数量
    int selectItemIndex;    //当前选中item索引
    int showItemNum;        //显示的菜单个数
    QStringList menuList;   //菜单内容



    int Select_Show_Menu_Spell_Index;    //当前选中拼音首字母索引
    int Total_Show_Menu_Spell_Num;        //菜单拼音首字母个数
    QStringList Menu_Spell_Display_List; //显示菜单拼音首字母
    QStringList Menu_Spell_List;    //菜单拼音首字母


    int baseFontSize;       //中心字体大小
    int itemIntervalH;      //菜单垂直间隔
    int itemRealH;          //菜单实际间隔（菜单字符宽度+间隔）

    QFont itemFont;         //菜单字体样式



    //快速滑动参数
    int  scrollDir;   //滑动方向  1：向上滑动  2：向下滑动d
    int maxScrollTime;   //默认滑动最大值，时间时间1S
    int timeCount;      //滑动计时（1秒）

    int Touchpad_Flage; //

    int Add_Times;
    int Touch_Allow_Flage;


    QTimer *timerScroll;
    QTimer *Touchpad_Time;
    QTimer *Release_Timeout;
    QTimer *Scroll_Quick_TimeOut;

    QTimer *Touch_Allow_Timeout;

    QLabel *Label_Button_Text;

    QPushButton *Button_Determine;//确定按钮
    QPushButton *Button_Back;     //取消按钮
    QSlider *Slider_p;            //滑动条


private slots:
    void timeUpdateScroll();
    void Touchpad_Time_Clear_Flage();
    void Release_TimeOut_Update_GUI();
    void Scroll_Quick_TimeOut_Update_GUI();
    void Touch_Allow_Timeout_Handle();

};


#endif // WIDGET_H
