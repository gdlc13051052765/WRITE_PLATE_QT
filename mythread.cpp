#include "mythread.h"
#include <QDebug>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<unistd.h>


MyThread::MyThread(QObject *parent)
{

}

MyThread::~MyThread()
{

}

void MyThread::run()
{
    //很复杂的数据处理
    //需要耗时5秒
    qDebug()<<"I am Runing";
    sleep(5);

    emit isDone();  //发送完成信号
}
