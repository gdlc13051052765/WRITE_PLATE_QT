#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "i2c-dev.h"
#include <stdint.h>
#include "page_pthread.h"
#include <QDebug>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "widget.h"
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>



volatile unsigned char  First_Page_Create = 1;
volatile unsigned char  First_Page_Update = 0;


Page_MyThread::Page_MyThread(QObject *parent)
{



}

Page_MyThread::~Page_MyThread()
{

}





void Page_MyThread::run()
{
    while(1)
    {
        if(First_Page_Create == 1)
        {


        }

        if(First_Page_Update == 1)
        {

        }


    }


    //emit isDone();  //发送完成信号
}
