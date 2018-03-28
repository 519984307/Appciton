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
    QFile::copy(SUPERVISOR_CFG_FILE, _bkSuperPath());
    QFile::copy(SYSTEM_CFG_FILE, _bkSystemPath());
    sync();
}

bool MergeConfig::checkAndMountConfigPartition()
{
    //check /proc/partitions
    QFile file("/proc/partitions");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray data = file.readAll();
        if(!data.contains("mmcblk0p3"))
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

    //check /proc/mounts
    file.setFileName("/proc/mounts");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray line;
        while(true)
        {
            line = file.readLine();
            if(line.isEmpty())
            {
                break;
            }
            else if(line.contains("mmcblk0p3"))
            {
                //already mount
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

    //when we get here, we found the configuration partition, but partition haven't mount

    //create the mount point
    QDir dir(_configPartitionMountPoint);
    if(!dir.exists())
    {
        dir.mkpath(_configPartitionMountPoint);
    }
    //try to mount the partition
    QString mountCmdStr = QString("mount /dev/mmcblk0p3 %1").arg(_configPartitionMountPoint);
    qdebug("try to mount configuration partition");
    if(QProcess::execute(mountCmdStr) == QProcess::NormalExit)
    {
        qdebug("Mount configuration partition success.");
        return true;
    }
    else
    {
        //try to format the partition
        qdebug("mount failed, try to format configuration partition.");
        QString formatCmdStr("mkfs.ext4 /dev/mmcblk0p3");
        qdebug("try to mount configuration partition");
        if(QProcess::execute(formatCmdStr) == QProcess::NormalExit)
        {
            if(QProcess::execute(mountCmdStr) == QProcess::NormalExit)
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
    superConfig.getNumAttr("Local|Language", "CurrentOption", langNo);
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

    bool ret = false;
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

    bool ret = false;
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

    superConfig.construction();
    Config config(_bkSuperPath());
    if (config.getFileName() != _bkSuperPath())
    {
        return;
    }

    QString str, str1;
/*************************General****************************/
    bool bret = config.getStrValue("General|Password", str);
    bret &= superConfig.getStrValue("General|Password", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|Password", str);
    }

    bret = config.getStrValue("General|DeviceIdentifier", str);
    bret &= superConfig.getStrValue("General|DeviceIdentifier", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|DeviceIdentifier", str);
    }

    bret = config.getStrValue("General|DefaultPatientType", str);
    bret &= superConfig.getStrValue("General|DefaultPatientType", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|DefaultPatientType", str);
    }

    bret = config.getStrValue("General|DefaultDisplayBrightness", str);
    bret &= superConfig.getStrValue("General|DefaultDisplayBrightness", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|DefaultDisplayBrightness", str);
    }

    bret = config.getStrValue("General|FullDisclosureRecording", str);
    bret &= superConfig.getStrValue("General|FullDisclosureRecording", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|FullDisclosureRecording", str);
    }

    bret = config.getStrValue("General|SuperPassword", str);
    bret &= superConfig.getStrValue("General|SuperPassword", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("General|SuperPassword", str);
    }

    /*************************Basic Defib****************************/
    bret = config.getStrValue("BasicDefib|DefaultVoicePromptTone", str);
    bret &= superConfig.getStrValue("BasicDefib|DefaultVoicePromptTone", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|DefaultVoicePromptTone", str);
    }

    bret = config.getStrValue("BasicDefib|DefaultDefibMode", str);
    bret &= superConfig.getStrValue("BasicDefib|DefaultDefibMode", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|DefaultDefibMode", str);
    }

    bret = config.getStrValue("BasicDefib|DefibDefaultToPads", str);
    bret &= superConfig.getStrValue("BasicDefib|DefibDefaultToPads", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|DefibDefaultToPads", str);
    }

    bret = config.getStrValue("BasicDefib|BasicAutoEnergyEscalation", str);
    bret &= superConfig.getStrValue("BasicDefib|BasicAutoEnergyEscalation", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|BasicAutoEnergyEscalation", str);
    }

    bret = config.getStrValue("BasicDefib|RetainSyncAfterShock", str);
    bret &= superConfig.getStrValue("BasicDefib|RetainSyncAfterShock", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|RetainSyncAfterShock", str);
    }

    bret = config.getStrValue("BasicDefib|DefibReadyHoldTime", str);
    bret &= superConfig.getStrValue("BasicDefib|DefibReadyHoldTime", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|DefibReadyHoldTime", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock1", str);
    bret &= superConfig.getStrValue("BasicDefib|AdultShock1", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|AdultShock1", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock2", str);
    bret &= superConfig.getStrValue("BasicDefib|AdultShock2", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|AdultShock2", str);
    }

    bret = config.getStrValue("BasicDefib|AdultShock3", str);
    bret &= superConfig.getStrValue("BasicDefib|AdultShock3", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|AdultShock3", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock1", str);
    bret &= superConfig.getStrValue("BasicDefib|PediatricShock1", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|PediatricShock1", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock2", str);
    bret &= superConfig.getStrValue("BasicDefib|PediatricShock2", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|PediatricShock2", str);
    }

    bret = config.getStrValue("BasicDefib|PediatricShock3", str);
    bret &= superConfig.getStrValue("BasicDefib|PediatricShock3", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("BasicDefib|PediatricShock3", str);
    }

    /*************************AED****************************/
    bret = config.getStrValue("AED|StartWithCPR", str);
    bret &= superConfig.getStrValue("AED|StartWithCPR", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|StartWithCPR", str);
    }

    bret = config.getStrValue("AED|PowerupToStartCPRDuration", str);
    bret &= superConfig.getStrValue("AED|PowerupToStartCPRDuration", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|PowerupToStartCPRDuration", str);
    }

    bret = config.getStrValue("AED|RestartAnalysisAfterCPR", str);
    bret &= superConfig.getStrValue("AED|RestartAnalysisAfterCPR", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|RestartAnalysisAfterCPR", str);
    }

    bret = config.getStrValue("AED|StartWithCPRDuration", str);
    bret &= superConfig.getStrValue("AED|StartWithCPRDuration", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|StartWithCPRDuration", str);
    }

    bret = config.getStrValue("AED|StartWithCPRMessage", str);
    bret &= superConfig.getStrValue("AED|StartWithCPRMessage", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|StartWithCPRMessage", str);
    }

    bret = config.getStrValue("AED|EnableManualModePassword", str);
    bret &= superConfig.getStrValue("AED|EnableManualModePassword", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|EnableManualModePassword", str);
    }

    bret = config.getStrValue("AED|ManualModePassword", str);
    bret &= superConfig.getStrValue("AED|ManualModePassword", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|ManualModePassword", str);
    }

    bret = config.getStrValue("AED|CPRDurationNoShock", str);
    bret &= superConfig.getStrValue("AED|CPRDurationNoShock", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|CPRDurationNoShock", str);
    }

    bret = config.getStrValue("AED|CPRMessageNoShock", str);
    bret &= superConfig.getStrValue("AED|CPRMessageNoShock", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|CPRMessageNoShock", str);
    }

    bret = config.getStrValue("AED|CPRDurationShock", str);
    bret &= superConfig.getStrValue("AED|CPRDurationShock", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|CPRDurationShock", str);
    }

    bret = config.getStrValue("AED|CPRMessageShock", str);
    bret &= superConfig.getStrValue("AED|CPRMessageShock", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|CPRMessageShock", str);
    }

    bret = config.getStrValue("AED|QRSPRTone", str);
    bret &= superConfig.getStrValue("AED|QRSPRTone", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|QRSPRTone", str);
    }

    bret = config.getStrValue("AED|PhyAlarmVisualIndicator", str);
    bret &= superConfig.getStrValue("AED|PhyAlarmVisualIndicator", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("AED|PhyAlarmVisualIndicator", str);
    }

    /*************************PACER****************************/
    bret = config.getStrValue("PACER|DefaultPacerRate", str);
    bret &= superConfig.getStrValue("PACER|DefaultPacerRate", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("PACER|DefaultPacerRate", str);
    }

    bret = config.getStrValue("PACER|FixedRatePacing", str);
    bret &= superConfig.getStrValue("PACER|FixedRatePacing", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("PACER|FixedRatePacing", str);
    }

    /*************************CPR****************************/
    bret = config.getStrValue("CPR|EnableVoicePrompts", str);
    bret &= superConfig.getStrValue("CPR|EnableVoicePrompts", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|EnableVoicePrompts", str);
    }

    bret = config.getStrValue("CPR|EnableTextPrompts", str);
    bret &= superConfig.getStrValue("CPR|EnableTextPrompts", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|EnableTextPrompts", str);
    }

    bret = config.getStrValue("CPR|EnableCPRMetronome", str);
    bret &= superConfig.getStrValue("CPR|EnableCPRMetronome", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|EnableCPRMetronome", str);
    }

    bret = config.getStrValue("CPR|AEDMetronome", str);
    bret &= superConfig.getStrValue("CPR|AEDMetronome", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|AEDMetronome", str);
    }

    bret = config.getStrValue("CPR|MetronomeRate", str);
    bret &= superConfig.getStrValue("CPR|MetronomeRate", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|MetronomeRate", str);
    }

    bret = config.getStrValue("CPR|CompressionDepthUnit", str);
    bret &= superConfig.getStrValue("CPR|CompressionDepthUnit", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|CompressionDepthUnit", str);
    }

    bret = config.getStrValue("CPR|TargetCompressionDepth", str);
    bret &= superConfig.getStrValue("CPR|TargetCompressionDepth", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|TargetCompressionDepth", str);
    }

    bret = config.getStrValue("CPR|EnableCPRMonitoring", str);
    bret &= superConfig.getStrValue("CPR|EnableCPRMonitoring", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|EnableCPRMonitoring", str);
    }

    bret = config.getStrValue("CPR|CPRWaveformRecording", str);
    bret &= superConfig.getStrValue("CPR|CPRWaveformRecording", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CPR|CPRWaveformRecording", str);
    }

    /*************************ECG****************************/
    bret = config.getStrValue("ECG|PadsECGBandwidth", str);
    bret &= superConfig.getStrValue("ECG|PadsECGBandwidth", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG|PadsECGBandwidth", str);
    }

    bret = config.getStrValue("ECG|ChestLeadsECGBandwidth", str);
    bret &= superConfig.getStrValue("ECG|ChestLeadsECGBandwidth", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG|ChestLeadsECGBandwidth", str);
    }

    bret = config.getStrValue("ECG|DefaultECGLeadInMonitorMode", str);
    bret &= superConfig.getStrValue("ECG|DefaultECGLeadInMonitorMode", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG|DefaultECGLeadInMonitorMode", str);
    }

    bret = config.getStrValue("ECG|QRSVolume", str);
    bret &= superConfig.getStrValue("ECG|QRSVolume", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG|QRSVolume", str);
    }

    bret = config.getStrValue("ECG|ECGLeadConvention", str);
    bret &= superConfig.getStrValue("ECG|ECGLeadConvention", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG|ECGLeadConvention", str);
    }

    bret = config.getStrValue("RESP|AutoActivation", str);
    bret &= superConfig.getStrValue("RESP|AutoActivation", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("RESP|AutoActivation", str);
    }

    /*************************NIBP****************************/

    bret = config.getStrValue("NIBP|MeasureMode", str);
    bret &= superConfig.getStrValue("NIBP|MeasureMode", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|MeasureMode", str);
    }

    bret = config.getStrValue("NIBP|AutoInterval", str);
    bret &= superConfig.getStrValue("NIBP|AutoInterval", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|AutoInterval", str);
    }

    bret = config.getStrValue("NIBP|StatFunction", str);
    bret &= superConfig.getStrValue("NIBP|StatFunction", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|StatFunction", str);
    }

    bret = config.getStrValue("NIBP|AutomaticRetry", str);
    bret &= superConfig.getStrValue("NIBP|AutomaticRetry", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|AutomaticRetry", str);
    }

    bret = config.getStrValue("NIBP|AdultInitialCuffInflation", str);
    bret &= superConfig.getStrValue("NIBP|AdultInitialCuffInflation", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|AdultInitialCuffInflation", str);
    }

    bret = config.getStrValue("NIBP|PedInitialCuffInflation", str);
    bret &= superConfig.getStrValue("NIBP|PedInitialCuffInflation", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("NIBP|PedInitialCuffInflation", str);
    }

//    bret = config.getStrValue("NIBP|NeoInitialCuffInflation", str);
//    bret &= superConfig.getStrValue("NIBP|NeoInitialCuffInflation", str1);
//    if (bret && (str1 != str))
//    {
//        superConfig.setStrValue("NIBP|NeoInitialCuffInflation", str);
//    }

    /**************************RESP & CO2*****************************/
    bret = config.getStrValue("CO2|CO2ModeDefault", str);
    bret &= superConfig.getStrValue("CO2|CO2ModeDefault", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CO2|CO2ModeDefault", str);
    }

    bret = config.getStrValue("CO2|CO2SweepMode", str);
    bret &= superConfig.getStrValue("CO2|CO2SweepMode", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("CO2|CO2SweepMode", str);
    }

    /*************************Alarm****************************/
    bret = config.getStrValue("Alarm|MinimumAlarmVolume", str);
    bret &= superConfig.getStrValue("Alarm|MinimumAlarmVolume", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|MinimumAlarmVolume", str);
    }

    bret = config.getStrValue("Alarm|DefaultAlarmVolume", str);
    bret &= superConfig.getStrValue("Alarm|DefaultAlarmVolume", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|DefaultAlarmVolume", str);
    }

    bret = config.getStrValue("Alarm|AlarmPauseTime", str);
    bret &= superConfig.getStrValue("Alarm|AlarmPauseTime", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|AlarmPauseTime", str);
    }

    bret = config.getStrValue("Alarm|AlarmOffPrompting", str);
    bret &= superConfig.getStrValue("Alarm|AlarmOffPrompting", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|AlarmOffPrompting", str);
    }

    bret = config.getStrValue("Alarm|EnableAlarmAudioOff", str);
    bret &= superConfig.getStrValue("Alarm|EnableAlarmAudioOff", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|EnableAlarmAudioOff", str);
    }

    bret = config.getStrValue("Alarm|EnableAlarmOff", str);
    bret &= superConfig.getStrValue("Alarm|EnableAlarmOff", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|EnableAlarmOff", str);
    }

    bret = config.getStrValue("Alarm|AlarmOffAtPowerOn", str);
    bret &= superConfig.getStrValue("Alarm|AlarmOffAtPowerOn", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|AlarmOffAtPowerOn", str);
    }

    bret = config.getStrValue("Alarm|NonAlertsBeepsInNonAED", str);
    bret &= superConfig.getStrValue("Alarm|NonAlertsBeepsInNonAED", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Alarm|NonAlertsBeepsInNonAED", str);
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
            bret &= superConfig.getStrAttr(prefix + subParamName, "Enable", str1);
            if (bret && (str1 != str))
            {
                superConfig.setStrAttr(prefix + subParamName, "Enable", str);
            }

            bret = config.getStrAttr(prefix + subParamName, "Prio", str);
            bret &= superConfig.getStrAttr(prefix + subParamName, "Prio", str1);
            if (bret && (str1 != str))
            {
                superConfig.setStrAttr(prefix + subParamName, "Prio", str);
            }

            QStringList unitList;
            switch (j)
            {
                case SUB_PARAM_NIBP_DIA:
                case SUB_PARAM_NIBP_MAP:
                case SUB_PARAM_NIBP_SYS:
                {
                    unitList<<Unit::getSymbol(UNIT_KPA)<<Unit::getSymbol(UNIT_MMHG);
                    break;
                }

                case SUB_PARAM_ETCO2:
                case SUB_PARAM_FICO2:
                {
                    unitList<<Unit::getSymbol(UNIT_KPA)<<Unit::getSymbol(UNIT_MMHG)<<Unit::getSymbol(UNIT_PERCENT);
                    break;
                }

                case SUB_PARAM_T1:
                case SUB_PARAM_T2:
                case SUB_PARAM_TD:
                {
                    unitList<<Unit::getSymbol(UNIT_TC)<<Unit::getSymbol(UNIT_TF);
                    break;
                }

                default:
                    unitList<<Unit::getSymbol(paramInfo.getUnitOfSubParam((SubParamID)j));
                    break;
            }

            foreach(QString unitStr, unitList)
            {
                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str);
                bret &= superConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str);
                bret &= superConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str);
                bret &= superConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", str);
                }

                bret = config.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str);
                bret &= superConfig.getStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|High", str);
                bret &= superConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|High", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|High", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Low", str);
                bret &= superConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Low", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Low", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Step", str);
                bret &= superConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Step", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Step", str);
                }

                bret = config.getStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str);
                bret &= superConfig.getStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str1);
                if (bret && (str1 != str))
                {
                    superConfig.setStrValue(prefix + subParamName + "|" + unitStr + "|Scale", str);
                }
            }
        }
    }

    /**************************Local*****************************/
    int languageCount = 0;
    int curLanguage = 0;
    int oldLanguage = 0;
    bret = config.getNumAttr("Local|Language", "CurrentOption", oldLanguage);
    bret &= superConfig.getNumAttr("Local|Language", "CurrentOption", curLanguage);
    superConfig.getNumAttr("Local|Language", "OptionCount", languageCount);
    if (bret && (oldLanguage != curLanguage) && (oldLanguage < languageCount))
    {
        superConfig.setNumAttr("Local|Language", "CurrentOption", oldLanguage);
    }

    bret = config.getStrValue("Local|NIBPUnit", str);
    bret &= superConfig.getStrValue("Local|NIBPUnit", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Local|NIBPUnit", str);
    }

    bret = config.getStrValue("Local|CO2Unit", str);
    bret &= superConfig.getStrValue("Local|CO2Unit", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Local|CO2Unit", str);
    }

    bret = config.getStrValue("Local|TEMPUnit", str);
    bret &= superConfig.getStrValue("Local|TEMPUnit", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Local|TEMPUnit", str);
    }

    /*************************Display****************************/
    bret = config.getStrValue("Display|ECGColor", str);
    bret &= superConfig.getStrValue("Display|ECGColor", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|ECGColor", str);
    }

    bret = config.getStrValue("Display|SPO2Color", str);
    bret &= superConfig.getStrValue("Display|SPO2Color", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|SPO2Color", str);
    }

    bret = config.getStrValue("Display|NIBPColor", str);
    bret &= superConfig.getStrValue("Display|NIBPColor", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|NIBPColor", str);
    }

    bret = config.getStrValue("Display|CO2Color", str);
    bret &= superConfig.getStrValue("Display|CO2Color", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|CO2Color", str);
    }

    bret = config.getStrValue("Display|RESPColor", str);
    bret &= superConfig.getStrValue("Display|RESPColor", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|RESPColor", str);
    }

    bret = config.getStrValue("Display|TEMPColor", str);
    bret &= superConfig.getStrValue("Display|TEMPColor", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Display|TEMPColor", str);
    }

    /*************************summary trriger****************************/
    bret = config.getStrValue("Print|PresentingECG", str);
    bret &= superConfig.getStrValue("Print|PresentingECG", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|PresentingECG", str);
    }

    bret = config.getStrValue("Print|ECGAnalysis", str);
    bret &= superConfig.getStrValue("Print|ECGAnalysis", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|ECGAnalysis", str);
    }

    bret = config.getStrValue("Print|ShockDelivery", str);
    bret &= superConfig.getStrValue("Print|ShockDelivery", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|ShockDelivery", str);
    }

    bret = config.getStrValue("Print|CheckPatient", str);
    bret &= superConfig.getStrValue("Print|CheckPatient", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|CheckPatient", str);
    }

    bret = config.getStrValue("Print|PacerStartUp", str);
    bret &= superConfig.getStrValue("Print|PacerStartUp", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|PacerStartUp", str);
    }

    bret = config.getStrValue("Print|PhysiologicalAlarm", str);
    bret &= superConfig.getStrValue("Print|PhysiologicalAlarm", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|PhysiologicalAlarm", str);
    }

    bret = config.getStrValue("Print|CoderMarker", str);
    bret &= superConfig.getStrValue("Print|CoderMarker", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|CoderMarker", str);
    }

    bret = config.getStrValue("Print|NIBPReading", str);
    bret &= superConfig.getStrValue("Print|NIBPReading", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|NIBPReading", str);
    }

    bret = config.getStrValue("Print|DiagnosticECG", str);
    bret &= superConfig.getStrValue("Print|DiagnosticECG", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|DiagnosticECG", str);
    }

    bret = config.getStrValue("Print|SummaryReportSnapshotPrintingInAED", str);
    bret &= superConfig.getStrValue("Print|SummaryReportSnapshotPrintingInAED", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|SummaryReportSnapshotPrintingInAED", str);
    }

    bret = config.getStrValue("Print|Print30JSelfTestReport", str);
    bret &= superConfig.getStrValue("Print|Print30JSelfTestReport", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Print|Print30JSelfTestReport", str);
    }

    /*************************ECG 12L****************************/
    bret = config.getStrValue("ECG12L|NotchFilter", str);
    bret &= superConfig.getStrValue("ECG12L|NotchFilter", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|NotchFilter", str);
    }

    bret = config.getStrValue("ECG12L|ECG12LeadBandwidth", str);
    bret &= superConfig.getStrValue("ECG12L|ECG12LeadBandwidth", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|ECG12LeadBandwidth", str);
    }

    bret = config.getStrValue("ECG12L|PrintSnapshotFormat", str);
    bret &= superConfig.getStrValue("ECG12L|PrintSnapshotFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|PrintSnapshotFormat", str);
    }

    bret = config.getStrValue("ECG12L|PDFReportFormat", str);
    bret &= superConfig.getStrValue("ECG12L|PDFReportFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|PDFReportFormat", str);
    }

    bret = config.getStrValue("ECG12L|DisplayFormat", str);
    bret &= superConfig.getStrValue("ECG12L|DisplayFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|DisplayFormat", str);
    }

    bret = config.getStrValue("ECG12L|TimeIntervalFor2x6Report", str);
    bret &= superConfig.getStrValue("ECG12L|TimeIntervalFor2x6Report", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|TimeIntervalFor2x6Report", str);
    }

    bret = config.getStrValue("ECG12L|AutoPrinting12LReport", str);
    bret &= superConfig.getStrValue("ECG12L|AutoPrinting12LReport", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|AutoPrinting12LReport", str);
    }

    bret = config.getStrValue("ECG12L|AutoTransmission", str);
    bret &= superConfig.getStrValue("ECG12L|AutoTransmission", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|AutoTransmission", str);
    }

    bret = config.getStrValue("ECG12L|TransmissionFormat", str);
    bret &= superConfig.getStrValue("ECG12L|TransmissionFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|TransmissionFormat", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintTopLead", str);
    bret &= superConfig.getStrValue("ECG12L|RealtimePrintTopLead", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|RealtimePrintTopLead", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintMiddleLead", str);
    bret &= superConfig.getStrValue("ECG12L|RealtimePrintMiddleLead", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|RealtimePrintMiddleLead", str);
    }

    bret = config.getStrValue("ECG12L|RealtimePrintBottomLead", str);
    bret &= superConfig.getStrValue("ECG12L|RealtimePrintBottomLead", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("ECG12L|RealtimePrintBottomLead", str);
    }

    /*************************DateTime****************************/
    bret = config.getStrValue("DateTime|DateFormat", str);
    bret &= superConfig.getStrValue("DateTime|DateFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("DateTime|DateFormat", str);
    }

    bret = config.getStrValue("DateTime|TimeFormat", str);
    bret &= superConfig.getStrValue("DateTime|TimeFormat", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("DateTime|TimeFormat", str);
    }

    bret = config.getStrValue("DateTime|DisplaySecond", str);
    bret &= superConfig.getStrValue("DateTime|DisplaySecond", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("DateTime|DisplaySecond", str);
    }

    /*************************CodeMarker****************************/
    superConfig.getNumAttr("Local|Language", "OptionCount", languageCount);
    for (int i = 0; i < languageCount; ++i)
    {
        bret = config.getStrValue("CodeMarker|Marker" + QString::number(i), str);
        bret &= superConfig.getStrValue("CodeMarker|Marker" + QString::number(i), str1);
        if (bret && (str1 != str))
        {
            superConfig.setStrValue("CodeMarker|Marker" + QString::number(i), str);
        }

        bret = config.getStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str);
        bret &= superConfig.getStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str1);
        if (bret && (str1 != str))
        {
            superConfig.setStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str);
        }
    }

    /*************************WIFI****************************/
    bret = config.getStrValue("WiFi|EnableWifi", str);
    bret &= superConfig.getStrValue("WiFi|EnableWifi", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("WiFi|EnableWifi", str);
    }

    int count = 0, current = 0;
    config.getNumAttr("WiFi|Profiles", "Count", count);
    superConfig.setNumAttr("WiFi|Profiles", "Count", count);
    config.getNumAttr("WiFi|Profiles", "CurrentSelect", current);
    superConfig.setNumAttr("WiFi|Profiles", "CurrentSelect", current);
    for (int i = 0; i < count; ++i)
    {
        QString strPath = "WiFi|Profiles|Profile" + QString::number(i);
        QString nodeName = strPath;
        strPath += "|";

        superConfig.addNode("WiFi|Profiles", QString("Profile%1").arg(i));

        config.getStrValue(strPath + "ProfileName", str);
        superConfig.addNode(nodeName, "ProfileName", str);

        config.getStrValue(strPath + "SSID", str);
        superConfig.addNode(nodeName, "SSID", str);

        config.getStrValue(strPath + "AuthType", str);
        superConfig.addNode(nodeName, "AuthType", str);

        config.getStrValue(strPath + "SecurityKey", str);
        superConfig.addNode(nodeName, "SecurityKey", str);

        config.getStrValue(strPath + "IsStatic", str);
        superConfig.addNode(nodeName, "IsStatic", str);

        if (1 == str.toInt())
        {
            config.getStrValue(strPath + "StaticIP", str);
            superConfig.addNode(nodeName, "StaticIP", str);

            config.getStrValue(strPath + "DefaultGateway", str);
            superConfig.addNode(nodeName, "DefaultGateway", str);

            config.getStrValue(strPath + "SubnetMask", str);
            superConfig.addNode(nodeName, "SubnetMask", str);

            config.getStrValue(strPath + "PreferedDNS", str);
            superConfig.addNode(nodeName, "PreferedDNS", str);

            config.getStrValue(strPath + "AlternateDNS", str);
            superConfig.addNode(nodeName, "AlternateDNS", str);
        }
    }

    /*************************Sftp****************************/
    bret = config.getStrValue("Sftp|ServerIP", str);
    bret &= superConfig.getStrValue("Sftp|ServerIP", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Sftp|ServerIP", str);
    }

    bret = config.getStrValue("Sftp|ServerPort", str);
    bret &= superConfig.getStrValue("Sftp|ServerPort", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Sftp|ServerPort", str);
    }

    bret = config.getStrValue("Sftp|Username", str);
    bret &= superConfig.getStrValue("Sftp|Username", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Sftp|Username", str);
    }

    bret = config.getStrValue("Sftp|Password", str);
    bret &= superConfig.getStrValue("Sftp|Password", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Sftp|Password", str);
    }

    /*************************Mail****************************/
    bret = config.getStrValue("Mail|SmtpServerPort", str);
    bret &= superConfig.getStrValue("Mail|SmtpServerPort", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Mail|SmtpServerPort", str);
    }

    bret = config.getStrValue("Mail|SmtpUsername", str);
    bret &= superConfig.getStrValue("Mail|SmtpUsername", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Mail|SmtpUsername", str);
    }

    bret = config.getStrValue("Mail|Password", str);
    bret &= superConfig.getStrValue("Mail|Password", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Mail|Password", str);
    }

    bret = config.getStrValue("Mail|ConnectionSecurity", str);
    bret &= superConfig.getStrValue("Mail|ConnectionSecurity", str1);
    if (bret && (str1 != str))
    {
        superConfig.setStrValue("Mail|ConnectionSecurity", str);
    }

    config.getNumAttr("Mail|Recipients", "Count", count);
    superConfig.setNumAttr("Mail|Recipients", "Count", count);

    for (int i = 0; i < count; ++i)
    {
        QString path = "Mail|Recipients|Recipient" + QString::number(i);
        QString nodeName = path;
        path += "|";

        superConfig.addNode("Mail|Recipients", "Recipient" + QString::number(i));

        config.getStrValue(path + "Name", str);
        superConfig.addNode(nodeName, "Name", str);

        config.getStrValue(path + "Address", str);
        superConfig.addNode(nodeName, "Address", str);
    }

    superConfig.saveToDisk();
}

