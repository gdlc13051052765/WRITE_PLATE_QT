#ifndef SETTINGS_H
#define SETTINGS_H

#include <qobject.h>
#include <qstring.h>

class SettingsData : public QObject
{
  private:
    unsigned long ringBufSize;
    int rate;
    int channels;
    bool enableJack;
    bool enable32bit;
    int midiChannel;
    int midiNote;
    int meterrange;
    int splitMB;

    int alsaPeriods;
    int alsaPeriodsize;
    QString alsaPcmName;

    void validateSettings();
  public:
    SettingsData();
    void setDefault();
    /**
     * @return 0 if the program should exit (--help and --version options),
     * nonzero otherwise
     */
    int parseCmdLine(int argc, char *argv[]);

    // Property accessors
    unsigned long getRingBufSize() { return ringBufSize; }
    int getRate() const { return rate; }
    void setRate(int r) { rate = r; } 
    int getChannels() const { return channels; }
    bool getEnableJack() const { return enableJack; }
    bool getEnable32bit() const { return enable32bit; }
    int getMidiChannel() const { return midiChannel; }
    int getMidiNote() const { return midiNote; }
    int getFrameSize() const { return channels * (enable32bit ? 4 : 2); }
    int getSampleSize() const { return (enable32bit ? 4 : 2); }
    int getMeterRange() const { return meterrange; }
    int getSplitMB() const { return splitMB; }

    int getAlsaPeriods() const { return alsaPeriods; }
    int getAlsaPeriodsize() const { return alsaPeriodsize; }
    QString getAlsaPcmName() const { return alsaPcmName; }

};


#endif
