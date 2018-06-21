#include "TriggerPageGenerator.h"
#include "Utility.h"

class TriggerPageGeneratorPrivate
{
public:
    TriggerPageGeneratorPrivate(EventStorageItem *item)
        :curPageType(RecordPageGenerator::TrendPage),
          item(item),
          curDrawWaveSegment(0)
    {
        Q_ASSERT(item != NULL);

        waveInfos = RecordPageGenerator::getWaveInfos(item->getStoreWaveforms());
    }

    bool fetchWaveData();

    RecordPageGenerator::PageType curPageType;
    EventStorageItem *item;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
};


bool TriggerPageGeneratorPrivate::fetchWaveData()
{
    QList<RecordWaveSegmentInfo>::iterator iter;
    for(iter = waveInfos.begin(); iter < waveInfos.end(); iter++)
    {
        if(iter->secondWaveBuff.size() != iter->sampleRate)
        {
            iter->secondWaveBuff.resize(iter->sampleRate);
        }

        int retryCount = 0;
        while(curDrawWaveSegment >= item->getCurWaveCacheDuration(iter->id))
        {
            if (recorderManager.isAbort())
            {
                // print abort
                return false;
            }

            if(++retryCount >= 220) //wait more than 1 second
            {
                break;
            }

            Util::waitInEventLoop(5);
        }

        if(curDrawWaveSegment < item->getCurWaveCacheDuration(iter->id))
        {
            item->getOneSecondWaveform(iter->id, iter->secondWaveBuff.data(), curDrawWaveSegment);
        }
        else
        {
            //fill with invalid data
            iter->secondWaveBuff.fill((WaveDataType)INVALID_WAVE_FALG_BIT);
        }
    }

    return true;
}

TriggerPageGenerator::TriggerPageGenerator(EventStorageItem *item,
                                           QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new TriggerPageGeneratorPrivate(item))
{
    connect(this, SIGNAL(stopped()), item, SLOT(onTriggerPrintStopped()));
}

TriggerPageGenerator::~TriggerPageGenerator()
{

}

int TriggerPageGenerator::type() const
{
    return Type;
}

RecordPageGenerator::PrintPriority TriggerPageGenerator::getPriority() const
{
    return PriorityTrigger;
}

RecordPage *TriggerPageGenerator::createPage()
{
    switch (d_ptr->curPageType) {
    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->item->getTrendData(), true,
                               QString(),
                               d_ptr->item->getEventTitle());

    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());

    case WaveSegmentPage:
        if(d_ptr->curDrawWaveSegment < d_ptr->item->getTotalWaveCacheDuration())
        {
            if(!d_ptr->fetchWaveData())
            {
                // fail to fetch wave data
                return NULL;
            }
            return createWaveSegments(d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());
        }
        //fall through

    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}

