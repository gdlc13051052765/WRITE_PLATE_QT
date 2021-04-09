#include "audiothread.h"
#include <QUrl>

AudioThread::AudioThread(QObject *parent) :
    QObject(parent)
{
    Voice_Play_times = 0;
    effect.moveToThread(&thread);

    //effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Quick.wav"));
    effect.setSource(QUrl::fromLocalFile("/home/Wolf/Alarm_Original.wav"));

    effect.setVolume(1.0f);

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
    if(!effect.isPlaying())
    {
        effect.setVolume(1.0f);
        effect.setLoopCount(0);
        emit this->doPlay();
    }
}
/*
255
*/
void AudioThread::play(int type)
{
    unsigned char  Play_Times = type;
    unsigned char  Record = 0;
    if(Play_Times >= 255)  //
    {
        effect.setLoopCount(0);
        effect.setVolume(0.0f);
        return ;
    }
    if(Play_Times < 255)
    {
        if(!effect.isPlaying()) //停止
        {
           effect.setVolume(1.0f);
           effect.setLoopCount(0);
           emit this->doPlay();
        }
        else if(effect.isPlaying()) //运行
        {
            effect.setVolume(1.0f);
            Record = effect.loopCount();
            effect.setLoopCount(Record+1);
            if(!effect.isPlaying())
            {
                emit this->doPlay();
            }

        }
    }
}

