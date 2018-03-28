#pragma once
#include "SummaryItem.h"

class PresentingRhythmSummaryItem : public CommonSummaryItem
{
public:
    ~PresentingRhythmSummaryItem() {delete _summary;}
    //override, update summary data checksum
    void updateChecksum() { _summary->sum();}
    //override, get summary data length
    int summaryDataLength() const { return sizeof(SummaryPresentRhythm);}
    //create an instance
    static PresentingRhythmSummaryItem *create(unsigned time);
private:
    PresentingRhythmSummaryItem(SummaryPresentRhythm *summarydata);
    SummaryPresentRhythm *_summary;

};

class DiagnosticEcgSummaryItem : public CommonSummaryItem
{
public:
    ~DiagnosticEcgSummaryItem() { delete _summary;}

    //override, update summary data checksum
    void updateChecksum() { _summary->sum();}

    //override, get summary data length
    int summaryDataLength() const { return sizeof(SummaryDiagnosticECG);}

    //override, wave cache control
    void startWaveCache();

    //override
    void stopWaveCache();

    //override
    bool isCompleted() const;

    //create an instance
    static DiagnosticEcgSummaryItem *create(unsigned time, ECGBandwidth diagBandwidth);

private:
    DiagnosticEcgSummaryItem(SummaryDiagnosticECG *summarydata);
    SummaryDiagnosticECG *_summary;
};
