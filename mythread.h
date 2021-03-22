#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QObject>
#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);
    ~MyThread();

protected:
    //QThread的虚函数
    //线程处理函数
    //不能直接调用，通过start()间接调用
    void run();

signals:
    void isDone();  //处理完成信号

signals:

public slots:
};

#endif // MYTHREAD_H
