#pragma once
#include "SummaryItem.h"

class PrinterActionSummaryItem : public CommonSummaryItem
{
public:
    ~PrinterActionSummaryItem();
    //override, update summary checksum
    void updateChecksum();
    //override, get the summary data length
    int summaryDataLength() const;

    // override, check completed
    bool isCompleted() const;

    // override, get current cache wave length
    int getCacheWaveLength(WaveformID id) const;

    //create a instance
    static PrinterActionSummaryItem *create(unsigned time);

protected:
    // override, start caching wave
    void startWaveCache();
    // override, stop caching wave
    void stopWaveCache();

private:
    PrinterActionSummaryItem(SummaryPrinterAction *summarydata);
    PrinterActionSummaryItem(SummaryPrinterAction2Trace *summarydata);
    PrinterActionSummaryItem(SummaryPrinterAction3Trace *summarydata);
    PrinterActionSummaryItem(SummaryPrinterAction4Trace *summarydata);
    SummaryPrinterAction *_summary;
    SummaryPrinterAction2Trace *_summary2Trace;
    SummaryPrinterAction3Trace *_summary3Trace;
    SummaryPrinterAction4Trace *_summary4Trace;
};
