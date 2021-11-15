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

    int index = QFontDatabase::addApplicationFont("/opt/Fonts/HYQiHei-65S.ttf");
    if(index != -1)
    {
        QStringList fontList(QFontDatabase::applicationFontFamilies(index));
        if(fontList.count() > 0)
        {
            qDebug("open\n");
            QFont font_zh(fontList.at(0));
            font_zh.setBold(false);
            a.setFont(font_zh);
        }
    }
    int id = -1;
    key_t key = ftok("/tmp", 66);
    id = msgget(key,  0666|IPC_CREAT);
    if(id == -1) {
        qDebug("msgget failure");
        return a.exec();
    } else {
        qDebug("msgget succeed");
    }

    First_Page_Create = 1;
    First_Page_Update = 0;

    IIC_MyThread * p_iic_thread  = new  IIC_MyThread();
    p_iic_thread->start();

    QObject::connect(p_iic_thread,SIGNAL(myslot(unsigned short)), &w,SLOT(Handle_Touch_Value_Event(unsigned short)));
    QObject::connect(&w, SIGNAL(Touch_Allow_Send_Handler()), p_iic_thread,SLOT(Touch_Allow_Receive_Handler()));
    w.move(0,0);
    w.resize(800, 480);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_AcceptTouchEvents, true);

    QGraphicsScene *scene = new QGraphicsScene;
    QGraphicsProxyWidget *g = scene->addWidget(&w);
    g->setRotation(-90);

    QGraphicsView *view = new QGraphicsView(scene);
    view->setWindowFlags(Qt::FramelessWindowHint);//无边框
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(QFrame::NoFrame);//无Frame边框

    view->setInteractive(true);  //设置场景内的是否允许交互
    view->resize(480, 800);
    view->move(0,0);
    view->show();
    First_Page_Create = 0;
    First_Page_Update = 1;

    // int e = a.exec();
    // if(e == 777)
    // {
    //     QProcess::startDetached(qApp->applicationFilePath(), QStringList()); 
    //     return 0;
    // }

    return a.exec();
}
