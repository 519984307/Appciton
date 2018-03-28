#pragma once
#include "SummaryItem.h"
#include "SystemDefine.h"

class CodeMarkerSummaryItem: public SummaryItem
{
public:
    ~CodeMarkerSummaryItem() { delete _summary;}
    //override, this summary is complete immediately
    bool isCompleted() const { return true; }
    //override, return the summary type
    SummaryType type() const { return (SummaryType) _summary->itemDesc.type;}
    //override, return the time this summary create
    unsigned time() const { return _summary->itemDesc.time;}
    //override, get the summary data pointer
    char * getSummaryData() const { return (char *) _summary;}
    //override, the the summary data length
    int summaryDataLength() const { return sizeof(SummaryCodeMarker);}
    //override, get the cache wave length, always 0
    int getCacheWaveLength(WaveformID ) const { return 0;}
    //override, update the checksum
    void updateChecksum() { _summary->sum();}
    //override, do nothing
    void startWaveCache() {}
    //override, do nothing
    void stopWaveCache() {}
    //create a instance
    static CodeMarkerSummaryItem *create(unsigned time, const char *codeName);

#ifdef CONFIG_UNIT_TEST
    friend class TestCodeMarker;
#endif
private:
    CodeMarkerSummaryItem(SummaryCodeMarker * summarydata);
    SummaryCodeMarker *_summary;
};
