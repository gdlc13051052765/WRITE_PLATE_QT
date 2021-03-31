#include "led.h"

//构造函数
Led::Led()
{
    system("echo 8   > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio8/direction");
}
Led::~Led()
{

}

void Led::Led_On(void)
{
    system("echo 1   > /sys/class/gpio/gpio8/value");
}

void Led::Led_Off(void)
{
    system("echo 0   > /sys/class/gpio/gpio8/value");
}
