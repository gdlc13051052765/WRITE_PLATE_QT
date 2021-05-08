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
#include "iic_pthread.h"

#include <QDebug>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <QMutex>
#include <QQueue>
#include <QTime>
#include <QTimer>


//#define  BUTTON_DEBUG

IIC_MyThread::IIC_MyThread(QObject *parent)
{
    Diff_Cur_Pos = 0;
    Diff_Pre_Pos = 0;
    Diff_Old_Pos = 0;

    Diff_Cur_Neg = 0;
    Diff_Pre_Neg = 0;
    Diff_Old_Neg = 0;

    Diff_Cal_Cur = 0;
    Diff_Cal_Cur = 0;
    Diff_Cal_Old = 0;

    Touch_Allow_Flage = 1;
    Set_Diff_Value_Flage = 0;

    Scroll_Allow_Times = 0;
    IIC_Send_Dir_Flage = 0xFF;
    Determine_Btn_Press = false;
    Cancel_Btn_Press = false;
    Determine_Pressd_Timer_Times = 0;
    Cancel_Pressd_Timer_Times = 0;
    Determine_Pressd_Out_Flage = 0;
    Cancel_Pressd_Out_Flage = 0;
    this->setObjectName("IIC_MyThread");
}

IIC_MyThread::~IIC_MyThread()
{

}
uint8_t read_slider(int fd);
uint8_t Calute_Speed(unsigned char m, unsigned char n, unsigned char j,unsigned char k);
unsigned char Current_Press_Value = 0xFF;
unsigned char Current_Previous_Press_Value = 0xFF;
unsigned char Previous_Press_Value = 0xFF;
unsigned char Old_Press_Value = 0xFF;
unsigned char Current_Previous_Press_Button_Value = 0xFF;

unsigned char Slow_Scroll_Change_Data = 0x00;
unsigned short Emit_Diff_Value = 0x0000;

unsigned char Quick_Scroll_Change_Data = 0x00;
unsigned char Quick_Scroll_Change_Max  = 0x00;

QQueue<char> Press_Positive_Value_Stored_Queue;
QQueue<char> Press_Negative_Value_Stored_Queue;
QQueue<char> Press_Value_Stored_Queue;


#define  TOUCH_LOW_VALUE    0x02
#define  TOUCH_HIGH_VALUE   0x62

#define  TOUCH_PRESSED_TIME   5     // 5 * 5    25Ms



//#define  Determine_Btn_Press
#define    DETERMINE_BTN_PRESS_LOW
#define    DETERMINE_BTN_PRESS_HIGH

//#define  Cancel_Btn_Press
#define    CANCEL_BTN_PRESS_LOW
#define    CANCEL_BTN_PRESS_HIGH

/*
void IIC_MyThread::Pressd_Timer_Handle()
{
    if(Pressd_Timer->isActive() == true)
    {
        Pressd_Timer->stop();
    }
    Pressd_Timer_Out_Flage = 1;
}
*/


void IIC_MyThread::Touch_Allow_Receive_Handler()
{
    Touch_Allow_Flage = 1;

}

void IIC_MyThread::run()
{
    unsigned int res = 0, len = 0;
    int i = 0;
    int fd = 0;
    unsigned char buff[5] = {0, 0, 0, 0, 0};
    unsigned char Scroll_Quick_Dir_Flage = 0;

    //Pressd_Timer = new QTimer();              //  线程  new QTimer()时不能加this；
    //Pressd_Timer->setSingleShot(true);        //  设置单次计时
    //connect(Pressd_Timer, SIGNAL(timeout()), this, SLOT(Pressd_Timer_Handle()));


    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0)
    {
        printf("[%s]:[%d] open i2c file error\r\n", __FUNCTION__, __LINE__);
        //return -1;
    }
    res = ioctl(fd, I2C_SLAVE, (0x08));     //设置I2C从设备地址

    QThread::msleep(500);

    Set_Param(fd);

    QThread::msleep(5);

    Get_Param(fd);

    while(1)
    {

        Current_Press_Value = read_slider(fd);

        if((Current_Press_Value==255)&&(Current_Previous_Press_Button_Value==255))  //松开状态
        {
            //qDebug()<<"Released";
            Press_Negative_Value_Stored_Queue.clear();
            Press_Positive_Value_Stored_Queue.clear();
            Press_Value_Stored_Queue.clear();
            Emit_Diff_Value = 0;

            Old_Press_Value = 0xFF;
            Previous_Press_Value = 0xFF;
            Current_Previous_Press_Value = 0xFF;

            Diff_Cur_Pos = 2;
            Diff_Pre_Pos = 1;
            Diff_Old_Pos = 1;

            Diff_Cur_Neg = -2;
            Diff_Pre_Neg = -1;
            Diff_Old_Neg = -1;

            Determine_Pressd_Timer_Times = 0;
            Cancel_Pressd_Timer_Times = 0;
            Determine_Pressd_Out_Flage = 0;
            Cancel_Pressd_Out_Flage = 0;
        }
        Current_Previous_Press_Button_Value = Current_Press_Value;

        if(Cancel_Pressd_Out_Flage == 1)
        {
            if((Current_Press_Value >= 1) && (Current_Press_Value <= 32))
            {
                #ifdef  BUTTON_DEBUG
                qDebug()<<"Cancel_Pressd_Timer_Times++";
                #endif
                Cancel_Pressd_Timer_Times++;
                if(Cancel_Pressd_Timer_Times >= TOUCH_PRESSED_TIME)
                {
                    Cancel_Btn_Press = true;
                    Cancel_Pressd_Timer_Times = 0;
                    Cancel_Pressd_Out_Flage = 0;
                    #ifdef  BUTTON_DEBUG
                    qDebug()<<"Cancel_Btn_Press = true";
                    #endif
                }
            }
        }

        if(Determine_Pressd_Out_Flage == 1)
        {
            if((Current_Press_Value >= 68)&&(Current_Press_Value <= 100))
            {
                #ifdef  BUTTON_DEBUG
                qDebug()<<"Determine_Pressd_Timer_Times++";
                #endif
                Determine_Pressd_Timer_Times++;
                if(Determine_Pressd_Timer_Times >= TOUCH_PRESSED_TIME)
                {
                    Determine_Btn_Press = true;
                    Determine_Pressd_Timer_Times = 0;
                    Determine_Pressd_Out_Flage = 0;
                    #ifdef  BUTTON_DEBUG
                    qDebug()<<"Determine_Btn_Press = true";
                    #endif
                }
            }
        }

        if(Current_Press_Value !=  Current_Previous_Press_Value)
        {
            //if(Current_Press_Value != 255)
            {
                //qDebug()<<"I am Runing = "<<Current_Press_Value;
                //qDebug()<<"I am Runing = "<<Current_Press_Value;
                qDebug()<<Current_Press_Value;
            }

            //qDebug()<<"I am Runing = "<<Current_Press_Value;


            if((Current_Press_Value != 255)&&(Current_Previous_Press_Value == 255)) // 原来松开    现在按下
            {
                Set_Diff_Value_Flage = 0;
                Scroll_Allow_Times = 0;
                Emit_Diff_Value = 0x2000;  //突然按下 要停止所有刷新
                Press_Negative_Value_Stored_Queue.clear();
                Press_Positive_Value_Stored_Queue.clear();
                Press_Value_Stored_Queue.clear();


                if((Current_Press_Value > 12) &&(Current_Press_Value < 88))
                {
                    #ifdef  BUTTON_DEBUG
                    qDebug()<<" Set 1-----0--------0---------------";
                    #endif
                    Diff_Cur_Pos = 1;
                    Diff_Pre_Pos = 0;
                    Diff_Old_Pos = 0;

                    Diff_Cur_Neg = -1;
                    Diff_Pre_Neg = 0;
                    Diff_Old_Neg = 0;
                }
                else
                {
                    #ifdef  BUTTON_DEBUG
                    qDebug()<<" Set 5---3--------------2------------";
                    #endif
                    Diff_Cur_Pos = 2;
                    Diff_Pre_Pos = 1;
                    Diff_Old_Pos = 1;

                    Diff_Cur_Neg = -2;
                    Diff_Pre_Neg = -1;
                    Diff_Old_Neg = -1;

                    #ifdef  BUTTON_DEBUG
                    printf("Diff_Set = %d %d %d\n", Diff_Cur_Neg, Diff_Pre_Neg,Diff_Old_Neg);
                    #endif
                }

                    if((Current_Press_Value >= 1) && (Current_Press_Value <= 12))
                    {
                        if(Touch_Allow_Flage == 1)
                        {
                            #ifdef  BUTTON_DEBUG
                            qDebug()<<"1 < press < 30  Touch 1";
                            #endif
                            Cancel_Pressd_Out_Flage = 1;
                        }
                        if(Touch_Allow_Flage == 0)
                        {
                           #ifdef  BUTTON_DEBUG
                           qDebug()<<"1 < press < 30  Touch 0";
                           #endif
                           Cancel_Pressd_Out_Flage = 0;
                           Touch_Allow_Flage = 1;
                           //emit myslot(Emit_Diff_Value);
                        }
                    }
                    else if((Current_Press_Value >= 88) && (Current_Press_Value <= 100))
                    {
                        //qDebug()<<"----press---right_btn_press---x:";
                        if(Touch_Allow_Flage == 1)
                        {
                            #ifdef  BUTTON_DEBUG
                            qDebug()<<"70 < press < 100  Touch 1";
                            #endif
                            Determine_Pressd_Out_Flage = 1;
                        }
                        if(Touch_Allow_Flage == 0)
                        {
                            #ifdef  BUTTON_DEBUG
                            qDebug()<<"70 < press < 100  Touch 0";
                            #endif
                            Determine_Pressd_Out_Flage = 0;
                            Touch_Allow_Flage = 1;
                            //emit myslot(Emit_Diff_Value);
                        }
                    }
                    //qDebug()<<"--Released--press----";
                    emit myslot(Emit_Diff_Value);
                }
                //按下
                //if((Current_Press_Value >= 0x0F)&&(Current_Press_Value <= 0x55)&&(Current_Previous_Press_Value >= 0x0F)&&(Current_Previous_Press_Value <=0x55)&&(Previous_Press_Value>= 0x0F)&&(Previous_Press_Value <= 0x55)&&(Old_Press_Value >= 0x0F)&&(Old_Press_Value <=0x55))  //在滑动区域
                if(abs(Current_Press_Value - Current_Previous_Press_Value) <= 30)
                {
                    if((Current_Press_Value >= TOUCH_LOW_VALUE)&&(Current_Press_Value <= TOUCH_HIGH_VALUE)&&(Current_Previous_Press_Value >= TOUCH_LOW_VALUE)&&(Current_Previous_Press_Value <=TOUCH_HIGH_VALUE)&&(Previous_Press_Value>= TOUCH_LOW_VALUE)&&(Previous_Press_Value <= TOUCH_HIGH_VALUE)&&(Old_Press_Value >= TOUCH_LOW_VALUE)&&(Old_Press_Value <=TOUCH_HIGH_VALUE))  //在滑动区域
                    {

                        //if(Scroll_Allow_Times >= 20)
                        //if((Determine_Btn_Press == false)&&(Cancel_Btn_Press == false))
                        if( (((Current_Press_Value >= 1)&&(Current_Press_Value <= 12)) || ((Current_Press_Value >= 88)&&(Current_Press_Value <= 100))) && (Set_Diff_Value_Flage == 0))
                        {
                            Diff_Cal_Cur = Current_Press_Value - Current_Previous_Press_Value;
                            Diff_Cal_Pre = Current_Previous_Press_Value - Previous_Press_Value;
                            Diff_Cal_Old = Previous_Press_Value - Old_Press_Value;


                            if( ( (Diff_Cal_Cur>=Diff_Cur_Pos) &&(Diff_Cal_Pre>=Diff_Pre_Pos)&&(Diff_Cal_Old>=Diff_Old_Pos) )                \
                              ||( (Diff_Cal_Cur<=Diff_Cur_Neg) &&(Diff_Cal_Pre<=Diff_Pre_Neg)&&(Diff_Cal_Old<=Diff_Old_Neg) ) )
                            {
                                #ifdef  BUTTON_DEBUG
                                if( (Diff_Cal_Cur>=Diff_Cur_Pos) &&(Diff_Cal_Pre>=Diff_Pre_Pos)&&(Diff_Cal_Old>=Diff_Old_Pos) )
                                {
                                    printf("Diff_Set = %d %d %d\n", Diff_Cur_Neg, Diff_Pre_Neg,Diff_Old_Neg);

                                    qDebug()<<"Diff = "<<Current_Press_Value - Current_Previous_Press_Value  \
                                                        <<Current_Previous_Press_Value - Previous_Press_Value \
                                                        <<Previous_Press_Value - Old_Press_Value;

                                    //                            19                        26                          25                   16
                                    qDebug()<<"Value = "<<Current_Press_Value << Current_Previous_Press_Value << Previous_Press_Value<< Old_Press_Value;
                                    qDebug()<<">=5 Button Flase----------------------------";
                                }

                                if( (Diff_Cal_Cur<=Diff_Cur_Neg) &&(Diff_Cal_Pre<=Diff_Pre_Neg)&&(Diff_Cal_Old<=Diff_Old_Neg) )
                                {
                                    printf("Diff_Set = %d %d %d\n", Diff_Cur_Neg, Diff_Pre_Neg,Diff_Old_Neg);

                                    qDebug()<<"Diff = "<<Current_Press_Value - Current_Previous_Press_Value  \
                                                        <<Current_Previous_Press_Value - Previous_Press_Value \
                                                        <<Previous_Press_Value - Old_Press_Value;

                                    //                            19                        26                          25                   16
                                    qDebug()<<"Value = "<<Current_Press_Value << Current_Previous_Press_Value << Previous_Press_Value<< Old_Press_Value;
                                    qDebug()<<"<=5 Button Flase----------------------------";
                                }
                                #endif
                                Determine_Pressd_Timer_Times = 0;
                                Cancel_Pressd_Timer_Times = 0;
                                Determine_Pressd_Out_Flage = 0;
                                Cancel_Pressd_Out_Flage = 0;

                                Determine_Btn_Press = false;
                                Cancel_Btn_Press = false;

                                Set_Diff_Value_Flage = 1;
                                Diff_Cur_Pos = 1;
                                Diff_Pre_Pos = 0;
                                Diff_Old_Pos = 0;

                                Diff_Cur_Neg = -1;
                                Diff_Pre_Neg = 0;
                                Diff_Old_Neg = 0;
                            }
                        }
                        else if( ((Current_Press_Value > 12) && (Current_Press_Value < 88)) && (Set_Diff_Value_Flage == 0) )
                        {
                            Set_Diff_Value_Flage = 1;
                            Diff_Cur_Pos = 1;
                            Diff_Pre_Pos = 0;
                            Diff_Old_Pos = 0;

                            Diff_Cur_Neg = -1;
                            Diff_Pre_Neg = 0;
                            Diff_Old_Neg = 0;
                        }

                        if(((Current_Press_Value - Current_Previous_Press_Value)>=Diff_Cur_Pos)&&(((Current_Previous_Press_Value - Previous_Press_Value)>=Diff_Pre_Pos))&&((Previous_Press_Value - Old_Press_Value)>=Diff_Old_Pos))
                        {
                            Press_Negative_Value_Stored_Queue.clear();

                            Determine_Pressd_Timer_Times = 0;
                            Cancel_Pressd_Timer_Times = 0;
                            Determine_Pressd_Out_Flage = 0;
                            Cancel_Pressd_Out_Flage = 0;

                            Determine_Btn_Press = false;
                            Cancel_Btn_Press = false;

                            if((Current_Press_Value - Current_Previous_Press_Value) >= 0)
                            {
                                Emit_Diff_Value = 0x0000;
                                IIC_Send_Dir_Flage = 0;
                            }
                            //Slow_Scroll_Change_Data = abs(Current_Press_Value - Current_Previous_Press_Value);
                            Slow_Scroll_Change_Data = Calute_Speed(Current_Press_Value, Current_Previous_Press_Value, Previous_Press_Value, Old_Press_Value);
                            if(Press_Positive_Value_Stored_Queue.size() < 8)
                            {
                                Press_Positive_Value_Stored_Queue.enqueue(Slow_Scroll_Change_Data);
                            }
                            else if(Press_Positive_Value_Stored_Queue.size() >= 8)
                            {
                                Press_Positive_Value_Stored_Queue.enqueue(Slow_Scroll_Change_Data);
                                Press_Positive_Value_Stored_Queue.dequeue();
                            }
                            Emit_Diff_Value+= 0x100;
                            Emit_Diff_Value+= Slow_Scroll_Change_Data;
                            //emit myslot(Emit_Diff_Value);
                        }
                        else if(((Current_Press_Value - Current_Previous_Press_Value)<=Diff_Cur_Neg)&&(((Current_Previous_Press_Value - Previous_Press_Value)<=Diff_Pre_Neg))&&((Previous_Press_Value - Old_Press_Value)<=Diff_Old_Neg))
                        {
                            Press_Positive_Value_Stored_Queue.clear();

                            Determine_Pressd_Timer_Times = 0;
                            Cancel_Pressd_Timer_Times = 0;
                            Determine_Pressd_Out_Flage = 0;
                            Cancel_Pressd_Out_Flage = 0;

                            Determine_Btn_Press = false;
                            Cancel_Btn_Press = false;

                            if((Current_Press_Value - Current_Previous_Press_Value) <= 0)
                            {
                                Emit_Diff_Value = 0x1000;
                                IIC_Send_Dir_Flage = 1;
                            }
                            //Slow_Scroll_Change_Data = abs(Current_Press_Value - Current_Previous_Press_Value);
                            Slow_Scroll_Change_Data = Calute_Speed(Current_Press_Value, Current_Previous_Press_Value, Previous_Press_Value, Old_Press_Value);
                            if(Press_Negative_Value_Stored_Queue.size() < 8)
                            {
                                Press_Negative_Value_Stored_Queue.enqueue(Slow_Scroll_Change_Data);
                            }
                            else if(Press_Negative_Value_Stored_Queue.size() >= 8)
                            {
                                Press_Negative_Value_Stored_Queue.enqueue(Slow_Scroll_Change_Data);
                                Press_Negative_Value_Stored_Queue.dequeue();
                            }
                            Emit_Diff_Value+= 0x100;
                            Emit_Diff_Value+= Slow_Scroll_Change_Data;
                            //emit myslot(Emit_Diff_Value);
                        }
                        emit myslot(Emit_Diff_Value);
                        //printf("emit myslot(%x)\r\n", Emit_Diff_Value);
                    }
                }
                //松开  原来不是255按下   现在是255
                //if((Current_Press_Value ==255)&&(Current_Previous_Press_Value != 255))
                if((Current_Press_Value ==255) &&((Current_Previous_Press_Value !=255)||(Previous_Press_Value != 255)))
                //if((Current_Press_Value ==255) &&(Current_Previous_Press_Value !=255)&&(Previous_Press_Value != 255)&&(Old_Press_Value != 255))
                {
                    if((Determine_Btn_Press == false)&&(Cancel_Btn_Press == false))
                    {
                        #ifdef  BUTTON_DEBUG
                        if(Determine_Btn_Press == false)
                        printf("Determine_Btn_Press == false \r\n");

                        if(Cancel_Btn_Press == false)
                        printf("Cancel_Btn_Press == false \r\n");
                        #endif

                        if((Press_Positive_Value_Stored_Queue.size() == 1)||(Press_Negative_Value_Stored_Queue.size() == 1))
                        {
                            if(Press_Positive_Value_Stored_Queue.size() == 1)                     //采集一个
                            {
                                Emit_Diff_Value = 0x0000;
                                IIC_Send_Dir_Flage = 0;
                                Press_Value_Stored_Queue = Press_Positive_Value_Stored_Queue;
                            }
                            else if(Press_Negative_Value_Stored_Queue.size() == 1)
                            {
                                Emit_Diff_Value = 0x1000;
                                IIC_Send_Dir_Flage = 1;
                                Press_Value_Stored_Queue = Press_Negative_Value_Stored_Queue;
                            }
                            if(Press_Value_Stored_Queue[0] <= 4)
                            {
                                Emit_Diff_Value += 0x400;  //慢滑
                                Emit_Diff_Value += 0xFF;
                                //printf("慢滑1 = %x\r\n", Emit_Diff_Value);
                                emit myslot(Emit_Diff_Value);
                            }
                            else
                            {
                                Emit_Diff_Value += 0x400;  //快滑
                                Emit_Diff_Value += Press_Value_Stored_Queue[0];
                                Touch_Allow_Flage = 0;
                                //printf("快滑1 = %x\r\n", Emit_Diff_Value);
                                emit myslot(Emit_Diff_Value);
                            }
                        }
                        else if((Press_Positive_Value_Stored_Queue.size() >= 2)||(Press_Negative_Value_Stored_Queue.size() >= 2))
                        {
                            if(Press_Positive_Value_Stored_Queue.size() >= 2)
                            {
                                Emit_Diff_Value = 0x0000;
                                IIC_Send_Dir_Flage = 0;
                                Press_Value_Stored_Queue = Press_Positive_Value_Stored_Queue;
                            }
                            else if(Press_Negative_Value_Stored_Queue.size() >= 2)
                            {
                                Emit_Diff_Value = 0x1000;
                                IIC_Send_Dir_Flage = 1;
                                Press_Value_Stored_Queue = Press_Negative_Value_Stored_Queue;
                            }

                            if(Press_Value_Stored_Queue.size() < 8)
                            {
                                //qDebug()<<"< 8\r\n";
                                Quick_Scroll_Change_Max = 0;
                                for(int i=0; i<(Press_Value_Stored_Queue.size()-1); i++) // 相邻两个相加
                                {
                                    if((Press_Value_Stored_Queue[i] + Press_Value_Stored_Queue[i+1]) >= 5) // 快滑
                                    {
                                        Emit_Diff_Value += 0x400;
                                        Quick_Scroll_Change_Data = (Press_Value_Stored_Queue[i] + Press_Value_Stored_Queue[i+1]);
                                        if(Quick_Scroll_Change_Max < Quick_Scroll_Change_Data)
                                        {
                                            Quick_Scroll_Change_Max = Quick_Scroll_Change_Data;
                                        }
                                        Emit_Diff_Value += Quick_Scroll_Change_Max;
                                        Touch_Allow_Flage = 0;
                                        //printf("快滑2 = %x\r\n", Emit_Diff_Value);
                                        emit myslot(Emit_Diff_Value);
                                        break;
                                    }
                                }
                                if((Emit_Diff_Value&0x400)< 0x400)  //慢滑松手
                                {
                                    Emit_Diff_Value += 0x400;
                                    Emit_Diff_Value += 0xFF;
                                    //printf("慢滑2 = %x\r\n", Emit_Diff_Value);
                                    emit myslot(Emit_Diff_Value);
                                }
                            }
                            if(Press_Value_Stored_Queue.size() >= 8)
                            {
                                for(int i=0; i<Press_Value_Stored_Queue.size()-2; i++)
                                {
                                    Quick_Scroll_Change_Data+=Press_Value_Stored_Queue.dequeue();
                                }
                                //qDebug()<<"8 == 8 "<<Quick_Scroll_Change_Data;
                                //Quick_Scroll_Change_Data = Quick_Scroll_Change_Data - 12;
                                if(Quick_Scroll_Change_Data > 12)  //快滑松手
                                {
                                    Quick_Scroll_Change_Data = Quick_Scroll_Change_Data-12;
                                    Emit_Diff_Value += 0x400;
                                    Emit_Diff_Value += Quick_Scroll_Change_Data;
                                    Touch_Allow_Flage = 0;
                                    //printf("快滑8 = %x\r\n", Emit_Diff_Value);
                                    emit myslot(Emit_Diff_Value);
                                }
                                else
                                {
                                    Emit_Diff_Value += 0x400;
                                    Emit_Diff_Value += 0xFF;
                                    //printf("慢滑8 = %x\r\n", Emit_Diff_Value);
                                    emit myslot(Emit_Diff_Value);
                                }
                            }
                            Quick_Scroll_Change_Data = 0;
                            Press_Value_Stored_Queue.clear();
                        }
                        else  //慢滑松手    //记录上一次方向
                        {
                            if((Press_Positive_Value_Stored_Queue.size() == 0)&&(Press_Negative_Value_Stored_Queue.size() == 0))
                            {
                                Press_Positive_Value_Stored_Queue.clear();
                                Press_Negative_Value_Stored_Queue.clear();

                                if(IIC_Send_Dir_Flage == 0)       // +++
                                {
                                    Emit_Diff_Value = 0x0000;
                                }
                                else if(IIC_Send_Dir_Flage == 1)  // ---
                                {
                                    Emit_Diff_Value = 0x1000;
                                }
                                Emit_Diff_Value += 0x400;
                                Emit_Diff_Value += 0xFF;
                            }
                            //printf("慢滑 Last_Dir\r\n");
                            emit myslot(Emit_Diff_Value);
                        }
                    }
                    else if(Determine_Btn_Press == true)  // //突然按下 要停止所有刷新
                    {
                        qDebug()<<"Determine_Btn_Press";
                        Emit_Diff_Value = 0x4000;
                        emit myslot(Emit_Diff_Value);   //确定
                        Determine_Btn_Press = false;
                        Determine_Pressd_Timer_Times = 0;
                        Cancel_Pressd_Timer_Times = 0;
                        Determine_Pressd_Out_Flage = 0;
                        Cancel_Pressd_Out_Flage = 0;
                    }
                    else if(Cancel_Btn_Press == true)   //突然按下 要停止所有刷新
                    {
                         qDebug()<<"Cancel_Btn_Press";
                         Emit_Diff_Value = 0x8000;
                         emit myslot(Emit_Diff_Value);  //取消
                         Cancel_Btn_Press = false;
                         Determine_Pressd_Timer_Times = 0;
                         Cancel_Pressd_Timer_Times = 0;
                         Determine_Pressd_Out_Flage = 0;
                         Cancel_Pressd_Out_Flage = 0;
                    }
                }

                Old_Press_Value = Previous_Press_Value;
                Previous_Press_Value = Current_Previous_Press_Value;
                Current_Previous_Press_Value = Current_Press_Value;
                Emit_Diff_Value = 0;
        }
        QThread::msleep(5);
    }
    //qDebug()<<"I am Runing";
}


uint8_t crc8(const uint8_t* data, uint32_t length)
{
    uint8_t i;
    uint32_t num = 0;
    uint8_t crc = 0;    // Initial value

    if (length <= 0)
    return 0;

    while (length-- > 0)
    {
        crc ^= data[num++]; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x80) > 0)
            {
                crc = (uint8_t)((crc << 1) ^ 0x07);
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}


uint8_t read_slider(int fd)
{
    int len = 0, count=0, i=0;
    uint8_t sed_buff[16] = {0};
    uint8_t rev_buff[16] = {0};
    //	printf("================  read_slider =============\n");
    sed_buff[count++] = 0x0;
    sed_buff[count++] = 0xaa;
    sed_buff[count++] = 2;
    sed_buff[count++] = 0x01;

    sed_buff[2] = count-2;
    sed_buff[count++] = crc8(sed_buff+2, count-3);

    len = write(fd, sed_buff, count);
    if (len < 0)
    {
        printf("write data addr failed \n");
        return -1;
    }
    //printf("sed read slider \n");
    usleep(1000);// 1ms

    len = read(fd, rev_buff, 6);
    if (len < 0)
    {
        printf("read data failed \n");
        return -1;
    }

    return rev_buff[4];
}

uint8_t Max_Three(unsigned char i, unsigned char j, unsigned char k)
{
    uint8_t tmp = 0;
    tmp = i;

    if(j > tmp)
    {
        tmp = j;
    }

    if(k > tmp)
    {
        tmp = k;
    }
    return tmp;
}

uint8_t Calute_Speed(unsigned char m, unsigned char n, unsigned char j,unsigned char k)
{
    unsigned char Diff0, Diff1, Diff2;
    unsigned char tmp;
    Diff0 = abs(m - n);
    Diff1 = abs(n - j);
    Diff2 = abs(j - k);

    if(Max_Three(Diff0,Diff1,Diff2) == 1)
    {
        return 1;
    }
    else
    {
       tmp = Diff0+Diff1+Diff2;
       tmp = tmp - Max_Three(Diff0,Diff1,Diff2);
       return tmp/2;
    }
}

uint8_t Set_Param(int fd)
{
    int len = 0, count=0, i=0;
    uint8_t sed_buff[16] = {0};
    uint8_t rev_buff[16] = {0};

    //finger_th    越小越灵敏
    uint16_t finger_th = 600;
    printf("================  set_param =============\n");
    sed_buff[count++] = 0x00;
    sed_buff[count++] = 0xAA;
    sed_buff[count++] = 2;
    sed_buff[count++] = SET_SLIDER_PARAM_CMD;

    sed_buff[count++] = (uint8_t)(finger_th &0x00FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((finger_th>>8) &0x00FF);   //低位在前，高位在后
    sed_buff[count++] = 100;   //低位在前，高位在后
    sed_buff[count++] = 75;   //低位在前，高位在后
    sed_buff[count++] = 30;   //低位在前，高位在后
    sed_buff[count++] = 6;   //低位在前，高位在后
    sed_buff[count++] = 3;   //低位在前，高位在后


    sed_buff[2] = count-2;
    sed_buff[count++] = crc8(sed_buff+2, count-3);

    len = write(fd, sed_buff, count);
    if (len < 0)
    {
        printf("write data addr failed \n");
        return -1;
    }
    printf("sed read slider \n");
    usleep(50000); //必须要加长延时 至少50ms以上


    len = read(fd, rev_buff, 5);
    if (len < 0)
    {
        printf("read data failed \n");
        return -1;
    }
    else
    {
        for(i = 0; i < len; i++)
        {
            printf("0x%02X  ", rev_buff[i]);
        }
        printf("\n");
    }

    return rev_buff[3];
}
/*
struct
{
    uint8_t sof; //帧起始
    uint8_t len; //长度    包含：cmd + buff + crc
    uint8_t cmd; //设置触摸值
    uint8_t buff[7]; // FINGER_TH(2Byte) NOISE_TH(1Byte)  NNOISE_TH(1Byte) HYSTERESIS(1B)  ON_DEBOUNCE(1B) LOW_BSLN_RST(1B)
    uint8_t crc;
}setParam;
*/
uint8_t Get_Param(int fd)
{
    int len = 0, count=0, i=0;
    uint8_t sed_buff[16] = {0};
    uint8_t rev_buff[16] = {0};

    printf("================  get_param =============\n");
    sed_buff[count++] = 0x00;
    sed_buff[count++] = 0xAA;
    sed_buff[count++] = 2;
    sed_buff[count++] = GET_SLIDER_PARAM_CMD;

    sed_buff[2] = count-2;
    sed_buff[count++] = crc8(sed_buff+2, count-3);

    len = write(fd, sed_buff, count);
    if (len < 0)
    {
        printf("write data addr failed \n");
        return -1;
    }
    printf("sed read slider \n");
    usleep(1000);


    len = read(fd, rev_buff, 11);
    if(len < 0)
    {
        printf("read data failed \n");
        return -1;
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            printf("0x%02X  ", rev_buff[i]);
        }
        printf("\n");
    }
    return rev_buff[3];
}




uint8_t Set_Sys_Param(int fd)
{
    int len = 0, count=0, i=0;
    uint8_t sed_buff[16] = {0};
    uint8_t rev_buff[16] = {0};
    uint32_t key_value = 0x011F;

    printf("================  set_sys_param =============\n");
    sed_buff[count++] = 0x00;
    sed_buff[count++] = 0xAA;
    sed_buff[count++] = 2;
    sed_buff[count++] = SET_SYS_ID_PARAM_CMD;

    sed_buff[count++] = OPT_KEY_N;

    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>0) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>8) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>16) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>24) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = OPT_KEY_Y;   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((key_value>>0) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((key_value>>8) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((key_value>>16) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((key_value>>24) &0x000000FF);   //低位在前，高位在后

    sed_buff[count++] = 0;

    sed_buff[2] = count-2;
    sed_buff[count++] = crc8(sed_buff+2, count-3);

    len = write(fd, sed_buff, count);
    if(len < 0)
    {
        printf("write data addr failed \n");
        return -1;
    }
    printf("sed read slider \n");
    usleep(50000); //必须要加长延时 至少50ms以上


    len = read(fd, rev_buff, 5);
    if(len < 0)
    {
        printf("read data failed \n");
        return -1;
    }
    else
    {
        for(i = 0; i < len; i++)
        {
            printf("0x%02X  ", rev_buff[i]);
        }
        printf("\n");
    }
    return rev_buff[3];
}


uint8_t Get_Sys_Param(int fd)
{
    int len = 0, count=0, i=0;
    uint8_t sed_buff[16] = {0};
    uint8_t rev_buff[16] = {0};
    uint32_t key_value = 0x011F;

    printf("================  Get_sys_param =============\n");
    sed_buff[count++] = 0x00;
    sed_buff[count++] = 0xAA;
    sed_buff[count++] = 2;
    sed_buff[count++] = GET_SYS_ID_PARAM_CMD;

    sed_buff[count++] = OPT_KEY_N;

    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>0) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>8) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>16) &0x000000FF);   //低位在前，高位在后
    sed_buff[count++] = (uint8_t)((CapSense_SLD_FINGER_TH_PARAM_ID>>24) &0x000000FF);   //低位在前，高位在后


    sed_buff[2] = count-2;
    sed_buff[count++] = crc8(sed_buff+2, count-3);

    len = write(fd, sed_buff, count);
    if(len < 0)
    {
        printf("write data addr failed \n");
        return -1;
    }

    printf("sed read slider \n");
    usleep(50000); //必须要加长延时 至少50ms以上


    len = read(fd, rev_buff, 9);
    if(len < 0)
    {
        printf("read data failed \n");
        return -1;
    }
    else
    {
        for(i = 0; i < len; i++)
        {
            printf("0x%02X  ", rev_buff[i]);
        }
        printf("\n");
    }
    return rev_buff[3];
}



















