#include "aplay_pthread.h"
#include <QUrl>
#include <QDebug>
#include <alsa/asoundlib.h>
#include <math.h>
#include <inttypes.h>

SoundThread::SoundThread(QObject *parent) :
    QObject(parent)
{
    // QMediaPlayer
    p_Sound = new QSound("/home/meican/Alarm.wav");
    //p_Sound = new QSound("/home/Wolf/Alarm.wav");
    p_Sound->moveToThread(&thread);

    //MyTest160.wav
    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/MyTest78.wav"));
    p_Sound->setLoops(1);

    connect(this, SIGNAL(doPlay()), p_Sound, SLOT(play()));
    thread.start();
}

SoundThread::~SoundThread()
{
    if(thread.isRunning())
    {
        thread.terminate();
    }
}

void SoundThread::play()
{
    //QThread::msleep(5);
    //emit this->doPlay();
}



