#ifndef IIC_PTHREAD_H
#define IIC_PTHREAD_H


#include <QObject>
#include <QThread>


class IIC_MyThread: public QThread
{
    Q_OBJECT
public:
    explicit IIC_MyThread(QObject *parent = 0);
    ~IIC_MyThread();

protected:
    //QThread的虚函数
    //线程处理函数
    //不能直接调用，通过start()间接调用
    void run();

signals:
    void myslot(unsigned short  Receive_Diff_Data_Total);     //声明一个信号，不需要实现

private:
    int val;
};
































#endif // IIC_PTHREAD_H

