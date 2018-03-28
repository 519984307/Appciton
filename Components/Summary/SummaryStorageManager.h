#pragma once
#include "AlarmDefine.h"
#include "SummaryItem.h"
#include "StorageManager.h"

class SummaryStorageManagerPrivate;
class SummaryStorageManager : public StorageManager
{
public:
    static SummaryStorageManager &construction(void);
    ~SummaryStorageManager();

    // add a summary item
    bool addSummaryItem(SummaryItem *item);

    // 获取summary公共信息
    void getSummaryCommonInfo(SummaryCommonInfo &commonInfo);

    // 获取波形对应的增益
    char getWaveGain(WaveformID id);

    // 循环运行
    void run();

    // 请求触发打印
    void requestTriggerPrint(SummaryItem *item);

    //call to inotify that the trigger print has finished
    void triggerPrintComplete(SummaryItem *item);

    //check whether the summary item is completed
    void checkCompletedItem(void);

    //添加各类型的数据
    void addPhyAlarm(unsigned alarmTime, ParamID paramID, int alarmType, bool oneshot, WaveformID id);
    void addOneshotAlarm(unsigned alarmTime);
    void addCodeMarker(unsigned time, const char *codeName);
    void addNIBP(unsigned time, int errorCode);
    void addPrinterAction(unsigned time);
    void addPrensentingRhythm(unsigned time);
    void addDiagnosticECG(unsigned time, ECGBandwidth diagBandwidth);

    //get the phy alarm message
    static QString getPhyAlarmMessage(ParamID paramId, int alarmType, bool isOneShot);

protected:
    //override
    void saveDataCallback(quint32 dataID, const char *data, quint32 len);

private:
    Q_DECLARE_PRIVATE(SummaryStorageManager)
    SummaryStorageManager();
    virtual void createDir();
};

#define summaryStorageManager (SummaryStorageManager::construction())
#define deleteSummaryStorageManager() (delete &summaryStorageManager)
