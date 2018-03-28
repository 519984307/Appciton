#pragma once
#include <SummaryItem.h>
#include <NIBPDefine.h>

class NIBPSummaryItem: public SummaryItem
{
public:
    ~NIBPSummaryItem() {delete _summary;}
    //override, this summary is complete immediately
    bool isCompleted() const { return true; }
    //override, return the summary type
    SummaryType type() const { return (SummaryType) _summary->itemDesc.type;}
    //override, return the time this summary create
    unsigned time() const { return _summary->itemDesc.time;}
    //override, get the summary data pointer
    char * getSummaryData() const { return (char *) _summary;}
    //override, the the summary data length
    int summaryDataLength() const { return sizeof(SummaryNIBP);}
    //override, get the cache wave length, always 0
    int getCacheWaveLength(WaveformID ) const { return 0;}
    //override, update the checksum
    void updateChecksum() { _summary->sum();}
    //override, do nothing
    void startWaveCache() {}
    //override, do nothing
    void stopWaveCache() {}
    //create a instance
    static NIBPSummaryItem *create(unsigned time, NIBPOneShotType error);
private:
    NIBPSummaryItem(SummaryNIBP *summarydata);
    SummaryNIBP *_summary;
};
