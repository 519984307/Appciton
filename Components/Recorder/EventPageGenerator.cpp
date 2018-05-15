#include "EventPageGenerator.h"
#include "PatientManager.h"

class EventPageGeneratorPrivate
{
public:
    EventPageGeneratorPrivate()
        : curPageType(RecordPageGenerator::TitlePage),
          timestamp(0),
          curDrawWaveSegment(0),
          totalDrawWaveSegment(0)
    {

    }

    RecordPageGenerator::PageType curPageType;
    unsigned timestamp;
    QString eventTitle;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
    int totalDrawWaveSegment;
};

EventPageGenerator::EventPageGenerator(QObject *parent)
    :d_ptr(new EventPageGeneratorPrivate)
{

}

EventPageGenerator::~EventPageGenerator()
{

}

int EventPageGenerator::type() const
{
    return Type;
}

RecordPage *EventPageGenerator::createPage()
{
    switch (d_ptr->curPageType) {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        return createTitlePage(d_ptr->eventTitle, patientManager.getPatientInfo(), d_ptr->timestamp);
    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->trendData, true);
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());

    case WaveSegmentPage:
    {
        RecordPage *page = NULL;

        if(recorderManager.isAbort())
        {
            // already stop
            return NULL;
        }
        page = createWaveSegments(d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());

        if(d_ptr->curDrawWaveSegment >= d_ptr->totalDrawWaveSegment)
        {
            d_ptr->curPageType = EndPage;
        }
        return page;
    }

    default:
        break;
    }

    return NULL;
}
