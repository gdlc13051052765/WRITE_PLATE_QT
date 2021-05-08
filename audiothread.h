#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include "audiothread.h"
#include <QtCore>
#include <QObject>
//#include <QtMultimedia/QSoundEffect>
#include <QSoundEffect>
#include <QThread>

enum VOICE_TYPE
{
    VOICE_POSITIVE,
    VOICE_NEGATIVE,
    VOICE_TEMPERATURE
};

class AudioThread : public QObject
{
    Q_OBJECT
public:
    AudioThread(QObject *parent = 0);
    ~AudioThread();

signals:
    void doPlay();

public slots:
    void play();
    //void play(unsigned char type);

private:
    int voice_type;
    volatile unsigned char Voice_Play_times;
    QSoundEffect effect;

    QThread thread;

    unsigned char Source_Flage;

};

#endif // DLTHREAD_H
