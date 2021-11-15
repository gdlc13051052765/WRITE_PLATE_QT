#ifndef APLAY_PTHREAD_H
#define APLAY_PTHREAD_H


#include <QtCore>
#include <QObject>
#include <QSoundEffect>
#include <QThread>
#include <QSound>
#include <QMediaPlayer>
#include <QMediaPlaylist>


class SoundThread : public QObject
{
    Q_OBJECT
public:
    SoundThread(QObject *parent = 0);
    ~SoundThread();

signals:
    void doPlay();

public slots:
    void play();

private:
    int voice_type;

    QSound *  p_Sound;
    QThread thread;

};

#endif // DLTHREAD_H
