#include "EventDataParseContext.h"
#include "Debug.h"

EventDataPraseContext::EventDataPraseContext()
    :eventDataBuf(NULL),
      infoSegment(NULL),
      trendSegment(NULL),
      almSegment(NULL),
      codeMarkerSegment(NULL)
{
}

void EventDataPraseContext::reset()
{
    if(eventDataBuf)
    {
        qFree(eventDataBuf);
        eventDataBuf = NULL;
    }
    infoSegment = NULL;
    trendSegment = NULL;
    waveSegments.clear();
    almSegment = NULL;
    codeMarkerSegment = NULL;
}

bool EventDataPraseContext::parse(IStorageBackend *backend, int index)
{
    if(!backend || index >= (int) backend->getBlockNR() || index < 0)
    {
        return false;
    }

    quint32 length  = backend->getBlockDataLen(index);

    char *buf = (char *)qMalloc(length);
    if(!buf)
    {
        return false;
    }

    if (backend->readBlockData(index, buf, length) != length)
    {
        qFree(buf);
        return false;
    }

    reset();

    eventDataBuf = buf;

    char *parseBuffer = buf;
    bool parseEnd = false;
    while(!parseEnd)
    {
        EventSegmentType *eventType = (EventSegmentType *)parseBuffer;
        switch (*eventType) {
        case EVENT_INFO_SEGMENT:
            //skip the offset of the segment type field
            parseBuffer += sizeof(EventSegmentType);
            infoSegment = (EventInfoSegment *) parseBuffer;
            //find the location of the next event type
            parseBuffer += sizeof(EventInfoSegment);
            break;
        case EVENT_TRENDDATA_SEGMENT:
            //skip the offset of the segment type field
            parseBuffer += sizeof(EventSegmentType);
            trendSegment = (TrendDataSegment *)parseBuffer;
            //find the location of the next event type
            parseBuffer += sizeof(TrendDataSegment) + trendSegment->trendValueNum * sizeof(TrendValueSegment);
            break;
        case EVENT_WAVEFORM_SEGMENT:
            //skip the offset of the segment type field
            parseBuffer += sizeof(EventSegmentType);
            waveSegments.append((WaveformDataSegment *) parseBuffer);
            //find the location of the next event type
            parseBuffer += sizeof(WaveformDataSegment) + waveSegments.last()->waveNum * sizeof(WaveDataType);
            break;
        case EVENT_ALARM_INFO_SEGMENT:
            parseBuffer += sizeof(EventSegmentType);
            almSegment = (AlarmInfoSegment *) parseBuffer;
            parseBuffer += sizeof(AlarmInfoSegment);
            break;
        case EVENT_CODEMARKER_SEGMENT:
            parseBuffer += sizeof(EventSegmentType);
            codeMarkerSegment = (CodeMarkerSegment *) parseBuffer;
            parseBuffer += sizeof(CodeMarkerSegment);
            break;
        default:
            qdebug("unknown segment type %d, stop parsing.",  *eventType);
            parseEnd = true;
            break;
        }
    }

    if(parseBuffer >= buf + length)
    {
        parseEnd = true;
    }
    return true;
}

EventDataPraseContext::~EventDataPraseContext()
{
    if(eventDataBuf)
    {
        qFree(eventDataBuf);
    }
}
