
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
#include "dispNetStatus.h"

//交替显示内容
QString dispContent = "网络中断";
//全屏显示标记
bool fullScreenFlag = false;
//窗口显示命令
DispNetStatus::DispNetStatus(QWidget *parent): QWidget(parent)
{

}

DispNetStatus::~DispNetStatus()
{

}

/*==================================================================================
* 函 数 名： paintEvent
* 参    数：
* 功能描述:  
* 返 回 值： 设备id
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
bool dispToggle = false;
void DispNetStatus::paintEvent(QPaintEvent *)
{   
    QPainter p(this);
    QPen pen;

    if (fullScreenFlag == false)//屏右边有显示，下面显示的进度条短一些 
    {
        //画下面红色的长条
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        QBrush brush3(QColor(255, 0, 0));
        p.setBrush(brush3);
        QRectF rectangle3(50, 420, 600, 4);
        rectangle3.setHeight(60);
        p.drawRoundedRect(rectangle3, 5, 5); 

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        int Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        //p.drawText(120,  465, dispContent);
        p.drawText(300 - dispContent.size()*10,  465, dispContent);
    } else {//全屏长度下面显示的进度条长度长一些
        //画下面红色的长条
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        QBrush brush3(QColor(255, 0, 0));
        p.setBrush(brush3);
        QRectF rectangle3(50, 420, 700, 4);
        rectangle3.setHeight(60);
        p.drawRoundedRect(rectangle3, 5, 5); 

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        int Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(350 - dispContent.size()*10,  465, dispContent);
    }
}

/*==================================================================================
* 函 数 名： set_disp_content
* 参    数：str == 网络断开显示内容；fullScreen == 全屏显示(true)
* 功能描述:  
* 返 回 值： 设置交替显示内容
* 备    注： None
* 作    者： lc
* 创建时间： 2021/8/2
==================================================================================*/
void DispNetStatus::set_disp_content(QString str, bool fullScreen)
{
    dispContent = "";
    dispContent.append(str);

    fullScreenFlag = fullScreen;
}
