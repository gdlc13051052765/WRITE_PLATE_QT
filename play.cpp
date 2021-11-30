#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "play.h"

#define u32 unsigned int
#define u8  unsigned char
#define u16 unsigned short
/*wav文件头格式*/
typedef  struct	{
	u8    	RiffID [4];
	u32     RiffSize;
	u8    	WaveID[4];
	u8    	FmtID[4];
	u32     FmtSize;
	u16   	wFormatTag;
	u16   	nChannels;
	u32 	nSamplesPerSec;  /*采样频率*/
	u32 	nAvgBytesPerSec; /*每秒所需字节数*/
	u16		nBlockAlign; /*数据块对齐单位,每个采样需要的字节数*/
	u16		wBitsPerSample;/*每个采样需要的bit数*/
	u8		DataID[4];
	u32 	nDataBytes;
} WAVE_HEADER;

WAVE_HEADER g_wave_header;

/*读取wav文件头格式*/
int Play:: open_and_print_file_params(char *file_name)
{
	FILE * fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		printf("can't open wav file\n");
		return NULL;
	}
	
	memset(&g_wave_header, 0, sizeof(g_wave_header));
	fread(&g_wave_header, 1, sizeof(g_wave_header), fp);
	
	printf("RiffID:%c%c%c%c\n", g_wave_header.RiffID[0], g_wave_header.RiffID[1], g_wave_header.RiffID[2], g_wave_header.RiffID[3]);
	printf("RiffSize:%d\n", g_wave_header.RiffSize);
	printf("WaveID:%c%c%c%c\n", g_wave_header.WaveID[0], g_wave_header.WaveID[1], g_wave_header.WaveID[2], g_wave_header.WaveID[3]);
	printf("FmtID:%c%c%c%c\n", g_wave_header.FmtID[0], g_wave_header.FmtID[1], g_wave_header.FmtID[2], g_wave_header.FmtID[3]);
	printf("FmtSize:%d\n", g_wave_header.FmtSize);
	printf("wFormatTag:%d\n", g_wave_header.wFormatTag);
	printf("nChannels:%d\n", g_wave_header.nChannels);
	printf("nSamplesPerSec:%d\n", g_wave_header.nSamplesPerSec);
	printf("nAvgBytesPerSec:%d\n", g_wave_header.nAvgBytesPerSec);
	printf("nBlockAlign:%d\n", g_wave_header.nBlockAlign);
	printf("wBitsPerSample:%d\n", g_wave_header.wBitsPerSample);
	printf("DataID:%c%c%c%c\n", g_wave_header.DataID[0], g_wave_header.DataID[1], g_wave_header.DataID[2], g_wave_header.DataID[3]);
	printf("nDataBytes:%d\n", g_wave_header.nDataBytes);
	
	return 0;
}
Play::Play(SettingsData * settings)
{
	channels 	= 2;
	samplesize 	= 2;
    framesize   = 1048576;
	//rate		= 80000;
	rate		= 44100;
	periods		= 2;
	periodsize	= 2048;
	qWarning("pcmName = %s \n\n",pcmName);
	//pcmName	= settings->getAlsaPcmName();
	pcmName = "hw:0,0";
    play_handle = NULL;

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
	
	if(play_handle !=NULL) 
		return(NULL);
	//err = snd_pcm_open(&pcm_handle, qPrintable(pcmName), SND_PCM_STREAM_PLAYBACK, 0); 
	err = snd_pcm_open(&pcm_handle, "hw:0,0", SND_PCM_STREAM_PLAYBACK, 0); 
	if (err < 0) {
		qWarning("Error opening PCM device %s (%s) for play", qPrintable(pcmName), snd_strerror(err));
		fclose(pWavFile);
    	snd_pcm_close(pcm_handle);
    	play_handle = NULL;
		pcm_handle = NULL;
		snd_pcm_open(&pcm_handle, "hw:0,0", SND_PCM_STREAM_PLAYBACK, 0); 
		
		//return(NULL);
	}
	//分配一个硬件参数对象
	snd_pcm_hw_params_alloca(&hwparams);
	//向对象填充默认值
    err = snd_pcm_hw_params_any(pcm_handle, hwparams);
	if (err < 0) {
		qWarning("Can not configure this PCM device (%s) for play.", snd_strerror(err));
		snd_pcm_close(pcm_handle);
		return(NULL);
	}
	//设置交错模式
	err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
       	qWarning("Error setting access (%s) interleaved for play.", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }
	//设置格式
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
	//设置通道
	err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels);
	if (err < 0) {
        qWarning("Error setting channels (%s) for play.", snd_strerror(err));
		snd_pcm_close(pcm_handle);
        return(NULL);
    }
	//设置采用率
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
	//将上面设置的参数传给驱动，只有经过这步，上面设置的参数才有效
	err = snd_pcm_hw_params(pcm_handle, hwparams);
	if (err < 0) {
        qWarning("Error setting HW params (%s) for play.", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }
	char *gp_buffer;
	snd_pcm_uframes_t g_frames;    //snd_pcm_uframes_t其实是unsigned long类型
	snd_pcm_hw_params_get_period_size(hwparams, &g_frames, 0);
	framesize = g_frames * 4;
	gp_buffer = (char *)malloc(framesize);
	if (gp_buffer == NULL)
	{
		printf("malloc failed\n");
		//goto err1;  
	}
    return pcm_handle;
}

void Play::run()
{
    // play_handle = open_pcm();

    if(play_handle != NULL)
    {
        doPlay = true;
    }
    // else
    // {
    //     doPlay = false;
    //     qWarning("Error can't open pcm device for play.");
    // }

    unsigned char audioBuf[4096*10];
    pWavFile = fopen(wavFileName.toLatin1(), "rb");
    if(pWavFile == NULL)
    {
        doPlay = false;
        qWarning("Wav File Open Error");
        return;
    }

    // read the file header of wav file
    fread(audioBuf, 1, 44, pWavFile);
	//qWarning("audioBuf = %S................ \n",audioBuf);
    int size = 0;
    while(doPlay)
    {
        size = fread(audioBuf, 1, 4096*10, pWavFile);
		//qWarning("audioBuf size = %d........ \n",size);
        if(size <= 0) {
            break;
        } else {
            Playback(audioBuf, size);
        }
    }
    snd_pcm_drain(play_handle);
    fclose(pWavFile);
    //qWarning("play wav file over....\n");
    // snd_pcm_close(play_handle);
    // play_handle = NULL;
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
    if(buffer && size>0) {
		snd_pcm_uframes_t ufcount = (snd_pcm_uframes_t)(size/framesize);
		snd_pcm_sframes_t sfcount = 0;
		snd_pcm_sframes_t sfeach = 0;

		while(sfcount < (snd_pcm_sframes_t) ufcount)
		{
			while((sfeach = snd_pcm_writei(play_handle, buffer+sfcount*framesize, (ufcount-sfcount)))<0)
			{
				snd_pcm_prepare(play_handle);
			}

			if(sfeach == 0) {
				qWarning("Error play device (%s) sfeach = %d",snd_strerror(sfeach), (int)sfeach);
				break;
			} else {
				sfcount += sfeach;
			}
		}
	} else {
		qWarning("Error writing data to playback device.");
	}		
}
