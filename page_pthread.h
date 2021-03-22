#ifndef PAGE_PTHREAD_H
#define PAGE_PTHREAD_H


#include <QObject>
#include <QThread>
#include "widget.h"

extern volatile unsigned char  First_Page_Create;
extern volatile unsigned char  First_Page_Update;


class Page_MyThread: public QThread
{
    Q_OBJECT
public:
    explicit Page_MyThread(QObject *parent = 0);
    ~Page_MyThread();

protected:
    //QThread的虚函数
    //线程处理函数
    //不能直接调用，通过start()间接调用
    void run();

private:
    int val;
    Widget * w;
};

































#endif // IIC_PTHREAD_H

