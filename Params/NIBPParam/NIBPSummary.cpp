#include "SummaryStorageManager.h"
#include "NIBPSummary.h"
#include "DataStorageDirManager.h"
#include "Utility.h"

/***************************************************************************************************
 * create an instance
 **************************************************************************************************/
NIBPSummaryItem *NIBPSummaryItem::create(unsigned time, NIBPOneShotType error)
{
    SummaryNIBP *item = new SummaryNIBP();
    memset(item, 0, sizeof(SummaryNIBP));
    item->itemDesc.time = time;
    item->itemDesc.type = SUMMARY_NIBP;
    item->nibpErrorCode = error;

    NIBPSummaryItem *summaryItem = new NIBPSummaryItem(item);
    return summaryItem;
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
NIBPSummaryItem::NIBPSummaryItem(SummaryNIBP *summarydata)
    :_summary(summarydata)
{
    SummaryCommonInfo commonInfo;
    memset(&commonInfo, 0, sizeof(SummaryCommonInfo));
    summaryStorageManager.getSummaryCommonInfo(commonInfo);

    _summary->moduleConfig = commonInfo.moduleConfig;
    _summary->patientType = commonInfo.patientType;
    ::memcpy(_summary->paramValue, commonInfo.paramValue, sizeof(commonInfo.paramValue));
    _summary->co2bro = commonInfo.co2bro;
    _summary->lastMeasureTime = commonInfo.lastMeasureTime;
    Util::strlcpy(_summary->deviceID, commonInfo.deviceID, sizeof(_summary->deviceID));
    Util::strlcpy(_summary->patientName, commonInfo.patientName, sizeof(_summary->patientName));
    Util::strlcpy(_summary->serialNum, commonInfo.serialNum, sizeof(_summary->serialNum));
}
