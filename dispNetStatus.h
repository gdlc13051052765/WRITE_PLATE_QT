#ifndef DISPSTRING_H
#define DISPSTRING_H

#include <QDialog> 
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMutex>
#include <QList>
#include <QQueue>
#include "led.h"
#include "music.h"
#include "audiothread.h"
#include "aplay_pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>


class DispNetStatus : public QWidget
{
    Q_OBJECT

    public:
        DispNetStatus(QWidget *parent = 0);
        ~DispNetStatus();
        void set_disp_content(QString str, bool fullScreen);
    protected:
        void paintEvent(QPaintEvent *); //重绘函数
    private:
        QFont itemFont;         //菜单字体样式
};

#endif // MUSIC_H
