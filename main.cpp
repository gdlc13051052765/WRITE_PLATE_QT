#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <QApplication>
#include <QTextCodec>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include "iic_pthread.h"
#include "page_pthread.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;

    int index = QFontDatabase::addApplicationFont("/home/HYQiHei-65S.ttf");

    if(index != -1)
    {
        QStringList fontList(QFontDatabase::applicationFontFamilies(index));

        // font -- LiHei Pro
        if(fontList.count() > 0)
        {
            qDebug("open\n");
            QFont font_zh(fontList.at(0));
            font_zh.setBold(false);
            a.setFont(font_zh);
        }
    }

    int id = -1;
    //strcpy(Snd_msg.mtext, "Hello I am Rabbit Snd\r\n");

    key_t key = ftok("/tmp", 66);

    id = msgget(key,  0666|IPC_CREAT);
    if(id == -1)
    {
        qDebug("msgget failure");
        return a.exec();
    }
    else
    {
        qDebug("msgget succeed");
    }

    First_Page_Create = 1;
    First_Page_Update = 0;

    IIC_MyThread * p_iic_thread  = new  IIC_MyThread();

    p_iic_thread->start();

    //qDebug()<<"Page_MyThread I am Runing";
    QObject::connect(p_iic_thread,SIGNAL(myslot(unsigned short)), &w,SLOT(Handle_Touch_Value_Event(unsigned short)));

    QObject::connect(&w, SIGNAL(Touch_Allow_Send_Handler()), p_iic_thread,SLOT(Touch_Allow_Receive_Handler()));

    w.resize(800, 480);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_AcceptTouchEvents, true);
    //w.Gui_Upate_Cycle_Timer->start(500);

    QGraphicsScene *scene = new QGraphicsScene;

    QGraphicsProxyWidget *g = scene->addWidget(&w);
    g->setRotation(-90);

    QGraphicsView *view = new QGraphicsView(scene);

    view->setWindowFlags(Qt::FramelessWindowHint);
    view->setWindowFlags(Qt::FramelessWindowHint);//无边框
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(QFrame::NoFrame);//无Frame边框

    //view->setInteractive(true);  //设置场景内的是否允许交互
    view->resize(480, 800);
    view->show();
    First_Page_Create = 0;
    First_Page_Update = 1;

    qDebug("16619851335------------------\n");


    return a.exec();
}


