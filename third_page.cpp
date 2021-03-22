#include "third_page.h"
#include <QtGui>
#include <QPainter>
#include <QBrush>
#include <QDebug>
#include <QKeyEvent>
#include <QStringList>
#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <QTime>
#include <QTextCodec>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>


//852
struct address
{
    int x;
    int y;
}Font_Address = {0, 0};



static bool left_btn_press = false;
static bool right_btn_press = false;

static bool is_show_left_btn = false;
static bool is_show_right_btn = false;

static bool left_btn_dblClick = false;
static bool right_btn_dblClick = false;

Third_Page_Widget::Third_Page_Widget(QWidget *parent): QWidget(parent)
{

    this->installEventFilter(this);


    Button_Back = new QPushButton(this);
    QStringList list1;
    list1.append("color:white");                         // 前景色
    list1.append("background-color:rgb(255,255,255)");   // 背景色
    list1.append("border-style:outset");                 // 边框风格
    list1.append("border-width:0px");                    // 边框宽度
    list1.append("border-color:rgb(167,203,74)");        // 边框颜色
    list1.append("border-radius:10px");                  // 边框倒角

    Button_Back->setStyleSheet(list1.join(';'));
    Button_Back->move(718,9);
    Button_Back->resize(75, 20);
    Button_Back->show();


    menuList<<"白菜花";

}

Third_Page_Widget::~Third_Page_Widget()
{

}

void Third_Page_Widget::paintEvent(QPaintEvent *) //重绘函数
{
    qDebug()<<"paintEvent";

    int w = this->width();      //宽
    int h = this->height();     //高
    int Current_FontSize = 0;   //中心字符大小

    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

    QBrush brush(QColor (57, 57 , 57, 255));

    p.setBrush(brush);
    p.drawRect(0, 0, w, h);

    QBrush brush2(QColor (167, 203, 74, 255));
    p.setBrush(brush2);
    QRectF rectangle(8, 32, 120, 4);
    p.drawRoundedRect(rectangle, 2, 2); //上面长条

    QPen pen;

    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);

    Current_FontSize = 28;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(716,  68,   "返回");

    Current_FontSize = 22;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(10,  30,  "周二午餐");

    Current_FontSize = 80;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(Font_Address.x+100,  Font_Address.y+240+Current_FontSize/2,  menuList.at(0));

}


bool Third_Page_Widget::eventFilter(QObject *watched, QEvent *event)
{
    QMouseEvent* hoverEvent = (QMouseEvent*)event;
    QPoint point = hoverEvent->globalPos();


    //qDebug("[qt debug] x=%02x, y=%02x", point.x(), point.y());

    //无
    //qDebug()<< event->type()<<"Qt C##";
    if(event->type() == QEvent::MouseButtonRelease) //
    {
        if(left_btn_press)
        {
            is_show_left_btn = true;
            left_btn_press = false;

            //qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
            //update();
        }

        if(right_btn_press)
        {
            is_show_right_btn = true;
            right_btn_press = false;

            //qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
            //update();
         }

        return true;
    }

    if(event->type() == QEvent::MouseButtonPress) //
    {


        if((point.y() <= 0x13) && (point.y() >= 0x0c))
        {
            qDebug()<<"----press---left_btn_press---x:";
            left_btn_press = true;
        }
        else if((point.y() <= 0x5) && (point.y() > 0x00))
        {
            qDebug()<<"----press---right_btn_press---x:";
            right_btn_press = true;
        }

        return true;
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {

    }
    else if(event->type() == QEvent::MouseMove)  //
    {
        return false;
    }
    return QWidget::eventFilter(watched,event);// 将事件传递给父类
}


