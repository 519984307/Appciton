/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#include <unistd.h>
#include <QFile>
#include <QDir>
#include <QProcess>
#include "MergeConfig.h"
#include "ConfigManager.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "LanguageManager.h"
#include "PatientDefine.h"
#include "ParamDefine.h"
#include "ParamInfo.h"
#include "Debug.h"

MergeConfig *MergeConfig::_object = NULL;
/***************************************************************************************************
 * construction
 **************************************************************************************************/
MergeConfig::MergeConfig() : _configPartitionMountPoint("/media/mmcblk0p3")
{
    checkAndMountConfigPartition();
}

/***************************************************************************************************
 * back up Old Config
 **************************************************************************************************/
void MergeConfig::backupOldConfig()
{
    if (QFile::exists(_bkMachinePath()))
    {
        QFile::remove(_bkMachinePath());
    }

    if (QFile::exists(_bkSuperPath()))
    {
        QFile::remove(_bkSuperPath());
    }

    if (QFile::exists(_bkSystemPath()))
    {
        QFile::remove(_bkSystemPath());
    }

    QFile::copy(MACHINE_CFG_FILE, _bkMachinePath());
    QFile::copy(systemConfig.getCurConfigName(), _bkSuperPath());
    QFile::copy(SYSTEM_CFG_FILE, _bkSystemPath());
    sync();
}

bool MergeConfig::checkAndMountConfigPartition()
{
    // check /proc/partitions
    QFile file("/proc/partitions");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray data = file.readAll();
        if (!data.contains("mmcblk0p3"))
        {
            qdebug("No exist configuration partition.");
            return false;
        }
    }
    else
    {
        qdebug("Open /proc/partitions failed %s", qPrintable(file.errorString()));
        return false;
    }

    file.close();

    // check /proc/mounts
    file.setFileName("/proc/mounts");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray line;
        while (true)
        {
            line = file.readLine();
            if (line.isEmpty())
            {
                break;
            }
            else if (line.contains("mmcblk0p3"))
            {
                // already mount
                _configPartitionMountPoint = line.split(' ').at(1);
                return true;
            }
        }
    }
    else
    {
        qdebug("Open /proc/mounts failed %s", qPrintable(file.errorString()));
        return false;
    }
    file.close();

    // when we get here, we found the configuration partition, but partition haven't mount

    // create the mount point
    QDir dir(_configPartitionMountPoint);
    if (!dir.exists())
    {
        dir.mkpath(_configPartitionMountPoint);
    }
    // try to mount the partition
    QString mountCmdStr = QString("mount /dev/mmcblk0p3 %1").arg(_configPartitionMountPoint);
    qdebug("try to mount configuration partition");
    if (QProcess::execute(mountCmdStr) == QProcess::NormalExit)
    {
        qdebug("Mount configuration partition success.");
        return true;
    }
    else
    {
        // try to format the partition
        qdebug("mount failed, try to format configuration partition.");
        QString formatCmdStr("mkfs.ext4 /dev/mmcblk0p3");
        qdebug("try to mount configuration partition");
        if (QProcess::execute(formatCmdStr) == QProcess::NormalExit)
        {
            if (QProcess::execute(mountCmdStr) == QProcess::NormalExit)
            {
                qdebug("Mount configuration partition success.");
                return true;
            }
            else
            {
                qdebug("Mount configuration partition failed.");
                return false;
            }
        }
        else
        {
            qdebug("format configuration partition failed.");
            return false;
        }
    }
}

QString MergeConfig::configPartitionMountPoint() const
{
    return _configPartitionMountPoint;
}

/***************************************************************************************************
 * backup machine path
 **************************************************************************************************/
QString MergeConfig::_bkMachinePath() const
{
    return _configPartitionMountPoint + "/Machine.xml";
}

/***************************************************************************************************
 * backup supervisor path
 **************************************************************************************************/
QString MergeConfig::_bkSuperPath() const
{
    return _configPartitionMountPoint + "/Supervision.Original.xml";
}

/***************************************************************************************************
 * backup system path
 **************************************************************************************************/
QString MergeConfig::_bkSystemPath() const
{
    return _configPartitionMountPoint + "/System.xml";
}

/***************************************************************************************************
 * compare config
 **************************************************************************************************/
void MergeConfig::config_compare(QString attr)
{
    Config config(_bkSuperPath());
    QString str, str1;
    bool bret = config.getStrValue(attr, str);
    bret &= currentConfig.getStrValue(attr, str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue(attr, str);
    }
}

/***************************************************************************************************
 * check config
 **************************************************************************************************/
void MergeConfig::checkConfig()
{
    machineConfig.construction();
    bool firstImport = 0;
    machineConfig.getNumValue("FirstImport", firstImport);
    if (!firstImport)
    {
        debug("not first import.");
        return;
    }

    if (!QFile::exists(_bkMachinePath()))
    {
        return;
    }

    _loadMachineConfig();
    _loadSupervisorConfig();
    _loadSystemConfig();
    sync();

    int langNo = 0;
    currentConfig.getNumAttr("Local|Language", "CurrentOption", langNo);
    if (languageManager.getCurLanguage() != (LanguageManager::LanguageName)langNo)
    {
        languageManager.reload(langNo);
    }
}

/***************************************************************************************************
 * load machine config
 **************************************************************************************************/
void MergeConfig::_loadMachineConfig()
{
    if (!QFile::exists(_bkMachinePath()))
    {
        return;
    }

    machineConfig.setNumValue("FirstImport", 0);
    Config config(_bkMachinePath());
    if (config.getFileName() != _bkMachinePath())
    {
        return;
    }

    bool ret;
    QString strValue;
    QString strOldValue;
    ret = machineConfig.getStrValue("PACEEnable", strValue);
    ret &= config.getStrValue("PACEEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("PACEEnable", strOldValue);
    }

    ret = machineConfig.getStrValue("RESPEnable", strValue);
    ret &= config.getStrValue("RESPEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("RESPEnable", strOldValue);
    }

    ret = machineConfig.getStrValue("ECG12LEADEnable", strValue);
    ret &= config.getStrValue("ECG12LEADEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("ECG12LEADEnable", strOldValue);
    }

    ret = machineConfig.getStrValue("SPO2Enable", strValue);
    ret &= config.getStrValue("SPO2Enable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("SPO2Enable", strOldValue);
    }

    ret = machineConfig.getStrValue("NIBPEnable", strValue);
    ret &= config.getStrValue("NIBPEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("NIBPEnable", strOldValue);
    }

    ret = machineConfig.getStrValue("CO2Enable", strValue);
    ret &= config.getStrValue("CO2Enable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("CO2Enable", strOldValue);
    }

    ret = machineConfig.getStrValue("TEMPEnable", strValue);
    ret &= config.getStrValue("TEMPEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("TEMPEnable", strOldValue);
    }

    ret = machineConfig.getStrValue("WIFIEnable", strValue);
    ret &= config.getStrValue("WIFIEnable", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("WIFIEnable", strOldValue);
    }

//    ret = machineConfig.getStrValue("Record|ECG", strValue);
//    ret &= config.getStrValue("Record|ECG", strOldValue);
//    if (ret && strValue != strOldValue)
//    {
//        machineConfig.setStrValue("Record|ECG", strOldValue);
//    }

//    ret = machineConfig.getStrValue("Record|SPO2", strValue);
//    ret &= config.getStrValue("Record|SPO2", strOldValue);
//    if (ret && strValue != strOldValue)
//    {
//        machineConfig.setStrValue("Record|SPO2", strOldValue);
//    }

//    ret = machineConfig.getStrValue("Record|NIBP", strValue);
//    ret &= config.getStrValue("Record|NIBP", strOldValue);
//    if (ret && strValue != strOldValue)
//    {
//        machineConfig.setStrValue("Record|NIBP", strOldValue);
//    }

//    ret = machineConfig.getStrValue("Record|TEMP", strValue);
//    ret &= config.getStrValue("Record|TEMP", strOldValue);
//    if (ret && strValue != strOldValue)
//    {
//        machineConfig.setStrValue("Record|TEMP", strOldValue);
//    }

//    ret = machineConfig.getStrValue("Record|PDCommLog", strValue);
//    ret &= config.getStrValue("Record|PDCommLog", strOldValue);
//    if (ret && strValue != strOldValue)
//    {
//        machineConfig.setStrValue("Record|PDCommLog", strOldValue);
//    }

    ret = machineConfig.getStrValue("SerialNumber", strValue);
    ret &= config.getStrValue("SerialNumber", strOldValue);
    if (ret && strValue != strOldValue)
    {
        machineConfig.setStrValue("SerialNumber", strOldValue);
    }

    machineConfig.saveToDisk();
}

/***************************************************************************************************
 * load system config
 **************************************************************************************************/
void MergeConfig::_loadSystemConfig()
{
    if (!QFile::exists(_bkSystemPath()))
    {
        return;
    }

    Config config(_bkSystemPath());
    if (config.getFileName() != _bkSystemPath())
    {
        return;
    }

    bool ret;
    QString strValue;
    QString strOldValue;
    ret = systemConfig.getStrValue("ResetServiceAdvisedMesg", strValue);
    ret &= config.getStrValue("ResetServiceAdvisedMesg", strOldValue);
    if (ret && strValue != strOldValue)
    {
        systemConfig.setStrValue("ResetServiceAdvisedMesg", strOldValue);
    }

    QString str, str1;

    bool bret = config.getStrValue("PrimaryCfg|NIBP|MeasureMode", str);
    bret &= systemConfig.getStrValue("PrimaryCfg|NIBP|MeasureMode", str1);
    if (bret && (str1 != str))
    {
        systemConfig.setStrValue("PrimaryCfg|NIBP|MeasureMode", str);
    }

    bret = config.getStrValue("PrimaryCfg|NIBP|AutoInterval", str);
    bret &= systemConfig.getStrValue("PrimaryCfg|NIBP|AutoInterval", str1);
    if (bret && (str1 != str))
    {
        systemConfig.setStrValue("PrimaryCfg|NIBP|AutoInterval", str);
    }

    bret = config.getStrValue("PrimaryCfg|NIBP|StatFunction", str);
    bret &= systemConfig.getStrValue("PrimaryCfg|NIBP|StatFunction", str1);
    if (bret && (str1 != str))
    {
        systemConfig.setStrValue("PrimaryCfg|NIBP|StatFunction", str);
    }

    bret = config.getStrValue("PrimaryCfg|NIBP|AutomaticRetry", str);
    bret &= systemConfig.getStrValue("PrimaryCfg|NIBP|AutomaticRetry", str1);
    if (bret && (str1 != str))
    {
        systemConfig.setStrValue("PrimaryCfg|NIBP|AutomaticRetry", str);
    }
}

/***************************************************************************************************
 * load supervisor config
 **************************************************************************************************/
void MergeConfig::_loadSupervisorConfig()
{
    if (!QFile::exists(_bkSuperPath()))
    {
        return;
    }

    // currentConfig.construction();
    Config config(_bkSuperPath());
    if (config.getFileName() != _bkSuperPath())
    {
        return;
    }
    bool bret;
    QString str, str1;
    /*************************General****************************/
    QStringList General_list;
    General_list<< "General|Password"
                << "General|DeviceIdentifier"
                << "General|DefaultPatientType"
                << "General|DefaultDisplayBrightness"
                << "General|FullDisclosureRecording"
                << "General|SuperPassword";
    for (QStringList::iterator iter = General_list.begin(); iter != General_list.end(); ++iter)
    {
        config_compare(*iter);
    }

    /*************************ECG****************************/
    QStringList ECG_list;
    ECG_list << "ECG|BandWidth"
             << "ECG|CalcLead"
             << "ECG|QRSVolume"
             << "ECG|ECGLeadConvention"
             << "RESP|AutoActivation";
    for (QStringList::iterator iter = ECG_list.begin(); iter != ECG_list.end(); ++iter)
    {
        config_compare(*iter);
    }

    /*************************NIBP****************************/
    QStringList NIBP_list;
    NIBP_list << "NIBP|AdultInitialCuffInflation"
              << "NIBP|PedInitialCuffInflation";
    for (QStringList::iterator iter = NIBP_list.begin(); iter != NIBP_list.end(); ++iter)
    {
        config_compare(*iter);
    }

//    bret = config.getStrValue("NIBP|NeoInitialCuffInflation", str);
//    bret &= currentConfig.getStrValue("NIBP|NeoInitialCuffInflation", str1);
//    if (bret && (str1 != str))
//    {
//        currentConfig.setStrValue("NIBP|NeoInitialCuffInflation", str);
//    }

    /**************************RESP & CO2*****************************/
    QStringList CO2_list;
    CO2_list << "CO2|CO2ModeDefault"
             << "CO2|CO2SweepMode";
    for (QStringList::iterator iter = CO2_list.begin(); iter != CO2_list.end(); ++iter)
    {
        config_compare(*iter);
    }

    /*************************Alarm****************************/
    QStringList Alarm_list;
    Alarm_list << "Alarm|MinimumAlarmVolume"
               << "Alarm|DefaultAlarmVolume"
               << "Alarm|AlarmPauseTime"
               << "Alarm|AlarmOffPrompting"
               << "Alarm|EnableAlarmAudioOff"
               << "Alarm|EnableAlarmOff"
               << "Alarm|AlarmOffAtPowerOn";
    for (QStringList::iterator iter = Alarm_list.begin(); iter != Alarm_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /**************************报警限*****************************/
    for (int i = 0; i < PATIENT_TYPE_NEO; ++i)
    {
        QString prefix = "AlarmSource|";
        for (int j = 0; j < SUB_PARAM_NR; ++j)
        {
            QString subParamName = paramInfo.getSubParamName((SubParamID)j, true);
            bret = config.getStrAttr(prefix + subParamName, "Enable", str);
            bret &= currentConfig.getStrAttr(prefix + subParamName, "Enable", str1);
            if (bret && (str1 != str))
            {
                currentConfig.setStrAttr(prefix + subParamName, "Enable", str);
            }

            bret = config.getStrAttr(prefix + subParamName, "Prio", str);
            bret &= currentConfig.getStrAttr(prefix + subParamName, "Prio", str1);
            if (bret && (str1 != str))
            {
                currentConfig.setStrAttr(prefix + subParamName, "Prio", str);
            }

            QStringList unitList;
            switch (j)
            {
            case SUB_PARAM_NIBP_DIA:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_NIBP_SYS:
            {
                unitList << Unit::getSymbol(UNIT_KPA) << Unit::getSymbol(UNIT_MMHG);
                break;
            }

            case SUB_PARAM_ETCO2:
            case SUB_PARAM_FICO2:
            {
                unitList << Unit::getSymbol(UNIT_KPA) << Unit::getSymbol(UNIT_MMHG) << Unit::getSymbol(UNIT_PERCENT);
                break;
            }

            case SUB_PARAM_T1:
            case SUB_PARAM_T2:
            case SUB_PARAM_TD:
            {
                unitList << Unit::getSymbol(UNIT_TC) << Unit::getSymbol(UNIT_TF);
                break;
            }

            default:
                unitList << Unit::getSymbol(paramInfo.getUnitOfSubParam((SubParamID)j));
                break;
            }

            foreach(QString unitStr, unitList)
            {
                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str);
                bret &= currentConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str);
                bret &= currentConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str);
                bret &= currentConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str);
                bret &= currentConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|High", str);
                bret &= currentConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|High", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|High", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Low", str);
                bret &= currentConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Low", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Low", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Step", str);
                bret &= currentConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Step", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Step", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str);
                bret &= currentConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str1);
                if (bret && (str1 != str))
                {
                    currentConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str);
                }
            }
        }
    }

    /**************************Local*****************************/
    int languageCount = 0;
    int curLanguage = 0;
    int oldLanguage = 0;
    bret = config.getNumAttr("Local|Language", "CurrentOption", oldLanguage);
    bret &= currentConfig.getNumAttr("Local|Language", "CurrentOption", curLanguage);
    currentConfig.getNumAttr("Local|Language", "OptionCount", languageCount);
    if (bret && (oldLanguage != curLanguage) && (oldLanguage < languageCount))
    {
        currentConfig.setNumAttr("Local|Language", "CurrentOption", oldLanguage);
    }
    QStringList Local_list;
    Local_list << "Local|NIBPUnit"
               << "Local|CO2Unit"
               << "Local|TEMPUnit";
    for (QStringList::iterator iter = Local_list.begin(); iter != Local_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************Display****************************/
    QStringList Display_list;
    Display_list << "Display|ECGColor"
                 << "Display|SPO2Color"
                 << "Display|NIBPColor"
                 << "Display|CO2Color"
                 << "Display|RESPColor"
                 << "Display|TEMPColor";
    for (QStringList::iterator iter = Display_list.begin(); iter != Display_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************summary trriger****************************/
    QStringList Summary_list;
    Summary_list << "Print|PresentingECG"
                 << "Print|ECGAnalysis"
                 << "Print|ShockDelivery"
                 << "Print|CheckPatient"
                 << "Print|PacerStartUp"
                 << "Print|PhysiologicalAlarm"
                 << "Print|CoderMarker"
                 << "Print|NIBPReading"
                 << "Print|DiagnosticECG"
                 << "Print|Print30JSelfTestReport";
    for (QStringList::iterator iter = Summary_list.begin(); iter != Summary_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************ECG 12L****************************/
    QStringList ECG12L_list;
    ECG12L_list << "ECG12L|NotchFilter"
                << "ECG12L|ECG12LeadBandwidth"
                << "ECG12L|PrintSnapshotFormat"
                << "ECG12L|PDFReportFormat"
                << "ECG12L|DisplayFormat"
                << "ECG12L|TimeIntervalFor2x6Report"
                << "ECG12L|AutoPrinting12LReport"
                << "ECG12L|AutoTransmission"
                << "ECG12L|TransmissionFormat"
                << "ECG12L|RealtimePrintTopLead"
                << "ECG12L|RealtimePrintMiddleLead"
                << "ECG12L|RealtimePrintBottomLead";
    for (QStringList::iterator iter = ECG12L_list.begin(); iter != ECG12L_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************DateTime****************************/
    QStringList DataTime_list;
    DataTime_list << "DateTime|DateFormat"
                  << "DateTime|TimeFormat"
                  << "DateTime|DisplaySecond";
    for (QStringList::iterator iter = DataTime_list.begin(); iter != DataTime_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************CodeMarker****************************/
    currentConfig.getNumAttr("Local|Language", "OptionCount", languageCount);
    for (int i = 0; i < languageCount; ++i)
    {
        str = "CodeMarker|Marker" + QString::number(i);
        config_compare(str);

        str = "CodeMarker|SelectMarker|Language" + QString::number(i);
        config_compare(str);
    }

    /*************************WIFI****************************/
    str = "WiFi|EnableWifi";
    config_compare(str);

    int count = 0, current = 0;
    config.getNumAttr("WiFi|Profiles", "Count", count);
    currentConfig.setNumAttr("WiFi|Profiles", "Count", count);
    config.getNumAttr("WiFi|Profiles", "CurrentSelect", current);
    currentConfig.setNumAttr("WiFi|Profiles", "CurrentSelect", current);
    for (int i = 0; i < count; ++i)
    {
        QString strPath = "WiFi|Profiles|Profile" + QString::number(i);
        QString nodeName = strPath;
        strPath += "|";

        currentConfig.addNode("WiFi|Profiles", QString("Profile%1").arg(i));

        config.getStrValue(strPath + "ProfileName", str);
        currentConfig.addNode(nodeName, "ProfileName", str);

        config.getStrValue(strPath + "SSID", str);
        currentConfig.addNode(nodeName, "SSID", str);

        config.getStrValue(strPath + "AuthType", str);
        currentConfig.addNode(nodeName, "AuthType", str);

        config.getStrValue(strPath + "SecurityKey", str);
        currentConfig.addNode(nodeName, "SecurityKey", str);

        config.getStrValue(strPath + "IsStatic", str);
        currentConfig.addNode(nodeName, "IsStatic", str);

        if (1 == str.toInt())
        {
            config.getStrValue(strPath + "StaticIP", str);
            currentConfig.addNode(nodeName, "StaticIP", str);

            config.getStrValue(strPath + "DefaultGateway", str);
            currentConfig.addNode(nodeName, "DefaultGateway", str);

            config.getStrValue(strPath + "SubnetMask", str);
            currentConfig.addNode(nodeName, "SubnetMask", str);

            config.getStrValue(strPath + "PreferedDNS", str);
            currentConfig.addNode(nodeName, "PreferedDNS", str);

            config.getStrValue(strPath + "AlternateDNS", str);
            currentConfig.addNode(nodeName, "AlternateDNS", str);
        }
    }

    /*************************Sftp****************************/
    QStringList SFTP_list;
    SFTP_list << "Sftp|ServerIP"
              << "Sftp|ServerPort"
              << "Sftp|Username"
              << "Sftp|Password";
    for (QStringList::iterator iter = SFTP_list.begin(); iter != SFTP_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    /*************************Mail****************************/
    QStringList Mail_list;
    Mail_list << "Mail|SmtpServerPort"
              << "Mail|SmtpUsername"
              << "Mail|Password"
              << "Mail|ConnectionSecurity";
    for (QStringList::iterator iter = Mail_list.begin(); iter != Mail_list.end(); ++iter)
        {
            config_compare(*iter);
        }

    config.getNumAttr("Mail|Recipients", "Count", count);
    currentConfig.setNumAttr("Mail|Recipients", "Count", count);

    for (int i = 0; i < count; ++i)
    {
        QString path = "Mail|Recipients|Recipient" + QString::number(i);
        QString nodeName = path;
        path += "|";

        currentConfig.addNode("Mail|Recipients", "Recipient" + QString::number(i));

        config.getStrValue(path + "Name", str);
        currentConfig.addNode(nodeName, "Name", str);

        config.getStrValue(path + "Address", str);
        currentConfig.addNode(nodeName, "Address", str);
    }

    currentConfig.saveToDisk();
}

