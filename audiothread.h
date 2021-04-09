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
    AudioThread(QObject *parent = nullptr);
    ~AudioThread();

signals:
    void doPlay();

public slots:
    void play();
    void play(int type);

private:
    int voice_type;
    volatile unsigned char Voice_Play_times;
    QSoundEffect effect;

    QThread thread;

};

#endif // DLTHREAD_H
