#ifndef IIC_PTHREAD_H
#define IIC_PTHREAD_H


#include <QObject>
#include <QThread>
#include <QTime>
#include <QTimer>

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

    bool Determine_Btn_Press;
    bool Cancel_Btn_Press;

    char Diff_Cur_Pos;
    char Diff_Pre_Pos;
    char Diff_Old_Pos;

    char Diff_Cur_Neg;
    char Diff_Pre_Neg;
    char Diff_Old_Neg;

    unsigned char Scroll_Allow_Times;

    unsigned char Set_Diff_Value_Flage;

    unsigned char IIC_Send_Dir_Flage;
    unsigned short Determine_Pressd_Timer_Times;
    unsigned short Cancel_Pressd_Timer_Times;
    unsigned char Determine_Pressd_Out_Flage;
    unsigned char Cancel_Pressd_Out_Flage;
    QTimer *Pressd_Timer;   //按下定时器


//private slots:
//    void Pressd_Timer_Handle();

};
































#endif // IIC_PTHREAD_H

