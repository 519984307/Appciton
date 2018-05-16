#pragma once
#include  "EventDataDefine.h"
#include "IStorageBackend.h"

struct EventDataPraseContext
{
    EventDataPraseContext();

    void reset();

    bool parse(IStorageBackend *backend, int index);

    ~EventDataPraseContext();

    char *eventDataBuf; //buffer for the event data
    EventInfoSegment *infoSegment;  //pointor of the info segment
    TrendDataSegment *trendSegment; //pointer of the trend segment
    QVector<WaveformDataSegment *> waveSegments;    //pointers of the wave segments
    AlarmInfoSegment *almSegment;   //pointer to the alarm segment
    CodeMarkerSegment *codeMarkerSegment;   //pointer to the code marker segment
};
