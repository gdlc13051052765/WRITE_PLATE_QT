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
#include "led.h"


#define   SLOW_SCROLL_RELEASED_UPDATE_MS    50

#define   SPEED_SORT          1     //高速低速分界线

#define   FRAME               7     //FRAME

#define   FONT_STEP_VALUE     2

struct timeval tpstart,tpend;
float timeuse;


static int  Address_Font[15][7] = {
/*向下翻页*/
    93,   168,    260,   340,   404,    451,      0,      //        0

    82,   156,    248,   332,   400,    450,      0,      //        1

    71,   144,    236,   324,   396,    449,      0,      //        2

    60,   132,    224,   316,   392,    448,      0,      //        3    只显示6个

    56,   124,    213,   305,   381,    438,    476,      //        4

    52,   116,    202,   294,   370,    428,    468,      //        5

    48,   108,    191,   283,   359,    418,    460,      //        6


    44,   104,    180,   272,   348,    408,    452,      //        中间    7


/*向上翻页*/
    32,    93,    168,   260,   340,    404,    451,      //        8

    20,    82,    156,   248,   332,    400,    450,      //        9

     8,    71,    144,   236,   324,    396,    449,      //       10


     0,    60,   132,    224,   316,   392,     448,      //        11 只显示6个

     0,    56,   124,    213,   305,   381,     438,      //        12

     0,    52,   116,    202,   294,   370,     428,      //        13

     0,    48,   108,    191,   283,   359,     418,      //        14

};

static int  Address_Spell[7] = { 130,   170,    210,   250,   290,    330,    370};

static bool left_btn_press = false;
static bool right_btn_press = false;

static bool is_show_left_btn = false;
static bool is_show_right_btn = false;

static bool left_btn_dblClick = false;
static bool right_btn_dblClick = false;

Widget::Widget(QWidget *parent): QWidget(parent)
{
    Total_Page = 3;
    Current_Page = 0;
    Widget_Page_Switch = 0;

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
    Button_Determine->hide();          //确定按键


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
    Button_Cancel->hide();             //取消按键


    Scroll_Times = 0;   //滑动次数
    Scroll_Dir = 0;     //慢滑方向

    Release_Slow_Dir_Update_Times = 0; // 松手滑动次数
    Release_Quick_Dir_Update_Times = 0; //松手滑动次数
    Release_Slow_Update_Times = 0;
    String_Display = "";

    //Quick_Allow_Flage = 1;  //

    //Released_Flage = 1;

    //High_Scroll_Flage = 0;

    //Is_Speed_Low_Scroll = 0;
    Last_Update_GUI = 1;

    Touchpad_Flage = 0;

    Quick_Scroll_Period = 0;
    Scroll_Times = 0;

    Slider_p =new QSlider(this);
    Slider_p->setOrientation(Qt::Vertical);
    Slider_p->setGeometry (752, 21, 6, 373);
    Slider_p->setMinimum(0);  // 最小值
    Slider_p->setMaximum(0);  // 最大值
    //pSlider->setSingleStep(373/nSingleStep);  // 步长

    //margin-left:-3pxmargin-left 就是设置标签的左外边距
    Slider_p->setStyleSheet("QSlider::groove:vertical{ height: 373px; width: 4px;  border-radius:2px;  left: 20px;        right: 20px;         background: #707070;}\
                             QSlider::handle:vertical{ height: 39px;  width: 6px;  border-radius:3px;  margin-left:-1px;  margin-right:-1px;   background: #A7CB4A;}");

    if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
    {
        Slider_p->setValue(totalItemNum-1-selectItemIndex);
    }
    Slider_p->hide();

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
    qDebug() << fm.height()<<"16619851335";            //获取文字高度  105   327
    qDebug() << fm.maxWidth()<<"16619851335";          //获取文字宽度

    //itemRealH = fm.height()*20/19 + itemIntervalH;  //计算实际间隔   //菜单实际间隔（菜单字符宽度+间隔）
    itemRealH = 0;

    //Motor_Shake_Slot_Timer = new QTimer(this);          //  震动电机      OK
    //connect(Motor_Shake_Slot_Timer, SIGNAL(timeout()), this, SLOT(Motor_Shake_Slot_Timer_Handler()));


    Quick_Scroll_Slot_Timer = new QTimer(this);       //  快速滑动
    connect(Quick_Scroll_Slot_Timer, SIGNAL(timeout()), this, SLOT(Quick_Scroll_Update_Timer_Slot_Handle()));


    Release_Quick_Dir_Timeout = new QTimer(this);     //  快滑松手  快滑滑动松手根据前一刻方向 来滑最后一格
    connect(Release_Quick_Dir_Timeout, SIGNAL(timeout()), this, SLOT(Scroll_Quick_TimeOut_Update_GUI()));




    Release_Timeout = new QTimer(this);          //  慢滑松手  靠近最近的那个    OK
    connect(Release_Timeout, SIGNAL(timeout()), this, SLOT(Release_TimeOut_Update_GUI()));

    Release_Slow_Dir_Timeout = new QTimer(this); // 慢滑松手   根据方向    来决定滑动  OK
    connect(Release_Slow_Dir_Timeout, SIGNAL(timeout()), this, SLOT(Release_Slow_Dir_TimeOut_Update_GUI()));


    Slow_Scroll_Timer = new QTimer(this);        // 慢滑滑动 处理函数         OK
    connect(Slow_Scroll_Timer, SIGNAL(timeout()), this, SLOT(Slow_Scroll_Timer_Handle()));

    qDebug() << fm.height()<<"16619851335+";            //获取文字高度  105   327
    qDebug() << fm.maxWidth()<<"16619851335+";          //获取文字宽度


    this->setObjectName("QTimer");

    QObject::connect(this,SIGNAL(Quick_Scroll_Slot(unsigned short)), this,SLOT(Handle_Touch_Value_Event(unsigned short)));

}

Widget::~Widget()
{

}


#define MXA_FIFTER_NUM   5
#define MAX_FIFTER_TIM   100  //MS
#define MAX_SPEED_THRESHOLD     20
#define MAX_SLOW_THREAHOLD      3


static int fifter_count = 0;
//static struct slider_fifter  mFifter[10] ;

void Widget::paintEvent(QPaintEvent *)
{
    //gettimeofday(&tpstart,NULL);
    int w = this->width();      //宽
    int h = this->height();     //高

    int Current_FontSize = 0;  //中心字符大小
    int curPos = 0;
    volatile int Write_Post = 0;

    //qDebug()<<"----press--is_show_left_btn:"<<w<<"is_show_right_btn"<<h;
    if(Current_Page == 0)
    {
        QPainter p(this);

        if(Widget_Page_Switch == 0)
        {
            Widget_Page_Switch = 1;
            menuList.clear();
            menuList<<"周一早餐"<<"周一午餐"<<"周二早餐"<<"周二午餐"<<"周三早餐"<<"周三午餐" <<"周四早餐" <<"周四午餐"<< "周五早餐"<<"周五晚餐";
            totalItemNum = menuList.length();
    
            selectItemIndex = totalItemNum/2;
        
            showItemNum = 7;    //界面显示菜单个数,最好为奇数
            baseFontSize = 70;  //中心字符大小
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;

            Scroll_Times = 0;   //滑动次数
            Scroll_Dir = 0;     //慢滑方向

            Release_Slow_Dir_Update_Times = 0; // 松手滑动次数
            Release_Quick_Dir_Update_Times = 0; //松手滑动次数
            Release_Slow_Update_Times = 0;


            Button_Cancel->hide();
            Button_Determine->show();  //确定

            Slider_p->setMinimum(0);               // 最小值
            Slider_p->setMaximum(totalItemNum-1);  // 最大值
            Slider_p->show();          //滑条

        }
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

        QBrush brush(QColor (57, 57 , 57, 255));

        p.setBrush(brush);
        p.drawRect(0, 0, w-90, h);


        QBrush brush1(QColor (0, 0 , 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90, h);

        if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
        {
            Slider_p->setValue(totalItemNum-1-selectItemIndex);
        }

        QPen pen;

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716,  440,  "确定");
        qDebug()<<"(Current_Page == 0)";

    }
    else if(Current_Page == 1)
    {
        if(Widget_Page_Switch == 0)
        {
            Widget_Page_Switch = 1;
            menuList.clear();
            menuList<<"白菜花"<<"白菜粉条"<<"白菜蛋花"<<"白菜肉片"<<"白菜黄瓜"<<"白菜萝卜"<<"菜花粉条"<<"菜花黄瓜"<<"菜花肉片"
                    <<"菜花"
                    <<"蛋花粉条"
                    <<"蛋花白菜"
                    <<"豆腐粉条"
                    <<"豆腐蛋花"
                    <<"豆腐白菜"
                    <<"干锅茄子"
                    <<"干锅白菜"
                    <<"干锅豆角"
                    <<"干锅肥肠"
                    <<"锅包肉"
                    <<"黄瓜白菜"
                    <<"黄瓜西红柿"
                    <<"黄瓜肉片"
                    <<"口水鸡"
                    <<"蓝莓土豆"
                    <<"蓝白菜"
                    <<"麻婆豆腐"
                    <<"麻婆土豆"
                    <<"麻婆白菜"
                    <<"牛奶白菜"
                    <<"牛奶土豆"
                    <<"片汤"
                    <<"烧白菜"
                    <<"烧茄子"
                    <<"烧土豆"
                    <<"酸菜鱼"
                    <<"甜玉米"
                    <<"甜馒头"
                    <<"西红柿 "
                    <<"西红柿鸡蛋"
                    <<"西红柿牛腩"
                    <<"西红柿菜花";

            Menu_Spell_List<<"B"<<"B"<<"B"<<"B"<<"B"<<"B"<<"C"<<"C"<<"C"<<"C"<<"D"<<"D"<<"D"<<"D"<<"D"
                           <<"G"<<"G"<<"G"<<"G"<<"G"<<"H"<<"H"<<"H"<<"K"<<"L"<<"L"<<"M"<<"M"<<"M"<<"N"
                           <<"N"<<"P"<<"S"<<"S"<<"S"<<"S"<<"T"<<"T"<<"X"<<"X"<<"X"<<"X";

            Menu_Spell_Display_List<<"B"<<"C"<<"D"<<"G"<<"H"<<"K"<<"L"<<"M"<<"N"<<"P"<<"S"<<"T"<<"X";

            totalItemNum = menuList.length();
            selectItemIndex = totalItemNum/2;

            Total_Show_Menu_Spell_Num = Menu_Spell_Display_List.length();
            //Select_Show_Menu_Spell_Index = Menu_Spell_Display_List.indexOf(Menu_Spell_List.at(selectItemIndex));

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

        QBrush brush(QColor (57, 57 , 57, 255));

        p.setBrush(brush);
        p.drawRect(0, 0, w-90, h);


        QBrush brush1(QColor (0, 0 , 0, 255));
        p.setBrush(brush1);
        p.drawRect(w-90, 0, 90, h);                 //右半边矩形

        QBrush brush2(QColor (167, 203, 74, 255));
        p.setBrush(brush2);
        QRectF rectangle(5, 32, 690, 4);
        p.drawRoundedRect(rectangle, 2, 2);         //上面长条


        p.setBrush(brush);                         //假滑条的棍
        QRectF rectangle1(750, 100, 16, 280);
        p.drawRoundedRect(rectangle1, 8, 8);
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
        QBrush brush3(QColor (167, 203, 74, 255));
        p.setBrush(brush3);
        p.drawEllipse(750, 225, 30, 30);           //假滑条的圆

        QPen pen;

        pen.setColor(QColor(255, 255, 255));
        p.setPen(pen);
        Current_FontSize = 28;
        itemFont.setPointSize(Current_FontSize);
        p.setFont(itemFont);
        p.drawText(716,  440,  "确定");
        p.drawText(716,  68,   "返回");
        p.drawText(320,  30,   "周二午餐");

        //qDebug()<<"(Current_Page == 1);";
    }

    if(Current_Page == (Total_Page-1))  //   第2页
    {
        if(Widget_Page_Switch == 0)
        {
            Widget_Page_Switch = 1;


            String_Display.clear();
            String_Display = menuList.at(selectItemIndex);
            //qDebug()<<"selectItemIndex = "<<selectItemIndex;

            menuList.clear();
            totalItemNum = 0;
            selectItemIndex = 0;

            QPainter p(this);
            p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

            QBrush brush(QColor (57, 57 , 57, 255));

            p.setBrush(brush);
            p.drawRect(0, 0, w, h);


            Slider_p->hide();
            Button_Determine->hide();
            Button_Cancel->show();


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
            p.drawText(100,  240+Current_FontSize/2,  String_Display);
        }
    }


    //  0  1   界面刷新
    if((Current_Page == 0)||(Current_Page == 1))
    {
        qDebug()<<"Current_Page 0  1 selectItemIndex = "<<selectItemIndex<<Last_Update_GUI;

        if(Current_Page == 0)
        {
            //qDebug()<<"Current_Page == 0); set value ";
            if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
            {
                Slider_p->setValue(totalItemNum-1-selectItemIndex);
            }
        }

        if(Current_Page == 1)
        {
            QPainter p(this);
            QPen pen;
            pen.setColor(QColor(212, 212, 212));
            p.setPen(pen);
            itemFont.setPointSize(20);
            p.setFont(itemFont);
            //qDebug()<<"Current_Page == 1); set value ";

            Select_Show_Menu_Spell_Index = Menu_Spell_Display_List.indexOf(Menu_Spell_List.at(selectItemIndex));

            for(int i=0; i<showItemNum; i++)
            {          //Select_Show_Menu_Spell_Index 1           //3
                curPos = Select_Show_Menu_Spell_Index + (i-(showItemNum/2));

                if((curPos < 0)||(curPos >= (Total_Show_Menu_Spell_Num)))    //边沿检测
                {
                    continue; //continue 语句只结束本次循环,而不是终止整个循环
                }
                p.drawText(755,  Address_Spell[i],  Menu_Spell_Display_List[curPos]);
            }
        }

    if((Last_Update_GUI == 1)&&(Add_Step_By_Step==FRAME))
    {
        showItemNum = 7;
        QPainter p(this);
        QPen pen;
        //qDebug()<<"Last_Update_GUI = " <<Last_Update_GUI<<"Add_Step_By_Step"<<Add_Step_By_Step;
        //绘制内容      界面显示菜单个数,最好为奇数  这里是7

        for(int i=0; i<showItemNum; i++)
        {
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

            //计算字体大小              70
            Current_FontSize = baseFontSize - abs((i-(showItemNum/2)))*16;
            itemFont.setPointSize(Current_FontSize);

            //QFontMetrics fms(itemFont);
            //QRect rec = fms.boundingRect(menuList[curPos]);

            //qDebug()<<rec.height()<<rec.width()<<"123-456-789";
            //绘制菜单
            p.setFont(itemFont);
            if(Current_Page == 0)
            {
                Write_Post =  Address_Font[6][i];
            }
            if(Current_Page == 1)
            {
                Write_Post =  Address_Font[6][i]+20;
            }
            //qDebug()<<"Write_Post" <<Write_Post<<"16619851335";

            //p.drawText(  10,   centerPos_y  +  (Current_FontSize/2 )+(i-(showItemNum/2))*(itemRealH) ,  menuList[curPos]);
            p.drawText(  10,  Write_Post,  menuList[curPos]);
        }
        Last_Update_GUI = 1;

        return;
    }
    else
    {
        //qDebug()<<"Last_Update_GUI = " <<Last_Update_GUI;   向上翻页
        //                      7                          7 * 2
        if((Add_Step_By_Step>FRAME)&&(Add_Step_By_Step<=(FRAME*2)))  //   8   9  10   11  12  13  14
        {
            QPainter p(this);
            QPen pen;
            int j = Add_Step_By_Step-FRAME;
//                                      14  -    3
            if(Add_Step_By_Step > ((FRAME*2)- ((FRAME-1)/2)))//  12 13 14
            {
                if((selectItemIndex + 4) <= (totalItemNum-1))    //边沿检测  //
                {
                    //计算字体大小   Current_FontSize =  16+(((FRAME-1)/2)- Add_Step_By_Step) *2;  //baseFontSize - abs((i-(showItemNum/2)))*16;
                    Current_FontSize =  14+ ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*2);  //baseFontSize - abs((i-(showItemNum/2)))*16;
                    itemFont.setPointSize(Current_FontSize);
                    p.setFont(itemFont);
                    pen.setColor(QColor(77+5*j,77+5*j,77+5*j));
                    p.setPen(pen);
                    if(Current_Page == 0)
                    {
                        Write_Post = 476 - ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*8);
                    }
                    if(Current_Page == 1)
                    {
                        Write_Post = 476 - ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*8)+20;
                    }
                    //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                    if((Write_Post >0)&&(Write_Post <480))
                    {
                        p.drawText(  10,  Write_Post,  menuList[selectItemIndex + 4]);
                    }

                }
            }
            {
                //qDebug()<<" Add_Step_By_Step...."<<Add_Step_By_Step<<"j = "<<j;                //绘制内容      界面显示菜单个数,最好为奇数  这里是7
                //qDebug()<<" showItemNum " <<showItemNum;
                //qDebug()<<"Add_Step_By_Step ?+? = "<<Add_Step_By_Step;

                for(int i=0; i<showItemNum; i++)
                {
                    curPos = selectItemIndex + (i-(showItemNum/2));
                    //qDebug()<<" curPos " <<curPos<<"selectItemIndex"<<selectItemIndex;


                    if((curPos < 0)||(curPos >= (totalItemNum)))    //边沿检测
                    {
                        continue; //continue 语句只结束本次循环,而不是终止整个循环
                    }

                    if(i == 0)
                    {   //                     (( 7*2    - (((7 -  1)/2))   11
                        if(Add_Step_By_Step < ((FRAME*2)- ((FRAME-1)/2)))  //   8   9   10
                        {
                            //printf("i = %d\r\n", i );
                            //计算字体大小
                            Current_FontSize =  22 - FONT_STEP_VALUE*j;
                            itemFont.setPointSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(79-5*j,79-5*j,79-5*j));
                            p.setPen(pen);
                            if(Current_Page == 0)
                            Write_Post = Address_Font[Add_Step_By_Step][i];
                            if(Current_Page == 1)
                            Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                            //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                            if((Write_Post >0)&&(Write_Post <480))
                            {
                                p.drawText(  10,  Write_Post,  menuList[selectItemIndex - 3]);
                            }
                         }

                    }
                    else if(i == 1)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小
                        Current_FontSize =  38 - FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(119-5*j,119-5*j,119-5*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;

                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 2)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小
                        Current_FontSize =  54 - FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(136-3*j,136-3*j,136-3*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else if(i == 3)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小
                        Current_FontSize =  70 - FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);                //   //朝下滑     6     //FRAME
                        Last_Update_GUI = 0;
                        pen.setColor(QColor(252-15*j,252-15*j,252-15*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;

                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 4)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小
                        Current_FontSize =  54 + FONT_STEP_VALUE*j ;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(136+15*j,136+15*j,136+15*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;

                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 5)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小
                        Current_FontSize =  38 + FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(116+3*j,116+3*j,116+3*j));
                        p.setPen(pen);

                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;

                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 6)
                    {
                        //printf("i = %d\r\n", i );
                        //计算字体大小

                        Current_FontSize =  22 + FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(76+5*j,76+5*j,76+5*j));
                        p.setPen(pen);                        
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;

                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                }

            }
        }//                         7
        else if((Add_Step_By_Step<FRAME) && (Add_Step_By_Step>=0)) // int j = Add_Step_By_Step 6  5   4   3   2   1  0
        {
            QPainter p(this);
            QPen pen;
            int j = FRAME - Add_Step_By_Step;
            {
                        //         3
                if(Add_Step_By_Step < ((FRAME-1)/2))  // 显示6 行之后继续下拉上头新增了一行  0    1    2
                {
                    if((selectItemIndex - 4) >= 0)    //边沿检测
                    {
                        Current_FontSize =  14+(((FRAME-1)/2)- Add_Step_By_Step) *2;  //baseFontSize - abs((i-(showItemNum/2)))*16;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(77+5*j,77+5*j,77+5*j));
                        p.setPen(pen);


                        if(Current_Page == 0)
                        Write_Post =  8 +(((FRAME-1)/2) - Add_Step_By_Step)*12;
                        if(Current_Page == 1)
                        Write_Post =  8 +(((FRAME-1)/2) - Add_Step_By_Step)*12+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;

                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[selectItemIndex - 4]);
                        }
                    }

                }

                //qDebug()<<"Add_Step_By_Step ?-? = "<<Add_Step_By_Step;  //6   5   4   3
                //绘制内容      界面显示菜单个数,最好为奇数  这里是7
                for(int i=0; i<showItemNum; i++)
                {
                    curPos = selectItemIndex + (i-(showItemNum/2));

                    if((curPos < 0)||(curPos >= (totalItemNum)))    //边沿检测
                    {
                        continue; //continue 语句只结束本次循环,而不是终止整个循环
                    }

                    if(i == 0)
                    {
                        //计算字体大小
                        Current_FontSize =  22 + FONT_STEP_VALUE*j;  //baseFontSize - abs((i-(showItemNum/2)))*16;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(79+5*j,79+5*j,79+5*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }

                    }
                    else if(i == 1)
                    {
                        //计算字体大小
                        Current_FontSize =  38 + FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);

                        pen.setColor(QColor(119+2*j,119+2*j,119+2*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 2)
                    {
                        //计算字体大小
                        Current_FontSize =  54 + FONT_STEP_VALUE*j ;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(135+15*j,135+15*j,135+15*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];

                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else if(i == 3)
                    {
                        //计算字体大小
                        Current_FontSize =  70 - FONT_STEP_VALUE*j;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(255-15*j,255-15*j,255-15*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 4)
                    {
                        //计算字体大小
                        Current_FontSize =  54 - FONT_STEP_VALUE*j ;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(135-2*j,135-2*j,135-2*j));
                        p.setPen(pen);
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else if(i == 5)
                    {
                        //计算字体大小
                        Current_FontSize =  38 - FONT_STEP_VALUE*j ;
                        itemFont.setPointSize(Current_FontSize);
                        p.setFont(itemFont);
                        pen.setColor(QColor(119-5*j,119-5*j,119-5*j));
                        p.setPen(pen);                        
                        if(Current_Page == 0)
                        Write_Post = Address_Font[Add_Step_By_Step][i];
                        if(Current_Page == 1)
                        Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[curPos]);
                        }
                    }
                    else  if(i == 6)
                    {
                        if(Add_Step_By_Step > ((FRAME-1)/2))
                        {
                            //计算字体大小
                            Current_FontSize =  22 - FONT_STEP_VALUE*j ;
                            itemFont.setPointSize(Current_FontSize);
                            p.setFont(itemFont);
                            pen.setColor(QColor(77-5*j,77-5*j,77-5*j));
                            p.setPen(pen);

                            if(Current_Page == 0)
                            Write_Post = Address_Font[Add_Step_By_Step][i];
                            if(Current_Page == 1)
                            Write_Post = Address_Font[Add_Step_By_Step][i]+20;
                            //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize<<"i = "<<i<<Address_Font[Add_Step_By_Step][i];
                            if((Write_Post >0)&&(Write_Post <480))
                            {
                                p.drawText(  10,  Write_Post,  menuList[curPos]);
                            }
                        }
                    }
                }
            }
        }
    }
}
    //qDebug()<<"----press--is_show_left_btn:"<<is_show_left_btn<<"is_show_right_btn"<<is_show_right_btn;
    if(is_show_left_btn)
    {
        is_show_left_btn = false;
        /*
        pen.setColor(Qt::green);
        p.setPen(pen);
        p.translate(650, 240);
        p.drawText(0, 0, "左键按下");
        */
    }
    else if(is_show_right_btn)
    {
        is_show_right_btn = false;
    }

    if(left_btn_dblClick)
    {
        left_btn_dblClick = false;

    }
    else if(left_btn_dblClick)
    {
        left_btn_dblClick = false;

    }
}





//快滑松手  //快滑滑动松手根据前一刻方向 来滑最后一格
void Widget::Scroll_Quick_TimeOut_Update_GUI()
{
    qDebug()<<"Scroll_Quick_TimeOut_Update_GUI >";

    if(Add_Step_By_Step == FRAME)
    {
        Release_Quick_Dir_Update_Times = 0;
        if(Release_Quick_Dir_Timeout->isActive() == true)
        {
             Release_Quick_Dir_Timeout->stop();
        }
        return;
    }

    Release_Quick_Dir_Update_Times++;

    if(Scroll_Dir == 0x01)
    {
        Add_Step_By_Step--;  //   //下拉菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step <=-1)  //
        {
            if(selectItemIndex > 0)
            {
                selectItemIndex--;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex == 0)
        {
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;

        }
    }
    else if(Scroll_Dir == 0x02)
    {
        Add_Step_By_Step++;       //上滑菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step >=(2*FRAME +1))  //  4   5   6
        {
            if(selectItemIndex < (totalItemNum-1))
            {
                selectItemIndex++;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }

        if(selectItemIndex == (totalItemNum-1))
        {
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;

        }
    }


    if(Add_Step_By_Step == FRAME)
    {
        Last_Update_GUI = 1;
        Release_Quick_Dir_Update_Times = 0;

        if(Release_Quick_Dir_Timeout->isActive() == true)
        {
            Release_Quick_Dir_Timeout->stop();
        }
        repaint();
    }
    else
    {
        Last_Update_GUI = 0;
        repaint();
        if(Release_Quick_Dir_Timeout->isActive() == false)
        {
            Release_Quick_Dir_Timeout->start(100*Release_Quick_Dir_Update_Times); //100ms
        }
    }
}
/*
void Widget::Motor_Shake_Slot_Timer_Handler()
{
    if(Motor_Shake_Slot_Timer->isActive() == true)
    {
        Motor_Shake_Slot_Timer->stop();
    }

    Shock_Motor.Led_Off();
}
*/
//慢滑松手  靠近最近的那个
void Widget::Release_TimeOut_Update_GUI()
{
    Release_Slow_Update_Times++;
    //                         3                              7
    if((Add_Step_By_Step >= (FRAME/2)) && (Add_Step_By_Step < FRAME))       //6  5  4  3
    {
        Add_Step_By_Step++;
        Last_Update_GUI = 0;
    }//                               11                                      8
    else if((Add_Step_By_Step <= (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step > FRAME))  //8  9  10  11  靠近
    {
        Add_Step_By_Step--;
        Last_Update_GUI = 0;
    }


    if(Add_Step_By_Step == FRAME)
    {
        Last_Update_GUI = 1;
        update();
        if(Release_Timeout->isActive() == true)
        {
            Release_Timeout->stop();
        }
    }
    else
    {
        Last_Update_GUI = 0;
        update();
        if(Release_Timeout->isActive() == false)
        {
            Release_Timeout->start(Release_Slow_Update_Times*10);
        }
    }
}
//慢滑松手   根据方向来决定滑动
void Widget::Release_Slow_Dir_TimeOut_Update_GUI()
{
    if(Add_Step_By_Step == FRAME)
    {
        Release_Slow_Dir_Update_Times = 0;
        if(Release_Slow_Dir_Timeout->isActive() == true)
        {
             Release_Slow_Dir_Timeout->stop();
        }
        return;
    }

    Release_Slow_Dir_Update_Times++;

    if(Scroll_Dir == 1)    //  0   1   2
    {
        Add_Step_By_Step--;  //   //朝下滑     6     //FRAME
        Last_Update_GUI = 0;

        if(Add_Step_By_Step <=-1)  //   6   5   4    3    2   1   0
        {
            if(selectItemIndex > 0)
            {
                selectItemIndex--;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex <= 0)  //最后一条不可以滑动
        {
            selectItemIndex = 0;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        //qDebug()<<"Add_Step_By_Step -- ";
    }
    else if(Scroll_Dir == 2)   // 12   13   14
    {
        Add_Step_By_Step++;       //上滑菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step >= (2*FRAME+1))  //   8    9   10     11    12  13  14
        {
            if(selectItemIndex < (totalItemNum-1))
            {
                selectItemIndex++;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
             }
        }

        if(selectItemIndex >= (totalItemNum-1))//最后一条不可以滑动
        {
            selectItemIndex = totalItemNum-1;
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        //qDebug()<<"Add_Step_By_Step ++ ";
    }

    if(Add_Step_By_Step == FRAME)
    {
        Last_Update_GUI = 1;
        Release_Slow_Dir_Update_Times = 0;

        if(Release_Slow_Dir_Timeout->isActive() == true)
        {
             Release_Slow_Dir_Timeout->stop();
        }
        update();
    }
    else
    {
        Last_Update_GUI = 0;
        update();
        if(Release_Slow_Dir_Timeout->isActive()== false)
        {
            Release_Slow_Dir_Timeout->start(Release_Slow_Dir_Update_Times*10);
        }
    }
}

//慢滑松手   根据方向  位置   来决定滑动
void Widget::Release_Slow_Dir_Near_Update_GUI()
{

    //                         3                              7
    if((Add_Step_By_Step >= (FRAME/2)) && (Add_Step_By_Step < FRAME))       //6  5  4  3   靠近
    {
        Release_TimeOut_Update_GUI();
    }//                            7/2
    else if((Add_Step_By_Step < (FRAME/2)) && (Add_Step_By_Step >= 0))       //2  1  0     方向
    {
        Release_Slow_Dir_TimeOut_Update_GUI();
    }                         //  7   +  7/2    +1
    else if((Add_Step_By_Step <= (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step > FRAME))  //8  9  10  11  靠近
    {
        Release_TimeOut_Update_GUI();
    }                        //   7  +   7/2   +1
    else if((Add_Step_By_Step > (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step < (2*FRAME + 1)))  //12  13  14 方向
    {
        Release_Slow_Dir_TimeOut_Update_GUI();
    }
}


void Widget::Slow_Scroll_Timer_Handle()
{
    if((selectItemIndex == 0)&&(Scroll_Dir == 1))//最后一条不可以滑动
    {
        if(Slow_Scroll_Timer->isActive() == true)
        {
            Slow_Scroll_Timer->stop();
        }
        return;
    }
    else if((selectItemIndex == (totalItemNum-1))&&(Scroll_Dir == 2))//最后一条不可以滑动
    {
        if(Slow_Scroll_Timer->isActive() == true)
        {
            Slow_Scroll_Timer->stop();
        }
        return;
    }

    if(Scroll_Dir == 1)
    {
        Add_Step_By_Step--;  //   //朝下滑     6     //FRAME
        Last_Update_GUI = 0;

        if(Add_Step_By_Step <=-1)  //   6   5   4    3    2   1   0
        {
            if(selectItemIndex > 0)
            {
                selectItemIndex--;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
        if(selectItemIndex == 0)  //最后一条不可以滑动
        {
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        //qDebug()<<"Add_Step_By_Step -- ";
        update();
    }
    else if(Scroll_Dir == 2)
    {
        Add_Step_By_Step++;       //上滑菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step >= (2*FRAME+1))  //   8    9   10     11    12  13  14
        {
            if(selectItemIndex < (totalItemNum-1))
            {
                selectItemIndex++;
                //Slider_p->setValue(totalItemNum-1-selectItemIndex);
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
             }
        }

        if(selectItemIndex == (totalItemNum-1))//最后一条不可以滑动
        {
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
        }
        //qDebug()<<"Add_Step_By_Step ++ ";
        update();
    }
    Scroll_Times--;

    if(Scroll_Times > 0)
    {
        Slow_Scroll_Timer->start(1);
    }
    else if(Scroll_Times <= 0)
    {
        if(Slow_Scroll_Timer->isActive() == true)
        {
            Slow_Scroll_Timer->stop();
            Scroll_Times = 0;
        }
    }
}



void Widget:: Quick_Scroll_Update_Timer_Slot_Handle()
{
    if((selectItemIndex == 0)&&(Scroll_Dir == 1))//最后一条不可以滑动
    {
        if(Quick_Scroll_Slot_Timer->isActive() == true)
        {
            Quick_Scroll_Slot_Timer->stop();
        }
        Scroll_Times = 0;
        return;
    }
    if((selectItemIndex == (totalItemNum-1))&&(Scroll_Dir == 2))//最后一条不可以滑动
    {
        if(Quick_Scroll_Slot_Timer->isActive() == true)
        {
            Quick_Scroll_Slot_Timer->stop();
        }
        Scroll_Times = 0;
        return;
    }

    if(Scroll_Times > 0)
    {
        emit Quick_Scroll_Slot(Scroll_Times);   // 发送信号
        Quick_Scroll_Slot_Timer->start(Quick_Scroll_Period);
    }
    else if(Scroll_Times <= 0)
    {
        if(Quick_Scroll_Slot_Timer->isActive() == true)
        {
            Quick_Scroll_Slot_Timer->stop();
            Scroll_Times = 0;
            Scroll_Quick_TimeOut_Update_GUI();
        }
    }
    //Scroll_Times--;
}

void Widget::Handle_Touch_Value_Event(unsigned short Receive_Diff_Data_Total)
{
    unsigned short Receive_Diff_Data = 0;
    unsigned char  Data_Flage = 0;
    unsigned char  Dir_Old = 0;

    if(sender()->objectName() == "QTimer")
    {
        Slow_Scroll_Timer_Handle();  //800ms 就可以执行完
    }
    else if(sender()->objectName() == "IIC_MyThread")
    {
        qDebug()<<"objectName() = IIC_MyThread";
        printf("Receive_Diff_Data_Total = %x\r\n", Receive_Diff_Data_Total);

        if(Quick_Scroll_Slot_Timer->isActive() == true)          //快滑定时器
        {
            Scroll_Times = 0;
            Quick_Scroll_Period = 0;
            qDebug()<<"Quick_Scroll_Update_Timer  stop ";
            Quick_Scroll_Slot_Timer->stop();
        }

        if(Slow_Scroll_Timer->isActive() == true)    //  关掉所有 刷新  定时器   慢滑定时器
        {
            Slow_Scroll_Timer->stop();
        }

        if(Release_Timeout->isActive() == true)            //  慢滑   松手 定时器
        {
            Release_Timeout->stop();
        }
        Release_Slow_Update_Times = 0;

        if(Release_Slow_Dir_Timeout->isActive() == true)   //   慢滑   松手 定时器
        {
            Release_Slow_Dir_Timeout->stop();
        }
        Release_Slow_Dir_Update_Times = 0;

        if(Release_Quick_Dir_Timeout->isActive() == true)   //  快滑  松手 定时器
        {
            Release_Quick_Dir_Timeout->stop();
        }
        Release_Quick_Dir_Update_Times = 0;

        //if()
        Scroll_Times = 0;

        if((Receive_Diff_Data_Total ==  0x4000)&& ((Current_Page == 0)||(Current_Page == 1)))     //确定  第0页  第1页
        {                          // 3 - 1
            if(Current_Page < (Total_Page-1))
            {
                Current_Page++; // 0   1   2
            }

            if(Current_Page >= (Total_Page-1))
            {
                Current_Page = Total_Page-1;
            }

            Widget_Page_Switch = 0;
            //repaint();
            update();
        }
        else if((Receive_Diff_Data_Total == 0x8000)&& ((Current_Page == 1)||(Current_Page == 2))) //取消
        {
            if(Current_Page > 0)
            {
                Current_Page--; // 2   1   0
            }
            if(Current_Page <= 0)
            {
               Current_Page = 0;
            }
            Widget_Page_Switch = 0;
            //repaint();
            update();

        }
        else if((Receive_Diff_Data_Total > 0x00)&&(Receive_Diff_Data_Total < 0x2000)&&((Current_Page == 0)||(Current_Page == 1)))
        {
            if((Receive_Diff_Data_Total>>12) == 1) //负
            {
                Receive_Diff_Data_Total = Receive_Diff_Data_Total - 0x1000;
                Data_Flage = 1;
            }
            if((Receive_Diff_Data_Total>>8) == 1)    // 按下
            {
                // 滑动值越来越大
                Receive_Diff_Data = Receive_Diff_Data_Total - 0x100;
                //printf("Receive_Diff_Data = %x\r\n", Receive_Diff_Data);

                if(Data_Flage == 0)   //  方向确定
                {
                    Scroll_Dir = 1;
                    Data_Flage = 0;
                }
                else if(Data_Flage == 1)
                {
                    Scroll_Dir = 2;
                    Data_Flage = 0;
                }

                if(Receive_Diff_Data >= 4) // 4S  内执行完
                {
                    Receive_Diff_Data = 4;
                }

                if(Dir_Old ==  Scroll_Dir)
                {
                    Scroll_Times = Receive_Diff_Data;
                }
                else if(Dir_Old != Scroll_Dir)
                {
                    Scroll_Times = Receive_Diff_Data;
                }
                Slow_Scroll_Timer_Handle();  //800ms 就可以执行完
            }

            if((Receive_Diff_Data_Total>>8) == 4)  //松手
            {
                Receive_Diff_Data = Receive_Diff_Data_Total - 0x400;

                if(Receive_Diff_Data == 0xFF)
                {
                    if(Add_Step_By_Step != FRAME)
                    {
                        Release_Slow_Dir_Near_Update_GUI();
                    }
                    qDebug()<<"slow 松手";
                }
                else if(Receive_Diff_Data < 0xFF)
                {
                    qDebug()<<"quick 松手"<<Receive_Diff_Data;
                    Scroll_Times = (Receive_Diff_Data*totalItemNum/10)*4;

                    if(Scroll_Times <= 5 )
                    {
                        Quick_Scroll_Period = 25;
                    }
                    if((Scroll_Times > 5)&&(Scroll_Times <=10))
                    {
                        Quick_Scroll_Period = 12;
                    }
                    if((Scroll_Times > 10)&&(Scroll_Times <=15))
                    {
                        Quick_Scroll_Period = 6;
                    }
                    if((Scroll_Times > 15)&&(Scroll_Times <=20))
                    {
                        Quick_Scroll_Period = 3;
                    }
                    if(Scroll_Times > 20)
                    {
                        Quick_Scroll_Period = 1;
                    }
                    qDebug()<<"Scroll_Times = "<<Scroll_Times<<"Quick_Scroll_Period ="<<Quick_Scroll_Period;
                    Quick_Scroll_Update_Timer_Slot_Handle();
                }
            }
        }
    }
}