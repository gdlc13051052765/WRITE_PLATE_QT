#include "audiothread.h"
#include <QUrl>
#include <QDebug>

AudioThread::AudioThread(QObject *parent) :
    QObject(parent)
{
    Voice_Play_times = 0;
    Source_Flage = 0;
    effect.moveToThread(&thread);

    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Quick.wav"));
    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Original.wav"));
    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/test.wav"));
    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Chage_Formt.wav"));
    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Chage_Formt_4.wav"));

    effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Chage_Formt_Double.wav"));

    effect.setVolume(0.8f);
    effect.setLoopCount(0);

    connect(this, SIGNAL(doPlay()), &effect, SLOT(play()));
    thread.start();
}

AudioThread::~AudioThread()
{
    if(thread.isRunning())
    {
        thread.terminate();
    }
}

void AudioThread::play()
{
    emit this->doPlay();
}


/*
-----------   255   ------------------
*/
/*
void AudioThread::play(unsigned char type)
{
    unsigned char  Play_Type = type;

    if(Play_Type == 0)        //while(1)
    {
        if(Source_Flage == 1)
        {
            //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Chage_Formt_Double.wav"));
            Source_Flage = 0;
        }
        effect.setLoopCount(0);
        emit this->doPlay();
    }
    else if(Play_Type == 1)   //while(1)
    {
        if(Source_Flage == 0)
        {
            //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Chage_Formt_4.wav"));
            Source_Flage = 1;
        }

        //setLoopCount(QSoundEffect::Infinite);注释掉只播放一次，打开循环播放
        effect.setLoopCount(QSoundEffect::Infinite);
        emit this->doPlay();
    }
}
*/

