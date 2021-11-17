#include <stdlib.h>
#include <getopt.h>

#include "settings.h"
//#include "mainwindow.h"


static struct option options[] =
        {{"help", 0, 0, 'h'},
         {"version", 0, 0, 'v'},
         {"device", 1, 0, 'd'},
         {"buffersize", 1, 0, 'b'},
         {"channels", 1, 0, 'c'},
         {"fragments", 1, 0, 'f'},
         {"meterrange", 1, 0, 'e'},
         {"periodsize", 1, 0, 'p'},
         {"midiChannel", 1, 0, 'm'},
         {"midiNote", 1, 0, 'n'},
         {"jack", 0, 0, 'j'},
         {"rate", 1, 0, 'r'},
         {"32bit", 0, 0, 'i'},
         {"split", 1, 0, 's'},
         {0, 0, 0, 0}};

SettingsData::SettingsData()
{
    setDefault();
}

static int checkRange(const char* n, int val, int minval, int maxval)
{
    if (val < minval) {
         qWarning("Changed %s to %d (valid range is %d to %d)", n,
                 minval, minval, maxval);
         return minval;
    }
    if (val > maxval) {
         qWarning("Changed %s to %d (valid range is %d to %d)", n,
                 maxval, minval, maxval);
         return maxval;
    }
    return val;
}

void SettingsData::validateSettings()
{
    channels = checkRange("channels", channels, 1, SHRT_MAX);

    ringBufSize = checkRange("buffersize", ringBufSize, 4096 * getFrameSize(),
            INT_MAX / 2);

    if (ringBufSize % getFrameSize() != 0) {
        ringBufSize += getFrameSize() - ringBufSize % getFrameSize();
        qWarning("Changed buffersize to %d (must be multiple of frame size)",
                (int) ringBufSize);
    }

    midiChannel = checkRange("midiChannel", midiChannel, 0, 15);
    midiNote = checkRange("midiNote", midiNote, 0, 127);
    meterrange = checkRange("meterrange", meterrange, 12, 192);
    splitMB = checkRange("split", splitMB, 1, 4095);

    /* Alsa parameters are checked in the alsa capture driver */
}


void SettingsData::setDefault()
{
    ringBufSize = 1048576;
    enableJack = false;
    enable32bit = false;
    midiChannel = 15;
    midiNote = 21;
    channels = 2;
    meterrange = 85;
    splitMB = 2000;

    rate = 44100;
    alsaPeriods = 2;
    alsaPeriodsize = 2048;
    alsaPcmName = "default";   //"plughw:0,0";
}

int SettingsData::parseCmdLine(int argc, char *argv[])
{
    int getopt_return;
    int option_index; 

    while((getopt_return = getopt_long(argc, argv, "jhvid:b:f:p:m:n:r:c:e:s:",
        options, &option_index)) >= 0) 
    {
        switch(getopt_return) {
            case 'd':
                alsaPcmName = QString(optarg);
                break;
            case 'b':
                ringBufSize = atoi(optarg);
                break;
            case 'c':
                channels = atoi(optarg);
                break;
            case 'e':
                meterrange = atoi(optarg);
                break;
            case 'f':
                alsaPeriods = atoi(optarg);
                break;
            case 'p':
                alsaPeriodsize = atoi(optarg);
                break;
            case 'm':
                midiChannel = atoi(optarg);
                midiChannel--;
                break;
            case 's':
                splitMB = atoi(optarg);
                break;
            case 'r':
                rate = atoi(optarg);
                break;
            case 'j': 
                enableJack = true;
                break;
            case 'i': 
                enable32bit = true;
                break;
            case 'n':
                midiNote = atoi(optarg);
                break;
            case 'v':
               
                return 0;
            case 'h':
                SettingsData def;
               
                printf("General options:\n");
                printf("--32bit                     Use 32 Bit format\n");
                printf("--buffersize <bytes>        Size of ringbuffer [%ld]\n",
                    def.ringBufSize);
                printf("--channels <num>            Channels [%d]\n",
                        def.channels + 1);
                printf("--jack                      Enable JACK mode\n");
                printf("--meterrange <dB>           Dynamic range of peak meter [%d]\n",
                    def.meterrange);
                printf("--midiChannel <num>         MIDI Channel [%d]\n",
                        def.midiChannel);
                printf("--midiNote <num>            MIDI Note [%d]\n",
                        def.midiNote);
                printf("--split <MB>                Maximum size of wave file [%d]\n\n",
                    def.splitMB);
                printf("ALSA specific options:\n");
                printf("--device <ALSA device>      ALSA Capture device [%s]\n",
                    qPrintable(def.alsaPcmName));
                printf("--fragments <num>           Number of fragments [%d]\n",
                    def.alsaPeriods);
                printf("--periodsize <frames>       Periodsize [%d]\n",
                        def.alsaPeriodsize);
                printf("--rate <num>                Sample rate [%d]\n\n",
                        def.rate);
                printf("Standard options:\n");
                printf("--help                      Print possible command-line options "
                    "and exit\n");
                printf("--version                   Print version information and exit\n\n");
                return 0;
        }
    }
    validateSettings();
    return 1;
}

