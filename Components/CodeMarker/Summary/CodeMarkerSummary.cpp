#include "SummaryStorageManager.h"
#include "CodeMarkerSummary.h"
#include "DataStorageDirManager.h"
#include "Utility.h"

/***************************************************************************************************
 * create : crate a instance
 **************************************************************************************************/
CodeMarkerSummaryItem *CodeMarkerSummaryItem::create(unsigned time, const char *codeName)
{
    SummaryCodeMarker *item = new SummaryCodeMarker();
    memset(item, 0, sizeof(SummaryCodeMarker));
    item->itemDesc.time = time;
    item->itemDesc.type = SUMMARY_CODE_MAKER;
    Util::strlcpy(item->selectCodeName, codeName, sizeof(item->selectCodeName));

    CodeMarkerSummaryItem *summaryItem = new CodeMarkerSummaryItem(item);
    return summaryItem;

}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
CodeMarkerSummaryItem::CodeMarkerSummaryItem(SummaryCodeMarker *summarydata)
    :_summary(summarydata)
{
    SummaryCommonInfo commonInfo;
    memset(&commonInfo, 0, sizeof(SummaryCommonInfo));
    summaryStorageManager.getSummaryCommonInfo(commonInfo);

    _summary->moduleConfig = commonInfo.moduleConfig;
    ::memcpy(_summary->paramValue, commonInfo.paramValue, sizeof(commonInfo.paramValue));
    _summary->co2bro = commonInfo.co2bro;
    _summary->lastMeasureCompleteTime = commonInfo.lastMeasureTime;
    Util::strlcpy(_summary->deviceID, commonInfo.deviceID, sizeof(_summary->deviceID));
    Util::strlcpy(_summary->patientName, commonInfo.patientName, sizeof(_summary->patientName));
    Util::strlcpy(_summary->serialNum, commonInfo.serialNum, sizeof(_summary->serialNum));
}
