#include <errno.h>
#include <QFile>
#include <QProcess>
#include <QRegExp>
#include "SupervisorConfigManager.h"
#include "USBManager.h"
#include "IMessageBox.h"
#include "ECGDefine.h"
#include "NIBPDefine.h"
#include "LanguageManager.h"
#include "TimeDate.h"
#include "TimeDefine.h"
#include "TimeSymbol.h"
#include "PatientDefine.h"
#include "FloatHandle.h"
#include "PrintDefine.h"
#include "AlarmDefine.h"
#include "SoundManager.h"
#include "SystemManager.h"

SupervisorConfigManager *SupervisorConfigManager::_selfobj = NULL;

/*******************************************************************************
 * 功能：加载工厂配置。
 ******************************************************************************/
void SupervisorConfigManager::loadDefaultConfig()
{
    bool ret = currentConfig.load(systemConfig.getCurConfigName());

    IMessageBox messageBox(ret ? trs("Prompt") : trs("Warn"),
                           ret ? trs("PromptLoadDefaultCfgOK") : trs("WarningLoadDefaultCfgFail"),
                           QStringList(trs("EnglishYESChineseSURE")));
    messageBox.exec();
}

/*******************************************************************************
 * 功能：导出配置。
 * 返回:
 *    true,成功；false，失败
 ******************************************************************************/
bool SupervisorConfigManager::exportConfig()
{
    //当前配置是否需要保存
    if (currentConfig.getSaveStatus())
    {
        //提示是否保存
        IMessageBox messageBox(trs("Prompt"), trs("PromptCfgNotSave"));
        if (1 == messageBox.exec())
        {
            currentConfig.saveToDisk();
        }
    }

    //配置文件大小
    QString srcFile(systemConfig.getCurConfigName());
    QFile file(srcFile);
    if (!file.isOpen())
    {
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            debug("\033[31m %s open fail,error:%s \033[m", qPrintable(srcFile),
                  strerror(errno));
            return false;
        }
    }

    //KB
    int fileSize = file.size() >> 10;
    file.close();
    int usdFreeSize = (int)usbManager.getUSBFreeSize();
    if (-1 == usdFreeSize)
    {
        //警告当前U断开连接
        IMessageBox messageBox(trs("Warn"), trs("WarnUSBDisconnected"), false);
        messageBox.exec();
        return false;
    }
    else if (fileSize > usdFreeSize)
    {
        //警告当前U盘剩余空间不足
        IMessageBox messageBox(trs("Warn"), trs("WarnLessUSBFreeSpace"), false);
        messageBox.exec();
        return false;
    }

    //目标文件命名
    QString dstFile(usbManager.getUdiskMountPoint());
    dstFile += "/nPMSConfig-";
    dstFile += systemManager.getSoftwareVersionNum();
    dstFile += "-";
    QString timeStr("");
    timeStr.sprintf("%04d%02d%02d-%02d%02d%02d.%03d", timeDate.getDateYear()
            ,timeDate.getDateMonth()
            ,timeDate.getDateDay()
            ,timeDate.getTimeHour()
            ,timeDate.getTimeMinute()
            ,timeDate.getTimeSenonds()
            ,timeDate.getTimeMsec());
    dstFile += timeStr;
    dstFile += ".xml";

    int code = QProcess::execute(QString("cp -f %1 %2").arg(srcFile).arg(dstFile));
    QProcess::execute("sync");

    if (code < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*******************************************************************************
 * 功能：导入配置。
 * 返回:
 *     true,成功；false，失败
 ******************************************************************************/
bool SupervisorConfigManager::importConfig()
{
    //检查导入文件是否存在
    QString importFile(usbManager.getUdiskMountPoint());
    importFile += IMPORT_CONFIG_FILE;
    if (!QFile::exists(importFile))
    {
        //导入文件不存在或文件名错误
        IMessageBox messageBox(trs("Warn"), trs("WarningNoImportFile"), false);
        messageBox.exec();
        return false;
    }

    //检查导入文件内容
    QString errorStr;
    if (!_checkImportFile(importFile, errorStr))
    {
        //内容错误
        IMessageBox messageBox(trs("Warn"), trs("WarningImportFileError") + errorStr, false);
        messageBox.exec();
        return false;
    }

    return currentConfig.load(importFile);
}

/*******************************************************************************
 * 功能：check import value is valid。
 ******************************************************************************/
bool SupervisorConfigManager::_checkRegexp(const QString &pattern, const QString &value)
{
    if (pattern.isEmpty())
    {
        return false;
    }

    QRegExp regExp(pattern);
    int pos = 0;
    QString resStr;
    while ((pos = regExp.indexIn(value, pos)) != -1)
    {
        resStr += regExp.cap();
        pos += regExp.matchedLength();
    }

    return (resStr == value);
}

/*******************************************************************************
 * 功能：检查导入配置文件内容。
 ******************************************************************************/
bool SupervisorConfigManager::_checkImportFile(QString file, QString &error)
{
    Config config(file);
    if (config.getFileName() != file)
    {
        error = trs("ErrorFormatFile");
        return false;
    }

    QString str;
    QString SuperCfgStr;
    int value;
    int tmpValue;
    bool bret = false;

/*************************General****************************/

    bret = config.getStrValue("General|Password", str);
    bret &= _checkRegexp("[0-9]", str);
    if (str.size() > 11 || !bret)
    {
        debug("Password size error:%s", qPrintable(str));
        error = "General|Password";
        return false;
    }

    bret = config.getStrValue("General|DeviceIdentifier", str);
    bret &= _checkRegexp("[a-zA-Z]|[0-9]|_", str);
    if (str.size() > 11 || !bret)
    {
        debug("DeviceID size error:%s", qPrintable(str));
        error = "General|DeviceIdentifier";
        return false;
    }

    bret = config.getNumValue("General|DefaultPatientType", value);
    if (value >= PATIENT_TYPE_NEO || !bret)
    {
        debug("patient type error:%d", value);
        error = "General|DefaultPatientType";
        return false;
    }

    bret = config.getNumValue("General|DefaultDisplayBrightness", value);
    if (value >= BRT_LEVEL_NR || !bret)
    {
        debug("Default Display Brightness error:%d", value);
        error = "General|DefaultDisplayBrightness";
        return false;
    }

    bret = config.getNumValue("General|FullDisclosureRecording", value);
    if (value > 1 || !bret)
    {
        debug("Full Disclosure Recording error:%d", value);
        error = "General|FullDisclosureRecording";
        return false;
    }

    bret = config.getStrValue("General|SuperPassword", str);
    if (!bret)
    {
        debug("SuperPassword size error:%s", qPrintable(str));
        error = "General|SuperPassword";
        return false;
    }

    currentConfig.getStrValue("General|SuperPassword", SuperCfgStr);
    if (str != SuperCfgStr)
    {
        config.setStrValue("General|SuperPassword", SuperCfgStr);
    }

/*************************Basic Defib****************************/
    bret = config.getNumValue("BasicDefib|DefaultVoicePromptTone", value);
    if (value > 5 || !bret)
    {
        debug("BasicDefib DefaultVoicePromptTone error:%d", value);
        error = "BasicDefib|DefaultVoicePromptTone";
        return false;
    }

    bret = config.getNumValue("BasicDefib|DefibDefaultToPads", value);
    if (value > 1 || !bret)
    {
        debug("BasicDefib DefibDefaultToPads error:%d", value);
        error = "BasicDefib|DefibDefaultToPads";
        return false;
    }

    bret = config.getNumValue("BasicDefib|BasicAutoEnergyEscalation", value);
    if (value > 1 || !bret)
    {
        debug("BasicDefib BasicAutoEnergyEscalation error:%d", value);
        error = "BasicDefib|BasicAutoEnergyEscalation";
        return false;
    }

    bret = config.getNumValue("BasicDefib|RetainSyncAfterShock", value);
    if (value > 1 || !bret)
    {
        debug("BasicDefib RetainSyncAfterShock error:%d", value);
        error = "BasicDefib|RetainSyncAfterShock";
        return false;
    }

/*************************AED****************************/
    bret = config.getNumValue("AED|StartWithCPR", value);
    if (value > 1 || !bret)
    {
        debug("AED StartWithCPR error:%d", value);
        error = "AED|StartWithCPR";
        return false;
    }

    bret = config.getNumValue("AED|RestartAnalysisAfterCPR", value);
    if (value > 1 || !bret)
    {
        debug("AED RestartAnalysisAfterCPR error:%d", value);
        error = "AED|RestartAnalysisAfterCPR";
        return false;
    }

    bret = config.getNumValue("AED|EnableManualModePassword", value);
    if (value > 1 || !bret)
    {
        debug("AED EnableManualModePassword error:%d", value);
        error = "AED|EnableManualModePassword";
        return false;
    }

    bret = config.getStrValue("AED|ManualModePassword", str);
    if (str.size() == 4)
    {
        bret &= _checkRegexp("[0-9]", str);
    }

    if (str.size() != 4 || !bret)
    {
        debug("AED ManualModePassword error:%s", qPrintable(str));
        error = "AED|ManualModePassword";
        return false;
    }

    bret = config.getNumValue("AED|QRSPRTone", value);
    if (value > 1 || !bret)
    {
        debug("AED QRSPRTone error:%d", value);
        error = "AED|QRSPRTone";
        return false;
    }

    bret = config.getNumValue("AED|PhyAlarmVisualIndicator", value);
    if (value > 1 || !bret)
    {
        debug("AED PhyAlarmVisualIndicator error:%d", value);
        error = "AED|PhyAlarmVisualIndicator";
        return false;
    }

/*************************PACER****************************/
    bret = config.getNumValue("PACER|DefaultPacerRate", value);
    if (value > 180 || value < 30 || !bret)
    {
        debug("PACER DefaultPacerRate error:%d", value);
        error = "PACER|DefaultPacerRate";
        return false;
    }

    bret = config.getNumValue("PACER|FixedRatePacing", value);
    if (value > 1 || !bret)
    {
        debug("PACER FixedRatePacing error:%d", value);
        error = "PACER|FixedRatePacing";
        return false;
    }

/*************************CPR****************************/
    bret = config.getNumValue("CPR|EnableVoicePrompts", value);
    if (value > 1 || !bret)
    {
        debug("CPR EnableVoicePrompts error:%d", value);
        error = "CPR|EnableVoicePrompts";
        return false;
    }

    bret = config.getNumValue("CPR|EnableTextPrompts", value);
    if (value > 1 || !bret)
    {
        debug("CPR EnableTextPrompts error:%d", value);
        error = "CPR|EnableTextPrompts";
        return false;
    }

    bret = config.getNumValue("CPR|MetronomeRate", value);
    if (value < 80 || value > 120 || !bret)
    {
        debug("CPR MetronomeRate error:%d", value);
        error = "CPR|MetronomeRate";
        return false;
    }

    bret = config.getNumValue("CPR|CompressionDepthUnit", value);
    if ((value !=  UNIT_CM && value != UNIT_INCH) || !bret)
    {
        debug("CPR CompressionDepthUnit error:%d", value);
        error = "CPR|CompressionDepthUnit";
        return false;
    }

    bret = config.getStrValue("CPR|TargetCompressionDepth", str);
    switch (value)
    {
        case UNIT_INCH:
        {
            float tempValue = str.toFloat();
            if (tempValue < 1.0 || tempValue > 3.0  || !bret)
            {
                debug("CPR TargetCPRDepth error:%f", tempValue);
                error = "CPR|TargetCompressionDepth";
                return false;
            }
            break;
        }
        case UNIT_CM:
        {
            float tempValue = str.toFloat();
            if (tempValue < 2.5 || tempValue > 7.8 || !bret)
            {
                debug("CPR TargetCPRDepth error:%f", tempValue);
                error = "CPR|TargetCompressionDepth";
                return false;
            }
            break;
        }
        default:
            break;
    }

    bret = config.getNumValue("CPR|CPRWaveformRecording", value);
    if (value > 1 || !bret)
    {
        debug("CPR CPRWaveformRecording error:%d", value);
        error = "CPR|CPRWaveformRecording";
        return false;
    }

/*************************ECG****************************/
    bret = config.getNumValue("ECG|PadsECGBandwidth", value);
    if (value > 1 || !bret)
    {
        debug("ECG PadsECGBandwidth error:%d", value);
        error = "ECG|PadsECGBandwidth";
        return false;
    }

    bret = config.getNumValue("ECG|ChestLeadsECGBandwidth", value);
    if (value > 1 || !bret)
    {
        debug("ECG ChestLeadsECGBandwidth error:%d", value);
        error = "ECG|ChestLeadsECGBandwidth";
        return false;
    }

    bret = config.getNumValue("ECG|DefaultECGLeadInMonitorMode", value);
    if (value > 3 || !bret)
    {
        debug("ECG DefaultECGLeadInMonitorMode error:%d", value);
        error = "ECG|DefaultECGLeadInMonitorMode";
        return false;
    }

    bret = config.getNumValue("ECG|ECGLeadConvention", value);
    if (value >= ECG_CONVENTION_NR || !bret)
    {
        debug("ECG ECGLeadConvention error:%d", value);
        error = "ECG|ECGLeadConvention";
        return false;
    }

    bret = config.getNumValue("RESP|AutoActivation", value);
    if (value > 1 || !bret)
    {
        debug("RESP AutoActivation error:%d", value);
        error = "RESP|AutoActivation";
        return false;
    }

/*************************NIBP****************************/

    bret = config.getNumValue("NIBP|MeasureMode", value);
    if (value > 1 || !bret)
    {
        debug("NIBP MeasureMode error:%d", value);
        error = "NIBP|MeasureMode";
        return false;
    }

    bret = config.getNumValue("NIBP|AutoInterval", value);
    if (value >= NIBP_AUTO_INTERVAL_NR || !bret)
    {
        debug("NIBP AutoInterval error:%d", value);
        error = "NIBP|AutoInterval";
        return false;
    }

    bret = config.getNumValue("NIBP|StatFunction", value);
    if (value > 1 || !bret)
    {
        debug("NIBP StatFunction error:%d", value);
        error = "NIBP|StatFunction";
        return false;
    }

    bret = config.getNumValue("NIBP|AutomaticRetry", value);
    if (value > 1 || !bret)
    {
        debug("NIBP AutomaticRetry error:%d", value);
        error = "NIBP|AutomaticRetry";
        return false;
    }

    bret = config.getNumValue("NIBP|AdultInitialCuffInflation", value);
    if (value >= NIBP_ADULT_INITIAL_CUFF_NR || !bret)
    {
        debug("NIBP AdultInitialCuffInflation error:%d", value);
        error = "NIBP|AdultInitialCuffInflation";
        return false;
    }

    bret = config.getNumValue("NIBP|PedInitialCuffInflation", value);
    if (value >= NIBP_PREDIATRIC_INITIAL_CUFF_NR || !bret)
    {
        debug("NIBP PedInitialCuffInflation error:%d", value);
        error = "NIBP|PedInitialCuffInflation";
        return false;
    }

//    bret = config.getNumValue("NIBP|NeoInitialCuffInflation", value);
//    if (value >= NIBP_NEONATAL_INITIAL_CUFF_NR || !bret)
//    {
//        debug("NIBP NeoInitialCuffInflation error:%d\n", value);
//        error = "NIBP|NeoInitialCuffInflation";
//        return false;
//    }

/**************************RESP & CO2*****************************/
    bret = config.getNumValue("CO2|CO2ModeDefault", value);
    if (value > 1 || !bret)
    {
        debug("CO2 CO2ModeDefault error:%d", value);
        error = "CO2|CO2ModeDefault";
        return false;
    }

    bret = config.getNumValue("CO2|CO2SweepMode", value);
    if (value > WAVE_DRAWING_MODE_FILL || !bret)
    {
        debug("CO2 CO2SweepMode error:%d", value);
        error = "CO2|CO2SweepMode";
        return false;
    }

/*************************Alarm****************************/
    bret = config.getNumValue("Alarm|MinimumAlarmVolume", value);
    if (value > SoundManager::VOLUME_LEV_4 || !bret)
    {
        debug("Alarm MinimumAlarmVolume error:%d", value);
        error = "Alarm|MinimumAlarmVolume";
        return false;
    }

    bret = config.getNumValue("Alarm|DefaultAlarmVolume", tmpValue);
    if (tmpValue > SoundManager::VOLUME_LEV_5 || tmpValue < (value + 1) || !bret)
    {
        debug("Alarm DefaultAlarmVolume error:%d", value);
        error = "Alarm|DefaultAlarmVolume";
        return false;
    }

    bret = config.getNumValue("Alarm|AlarmPauseTime", value);
    if (value > ALARM_PAUSE_TIME_NR || !bret)
    {
        debug("Alarm AlarmPauseTime error:%d", value);
        error = "Alarm|AlarmPauseTime";
        return false;
    }

    bret = config.getNumValue("Alarm|AlarmOffPrompting", value);
    if (value > ALARM_CLOSE_PROMPT_NR || !bret)
    {
        debug("Alarm AlarmOffPrompting error:%d", value);
        error = "Alarm|AlarmOffPrompting";
        return false;
    }

    bret = config.getNumValue("Alarm|EnableAlarmAudioOff", value);
    if (value > 1 || !bret)
    {
        debug("Alarm EnableAlarmAudioOff error:%d", value);
        error = "Alarm|EnableAlarmAudioOff";
        return false;
    }

    bret = config.getNumValue("Alarm|EnableAlarmOff", value);
    if (value > 1 || !bret)
    {
        debug("Alarm EnableAlarmOff error:%d", value);
        error = "Alarm|EnableAlarmOff";
        return false;
    }

    bret = config.getNumValue("Alarm|AlarmOffAtPowerOn", value);
    if (value > 1 || !bret)
    {
        debug("Alarm AlarmOffAtPowerOn error:%d", value);
        error = "Alarm|AlarmOffAtPowerOn";
        return false;
    }

    bret = config.getNumValue("Alarm|NonAlertsBeepsInNonAED", value);
    if (value > 1 || !bret)
    {
        debug("Alarm NonAlertsBeepsInNonAED error:%d", value);
        error = "Alarm|NonAlertsBeepsInNonAED";
        return false;
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
            bret = config.getNumAttr(prefix + subParamName, "Enable", value);
            if (value > 1 || !bret)
            {
                debug("AlarmSource %s Enable error:%d", qPrintable(subParamName), value);
                error = prefix + subParamName;
                error += "|Enable";
                return false;
            }

            bret = config.getNumAttr(prefix + subParamName, "Prio", value);
            if (value > ALARM_PRIO_HIGH || !bret)
            {
                debug("AlarmSource %s Prio error:%d", qPrintable(subParamName), value);
                error = prefix + subParamName;
                error += "|Prio";
                return false;
            }

            QStringList unitList;
            switch(j)
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
            }

            foreach(QString unitStr, unitList)
            {
                int maxHV = 0, maxLV = 0, minHV = 0, minLV = 0, max = 0, min = 0;

                bret = config.getNumAttr(prefix + subParamName + "|" + unitStr + "|High", "Max", maxHV);
                bret &= config.getNumAttr(prefix + subParamName + "|" + unitStr + "|High", "Min", maxLV);
                bret &= config.getNumAttr(prefix + subParamName + "|" + unitStr + "|Low", "Max", minHV);
                bret &= config.getNumAttr(prefix + subParamName + "|" + unitStr + "|Low", "Min", minLV);
                bret &= config.getNumValue(prefix + subParamName + "|" + unitStr + "|High", max);
                bret &= config.getNumValue(prefix + subParamName + "|" + unitStr + "|Low", min);
                if ((min > max) || (maxLV > max) ||
                        (max > maxHV) || (min > minHV) || (minLV > min) || !bret)
                {
                    debug("AlarmSource %s limit error:%d", qPrintable(subParamName), value);
                    error = prefix + subParamName + "|" + unitStr;
                    return false;
                }

                int stepImport = 0, stepOld = 0;
                bret = config.getNumValue(prefix + subParamName + "|" + unitStr + "|Step", stepImport);
                currentConfig.getNumValue(prefix + subParamName + "|" + unitStr + "|Step", stepOld);
                if (stepImport != stepOld || !bret)
                {
                    debug("AlarmSource %s step error:%d", qPrintable(subParamName), stepImport);
                    error = prefix + subParamName + "|" + unitStr + "|Step";
                    return false;
                }

                int scaleImport = 0, scaleOld = 0;
                bret = config.getNumValue(prefix + subParamName + "|" + unitStr + "|Scale", scaleImport);
                currentConfig.getNumValue(prefix + subParamName + "|" + unitStr + "|Scale", scaleOld);
                if (stepImport != stepOld || !bret)
                {
                    debug("AlarmSource %s Scale error:%d", qPrintable(subParamName), scaleImport);
                    error = prefix + subParamName + "|" + unitStr + "|Scale";
                    return false;
                }
            }
        }
    }

/**************************Local*****************************/
    int languageCount = 0;
    int curLanguage = 0;
    bret = config.getNumAttr("Local|Language", "CurrentOption", curLanguage);
    bret &= config.getNumAttr("Local|Language", "OptionCount", languageCount);
    if (curLanguage >= languageCount || !bret)
    {
        debug("Local Language error:%d", curLanguage);
        error = "Local|Language|CurrentOption";
        return false;
    }

    QString prefix = "/usr/local/nPM/locale/";
    for (int i = 0; i < languageCount; ++i)
    {
        if (!config.getStrAttr("Local|Language|Opt" + QString::number(i), "XmlFileName", str))
        {
            debug("Local Language Opt%d error:", i);
            error = "Local|Language|Opt" + QString::number(i);
            return false;
        }

        if (!QFile::exists(prefix + str + ".xml"))
        {
            debug("Local Language %s.xml not exist", qPrintable(str));
            error = prefix + str + ".xml";
            error += "not exist";
            return false;
        }
    }

    bret = config.getNumValue("Local|NIBPUnit", value);
    if ((value != UNIT_KPA && value != UNIT_MMHG) || !bret)
    {
        debug("Local NIBPUnit error:%d", value);
        error = "Local|NIBPUnit";
        return false;
    }

    bret = config.getNumValue("Local|CO2Unit", value);
    if ((value != UNIT_KPA && value != UNIT_MMHG && value != UNIT_PERCENT) || !bret)
    {
        debug("Local CO2Unit error:%d", value);
        error = "Local|CO2Unit";
        return false;
    }

    bret = config.getNumValue("Local|TEMPUnit", value);
    if ((value != UNIT_TC && value != UNIT_TF) || !bret)
    {
        debug("Local TEMPUnit error:%d", value);
        error = "Local|TEMPUnit";
        return false;
    }

/*************************Display****************************/
    bret = config.getStrValue("Display|AllColors", str);
    if (!bret)
    {
        debug("Get Display AllColors error");
        error = "Display|AllColors";
        return false;
    }
    QStringList strList = str.split(',', QString::KeepEmptyParts);

    tmpValue = strList.count();
    for (int i = 0; i < tmpValue; ++i)
    {
        bret = config.getNumValue("Display|" + strList.at(i) + "|r", value);
        if (value > 255 || !bret)
        {
            debug("Display %s r error:%d", qPrintable(strList.at(i)), value);
            error = "Display|" + strList.at(i) + "|r";
            return false;
        }

        bret = config.getNumValue("Display|" + strList.at(i) + "|g", value);
        if (value > 255 || !bret)
        {
            debug("Display %s g error:%d", qPrintable(strList.at(i)), value);
            error = "Display|" + strList.at(i) + "|g";
            return false;
        }

        bret = config.getNumValue("Display|" + strList.at(i) + "|b", value);
        if (value > 255 || !bret)
        {
            debug("Display %s b error:%d", qPrintable(strList.at(i)), value);
            error = "Display|" + strList.at(i) + "|b";
            return false;
        }
    }

    bret = config.getStrValue("Display|ECGColor", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display ECGColor error:%s", qPrintable(str));
        error = "Display|ECGColor";
        return false;
    }

    bret = config.getStrValue("Display|SPO2Color", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display SPO2Color error:%s", qPrintable(str));
        error = "Display|SPO2Color";
        return false;
    }

    bret = config.getStrValue("Display|NIBPColor", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display NIBPColor error:%s", qPrintable(str));
        error = "Display|NIBPColor";
        return false;
    }

    bret = config.getStrValue("Display|CO2Color", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display CO2Color error:%s", qPrintable(str));
        error = "Display|CO2Color";
        return false;
    }

    bret = config.getStrValue("Display|RESPColor", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display RESPColor error:%s", qPrintable(str));
        error = "Display|RESPColor";
        return false;
    }

    bret = config.getStrValue("Display|TEMPColor", str);
    if (-1 == strList.indexOf(str) || !bret)
    {
        debug("Display TEMPColor error:%s", qPrintable(str));
        error = "Display|TEMPColor";
        return false;
    }

    bret = config.getStrValue("Display|CPRBarGraphColor", str);
    currentConfig.getStrValue("Display|CPRBarGraphColor", SuperCfgStr);
    if (str != SuperCfgStr || !bret)
    {
        debug("Display CPRBarGraphColor error:%s", qPrintable(str));
        error = "Display|CPRBarGraphColor";
        return false;
    }

/*************************summary trriger****************************/
    bret = config.getNumValue("Print|PresentingECG", value);
    if (value > 1 || !bret)
    {
        debug("Print PresentingECG error:%d", value);
        error = "Print|PresentingECG";
        return false;
    }

    bret = config.getNumValue("Print|ECGAnalysis", value);
    if (value > 1 || !bret)
    {
        debug("Print ECGAnalysis error:%d", value);
        error = "Print|ECGAnalysis";
        return false;
    }

    bret = config.getNumValue("Print|ShockDelivery", value);
    if (value > 1 || !bret)
    {
        debug("Print ShockDelivery error:%d", value);
        error = "Print|ShockDelivery";
        return false;
    }

    bret = config.getNumValue("Print|CheckPatient", value);
    if (value > 1 || !bret)
    {
        debug("Print CheckPatient error:%d", value);
        error = "Print|CheckPatient";
        return false;
    }

    bret = config.getNumValue("Print|PacerStartUp", value);
    if (value > 1 || !bret)
    {
        debug("Print PacerStartUp error:%d", value);
        error = "Print|PacerStartUp";
        return false;
    }

    bret = config.getNumValue("Print|PhysiologicalAlarm", value);
    if (value > 1 || !bret)
    {
        debug("Print PhysiologicalAlarm error:%d", value);
        error = "Print|PhysiologicalAlarm";
        return false;
    }

    bret = config.getNumValue("Print|CoderMarker", value);
    if (value > 1 || !bret)
    {
        debug("Print CoderMarker error:%d", value);
        error = "Print|CoderMarker";
        return false;
    }

    bret = config.getNumValue("Print|NIBPReading", value);
    if (value > 1 || !bret)
    {
        debug("Print NIBPReading error:%d", value);
        error = "Print|NIBPReading";
        return false;
    }

    bret = config.getNumValue("Print|DiagnosticECG", value);
    if (value > 1 || !bret)
    {
        debug("Print DiagnosticECG error:%d", value);
        error = "Print|DiagnosticECG";
        return false;
    }

    bret = config.getNumValue("Print|SummaryReportSnapshotPrintingInAED", value);
    if (value > 1 || !bret)
    {
        debug("Print SummaryReportSnapshotPrintingInAED error:%d", value);
        error = "Print|SummaryReportSnapshotPrintingInAED";
        return false;
    }

    bret = config.getNumValue("Print|Print30JSelfTestReport", value);
    if (value > 1 || !bret)
    {
        debug("Print Print30JSelfTestReport error:%d", value);
        error = "Print|Print30JSelfTestReport";
        return false;
    }

/*************************ECG 12L****************************/
    bret = config.getNumValue("ECG12L|NotchFilter", value);
    if (value >= ECG_NOTCH_50_AND_60HZ || !bret)
    {
        debug("ECG12L NotchFilter error:%d", value);
        error = "ECG12L|NotchFilter";
        return false;
    }

    bret = config.getNumValue("ECG12L|ECG12LeadBandwidth", value);
    if (value > 1 || !bret)
    {
        debug("ECG12L ECG12LeadBandwidth error:%d", value);
        error = "ECG12L|ECG12LeadBandwidth";
        return false;
    }

    bret = config.getNumValue("ECG12L|PrintSnapshotFormat", value);
    if (value >= PRINT_12LEAD_SNAPSHOT_NR || !bret)
    {
        debug("ECG12L PrintSnapshotFormat error:%d", value);
        error = "ECG12L|PrintSnapshotFormat";
        return false;
    }

    bret = config.getNumValue("ECG12L|PDFReportFormat", value);
    if (value >= PRINT_12LEAD_PDF_NR || !bret)
    {
        debug("ECG12L PDFReportFormat error:%d", value);
        error = "ECG12L|PDFReportFormat";
        return false;
    }

    bret = config.getNumValue("ECG12L|DisplayFormat", value);
    if (value >= DISPLAY_12LEAD_NR || !bret)
    {
        debug("ECG12L DisplayFormat error:%d", value);
        error = "ECG12L|DisplayFormat";
        return false;
    }

    bret = config.getNumValue("ECG12L|TimeIntervalFor2x6Report", value);
    if (value > 2 || !bret)
    {
        debug("ECG12L TimeIntervalFor2x6Report error:%d", value);
        error = "ECG12L|TimeIntervalFor2x6Report";
        return false;
    }

    bret = config.getNumValue("ECG12L|AutoPrinting12LReport", value);
    if (value > 1 || !bret)
    {
        debug("ECG12L AutoPrinting12LReport error:%d", value);
        error = "ECG12L|AutoPrinting12LReport";
        return false;
    }

    bret = config.getNumValue("ECG12L|AutoTransmission", value);
    if (value > 1 || !bret)
    {
        debug("ECG12L AutoTransmission error:%d", value);
        error = "ECG12L|AutoTransmission";
        return false;
    }

    bret = config.getNumValue("ECG12L|TransmissionFormat", value);
    if (value > 1 || !bret)
    {
        debug("ECG12L TransmissionFormat error:%d", value);
        error = "ECG12L|TransmissionFormat";
        return false;
    }

    bret = config.getNumValue("ECG12L|RealtimePrintTopLead", value);
    if (value > ECG_LEAD_V6 || !bret)
    {
        debug("ECG12L RealtimePrintTopLead error:%d", value);
        error = "ECG12L|RealtimePrintTopLead";
        return false;
    }

    bret = config.getNumValue("ECG12L|RealtimePrintMiddleLead", value);
    if (value > ECG_LEAD_V6 || !bret)
    {
        debug("ECG12L RealtimePrintMiddleLead error:%d", value);
        error = "ECG12L|RealtimePrintMiddleLead";
        return false;
    }

    bret = config.getNumValue("ECG12L|RealtimePrintBottomLead", value);
    if (value > ECG_LEAD_V6 || !bret)
    {
        debug("ECG12L RealtimePrintBottomLead error:%d", value);
        error = "ECG12L|RealtimePrintBottomLead";
        return false;
    }

/*************************DateTime****************************/
    bret = config.getNumValue("DateTime|DateFormat", value);
    if (value >= DATE_FORMAT_NR || !bret)
    {
        debug("DateTime DateFormat error:%d", value);
        error = "DateTime|DateFormat";
        return false;
    }

    bret = config.getNumValue("DateTime|TimeFormat", value);
    if (value >= TIME_FORMAT_NR || !bret)
    {
        debug("DateTime TimeFormat error:%d", value);
        error = "DateTime|TimeFormat";
        return false;
    }

    bret = config.getNumValue("DateTime|DisplaySecond", value);
    if (value > 1 || !bret)
    {
        debug("DateTime DisplaySecond error:%d", value);
        error = "DateTime|DisplaySecond";
        return false;
    }

/*************************CodeMarker****************************/
    config.getNumAttr("Local|Language", "OptionCount", value);

    QString importStr;
    QStringList selectStr;
    for (int i = 0; i < value; ++i)
    {
        importStr.clear();
        selectStr.clear();
        bret = config.getStrValue("CodeMarker|Marker" + QString::number(i), importStr);
        if (!bret)
        {
            debug("CodeMarker Marker%d error", i);
            error = "CodeMarker|Marker" + QString::number(i);
            return false;
        }

        bret = config.getStrValue("CodeMarker|SelectMarker|Language" + QString::number(i), str);
        selectStr = str.split(',', QString::KeepEmptyParts);
        if (selectStr.size() > 20 || !bret)
        {
            debug("CodeMarker SelectMarker Language%d error", i);
            error = "CodeMarker|SelectMarker|Language" + QString::number(i);
            return false;
        }

        int size = selectStr.size();
        for (int j = 0; j < size; ++j)
        {
            if (-1 == importStr.indexOf(selectStr.at(j)))
            {
                debug("SelectMarker error:%s", qPrintable(selectStr.at(j)));
                error = "CodeMarker|SelectMarker|Language" + QString::number(i);
                return false;
            }
        }
    }

/*************************WIFI****************************/
    bret = config.getNumValue("WiFi|EnableWifi", value);
    if (value > 1 || !bret)
    {
        debug("WiFi EnableWifi error:%d", value);
        error = "WiFi|EnableWifi";
        return false;
    }

/*************************Sftp****************************/
    bret = config.getNumValue("Sftp|ServerPort", value);
    if (value <= 0 || value >= 65536 || !bret)
    {
        debug("Sftp ServerPort error:%d", value);
        error = "Sftp|ServerPort";
        return false;
    }


/*************************Mail****************************/
    config.getNumValue("Mail|ConnectionSecurity", value);
    if (value > 2)
    {
        debug("Mail ConnectionSecurity error:%d", value);
        error = "Mail|ConnectionSecurity";
        return false;
    }

    return true;
}
