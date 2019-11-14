/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/1/16
 **/

#pragma once
#include  "EventDataDefine.h"
#include "Framework/Storage/IStorageBackend.h"

struct EventDataPraseContext
{
    EventDataPraseContext();

    void reset();

    bool parse(IStorageBackend *backend, int index);

    ~EventDataPraseContext();

    char *eventDataBuf;  // buffer for the event data
    EventInfoSegment *infoSegment;  // pointor of the info segment
    TrendDataSegment *trendSegment;  // pointer of the trend segment
    QVector<WaveformDataSegment *> waveSegments;    // pointers of the wave segments
    AlarmInfoSegment *almSegment;   // pointer to the alarm segment
    CodeMarkerSegment *codeMarkerSegment;   // pointer to the code marker segment
    NIBPMeasureSegment *measureSegment;   // pointer to the nibp measure segment
};
