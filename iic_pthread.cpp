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

IIC_MyThread::IIC_MyThread(QObject *parent)
{
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

unsigned char Slow_Scroll_Change_Data = 0x00;
unsigned short Emit_Diff_Value = 0x0000;

unsigned char Quick_Scroll_Change_Data = 0x00;
unsigned char Quick_Scroll_Change_Max  = 0x00;

QQueue<char> Press_Positive_Value_Stored_Queue;
QQueue<char> Press_Negative_Value_Stored_Queue;
QQueue<char> Press_Value_Stored_Queue;


#define  TOUCH_LOW_VALUE    0x02
#define  TOUCH_HIGH_VALUE   0x62

void IIC_MyThread::run()
{
    unsigned int res = 0, len = 0;
    int i = 0;
    int fd = 0;
    unsigned char buff[5] = {0, 0, 0, 0, 0};
    unsigned char Scroll_Quick_Dir_Flage = 0;



    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0)
    {
        printf("[%s]:[%d] open i2c file error\r\n", __FUNCTION__, __LINE__);
        //return -1;
    }
    res = ioctl(fd, I2C_SLAVE, (0x08));     //设置I2C从设备地址

    while(1)
    {
        Current_Press_Value = read_slider(fd);
        if((Current_Press_Value==255)&&(Current_Previous_Press_Value==255))  //在滑动区域
        {
            Press_Negative_Value_Stored_Queue.clear();
            Press_Positive_Value_Stored_Queue.clear();
            Press_Value_Stored_Queue.clear();
            Emit_Diff_Value = 0;
            Scroll_Quick_Dir_Flage = 0xFF;
        }


        if(Current_Press_Value !=  Current_Previous_Press_Value)
        {
        //按下
        //if((Current_Press_Value >= 0x0F)&&(Current_Press_Value <= 0x55)&&(Current_Previous_Press_Value >= 0x0F)&&(Current_Previous_Press_Value <=0x55)&&(Previous_Press_Value>= 0x0F)&&(Previous_Press_Value <= 0x55)&&(Old_Press_Value >= 0x0F)&&(Old_Press_Value <=0x55))  //在滑动区域
        if((Current_Press_Value >= TOUCH_LOW_VALUE)&&(Current_Press_Value <= TOUCH_HIGH_VALUE)&&(Current_Previous_Press_Value >= TOUCH_LOW_VALUE)&&(Current_Previous_Press_Value <=TOUCH_HIGH_VALUE)&&(Previous_Press_Value>= TOUCH_LOW_VALUE)&&(Previous_Press_Value <= TOUCH_HIGH_VALUE)&&(Old_Press_Value >= TOUCH_LOW_VALUE)&&(Old_Press_Value <=TOUCH_HIGH_VALUE))  //在滑动区域
        {
            Emit_Diff_Value = 0x8000;
            if(((Current_Press_Value - Current_Previous_Press_Value)>=1)&&(((Current_Previous_Press_Value - Previous_Press_Value)>=0))&&((Previous_Press_Value - Old_Press_Value)>=0))
            {
                Press_Negative_Value_Stored_Queue.clear();

                if((Current_Press_Value - Current_Previous_Press_Value) >= 0)
                {
                    Emit_Diff_Value = 0x0000;
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
                Scroll_Quick_Dir_Flage = 0;
                //emit myslot(Emit_Diff_Value);
            }
            else if(((Current_Press_Value - Current_Previous_Press_Value)<=-1)&&(((Current_Previous_Press_Value - Previous_Press_Value)<=0))&&((Previous_Press_Value - Old_Press_Value)<=0))
            {
                Press_Positive_Value_Stored_Queue.clear();
                if((Current_Press_Value - Current_Previous_Press_Value) <= 0)
                {
                    Emit_Diff_Value = 0x1000;
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
                Scroll_Quick_Dir_Flage = 1;
                //emit myslot(Emit_Diff_Value);
            }
            emit myslot(Emit_Diff_Value);
            //printf("thread= %x \r\n", Current_Press_Value);
        }

        //松开  原来不是255按下   现在是255
        if((Current_Press_Value ==255) &&(Current_Previous_Press_Value !=255)&&(Previous_Press_Value != 255)&&(Old_Press_Value != 255))
        //if((Current_Press_Value ==255) &&(Current_Previous_Press_Value ==255)&&(Previous_Press_Value != 255)&&(Old_Press_Value != 255))
        //if(((Current_Press_Value < 0x0F)||(Current_Press_Value > 0x55))&&((Current_Previous_Press_Value < 0x0F)||(Current_Previous_Press_Value > 0x55)) &&(Previous_Press_Value>= 0x0F)&&(Previous_Press_Value <= 0x55)&&(Old_Press_Value >= 0x0F)&&(Old_Press_Value <=0x55))  //在滑动区域
        //if(((Current_Press_Value < 0x0F)||(Current_Press_Value > 0x55))||((Current_Previous_Press_Value < 0x0F)||(Current_Previous_Press_Value > 0x55)) &&(Previous_Press_Value>= 0x0F)&&(Previous_Press_Value <= 0x55)&&(Old_Press_Value >= 0x0F)&&(Old_Press_Value <=0x55))  //在滑动区域
        {
            printf("255  255--------------\r\n");
            if((Press_Positive_Value_Stored_Queue.size() == 1)||(Press_Negative_Value_Stored_Queue.size() == 1))
            {
                if(Press_Positive_Value_Stored_Queue.size() == 1)
                {
                    Emit_Diff_Value = 0x0000;
                    Press_Value_Stored_Queue = Press_Positive_Value_Stored_Queue;
                }
                else if(Press_Negative_Value_Stored_Queue.size() == 1)
                {
                    Emit_Diff_Value = 0x1000;
                    Press_Value_Stored_Queue = Press_Negative_Value_Stored_Queue;
                }
                if(Press_Value_Stored_Queue[0] >= 4)
                {
                    Emit_Diff_Value += 0x400;  //慢滑
                    Emit_Diff_Value += 0xFF;
                    printf("慢滑1 = %x\r\n", Emit_Diff_Value);
                    emit myslot(Emit_Diff_Value);
                }
                else
                {
                    Emit_Diff_Value += 0x400;  //快滑
                    Emit_Diff_Value += Press_Value_Stored_Queue[0];
                    printf("快滑1 = %x\r\n", Emit_Diff_Value);
                    emit myslot(Emit_Diff_Value);
                }
                //采集一个暂不考虑
            }
            else if((Press_Positive_Value_Stored_Queue.size() >= 2)||(Press_Negative_Value_Stored_Queue.size() >= 2))
            {
                if(Press_Positive_Value_Stored_Queue.size() >= 2)
                {
                    Emit_Diff_Value = 0x0000;
                    Press_Value_Stored_Queue = Press_Positive_Value_Stored_Queue;
                }
                else if(Press_Negative_Value_Stored_Queue.size() >= 2)
                {
                    Emit_Diff_Value = 0x1000;
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

                            printf("快滑2 = %x\r\n", Emit_Diff_Value);
                            emit myslot(Emit_Diff_Value);
                            break;
                        }
                    }
                    if((Emit_Diff_Value&0x400)< 0x400)  //慢滑松手
                    {
                        Emit_Diff_Value += 0x400;
                        Emit_Diff_Value += 0xFF;
                        printf("慢滑2 = %x\r\n", Emit_Diff_Value);
                        emit myslot(Emit_Diff_Value);
                    }
                }
                if(Press_Value_Stored_Queue.size() >= 8)
                {
                    for(int i=0; i<Press_Value_Stored_Queue.size()-2; i++)
                    {
                        Quick_Scroll_Change_Data+=Press_Value_Stored_Queue.dequeue();
                    }
                    qDebug()<<"8 == 8 "<<Quick_Scroll_Change_Data;
                    if(Quick_Scroll_Change_Data >= 16)  //快滑松手
                    {
                        Emit_Diff_Value += 0x400;
                        Quick_Scroll_Change_Data = Quick_Scroll_Change_Data - 12;
                        Emit_Diff_Value = Quick_Scroll_Change_Data;
                        printf("快滑8 = %x\r\n", Emit_Diff_Value);
                        emit myslot(Emit_Diff_Value);
                    }
                    else
                    {
                        Emit_Diff_Value += 0x400;
                        Emit_Diff_Value += 0xFF;
                        printf("慢滑8 = %x\r\n", Emit_Diff_Value);
                        emit myslot(Emit_Diff_Value);
                    }
                }
                Quick_Scroll_Change_Data = 0;
                Press_Value_Stored_Queue.clear();
            }
            else  //慢滑松手  靠近最近那个
            {
                if(Press_Positive_Value_Stored_Queue.size() >0)
                {
                    Press_Positive_Value_Stored_Queue.clear();
                    Emit_Diff_Value = 0x0000;
                    Emit_Diff_Value += 0x400;
                    Emit_Diff_Value += 0xFF;
                    emit myslot(Emit_Diff_Value);
                }
                if(Press_Negative_Value_Stored_Queue.size() >0)
                {
                    Press_Negative_Value_Stored_Queue.clear();
                    Emit_Diff_Value = 0x1000;
                    Emit_Diff_Value += 0x400;
                    Emit_Diff_Value += 0xFF;
                    emit myslot(Emit_Diff_Value);
                }
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

