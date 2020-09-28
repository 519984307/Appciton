/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#include "ErrorLogItem.h"
#include <QFile>

void ErrorLogItemBase::setPdCommLogCache(const QVariantList &cachelist)
{
    content["pdCommLog"] = cachelist;
}

void ErrorLogItemBase::setSubSystem(ErrorLogItemBase::SubSystem subSys)
{
    QString subSysStr = "Unknown";
    switch (subSys)
    {
    case SUB_SYS_MAIN_PROCESSOR:
        subSysStr = "Main Processor";
        break;
    case SUB_SYS_SYSTEM_BOARD:
        subSysStr = "System Board";
        break;
    case SUB_SYS_E5:
        subSysStr = "E5";
        break;
    case SUB_SYS_S5:
        subSysStr = "S5";
        break;
    case SUB_SYS_BLMSPO2:
        subSysStr = "BLMSPO2";
        break;
    case SUB_SYS_N5:
        subSysStr = "N5";
        break;
    case SUB_SYS_T5:
        subSysStr = "T5";
        break;
    case SUB_SYS_CO2:
        subSysStr = "CO2";
        break;
    case SUB_SYS_RESP:
        subSysStr = "Resp";
        break;
    case SUB_SYS_WIFI:
        subSysStr = "WiFi";
        break;
    case SUB_SYS_PRINTER:
        subSysStr = "PRT72";
        break;
    case SUB_SYS_SUNTECH_NIBP:
        subSysStr = "NIBP";
    default:
        break;
    }

    content["subSys"] = subSysStr;
}

void ErrorLogItemBase::setSystemState(ErrorLogItemBase::SystemState sysState)
{
    QString sysStateStr = "Unknown";
    switch (sysState)
    {
    case SYS_STAT_SELFTEST:
        sysStateStr = "Self-test";
        break;
    case SYS_STAT_RUNTIME:
        sysStateStr = "Runtime";
        break;
    case SYS_STAT_MONITOR_MODE:
        sysStateStr = "Monitor Mode";
        break;
    case SYS_STAT_PACER_MODE:
        sysStateStr = "Pacer Mode";
        break;
    default:
        break;
    }

    content["sysState"] = sysStateStr;
}

void ErrorLogItemBase::setSystemResponse(ErrorLogItemBase::SystemResponeToError sysRsp)
{
    QString sysRspStr = "Unknown";
    switch (sysRsp)
    {
    case SYS_RSP_REPORT:
        sysRspStr = "Report";
        break;
    case SYS_RSP_RETRY:
        sysRspStr = "Retry";
        break;
    case SYS_RSP_RESET:
        sysRspStr = "Reset";
        break;
    case SYS_RSP_DISABLE:
        sysRspStr = "Disable";
        break;
    default:
        break;
    }

    content["sysRsp"] = sysRspStr;
}

QMap<int, ErrorLogItemCreateFunc> &ErrorLogItemBase::getItemCreatorMap()
{
    static QMap<int, ErrorLogItemCreateFunc> creatorMap;
    return creatorMap;
}

int ErrorLogItem::type() const
{
    return Type;
}

void ErrorLogItem::outputInfo(QTextStream *stream) const
{
    ErrorLogItemBase::outputInfo(stream);
    if (content.contains("subSys"))
    {
        *stream << "Subsystem" << content["subSys"].toString() << "\r\n";
    }
    if (content.contains("sysState"))
    {
        *stream << "System State" << content["sysState"].toString() << "\r\n";
    }
    if (content.contains("sysRsp"))
    {
        *stream << "System Response" << content["sysRsp"].toString() << "\r\n";
    }

    *stream << content["log"].toString();
}

bool ErrorLogItem::isLogEmpty() const
{
    if (content.contains("subSys") || content.contains("sysState") || content.contains("sysRsp")
            || content.contains("pdCommLog"))
    {
        return false;
    }

    if (content["log"].toString().isEmpty())
    {
        return true;
    }

    return false;
}

int CrashLogItem::type() const
{
    return Type;
}

bool CrashLogItem::isLogEmpty() const
{
    return false;
}

void CrashLogItem::outputInfo(QTextStream *stream) const
{
    ErrorLogItemBase::outputInfo(stream);
    if (content.contains("subSys"))
    {
        *stream << "Subsystem" << content["subSys"].toString() << "\r\n";
    }
    if (content.contains("sysState"))
    {
        *stream << "System State" << content["sysState"].toString() << "\r\n";
    }
    if (content.contains("sysRsp"))
    {
        *stream << "System Response" << content["sysRsp"].toString() << "\r\n";
    }

    *stream << content["log"].toByteArray();
}
// 将原始日志内容读取到ErrorLogItemBase基类生成的全局对象中
void CrashLogItem::collect(const QString &filename)
{
    QFile logfile(filename);
    if (logfile.open(QIODevice::ReadOnly))
    {
        content["log"] = logfile.readAll();
        logfile.close();
    }
}

int CriticalFaultLogItem::type() const
{
    return Type;
}

REGISTER_ERROR_ITEM(ErrorLogItem)
REGISTER_ERROR_ITEM(CrashLogItem)
REGISTER_ERROR_ITEM(CriticalFaultLogItem)
