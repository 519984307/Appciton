#include "ActivityLogManager.h"

ActivityLogManager *ActivityLogManager::_selfobj = NULL;

#define LOG_FILE ("/nPMLogInfo.zol")     //日志文件名
#define MAX_LOG_NUM (5000)              //日志最长记录5000条

/***************************************************************************************************
 * 构造
 **************************************************************************************************/
ActivityLogManager::ActivityLogManager() : CircleFile(LOG_FILE)
{

}

/***************************************************************************************************
 * 功能:添加数据项
 * 参数:
 *    time_t:时间
 *    str:内容
 **************************************************************************************************/
void ActivityLogManager::addLogItem(int time_t, QString str)
{
    struct IncidentEntry logInfo(time_t, str);
    logInfo.checkSum = logInfo.makeSum();

    appendBlockData((char *)&logInfo, sizeof(logInfo));
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
ActivityLogManager::~ActivityLogManager()
{

}
