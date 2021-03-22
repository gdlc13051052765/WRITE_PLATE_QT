#include "widget.h"
#include "widget_breakfast.h"
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



struct mymesg Snd_msg;
int id = 0;

static bool left_btn_press = false;
static bool right_btn_press = false;

static bool is_show_left_btn = false;
static bool is_show_right_btn = false;

static bool left_btn_dblClick = false;
static bool right_btn_dblClick = false;

Widget_Breakfast::Widget_Breakfast(QWidget *parent): QWidget(parent)
{

    this->installEventFilter(this);

    menuList<<"海参烩猪筋" <<"鲜蛏萝卜丝羹" << "海带猪肚丝羹" << "淡菜虾子汤" << "鱼翅螃蟹羹" << "鲨鱼皮鸡汁羹" << "辘轳锤" << "鱼肚煨火腿"
            << "淡菜虾子汤" << "鲍鱼烩珍珠菜" <<"辣子鸡丁  "
            <<"东坡肘子  "
            <<"豆瓣鲫鱼  "
            <<"口袋豆腐  "
            <<"酸菜鱼    "
            <<"夫妻肺片  "
            <<"蚂蚁上树  "
            <<"叫化鸡    "
            <<"茄汁鱼卷  "
            <<"鱼香肉丝  "
            <<"干煸冬笋  "
            <<"魔芋烧鸭  "
            <<"锅贴鱼片  "
            <<"麻辣肉丁  "
            <<"鱼香茄饼  "
            <<"冬菜肉末  "
            <<"粉蒸鸡    "
            <<"酿茄子    "
            <<"酿豆腐    "
            <<"梅菜扣肉  "
            <<"客家盐焗鸡"
            <<"红糟排骨  "
            <<"清风送爽  "
            <<"炊太极虾  "
            <<"广式烧填鸭"
            <<"池塘莲花  "
            <<"佛跳墙    "
            <<"醉排骨    "
            <<"荔枝肉    "
            <<"扳指干贝  "
            <<"尤溪卜鸭  "
            <<"七星鱼丸汤"
            <<"软溜珠廉鱼"
            <<"龙身凤尾虾"
            <<"清炖全鸡  "
            <<"油爆双脆  "
            <<"香露全鸡  "
            <<"醉蚌肉    ";

    scrollDir  = 0;   //滑动方向  1：向上滑动  2：向下滑动d
    maxScrollTime  = 100;   //默认滑动最大值，时间时间1S
    timeCount = 0;      //滑动计时（1秒）

    totalItemNum = menuList.length();
    selectItemIndex = (totalItemNum/2);

    showItemNum = 7;    //界面显示菜单个数，最好为奇数
    baseFontSize = 38;  //中心字符大小
    itemIntervalH = 15;  //菜单垂直间隔

    //设置字体样式

    //设置文字字体
    itemFont.setFamily("宋体");
    //设置文字大小为50像素
    itemFont.setPixelSize(50);
    //设置文字为粗体
    itemFont.setBold(true);             //封装的setWeight函数
    //设置文字为斜体
    //itemFont.setItalic(true);           //封装的setStyle函数
    //设置文字大小
    itemFont.setPointSize(baseFontSize);
    //设置文字倾斜
    //itemFont.setStyle(QFont::StyleItalic);
    //设置文字粗细//enum Weight 存在5个值
    itemFont.setWeight(QFont::Light);

    QFontMetrics fm(itemFont);
    qDebug() << fm.height();            //获取文字高度
    qDebug() << fm.maxWidth();          //获取文字宽度

    itemRealH = fm.height()+itemIntervalH;  //计算实际间隔

    timerScroll = new QTimer(this);

    connect(timerScroll, SIGNAL(timeout()), this, SLOT(timeUpdateScroll()));

    timerSlowScroll = new QTimer(this);
    connect(timerSlowScroll, SIGNAL(timeout()), this, SLOT(timeSlowUpdateScroll()));
}

Widget_Breakfast::~Widget_Breakfast()
{

}


#define MXA_FIFTER_NUM   5
#define MAX_FIFTER_TIM   100  //MS
#define MAX_SPEED_THRESHOLD     20
#define MAX_SLOW_THREAHOLD      3

static int fifter_count = 0;
static struct slider_fifter_breakfast  Fifter_Breakfast[10] ;

void Widget_Breakfast::paintEvent(QPaintEvent *)
{
    unsigned char Identifier_Write = 0;

    unsigned char Version_Write = 0;

    unsigned short Price_Write = 0;

    int w = this->width();      //宽
    int h = this->height();     //高
    int curPos = 0;
    int centerPos_y = (h/2);    //计算窗体中心位置

    QPainter p(this);
    //p.setAntiAlias(true);

    //绘制背景渐变色
    QLinearGradient grad(0.5, 0, 0.5, 1.0);
    grad.setColorAt(0.1, QColor(38, 41, 100, 255));
    grad.setColorAt(0.3, QColor(38, 41, 100, 220));
    grad.setColorAt(0.5, QColor(38, 41, 180, 200));
    grad.setColorAt(0.7, QColor(38, 41, 100, 220));
    grad.setColorAt(0.9, QColor(38, 41, 100, 255));
    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush brush(grad);

    p.setBrush(brush);
    p.drawRect(0, 0, w, h);

    QPen pen;

    //绘制内容
    for(int i=0; i<showItemNum; i++)
    {
        curPos = selectItemIndex + (i-(showItemNum/2));

        if((curPos < 0)||(curPos >= (totalItemNum)))    //边沿检测
        {
            continue;
        }

        if(i == (showItemNum/2))
        {
            pen.setColor(QColor(0,153,255));
            p.setPen(pen);
        }
        else
        {
            pen.setColor(QColor(255,255,255));
            p.setPen(pen);
        }

        //计算字体大小
        itemFont.setPointSize(baseFontSize - abs((i-(showItemNum/2)))*7);

        QFontMetrics fms(itemFont);
        QRect rec = fms.boundingRect(menuList[curPos]);

        //绘制菜单
        p.setFont(itemFont);

        //绘制
        p.drawText(w/2-(rec.width()/2 ), centerPos_y+(rec.height()/2 )+((i-(showItemNum/2))*(itemRealH)), menuList[curPos]);
    }



    itemFont.setPointSize(20);

    //绘制菜单
    p.setFont(itemFont);

    qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
    if(is_show_left_btn)
    {
        is_show_left_btn = false;

        pen.setColor(Qt::green);
        p.setPen(pen);
        p.translate(650, 240);
        p.drawText(0, 0, "左键按下");

        this->close();

        qDebug()<<"breakfast over....";


        Widget *w_breakfast_back = new Widget();


        w_breakfast_back->resize(800, 480);
        w_breakfast_back->setWindowFlags(Qt::FramelessWindowHint);
        w_breakfast_back->setAttribute(Qt::WA_AcceptTouchEvents, true);




        QGraphicsScene *scene_bre_back = new QGraphicsScene;

        QGraphicsProxyWidget *g_bre_back = scene_bre_back->addWidget(w_breakfast_back);
        g_bre_back->setRotation(90);

        QGraphicsView *view_bre_bac = new QGraphicsView(scene_bre_back);

        view_bre_bac->setWindowFlags(Qt::FramelessWindowHint);
        view_bre_bac->setWindowFlags(Qt::FramelessWindowHint);//无边框
        view_bre_bac->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view_bre_bac->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view_bre_bac->setFrameStyle(QFrame::NoFrame);//无Frame边框

        view_bre_bac->setInteractive(true);  //设置场景内的是否允许交互
        view_bre_bac->resize(480, 800);
        view_bre_bac->show();

    }
    else if(is_show_right_btn)
    {
        Snd_msg.mtype = 66;
        Snd_msg.mtext[0] = 0x01;    //  1 表示 允许写卡      2 表示   停止写卡
        Snd_msg.mtext[1] = Identifier_Write;
        Snd_msg.mtext[2] = Version_Write<<4;
        Snd_msg.mtext[2] |= (Price_Write>>10);
        Snd_msg.mtext[3] = Price_Write>>2;
        Snd_msg.mtext[4] = ((Price_Write>>12)<<6);

        //flage = 0时，当消息队列满时，msgsnd将会阻塞，直到消息能写进消息队列或者消息队列被删除。
        if(msgsnd(id, (void *)&Snd_msg,  512,  IPC_NOWAIT) == -1)
        {
            qDebug()<<"send msg error \n";
        }
        else
        {
            qDebug()<<"send msg succeed\n";
        }

        is_show_right_btn = false;
        //p.rotate(90);

        pen.setColor(Qt::red);
        p.setPen(pen);
        p.translate(650, 240);
        p.drawText(0, 0, "右键按下");
    }

    if(left_btn_dblClick)
    {
        left_btn_dblClick = false;

        pen.setColor(Qt::green);
        p.setPen(pen);
        p.translate(650, 300);
        p.drawText(0, 0, "左键双击");
    }
    else if(left_btn_dblClick)
    {
        left_btn_dblClick = false;

        pen.setColor(Qt::red);
        p.setPen(pen);
        p.translate(650, 300);
        p.drawText(0, 0, "右键双击");
    }

}

static int old_pos_y = 0;
static int is_speed_scroll = 0; //1 start
static int scroll_dir = 0;  //1 up  2 down

static int inv_scroll_value = 0;

static int timer_count = 0;
//speed 1~10
void Widget_Breakfast::scroll_quick(int speed, int dir)
{
    timerScroll->start(speed);
    timer_count = 0;
}

void Widget_Breakfast::timeSlowUpdateScroll()
{
    int speed_count = 0;
    int is_release = 0;
    qDebug()<<"entry timeSlowUpdateScroll";

    for(int i=0; i<fifter_count; i++)
    {
        if(Fifter_Breakfast[i].myType == QEvent::MouseButtonRelease)
        {
            is_release = 0x01; //speed
        }
    }

    qDebug("qt debug] is_release=%d, fifter_count=%d", is_release, fifter_count);

    if(is_release)
    {
        //speed
        for(int i=1; i<fifter_count; i++)
        {
            //song kai
            if(abs(Fifter_Breakfast[i-1].value-Fifter_Breakfast[i].value) > MAX_SPEED_THRESHOLD)
            {
                speed_count++;
            }
        }

        if(speed_count > 0)
        {
        //start speed scroll
        }
        else
        {
        //slow scroll
        }
    }
    else
    {
        if(fifter_count > 2)
        {
            if(Fifter_Breakfast[fifter_count-1].value > Fifter_Breakfast[fifter_count].value )
            {
                if(selectItemIndex < (totalItemNum-1))
                {
                    selectItemIndex++;
                }
                qDebug()<<"key down";
            }
            else
            {
                if(selectItemIndex > 0)
                {
                    selectItemIndex--;
                }
                qDebug()<<"key up";
            }

        update();
        }
    }

    fifter_count = 0;

    timerSlowScroll->stop();
}

void Widget_Breakfast::timeUpdateScroll()
{
    if(scroll_dir == 0x01)
    {
        if(selectItemIndex < (totalItemNum-1))
        {
            selectItemIndex++;
        }
        qDebug()<<"key down";
    }
    else if(scroll_dir == 0x02)
    {
        if(selectItemIndex > 0)
        {
            selectItemIndex--;
        }
        qDebug()<<"key up";
    }


    update();
    timer_count += inv_scroll_value;

    if(timer_count >= 60)
    {
        is_speed_scroll = 0;
        timerScroll->stop();
    }
}

/*
void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
*/

static QTimer timer_close;
static int touch_press_value = 0;

bool Widget_Breakfast::eventFilter(QObject *watched, QEvent *event)
{
    QMouseEvent* hoverEvent = (QMouseEvent*)event;
    QPoint point = hoverEvent->globalPos();

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    qDebug("[qt debug] x=%02x, y=%02x", point.x(), point.y());

    //无
    qDebug()<< event->type()<<"Qt C##";
    if(event->type() == QEvent::MouseButtonRelease)
    {
        old_pos_y = 0xee;   //clear

        if(is_speed_scroll == 0x01)
        {
            scroll_quick(inv_scroll_value*8, scroll_dir);
        }


        if(left_btn_press)
        {
            is_show_left_btn = true;
            left_btn_press = false;

            qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
            update();
        }

        if(right_btn_press)
        {
            is_show_right_btn = true;
            right_btn_press = false;

            qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
            update();
         }

        return true;
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        old_pos_y = point.y();

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

        touch_press_value =point.y();
        return true;
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        qDebug()<<"----press---MouseButtonDblClick---x:";
        //update();
    }
    else if(event->type() == QEvent::MouseMove)
    {
        if(abs(touch_press_value - point.y()) > 3 )
        {
            qDebug()<<"----press---clear---touch_press_value:"<<touch_press_value;
            left_btn_press = false;
            right_btn_press = false;
        }

        qDebug()<<"----mouseMove---globalpos---x:"<<mouseEvent->globalPos().x()<<"  y:"<<mouseEvent->globalPos().y();
        qDebug()<<"----mouseMove---pos---x:"<<mouseEvent->pos().x()<<"  y:"<<mouseEvent->pos().y();

        if(abs(old_pos_y - point.y()) > 3) //h speed
        {
            is_speed_scroll = 0x01;
            qDebug()<<"speed high\n";

            inv_scroll_value =abs(old_pos_y - point.y());

            if(old_pos_y > point.y())
            {
                scroll_dir = 0x01;
            }
            else
            {
                scroll_dir = 0x02;
            }

            old_pos_y = point.y();
        }
        else if(abs(old_pos_y - point.y()) > 1 ) //low speed
        {
            is_speed_scroll = 0x00;

            qDebug()<<"speed low"<<"old_pos_y="<<old_pos_y<<"new y="<<point.y()<<"\n";
            if(old_pos_y > point.y())
            {
                if(selectItemIndex < (totalItemNum-1))
                {
                    selectItemIndex++;
                }
                qDebug()<<"key down";
            }
            else
            {
                if(selectItemIndex > 0)
                {
                    selectItemIndex--;
                }
                qDebug()<<"key up";
            }

            old_pos_y = point.y();
            repaint();
        }
        return false;
    }
    return QWidget::eventFilter(watched,event);// 将事件传递给父类
}



