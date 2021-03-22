#include "second_ui.h"
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


#define   UPDATE_MS           50

#define   SPEED_SORT          1     //高速低速分界线

#define   FRAME               7     //FRAME

#define   FONT_STEP_VALUE     2


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

//852
static int  Address_Spell[7] = { 130,   170,    210,   250,   290,    330,    370};

static bool left_btn_press = false;
static bool right_btn_press = false;

static bool is_show_left_btn = false;
static bool is_show_right_btn = false;

static bool left_btn_dblClick = false;
static bool right_btn_dblClick = false;

Second_Page_Widget::Second_Page_Widget(QWidget *parent): QWidget(parent)
{

    this->installEventFilter(this);

    Button_Determine = new QPushButton(this);
    QStringList list;
    list.append("color:white");                         // 前景色
    list.append("background-color:rgb(167,203,74)");    // 背景色
    list.append("border-style:outset");                 // 边框风格
    list.append("border-width:0px");                    // 边框宽度
    list.append("border-color:rgb(167,203,74)");        // 边框颜色
    list.append("border-radius:10px");                  // 边框倒角

    Button_Determine->setStyleSheet(list.join(';'));
    Button_Determine->move(718,451);
    Button_Determine->resize(75, 20);
    Button_Determine->show();


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
            <<"黄瓜白菜"
            <<"黄瓜西红柿"
            <<"黄瓜肉片"
            <<"锅包肉"
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
    scrollDir  = 0;         //滑动方向  1：向上滑动  2：向下滑动d
    maxScrollTime  = 100;   //默认滑动最大值,时间时间1S
    timeCount = 0;          //滑动计时（1秒）

    totalItemNum = menuList.length();
    //selectItemIndex = (totalItemNum/2);
    selectItemIndex = 0;

    Total_Show_Menu_Spell_Num = Menu_Spell_Display_List.length();
    qDebug()<<Menu_Spell_List.at(selectItemIndex);
    Select_Show_Menu_Spell_Index = Menu_Spell_Display_List.indexOf(Menu_Spell_List.at(selectItemIndex));
    qDebug()<<Select_Show_Menu_Spell_Index;

    showItemNum = 7;    //界面显示菜单个数,最好为奇数
    baseFontSize = 70;  //中心字符大小
    itemIntervalH = -20;  //菜单垂直间隔

    Add_Step_By_Step = FRAME;

    Is_Speed_Low_Scroll = 0;
    Last_Update_GUI = 1;

    Touchpad_Flage = 0;

    Touch_Allow_Flage = 1;
    Add_Times = 0;
/*
    Slider_p =new QSlider(this);
    Slider_p->setOrientation(Qt::Vertical);
    Slider_p->setGeometry (730, 60, 44, 360);
    Slider_p->setMinimum(0);  // 最小值
    Slider_p->setMaximum(6);  // 最大值



    //margin-left:-3pxmargin-left 就是设置标签的左外边距
    Slider_p->setStyleSheet("QSlider::groove:vertical{ height: 300;   width: 16px;   border-radius:8px;  left: 20px;        right: 20px;         background: #707070;}\
                             QSlider::handle:vertical{ height: 30px;  width: 30px;   border-radius:15px; margin: 0px -14px 0px 0px;              background: #A7CB4A;}");

    if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
    {
        Slider_p->setValue(3);
    }
    Slider_p->setTickPosition(QSlider::TicksBothSides);
    Slider_p->show();
*/
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
    //qDebug() << fm.height()<<"16619851335";            //获取文字高度  105   327
    //qDebug() << fm.maxWidth()<<"16619851335";          //获取文字宽度

    //itemRealH = fm.height()*20/19 + itemIntervalH;  //计算实际间隔   //菜单实际间隔（菜单字符宽度+间隔）
    itemRealH = 0;

    timerScroll = new QTimer(this);//13
    connect(timerScroll, SIGNAL(timeout()), this, SLOT(timeUpdateScroll()));

    Touchpad_Time = new QTimer(this);
    connect(Touchpad_Time, SIGNAL(timeout()), this, SLOT(Touchpad_Time_Clear_Flage()));


    Scroll_Quick_TimeOut = new QTimer(this);
    connect(Scroll_Quick_TimeOut, SIGNAL(timeout()), this, SLOT(Scroll_Quick_TimeOut_Update_GUI()));


    Release_Timeout = new QTimer(this);
    connect(Release_Timeout, SIGNAL(timeout()), this, SLOT(Release_TimeOut_Update_GUI()));


    Touch_Allow_Timeout = new QTimer(this);
    connect(Touch_Allow_Timeout, SIGNAL(timeout()), this, SLOT(Touch_Allow_Timeout_Handle()));

}

Second_Page_Widget::~Second_Page_Widget()
{

}


#define MXA_FIFTER_NUM   5
#define MAX_FIFTER_TIM   100  //MS
#define MAX_SPEED_THRESHOLD     20
#define MAX_SLOW_THREAHOLD      3




//int fifter_count = 0;
//struct slider_fifter  mFifter[10] ;
void Second_Page_Widget::paintEvent(QPaintEvent *)
{
    int w = this->width();      //宽
    int h = this->height();     //高

    int Current_FontSize = 0;  //中心字符大小

    //qDebug()<<"----press--is_show_left_btn:"<<w<<"is_show_right_btn"<<h;

    int curPos = 0;
    volatile int Write_Post = 0;
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法


    //qDebug()<<"Add_Step_By_Step = ?" << Add_Step_By_Step;
    //QPalette bgpal = palette();

    //bgpal.setColor (QPalette::Window , QColor (0, 0 , 0, 255));
    //setPalette (bgpal);


    QBrush brush(QColor (57, 57 , 57, 255));

    p.setBrush(brush);
    p.drawRect(0, 0, w-90, h); //左半边矩形


    QBrush brush1(QColor (0, 0 , 0, 255));
    p.setBrush(brush1);
    p.drawRect(w-90, 0, 90, h);  //右半边矩形

    QBrush brush2(QColor (167, 203, 74, 255));
    p.setBrush(brush2);
    QRectF rectangle(5, 32, 690, 4);
    p.drawRoundedRect(rectangle, 2, 2); //上面长条


    p.setBrush(brush);                    //假滑条的棍
    QRectF rectangle1(750, 100, 16, 280);
    p.drawRoundedRect(rectangle1, 8, 8);
    p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    QBrush brush3(QColor (167, 203, 74, 255)); //假滑条的块
    p.setBrush(brush3);
    p.drawEllipse(750, 225, 30, 30);

    QPen pen;
    pen.setColor(QColor(212, 212, 212));
    p.setPen(pen);
    itemFont.setPointSize(20);
    p.setFont(itemFont);


    qDebug()<<"Select_Show_Menu_Spell_Index = ?" <<Select_Show_Menu_Spell_Index;
    qDebug()<<"Total_Show_Menu_Spell_Num = ?" <<Total_Show_Menu_Spell_Num;

    for(int i=0; i<showItemNum; i++)
    {          //Select_Show_Menu_Spell_Index 1           //3
        curPos = Select_Show_Menu_Spell_Index + (i-(showItemNum/2));

        if((curPos < 0)||(curPos >= (Total_Show_Menu_Spell_Num)))    //边沿检测
        {
            continue; //continue 语句只结束本次循环,而不是终止整个循环
        }

        p.drawText(755,  Address_Spell[i],  Menu_Spell_Display_List[curPos]);
    }

    //if( ((totalItemNum-1-selectItemIndex) >=0) && ((totalItemNum-1-selectItemIndex)<= totalItemNum-1))
    //{
    //    Slider_p->setValue(totalItemNum-1-selectItemIndex);
    //}



    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);
    Current_FontSize = 28;
    itemFont.setPointSize(Current_FontSize);
    p.setFont(itemFont);
    p.drawText(716,  440,  "确定");
    p.drawText(716,  68,   "返回");

    p.drawText(320,  30,  "周二午餐");
    if((Last_Update_GUI == 1)&&(Add_Step_By_Step==FRAME))
    {
        showItemNum = 7;
        qDebug()<<"Last_Update_GUI = " <<Last_Update_GUI<<"Add_Step_By_Step"<<Add_Step_By_Step;
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

            Write_Post =  Address_Font[6][i]+20;
            //qDebug()<<"Write_Post" <<Write_Post<<"16619851335";

            //p.drawText(  10,   centerPos_y  +  (Current_FontSize/2 )+(i-(showItemNum/2))*(itemRealH) ,  menuList[curPos]);
            p.drawText(  10,  Write_Post,  menuList[curPos]);


            //QPainter paint_slider(this);
            //pen.setColor(QColor(212, 212, 212));
            //paint_slider.setPen(pen);
            //itemFont.setPointSize(20);
            //paint_slider.setFont(itemFont);
            //paint_slider.drawText(735,  Address_Spell[i],  Menu_Spell_List[curPos]);
        }

        Last_Update_GUI = 0;

        return;
    }
    else
    {
        //qDebug()<<"Last_Update_GUI = " <<Last_Update_GUI;   向上翻页
        //                      7                          7 * 2
        if((Add_Step_By_Step>=FRAME)&&(Add_Step_By_Step<=(FRAME*2)))  //   8   9  10   11  12  13  14
        {
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

                    Write_Post = 476 - ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*8) + 20;
                    //Write_Post = 476 +  ((Add_Step_By_Step - ((FRAME*2)- ((FRAME-1)/2)))*2) ;
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
                qDebug()<<"Add_Step_By_Step ?+? = "<<Add_Step_By_Step;

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

                        Write_Post =  8 +(((FRAME-1)/2) - Add_Step_By_Step)*12+20;
                        //qDebug()<<"Write_Post" <<Write_Post<<"Current_FontSize" <<Current_FontSize;

                        if((Write_Post >0)&&(Write_Post <480))
                        {
                            p.drawText(  10,  Write_Post,  menuList[selectItemIndex - 4]);
                        }
                    }

                }

                qDebug()<<"Add_Step_By_Step ?-? = "<<Add_Step_By_Step;  //6   5   4   3
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
        //   p.rotate(90);
        /*
        pen.setColor(Qt::red);
        p.setPen(pen);
        p.translate(650, 240);
        p.drawText(0, 0, "右键按下");
        */
/*
        this->close();

        qDebug()<<"breakfast comeing....";

        Widget_Breakfast *w_breakfast = new Widget_Breakfast();


        w_breakfast->resize(800, 480);
        w_breakfast->setWindowFlags(Qt::FramelessWindowHint);
        w_breakfast->setAttribute(Qt::WA_AcceptTouchEvents, true);


        QGraphicsScene *scene_bre = new QGraphicsScene;

        QGraphicsProxyWidget *g_bre = scene_bre->addWidget(w_breakfast);
        g_bre->setRotation(90);

        QGraphicsView *view_bre = new QGraphicsView(scene_bre);

        view_bre->setWindowFlags(Qt::FramelessWindowHint);
        view_bre->setWindowFlags(Qt::FramelessWindowHint);//无边框
        view_bre->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view_bre->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view_bre->setFrameStyle(QFrame::NoFrame);//无Frame边框

        view_bre->setInteractive(true);  //设置场景内的是否允许交互
        view_bre->resize(480, 800);
        view_bre->show();
*/
    }

    if(left_btn_dblClick)
    {
        left_btn_dblClick = false;
        /*
        pen.setColor(Qt::green);
        p.setPen(pen);
        p.translate(650, 300);
        p.drawText(0, 0, "左键双击");
        */
    }
    else if(left_btn_dblClick)
    {
        left_btn_dblClick = false;
        /*
        pen.setColor(Qt::red);
        p.setPen(pen);
        p.translate(650, 300);
        p.drawText(0, 0, "右键双击");
        */
    }

}


static int old_pos_y = 0;
static int is_speed_scroll = 0; //1 start
static int Scroll_Dir = 0;      //1 up  2 down

static int inv_scroll_value = 0;

static int timer_count = 0;

static int Scroll_Period = 0;
static int Scroll_Times = 0;
//speed 1~10
void Second_Page_Widget::scroll_quick(int speed, int dir)
{
//                   2                 1
    qDebug()<<"speed ="<< speed <<"dir = "<<dir;

    Scroll_Times = (speed*7)*(totalItemNum/10)+3;                     // 次数        变化是 2
    Scroll_Period  =  500/Scroll_Times;           // Scroll_Times 是  10     update 周期  50

    if(timerScroll->isActive() == false)
    {
        timerScroll->start(Scroll_Period); //30ms
    }
    timer_count = 0;
}

void Second_Page_Widget::Touchpad_Time_Clear_Flage()
{
    Touchpad_Flage = 0;
    is_speed_scroll = 0;
    inv_scroll_value = 0;
    Scroll_Dir = 0;
    if(Touchpad_Time->isActive() == true)
    {
        Touchpad_Time->stop();
    }
}

void Second_Page_Widget::Touch_Allow_Timeout_Handle()
{
    Touch_Allow_Flage = 1;
    if(Touch_Allow_Timeout ->isActive() == true)
    {
        Touch_Allow_Timeout ->stop();
    }
}
//
void Second_Page_Widget::timeUpdateScroll()
{
    if(Scroll_Dir == 0x02)
    {
        Add_Step_By_Step++;       //上滑菜单

        if(Add_Step_By_Step >=(2*FRAME +1))  //  4   5   6
        {
            if(selectItemIndex < (totalItemNum-1))
            {
                selectItemIndex++;
                if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                {
                    if(Select_Show_Menu_Spell_Index  < (Total_Show_Menu_Spell_Num-1))
                    {
                        Select_Show_Menu_Spell_Index++;
                    }
                }
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
    else if(Scroll_Dir == 0x01)
    {
        Add_Step_By_Step--;  //   //下拉菜单

        if(Add_Step_By_Step <=-1)  //  2   1   0
        {
            if(selectItemIndex > 0)
            {
                selectItemIndex--;
                if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                {
                    if(Select_Show_Menu_Spell_Index > 0)
                    {
                        Select_Show_Menu_Spell_Index--;
                    }
                }
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


    update();

    timer_count += 1;

    if(timer_count >= Scroll_Times)
    {
        qDebug()<<"timecount >";
        is_speed_scroll = 0;
        Scroll_Times = 0;
        timer_count = 0;


        if(timerScroll->isActive() == true)
        {
            timerScroll->stop();
        }

        if(Add_Step_By_Step != FRAME)
        {
            Add_Times = 0;
            Scroll_Quick_TimeOut_Update_GUI();
        }
    }
}
//快滑松手
void Second_Page_Widget::Scroll_Quick_TimeOut_Update_GUI()
{
    //快滑滑动松手根据前一刻方向 来滑最后一格

    qDebug()<<"Scroll_Quick_TimeOut_Update_GUI >";
    if(Scroll_Dir == 0x02)
    {
        Add_Step_By_Step++;       //上滑菜单
        Last_Update_GUI = 0;

        if(Add_Step_By_Step >=(2*FRAME +1))  //  4   5   6
        {
            if(selectItemIndex < (totalItemNum-1))
            {
                selectItemIndex++;
                if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                {
                    if(Select_Show_Menu_Spell_Index  < (Total_Show_Menu_Spell_Num-1))
                    {
                        Select_Show_Menu_Spell_Index++;
                    }
                }
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
    else if(Scroll_Dir == 0x01)
    {
        Add_Step_By_Step--;  //   //下拉菜单
        Last_Update_GUI = 0;

         if(Add_Step_By_Step <=-1)  //
         {
             if(selectItemIndex > 0)
             {
                selectItemIndex--;
                if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                {
                    if(Select_Show_Menu_Spell_Index > 0)
                    {
                        Select_Show_Menu_Spell_Index--;
                    }
                }

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


    if(Add_Step_By_Step == FRAME)
    {
        if(Scroll_Quick_TimeOut->isActive() == true)
        {
            Scroll_Quick_TimeOut->stop();
        }

        Last_Update_GUI = 1;
    }
    else
    {
        if(Scroll_Quick_TimeOut->isActive() == false)
        {
            Add_Times++;
            Scroll_Quick_TimeOut->start(50*Add_Times); //100ms
        }
    }
    update();
}

//慢滑松手
void Second_Page_Widget::Release_TimeOut_Update_GUI()
{
    //                         3                              7
    if((Add_Step_By_Step >= (FRAME/2)) && (Add_Step_By_Step < FRAME))       //6  5  4  3
    {
        Add_Step_By_Step++;
        Last_Update_GUI = 0;
    }//                            7/2
    else if((Add_Step_By_Step < (FRAME/2)) && (Add_Step_By_Step >= 0))       //2  1  0
    {
        Add_Step_By_Step--;
        Last_Update_GUI = 0;
        if(Add_Step_By_Step == -1)
        {
            if(selectItemIndex > 0)
            {
                selectItemIndex--;
                if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                {
                    if(Select_Show_Menu_Spell_Index > 0)
                    {
                        Select_Show_Menu_Spell_Index--;
                    }
                }

                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
            }
        }
    }                         //  7   +  7/2    +1
    else if((Add_Step_By_Step <= (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step > FRAME))  //8  9  10  11
    {
        Add_Step_By_Step--;
        Last_Update_GUI = 0;
    }                        //   7  +   7/2   +1
    else if((Add_Step_By_Step > (FRAME+(FRAME/2)+1)) && (Add_Step_By_Step < (2*FRAME + 1)))  //12  13  14
    {
        Add_Step_By_Step++;
        Last_Update_GUI = 0;
        if(selectItemIndex < (totalItemNum-1))
        {
            selectItemIndex++;
            if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
            {
               if(Select_Show_Menu_Spell_Index  < (Total_Show_Menu_Spell_Num-1))
               {
                   Select_Show_Menu_Spell_Index++;
               }
            }
            Add_Step_By_Step = FRAME;
            Last_Update_GUI = 1;
         }
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
            Release_Timeout->start(UPDATE_MS);
        }
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


bool Second_Page_Widget::eventFilter(QObject *watched, QEvent *event)
{
    QMouseEvent* hoverEvent = (QMouseEvent*)event;
    QPoint point = hoverEvent->globalPos();

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    //qDebug("[qt debug] x=%02x, y=%02x", point.x(), point.y());

    //无
    //qDebug()<< event->type()<<"Qt C##";
    if(event->type() == QEvent::MouseButtonRelease) //
    {
        old_pos_y = 0xee;   //clear
        //Touch_Allow_Flage = 0;
        //if(Touch_Allow_Timeout->isActive() == false)
        // {
        //    Touch_Allow_Timeout->start(200);
        //}

        qDebug()<<"Release mouse"<< is_speed_scroll<<Touchpad_Flage;
        if((is_speed_scroll == 0x01)&&(Touchpad_Flage == 1))
        {
            qDebug()<< "Quick MouseButtonRelease";
            Is_Speed_Low_Scroll = 0;
            if(Touchpad_Time->isActive() == true)
            {
                Touchpad_Time->stop();
            }
            scroll_quick(inv_scroll_value, Scroll_Dir);

        }
        if((Is_Speed_Low_Scroll == 1))//
        {
            Is_Speed_Low_Scroll = 0;

            qDebug()<< "Slow MouseButtonRelease";

            /*
            if(Add_Step_By_Step > FRAME)  //
            {
                Add_Step_By_Step--;
                Last_Update_GUI = 0;
            }
            else if(Add_Step_By_Step < FRAME)  //    0
            {
                Add_Step_By_Step++;
                Last_Update_GUI = 0;
            }

            if(Add_Step_By_Step == FRAME)  //    //下拉菜单
            {
                Add_Step_By_Step = FRAME;
                Last_Update_GUI = 1;
                update();
            }
            else
            {
                Last_Update_GUI = 0;
                update();
                if(Release_Timeout->isActive()== false)
                {
                    Release_Timeout->start(UPDATE_MS); //50ms
                }
            }
            */
            Release_Timeout->start(UPDATE_MS);
        }



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

        //Add_Step_By_Step = FRAME;
        //Last_Update_GUI = 1;
        //update();

        touch_press_value =point.y();
        return true;
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        //qDebug()<<"----press---MouseButtonDblClick---x:";
        //update();
    }
    else if(event->type() == QEvent::MouseMove)  //
    {
        if(Touch_Allow_Flage == 1)
        {
            if(abs(touch_press_value - point.y()) > 3)
            {
                //qDebug()<<"----press---clear---touch_press_value:"<<touch_press_value;
                left_btn_press = false;
                right_btn_press = false;
            }

            //qDebug()<<"----mouseMove---globalpos---x:"<<mouseEvent->globalPos().x()<<"  y:"<<mouseEvent->globalPos().y();
            //qDebug()<<"----mouseMove---pos---x:"<<mouseEvent->pos().x()<<"  y:"<<mouseEvent->pos().y();
            if(abs(old_pos_y - point.y()) >= 3) //h speed     SPEED_SORT  高速   低速  分界线  3
            {
                qDebug()<<"high\n";
                is_speed_scroll = 0x01;
                Touchpad_Flage = 1;

                if(Touchpad_Time->isActive() == false)
                {
                    Touchpad_Time->start(2000);  // 2S还在触摸板上   则认为是慢滑
                }


                inv_scroll_value = abs(old_pos_y - point.y());

                if(old_pos_y > point.y())  //朝下滑
                {
                    Scroll_Dir = 0x01;     //
                }
                else
                {
                    Scroll_Dir = 0x02;
                }
            }

            if(abs(old_pos_y - point.y()) >= 1)//low speed   只要有滑动
            {
                Is_Speed_Low_Scroll = 1; //
                qDebug()<<"low\n";
                //qDebug()<<"speed low"<<"old_pos_y="<<old_pos_y<<"new y="<<point.y()<<"\n";
                if(old_pos_y > point.y()) //
                {

                    Add_Step_By_Step--;  //   //朝下滑     6     //FRAME
                    Last_Update_GUI = 0;

                    if(Add_Step_By_Step <=-1)  //   6   5   4    3    2   1   0
                    {
                        if(selectItemIndex > 0)
                        {
                            selectItemIndex--;
                            if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                            {
                                if(Select_Show_Menu_Spell_Index > 0)
                                {
                                    Select_Show_Menu_Spell_Index--;
                                }
                            }
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

                    qDebug()<<"Add_Step_By_Step -- ";
                }
                else  if(old_pos_y < point.y())
                {
                    Add_Step_By_Step++;       //上滑菜单
                    Last_Update_GUI = 0;

                    if(Add_Step_By_Step >= (2*FRAME+1))  //   8    9   10     11    12  13  14
                    {
                        if(selectItemIndex < (totalItemNum-1))
                        {
                            selectItemIndex++;
                            if((Menu_Spell_List.at(selectItemIndex)) != (Menu_Spell_Display_List.at(Select_Show_Menu_Spell_Index)))
                            {
                                if(Select_Show_Menu_Spell_Index  < (Total_Show_Menu_Spell_Num-1))
                                {
                                    Select_Show_Menu_Spell_Index++;
                                }
                            }
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
                    qDebug()<<"Add_Step_By_Step ++ ";
                }

                //         Add_Step_By_Step 8                    Last_Update_GUI= 0
                qDebug()<<"Add_Step_By_Step"<<Add_Step_By_Step<<"Last_Update_GUI="<<Last_Update_GUI<<"\n";
                update();
            }
             old_pos_y = point.y();
        }

        return false;
    }
    return QWidget::eventFilter(watched,event);// 将事件传递给父类
}


