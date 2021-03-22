#ifndef WIDGET_BREAKFAST_H
#define WIDGET_BREAKFAST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

#include <QWidget>
#include <QTouchEvent>


struct mymesg
{
    int  mtype;
    char mtext[512];
};



extern struct mymesg Snd_msg;
extern int id;


struct slider_fifter_breakfast
{
    int myType;
    int value;
};


class Widget_Breakfast : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Breakfast(QWidget *parent = 0);
    ~Widget_Breakfast();

protected:
    void paintEvent(QPaintEvent *); //重绘函数

    void paintItems(QPainter *, QStringList*, int index, QFont*); //绘制文字
    void scroll_quick(int speed, int dir); //快速滑动

    bool eventFilter(QObject *watched, QEvent *event);

private:

    int totalItemNum;       //总的item数量
    int selectItemIndex;    //当前选中item索引
    QStringList menuList;   //菜单内容
    int showItemNum;        //显示的菜单个数
    int baseFontSize;       //中心字体大小
    int itemIntervalH;      //菜单垂直间隔
    int itemRealH;          //菜单实际间隔（菜单字符宽度+间隔）

    QFont itemFont;         //菜单字体样式

    //快速滑动参数
    int  scrollDir;   //滑动方向  1：向上滑动  2：向下滑动d
    int maxScrollTime;   //默认滑动最大值，时间时间1S
    int timeCount;      //滑动计时（1秒）
    QTimer *timerScroll;    //0~10滑动一格

    QTimer *timerSlowScroll;

private slots:
    void timeUpdateScroll();
    void timeSlowUpdateScroll();

};

#endif // WIDGET_H
