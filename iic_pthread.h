#ifndef IIC_PTHREAD_H
#define IIC_PTHREAD_H


#include <QObject>
#include <QThread>
#include <QTime>
#include <QTimer>


#define CapSense_SLD_FINGER_TH_PARAM_ID                     (0xAB800020u)
#define OPT_KEY_N       0x11
#define OPT_KEY_Y       0xEE

#define MAX_FRAME_NUM   16

typedef  unsigned char uint8_t;

//帧格式： sof(1B)+cmd(1B)+len(1B)+data(最大16)+crc8
typedef union
{
    struct
    {
        uint8_t sof; //帧起始
        uint8_t len;  //长度  包含：cmd + buff + crc
        uint8_t cmd; //指令
        uint8_t buff[MAX_FRAME_NUM-4]; //保留
        uint8_t crc;    //crc校验值 包含：len+cmd+buff
    }common;

    struct
    {
      uint8_t sof; //帧起始
      uint8_t len;  //长度  包含：cmd + buff + crc
      uint8_t cmd; //指令
      uint8_t buff[2]; //状态  + 有效值 0~100 (无触摸为0xFF)
      uint8_t crc;   //crc校验值 包含：len+cmd+buff
    }slider;

    struct
    {
      uint8_t sof; //帧起始
      uint8_t len;  //长度    包含：cmd + buff + crc
      uint8_t cmd; //设置触摸值
      uint8_t buff[7]; // FINGER_TH(2Byte) NOISE_TH(1Byte)  NNOISE_TH(1Byte) HYSTERESIS(1B)  ON_DEBOUNCE(1B) LOW_BSLN_RST(1B)
      uint8_t crc;
    }setParam;

    struct
    {
        uint8_t sof; //帧起始
        uint8_t len;  //长度  包含：cmd + buff + crc
        uint8_t cmd; //指令
        uint8_t buff[1]; //保留
        uint8_t crc;     //crc校验值 包含：len+cmd+buff
    }ack;

    struct
    {
        uint8_t sof; //帧起始
        uint8_t len;  //长度  包含：cmd + buff + crc
        uint8_t cmd; //指令
        uint8_t crc;     //crc校验值 包含：len+cmd+buff
    }cmd;
    uint8_t buff[MAX_FRAME_NUM];
}Comm_Frame,*_pComm_Frame;

typedef enum
{
    GET_SLIDER_VALUE_CMD = 0x01,    //获取滑条有效值
    SET_SLIDER_PARAM_CMD = 0x02,    //设置参数
    GET_SLIDER_PARAM_CMD = 0x03,    //获取配置参数

    SET_SYS_ID_PARAM_CMD = 0x08,    //根据系统ID表进行参数修改
    GET_SYS_ID_PARAM_CMD = 0x09,    //根据系统ID表获取配置参数
    RESET_SLIDER_CMD     = 0x0a,    //使用新参数重启senor
}HostCmd;


class IIC_MyThread: public QThread
{
    Q_OBJECT
public:
    explicit IIC_MyThread(QObject *parent = 0);
    ~IIC_MyThread();
    int get_touch_value();//获取当前按键值
    void aplay_touch_voice();

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

    volatile int Diff_Cur_Pos;
    volatile int Diff_Pre_Pos;
    volatile int Diff_Old_Pos;

    volatile int Diff_Cur_Neg;
    volatile int Diff_Pre_Neg;
    volatile int Diff_Old_Neg;

    volatile int Diff_Cal_Cur;
    volatile int Diff_Cal_Pre;
    volatile int Diff_Cal_Old;


    unsigned char Total_Page;
    unsigned char Current_Page;

    unsigned char Touch_Allow_Flage;

    unsigned char Scroll_Allow_Times;

    unsigned char Set_Diff_Value_Flage;

    unsigned char IIC_Send_Dir_Flage;
    unsigned short Determine_Pressd_Timer_Times;
    unsigned short Cancel_Pressd_Timer_Times;
    unsigned char Determine_Pressd_Out_Flage;
    unsigned char Cancel_Pressd_Out_Flage;
    QTimer *Pressd_Timer;   //按下定时器

private slots:
    void Touch_Allow_Receive_Handler();
//private slots:
//    void Pressd_Timer_Handle();
};

uint8_t set_touch_config_param(void);
uint8_t Set_Sys_Param(int fd);

uint8_t Get_Sys_Param(int fd);

uint8_t Get_Param(int fd);

uint8_t Set_Param(int fd);





























#endif // IIC_PTHREAD_H

