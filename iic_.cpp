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

IIC_MyThread::IIC_MyThread(QObject *parent)
{
    val = 0xFF;
}

IIC_MyThread::~IIC_MyThread()
{

}

void IIC_MyThread::Press_Value_Changed( int v )
{
    val = v;
}

void IIC_MyThread::run()
{
    unsigned int res = 0, len = 0;
    int i = 0;
    int fd = 0;
    char buff[5] = {0, 0, 0, 0, 0};

    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0)
    {
        printf("[%s]:[%d] open i2c file error\r\n", __FUNCTION__, __LINE__);
        //return -1;
    }
    res = ioctl(fd, I2C_SLAVE, (0x08));     //设置I2C从设备地址

    while(1)
    {

        len = read(fd, buff, sizeof(buff));
        if(len < 0)
        {
            printf("read data failed \n");
            //return -1;
        }
        else
        {
            for(i=0; i<sizeof(buff); i++)
            {
                printf("0x%02X  ", buff[i]);
            }
            printf("\n");
        }
    }
    qDebug()<<"I am Runing";
    int v_temp = buff[1];
    if(v_temp != val)
    {
        emit Press_Value_Changed(v_temp);
    }
    sleep(5);

    //emit isDone();  //发送完成信号
}
