#ifndef PLAY_H
#define PLAY_H

#include <QString>
#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>

#include <alsa/asoundlib.h>

#include "settings.h"
#include "ringbuffer.h"

class Play : public QThread
{
private:
	snd_pcm_t *play_handle;
	bool doPlay;
	QString pcmName;
        QString wavFileName;
        FILE* pWavFile;

	int samplesize;
	int framesize;
	int channels;
	int rate;
	int periods;
	int periodsize;
	
private:
	snd_pcm_t *open_pcm();
	
public:
	Play(SettingsData * settings);
	~Play();
        virtual void run();
        //void Start();
	void Stop();
	void Playback(unsigned char* buffer, int size);
        void SetWavFileName(QString strWavFileName);
};


#endif
