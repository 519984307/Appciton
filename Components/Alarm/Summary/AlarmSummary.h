#pragma once
#include "SummaryItem.h"

class PhyAlarmSummaryItem : public CommonSummaryItem
{
public:
    ~PhyAlarmSummaryItem() { delete _summary;}
    //check this summary is finish collecting data
    bool isCompleted() const;
    //get the summary data len of this summary
    int summaryDataLength() const { return sizeof(SummaryPhyAlarm);}
    //update the checksum
    void updateChecksum() { _summary->sum();}
    //get the curretn cache wavelen
    int getCacheWaveLength(WaveformID id) const;
    //create an instance
    static PhyAlarmSummaryItem *create(unsigned alarmTime, ParamID paramID, int alarmType, bool oneshot, WaveformID id);
protected:
    //start caching wave
    void startWaveCache();
    //stop caching wave
    void stopWaveCache();
private:
    PhyAlarmSummaryItem(SummaryPhyAlarm *summaryData);
    SummaryPhyAlarm *_summary;
};

class OneShotAlarmSummaryItem : public CommonSummaryItem
{
public:
    ~OneShotAlarmSummaryItem() {delete _summary;}
    //update checksum
    void updateChecksum() {_summary->sum();}
    //get teh summary data length
    int summaryDataLength() const { return sizeof(SummaryCheckPatient);}
    //crete an instance
    static OneShotAlarmSummaryItem *create(unsigned alarmTime);
private:
    OneShotAlarmSummaryItem(SummaryCheckPatient *summarydata);
    SummaryCheckPatient *_summary;
};
