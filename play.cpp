#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "play.h"

Play::Play(SettingsData * settings)
{
	channels 		= settings->getChannels();
	samplesize 	= settings->getSampleSize();
        framesize       = settings->getFrameSize();
	rate			= settings->getRate();
	periods		= settings->getAlsaPeriods();
	periodsize	= settings->getAlsaPeriodsize();

	pcmName	= settings->getAlsaPcmName();

        play_handle     = NULL;

}

Play::~Play()
{
	doPlay = false;
	if(play_handle)
	{
            snd_pcm_close(play_handle);
            play_handle = NULL;
	}
}

snd_pcm_t *Play::open_pcm()
{
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_uframes_t tmpbuffersize;
        unsigned int tmprate;
	
	int err;

	 err = snd_pcm_open(&pcm_handle, qPrintable(pcmName), SND_PCM_STREAM_PLAYBACK, 0); 

	 if (err < 0) {
        	qWarning("Error opening PCM device %s (%s) for play", qPrintable(pcmName), snd_strerror(err));
        	return(NULL);
	 }

	 snd_pcm_hw_params_alloca(&hwparams);
	 
    	 err = snd_pcm_hw_params_any(pcm_handle, hwparams);
	 if (err < 0) {
        	qWarning("Can not configure this PCM device (%s) for play.", snd_strerror(err));
        	snd_pcm_close(pcm_handle);
        	return(NULL);
    	}

	err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
       	qWarning("Error setting access (%s) interleaved for play.", snd_strerror(err));
        	snd_pcm_close(pcm_handle);
        	return(NULL);
    	}

	if(samplesize == 2)
	{
		err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE);	
		if (err < 0) {
            		qWarning("Error setting format (%s) for play.", snd_strerror(err));
			snd_pcm_close(pcm_handle);
            		return(NULL);
        	}		
	}
	else
	{
		err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S32_LE);
		if (err < 0) {
            		qWarning("Error setting format (%s) for play.", snd_strerror(err));
			snd_pcm_close(pcm_handle);
            		return(NULL);
        	}		
	}

	err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels);
	if (err < 0) {
        	qWarning("Error setting channels (%s) for play.", snd_strerror(err));
		snd_pcm_close(pcm_handle);
        	return(NULL);
    	}

	tmprate = (unsigned int)rate;
	err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &tmprate, 0);
	if (err < 0) {
        	qWarning("Error setting rate (%s) for play.", snd_strerror(err));
        	snd_pcm_close(pcm_handle);
        	return(NULL);
    	}


	tmpbuffersize = periodsize * periods;
	err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams, &tmpbuffersize);
	if (err < 0) {
        	qWarning("Error setting buffersize (%s) for play.", snd_strerror(err));
        	snd_pcm_close(pcm_handle);
        	return(NULL);
    	}

        if(tmpbuffersize != (snd_pcm_uframes_t)(periods * periodsize))
	{
		periodsize = tmpbuffersize/periods;
	}


	err = snd_pcm_hw_params(pcm_handle, hwparams);
	if (err < 0) {
        	qWarning("Error setting HW params (%s) for play.", snd_strerror(err));
        	snd_pcm_close(pcm_handle);
        	return(NULL);
    	}

        return pcm_handle;
}

void Play::run()
{
    play_handle = open_pcm();

    if(play_handle != NULL)
    {
        doPlay = true;
    }
    else
    {
         doPlay = false;
         qWarning("Error can't open pcm device for play.");
    }

    unsigned char audioBuf[4096*4];
    pWavFile = fopen(wavFileName.toLatin1(), "rb");
    if(pWavFile == NULL)
    {
        doPlay = false;
        qWarning("Wav File Open Error");
        return;
    }

    // read the file header of wav file
    fread(audioBuf, 1, 44, pWavFile);

    int size = 0;
    while(doPlay)
    {
        size = fread(audioBuf, 1, 4096*4, pWavFile);
        if(size <= 0)
        {
            break;
        }
        else
        {
            Playback(audioBuf, size);
        }
    }

    snd_pcm_drain(play_handle);

    fclose(pWavFile);
    qWarning("play wav file over");
    snd_pcm_close(play_handle);
    play_handle = NULL;
    return;
}

void Play::SetWavFileName(QString strWavFileName)
{
    wavFileName = strWavFileName;
}

void Play::Stop()
{
	doPlay = false;
}

void Play::Playback(unsigned char* buffer, int size)
{
        if(buffer && size>0)
	{
            snd_pcm_uframes_t ufcount = (snd_pcm_uframes_t)(size/framesize);
            snd_pcm_sframes_t sfcount = 0;
            snd_pcm_sframes_t sfeach = 0;

            while(sfcount < (snd_pcm_sframes_t) ufcount)
            {
                while((sfeach = snd_pcm_writei(play_handle, buffer+sfcount*framesize, (ufcount-sfcount)))<0)
                {
                    snd_pcm_prepare(play_handle);
                }

                if(sfeach == 0)
                {
                    qWarning("Error play device (%s) sfeach = %d",snd_strerror(sfeach), (int)sfeach);
                    break;
                }
                else
                {
                    sfcount += sfeach;
                }
            }
	}
        else
        {
            qWarning("Error writing data to playback device.");
        }

		
}


