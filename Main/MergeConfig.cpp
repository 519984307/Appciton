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
#include "IConfig.h"
#include "LanguageManager.h"
#include "PatientDefine.h"
#include "ParamDefine.h"
#include "ParamInfo.h"

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

    QString str, str1;
    /*************************General****************************/
    bool bret = config.getStrValue("General|Password", str);
    bret &= currentConfig.getStrValue("General|Password", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|Password", str);
    }

    bret = config.getStrValue("General|DeviceIdentifier", str);
    bret &= currentConfig.getStrValue("General|DeviceIdentifier", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|DeviceIdentifier", str);
    }

    bret = config.getStrValue("General|DefaultPatientType", str);
    bret &= currentConfig.getStrValue("General|DefaultPatientType", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|DefaultPatientType", str);
    }

    bret = config.getStrValue("General|DefaultDisplayBrightness", str);
    bret &= currentConfig.getStrValue("General|DefaultDisplayBrightness", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|DefaultDisplayBrightness", str);
    }

    bret = config.getStrValue("General|FullDisclosureRecording", str);
    bret &= currentConfig.getStrValue("General|FullDisclosureRecording", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|FullDisclosureRecording", str);
    }

    bret = config.getStrValue("General|SuperPassword", str);
    bret &= currentConfig.getStrValue("General|SuperPassword", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("General|SuperPassword", str);
    }

    /*************************Basic Defib****************************/
    bret = config.getStrValue("BasicDefib|DefaultVoicePromptTone", str);
    bret &= currentConfig.getStrValue("BasicDefib|DefaultVoicePromptTone", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|DefaultVoicePromptTone", str);
    }

    bret = config.getStrValue("BasicDefib|DefaultDefibMode", str);
    bret &= currentConfig.getStrValue("BasicDefib|DefaultDefibMode", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|DefaultDefibMode", str);
    }

    bret = config.getStrValue("BasicDefib|DefibDefaultToPads", str);
    bret &= currentConfig.getStrValue("BasicDefib|DefibDefaultToPads", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|DefibDefaultToPads", str);
    }

    bret = config.getStrValue("BasicDefib|BasicAutoEnergyEscalation", str);
    bret &= currentConfig.getStrValue("BasicDefib|BasicAutoEnergyEscalation", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|BasicAutoEnergyEscalation", str);
    }

    bret = config.getStrValue("BasicDefib|RetainSyncAfterShock", str);
    bret &= currentConfig.getStrValue("BasicDefib|RetainSyncAfterShock", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|RetainSyncAfterShock", str);
    }

    bret = config.getStrValue("BasicDefib|DefibReadyHoldTime", str);
    bret &= currentConfig.getStrValue("BasicDefib|DefibReadyHoldTime", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|DefibReadyHoldTime", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock1", str);
    bret &= currentConfig.getStrValue("BasicDefib|AdultShock1", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|AdultShock1", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock2", str);
    bret &= currentConfig.getStrValue("BasicDefib|AdultShock2", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|AdultShock2", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock3", str);
    bret &= currentConfig.getStrValue("BasicDefib|AdultShock3", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|AdultShock3", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock1", str);
    bret &= currentConfig.getStrValue("BasicDefib|PediatricShock1", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|PediatricShock1", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock2", str);
    bret &= currentConfig.getStrValue("BasicDefib|PediatricShock2", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|PediatricShock2", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock3", str);
    bret &= currentConfig.getStrValue("BasicDefib|PediatricShock3", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("BasicDefib|PediatricShock3", str);
    }

    /*************************PACER****************************/
    bret = config.getStrValue("PACER|DefaultPacerRate", str);
    bret &= currentConfig.getStrValue("PACER|DefaultPacerRate", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("PACER|DefaultPacerRate", str);
    }

    bret = config.getStrValue("PACER|FixedRatePacing", str);
    bret &= currentConfig.getStrValue("PACER|FixedRatePacing", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("PACER|FixedRatePacing", str);
    }

    /*************************ECG****************************/
    bret = config.getStrValue("ECG|PadsECGBandwidth", str);
    bret &= currentConfig.getStrValue("ECG|PadsECGBandwidth", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG|PadsECGBandwidth", str);
    }

    bret = config.getStrValue("ECG|ChestLeadsECGBandwidth", str);
    bret &= currentConfig.getStrValue("ECG|ChestLeadsECGBandwidth", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG|ChestLeadsECGBandwidth", str);
    }

    bret = config.getStrValue("ECG|DefaultECGLeadInMonitorMode", str);
    bret &= currentConfig.getStrValue("ECG|DefaultECGLeadInMonitorMode", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG|DefaultECGLeadInMonitorMode", str);
    }

    bret = config.getStrValue("ECG|QRSVolume", str);
    bret &= currentConfig.getStrValue("ECG|QRSVolume", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG|QRSVolume", str);
    }

    bret = config.getStrValue("ECG|ECGLeadConvention", str);
    bret &= currentConfig.getStrValue("ECG|ECGLeadConvention", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG|ECGLeadConvention", str);
    }

    bret = config.getStrValue("RESP|AutoActivation", str);
    bret &= currentConfig.getStrValue("RESP|AutoActivation", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("RESP|AutoActivation", str);
    }

    /*************************NIBP****************************/

    bret = config.getStrValue("NIBP|MeasureMode", str);
    bret &= currentConfig.getStrValue("NIBP|MeasureMode", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|MeasureMode", str);
    }

    bret = config.getStrValue("NIBP|AutoInterval", str);
    bret &= currentConfig.getStrValue("NIBP|AutoInterval", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|AutoInterval", str);
    }

    bret = config.getStrValue("NIBP|StatFunction", str);
    bret &= currentConfig.getStrValue("NIBP|StatFunction", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|StatFunction", str);
    }

    bret = config.getStrValue("NIBP|AutomaticRetry", str);
    bret &= currentConfig.getStrValue("NIBP|AutomaticRetry", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|AutomaticRetry", str);
    }

    bret = config.getStrValue("NIBP|AdultInitialCuffInflation", str);
    bret &= currentConfig.getStrValue("NIBP|AdultInitialCuffInflation", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|AdultInitialCuffInflation", str);
    }

    bret = config.getStrValue("NIBP|PedInitialCuffInflation", str);
    bret &= currentConfig.getStrValue("NIBP|PedInitialCuffInflation", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("NIBP|PedInitialCuffInflation", str);
    }

//    bret = config.getStrValue("NIBP|NeoInitialCuffInflation", str);
//    bret &= currentConfig.getStrValue("NIBP|NeoInitialCuffInflation", str1);
//    if (bret && (str1 != str))
//    {
//        currentConfig.setStrValue("NIBP|NeoInitialCuffInflation", str);
//    }

    /**************************RESP & CO2*****************************/
    bret = config.getStrValue("CO2|CO2ModeDefault", str);
    bret &= currentConfig.getStrValue("CO2|CO2ModeDefault", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("CO2|CO2ModeDefault", str);
    }

    bret = config.getStrValue("CO2|CO2SweepMode", str);
    bret &= currentConfig.getStrValue("CO2|CO2SweepMode", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("CO2|CO2SweepMode", str);
    }

    /*************************Alarm****************************/
    bret = config.getStrValue("Alarm|MinimumAlarmVolume", str);
    bret &= currentConfig.getStrValue("Alarm|MinimumAlarmVolume", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|MinimumAlarmVolume", str);
    }

    bret = config.getStrValue("Alarm|DefaultAlarmVolume", str);
    bret &= currentConfig.getStrValue("Alarm|DefaultAlarmVolume", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|DefaultAlarmVolume", str);
    }

    bret = config.getStrValue("Alarm|AlarmPauseTime", str);
    bret &= currentConfig.getStrValue("Alarm|AlarmPauseTime", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|AlarmPauseTime", str);
    }

    bret = config.getStrValue("Alarm|AlarmOffPrompting", str);
    bret &= currentConfig.getStrValue("Alarm|AlarmOffPrompting", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|AlarmOffPrompting", str);
    }

    bret = config.getStrValue("Alarm|EnableAlarmAudioOff", str);
    bret &= currentConfig.getStrValue("Alarm|EnableAlarmAudioOff", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|EnableAlarmAudioOff", str);
    }

    bret = config.getStrValue("Alarm|EnableAlarmOff", str);
    bret &= currentConfig.getStrValue("Alarm|EnableAlarmOff", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|EnableAlarmOff", str);
    }

    bret = config.getStrValue("Alarm|AlarmOffAtPowerOn", str);
    bret &= currentConfig.getStrValue("Alarm|AlarmOffAtPowerOn", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Alarm|AlarmOffAtPowerOn", str);
    }

    /**************************报警限*****************************/
    for (int i = 0; i < PATIENT_TYPE_NEO; ++i)
    {
        QString prefix = "AlarmSource|";
        prefix += PatientSymbol::convert((PatientType)i);
        prefix += "|";
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

    bret = config.getStrValue("Local|NIBPUnit", str);
    bret &= currentConfig.getStrValue("Local|NIBPUnit", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Local|NIBPUnit", str);
    }

    bret = config.getStrValue("Local|CO2Unit", str);
    bret &= currentConfig.getStrValue("Local|CO2Unit", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Local|CO2Unit", str);
    }

    bret = config.getStrValue("Local|TEMPUnit", str);
    bret &= currentConfig.getStrValue("Local|TEMPUnit", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Local|TEMPUnit", str);
    }

    /*************************Display****************************/
    bret = config.getStrValue("Display|ECGColor", str);
    bret &= currentConfig.getStrValue("Display|ECGColor", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|ECGColor", str);
    }

    bret = config.getStrValue("Display|SPO2Color", str);
    bret &= currentConfig.getStrValue("Display|SPO2Color", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|SPO2Color", str);
    }

    bret = config.getStrValue("Display|NIBPColor", str);
    bret &= currentConfig.getStrValue("Display|NIBPColor", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|NIBPColor", str);
    }

    bret = config.getStrValue("Display|CO2Color", str);
    bret &= currentConfig.getStrValue("Display|CO2Color", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|CO2Color", str);
    }

    bret = config.getStrValue("Display|RESPColor", str);
    bret &= currentConfig.getStrValue("Display|RESPColor", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|RESPColor", str);
    }

    bret = config.getStrValue("Display|TEMPColor", str);
    bret &= currentConfig.getStrValue("Display|TEMPColor", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Display|TEMPColor", str);
    }

    /*************************summary trriger****************************/
    bret = config.getStrValue("Print|PresentingECG", str);
    bret &= currentConfig.getStrValue("Print|PresentingECG", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|PresentingECG", str);
    }

    bret = config.getStrValue("Print|ECGAnalysis", str);
    bret &= currentConfig.getStrValue("Print|ECGAnalysis", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|ECGAnalysis", str);
    }

    bret = config.getStrValue("Print|ShockDelivery", str);
    bret &= currentConfig.getStrValue("Print|ShockDelivery", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|ShockDelivery", str);
    }

    bret = config.getStrValue("Print|CheckPatient", str);
    bret &= currentConfig.getStrValue("Print|CheckPatient", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|CheckPatient", str);
    }

    bret = config.getStrValue("Print|PacerStartUp", str);
    bret &= currentConfig.getStrValue("Print|PacerStartUp", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|PacerStartUp", str);
    }

    bret = config.getStrValue("Print|PhysiologicalAlarm", str);
    bret &= currentConfig.getStrValue("Print|PhysiologicalAlarm", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|PhysiologicalAlarm", str);
    }

    bret = config.getStrValue("Print|CoderMarker", str);
    bret &= currentConfig.getStrValue("Print|CoderMarker", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|CoderMarker", str);
    }

    bret = config.getStrValue("Print|NIBPReading", str);
    bret &= currentConfig.getStrValue("Print|NIBPReading", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|NIBPReading", str);
    }

    bret = config.getStrValue("Print|DiagnosticECG", str);
    bret &= currentConfig.getStrValue("Print|DiagnosticECG", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|DiagnosticECG", str);
    }

    bret = config.getStrValue("Print|Print30JSelfTestReport", str);
    bret &= currentConfig.getStrValue("Print|Print30JSelfTestReport", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Print|Print30JSelfTestReport", str);
    }

    /*************************ECG 12L****************************/
    bret = config.getStrValue("ECG12L|NotchFilter", str);
    bret &= currentConfig.getStrValue("ECG12L|NotchFilter", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|NotchFilter", str);
    }

    bret = config.getStrValue("ECG12L|ECG12LeadBandwidth", str);
    bret &= currentConfig.getStrValue("ECG12L|ECG12LeadBandwidth", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|ECG12LeadBandwidth", str);
    }

    bret = config.getStrValue("ECG12L|PrintSnapshotFormat", str);
    bret &= currentConfig.getStrValue("ECG12L|PrintSnapshotFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|PrintSnapshotFormat", str);
    }

    bret = config.getStrValue("ECG12L|PDFReportFormat", str);
    bret &= currentConfig.getStrValue("ECG12L|PDFReportFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|PDFReportFormat", str);
    }

    bret = config.getStrValue("ECG12L|DisplayFormat", str);
    bret &= currentConfig.getStrValue("ECG12L|DisplayFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|DisplayFormat", str);
    }

    bret = config.getStrValue("ECG12L|TimeIntervalFor2x6Report", str);
    bret &= currentConfig.getStrValue("ECG12L|TimeIntervalFor2x6Report", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|TimeIntervalFor2x6Report", str);
    }

    bret = config.getStrValue("ECG12L|AutoPrinting12LReport", str);
    bret &= currentConfig.getStrValue("ECG12L|AutoPrinting12LReport", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|AutoPrinting12LReport", str);
    }

    bret = config.getStrValue("ECG12L|AutoTransmission", str);
    bret &= currentConfig.getStrValue("ECG12L|AutoTransmission", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|AutoTransmission", str);
    }

    bret = config.getStrValue("ECG12L|TransmissionFormat", str);
    bret &= currentConfig.getStrValue("ECG12L|TransmissionFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|TransmissionFormat", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintTopLead", str);
    bret &= currentConfig.getStrValue("ECG12L|RealtimePrintTopLead", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|RealtimePrintTopLead", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintMiddleLead", str);
    bret &= currentConfig.getStrValue("ECG12L|RealtimePrintMiddleLead", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|RealtimePrintMiddleLead", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintBottomLead", str);
    bret &= currentConfig.getStrValue("ECG12L|RealtimePrintBottomLead", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("ECG12L|RealtimePrintBottomLead", str);
    }

    /*************************DateTime****************************/
    bret = config.getStrValue("DateTime|DateFormat", str);
    bret &= currentConfig.getStrValue("DateTime|DateFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("DateTime|DateFormat", str);
    }

    bret = config.getStrValue("DateTime|TimeFormat", str);
    bret &= currentConfig.getStrValue("DateTime|TimeFormat", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("DateTime|TimeFormat", str);
    }

    bret = config.getStrValue("DateTime|DisplaySecond", str);
    bret &= currentConfig.getStrValue("DateTime|DisplaySecond", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("DateTime|DisplaySecond", str);
    }

    /*************************CodeMarker****************************/
    currentConfig.getNumAttr("Local|Language", "OptionCount", languageCount);
    for (int i = 0; i < languageCount; ++i)
    {
        bret = config.getStrValue("CodeMarker|Marker" + QString::number(i), str);
        bret &= currentConfig.getStrValue("CodeMarker|Marker" + QString::number(i), str1);
        if (bret && (str1 != str))
        {
            currentConfig.setStrValue("CodeMarker|Marker" + QString::number(i), str);
        }

        bret = config.getStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str);
        bret &= currentConfig.getStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str1);
        if (bret && (str1 != str))
        {
            currentConfig.setStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str);
        }
    }

    /*************************WIFI****************************/
    bret = config.getStrValue("WiFi|EnableWifi", str);
    bret &= currentConfig.getStrValue("WiFi|EnableWifi", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("WiFi|EnableWifi", str);
    }

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
    bret = config.getStrValue("Sftp|ServerIP", str);
    bret &= currentConfig.getStrValue("Sftp|ServerIP", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Sftp|ServerIP", str);
    }

    bret = config.getStrValue("Sftp|ServerPort", str);
    bret &= currentConfig.getStrValue("Sftp|ServerPort", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Sftp|ServerPort", str);
    }

    bret = config.getStrValue("Sftp|Username", str);
    bret &= currentConfig.getStrValue("Sftp|Username", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Sftp|Username", str);
    }

    bret = config.getStrValue("Sftp|Password", str);
    bret &= currentConfig.getStrValue("Sftp|Password", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Sftp|Password", str);
    }

    /*************************Mail****************************/
    bret = config.getStrValue("Mail|SmtpServerPort", str);
    bret &= currentConfig.getStrValue("Mail|SmtpServerPort", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Mail|SmtpServerPort", str);
    }

    bret = config.getStrValue("Mail|SmtpUsername", str);
    bret &= currentConfig.getStrValue("Mail|SmtpUsername", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Mail|SmtpUsername", str);
    }

    bret = config.getStrValue("Mail|Password", str);
    bret &= currentConfig.getStrValue("Mail|Password", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Mail|Password", str);
    }

    bret = config.getStrValue("Mail|ConnectionSecurity", str);
    bret &= currentConfig.getStrValue("Mail|ConnectionSecurity", str1);
    if (bret && (str1 != str))
    {
        currentConfig.setStrValue("Mail|ConnectionSecurity", str);
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

