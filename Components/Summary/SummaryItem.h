#pragma once
#include <SummaryDefine.h>
#include <QAtomicInt>
#include <QDebug>
#define SYNC_CACHE_TIME 12

class SummaryItem
{
public:
    SummaryItem() { qDebug("summary item create:0x%lx", (long)this);}
    virtual ~SummaryItem(){qDebug("summary item destroy:0x%lx", (long)this);}
    //check wether the summary complete collecting data
    virtual bool isCompleted() const = 0;
    //get the type of the summary
    virtual SummaryType type() const = 0;
    //get the time summary create
    virtual unsigned time() const = 0;
    //get the summary data
    virtual char * getSummaryData() const = 0;
    //get the summary data length
    virtual int summaryDataLength() const = 0;
    //get the summary current cache wave length
    virtual int getCacheWaveLength(WaveformID id) const = 0;
    //update the summary data checksum
    virtual void updateChecksum() = 0;
    //start the wave cache
    virtual void startWaveCache() = 0;
    //stop the wave cache
    virtual void stopWaveCache() = 0;

    //increase the item reference
    void increaseRef();

    //decrease the item refreence, if the item's reference become zero, will delete the item
    void decreaseRef();

private:
    QAtomicInt ref;
};

class CommonSummaryItem : public SummaryItem
{
public:
    //override
    bool isCompleted() const ;
    //override
    SummaryType type() const;
    //override
    unsigned time() const;
    //overrid
    char * getSummaryData() const;
    ~CommonSummaryItem();
    //override
    int getCacheWaveLength(WaveformID id) const;
    //override
    void startWaveCache();
    //override
    void stopWaveCache();
protected:
    CommonSummaryItem(char *summaryData);
private:
    char *_data;
};
