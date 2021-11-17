#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>

#if QT_VERSION >= 0x040400
#include <QAtomicInt>
#else
#define QAtomicInt int
#endif

/**
 * Thread-safe ring buffer - one thread produces and another consumes.
 * A third thread reads maximum
 */
class RingBuffer : public QObject
{
  private:
    unsigned char *ringBuf;
    unsigned long ringBufSize;
    unsigned long fillRateMax;
    unsigned long writeOfs;
    unsigned long readOfs;
    QAtomicInt fillRate;

    int frameCounter;
    QMutex waitForDataMutex;
    QWaitCondition waitForData;

    int channels, sampleSize;
    QVector<QAtomicInt> max;
    void updateMax(long startpos, long endpos);
  public:
    RingBuffer(long size, int channelCount, bool enable32bit);
    ~RingBuffer();
    void reset();
    long readMax(int channelIndex);
    unsigned long getFillRate() { return fillRate; }
    unsigned long getFillRateMax() { return fillRateMax; }

    unsigned char* beginWrite(long& curPos, long& size);
    void endWrite(long curPos);
    unsigned char* beginRead(long& curPos, long& size, bool waitIfEmpty);
    void endRead(long curPos);
    void discardBuffer();

};

#endif

