#include "ringbuffer.h"

RingBuffer::RingBuffer(long size, int channelCount, bool enable32bit)
{
    sampleSize = enable32bit ? 4 : 2;
    channels = channelCount;
    ringBufSize = size;

    if (ringBufSize % (sampleSize * channels) != 0) {
        qCritical("Ringbuf size must be multiple of frame size (%d)\n", 
          sampleSize * channels);
        exit(1);
    }
    ringBuf = (unsigned char *)malloc(ringBufSize);
    if (ringBuf == NULL) {
        qCritical("Out of memory");
        exit(1);
    }

    reset();
}

RingBuffer::~RingBuffer()
{
    free(ringBuf);
}

void RingBuffer::reset()
{
    max = QVector<QAtomicInt>(channels);
    for (int i=0; i < channels; i++) {
        max[i] = 0;
    }
    frameCounter = 0;
    fillRateMax = 0;
    readOfs = 0;
    writeOfs = 0;
#if QT_VERSION >= 0x040400
    fillRate.fetchAndStoreOrdered(0);
#else
    q_atomic_set_int(&fillRate, 0);
#endif
}

unsigned char* RingBuffer::beginWrite(long& curPos, long& size)
{
    size = ringBufSize;
    curPos = writeOfs;
    return ringBuf;
}

void RingBuffer::endWrite(long curPos)
{
    long oldPos, nframes;

    curPos %= ringBufSize;

    oldPos = writeOfs;
    writeOfs = curPos;

    updateMax(oldPos, curPos);

    // Update fill rate
    nframes = curPos - oldPos;
    if (nframes < 0) {
        nframes += ringBufSize;
    }

#if QT_VERSION >= 0x040400
    unsigned long temp = fillRate.fetchAndAddRelease(nframes);
#else
    unsigned long temp;
    do {
        temp = fillRate;
    } while (!q_atomic_test_and_set_release_int(&fillRate, temp, temp+nframes));
#endif
    temp += nframes;
    if (fillRateMax < temp) {
        fillRateMax = temp;
    }

    // Wake read thread if we have written at least 4096 frames
    nframes /= (channels * sampleSize);
    frameCounter += nframes;

    if (frameCounter >= 4096) {
        waitForData.wakeOne();
        frameCounter = 0;
    }
}

void RingBuffer::updateMax(long startpos, long endpos)
{
    int framesize = channels * sampleSize;
    long curMax[channels];
    for (int i=0; i < channels; i++) {
        curMax[i] = 0;
    }

    if (startpos % framesize != 0 || endpos % framesize != 0) {
        qWarning("Internal error: not even frames written!");
        startpos -= startpos % framesize;
        endpos -= endpos % framesize;
    }

    for (; startpos != endpos; startpos = (startpos + framesize) % ringBufSize) {
        for (int i=0; i < channels; i++) {
            long curs;
            if (sampleSize == 4) {
                curs = ringBuf[startpos+i*sampleSize] + 
                    (ringBuf[startpos+i*sampleSize+1] << 8) +
                    (ringBuf[startpos+i*sampleSize+2] << 16) +
                    (ringBuf[startpos+i*sampleSize+3] << 24);
            } else {
                signed short temp;
                temp = ringBuf[startpos+i*sampleSize] + 
                    (ringBuf[startpos+i*sampleSize+1] << 8);
                curs = temp;
            }
            if (curs < 0) curs = -curs;
            if (curMax[i] < curs) curMax[i] = curs;
        }
    }

    for (int i=0; i < channels; i++) {
        /* Actually, we don't need to be atomic here */
        int q = max[i];
        if (curMax[i] > q) {
            max[i] = curMax[i];
        }
    }
}

/**
 * @param waitIfEmpty if true, waits up to 1000 ms if buffer is empty. 
 * @param size amount of data in buffer (can be zero)
 */
unsigned char* RingBuffer::beginRead(long& curPos, long& size, bool waitIfEmpty)
{
    unsigned long fRate;

#if QT_VERSION >= 0x040400
    fRate = fillRate.fetchAndAddOrdered(0);
#else
    q_atomic_test_and_set_acquire_int(&fillRate, 0, 0); // Just for the memory barrier
    fRate = fillRate;
#endif

    if (waitIfEmpty && fRate == 0) {
        waitForDataMutex.lock();
        waitForData.wait(&waitForDataMutex, 1000);
        waitForDataMutex.unlock();

#if QT_VERSION >= 0x040400
        fRate = fillRate.fetchAndAddOrdered(0);
#else
        q_atomic_test_and_set_acquire_int(&fillRate, 0, 0); // Just for the memory barrier
        fRate = fillRate;
#endif
    }

    if (fRate+readOfs > ringBufSize) {
        fRate = ringBufSize-readOfs;
    }
    size = fRate;
    curPos = readOfs;
    return ringBuf;
}

void RingBuffer::endRead(long curPos)
{
    curPos %= ringBufSize;

    long nframes = curPos - readOfs;
    if (nframes < 0) {
        nframes += ringBufSize;
    }

#if QT_VERSION >= 0x040400
    fillRate.fetchAndAddRelaxed(-nframes);
#else
    unsigned long temp;
    do {
        temp = fillRate;
    } while (!q_atomic_test_and_set_int(&fillRate, temp, temp-nframes));
#endif
    readOfs = curPos;
}


long RingBuffer::readMax(int channelIndex)
{
#if QT_VERSION >= 0x040400
    return max[channelIndex].fetchAndStoreRelaxed(0);
#else
    return q_atomic_set_int(&max[channelIndex], 0);
#endif
}

void RingBuffer::discardBuffer()
{
    fillRateMax = 0;
#if QT_VERSION >= 0x040400
    readOfs += fillRate.fetchAndStoreOrdered(0);
#else
    readOfs += q_atomic_set_int(&fillRate, 0);
#endif
}

