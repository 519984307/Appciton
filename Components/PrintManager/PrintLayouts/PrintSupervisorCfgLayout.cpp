#include "PrintSupervisorCfgLayout.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "FontManager.h"
#include "TimeManager.h"
#include "TimeDate.h"
#include "TimeSymbol.h"
#include "ParamManager.h"
#include "PatientManager.h"
#include "ECGParam.h"
#include "NIBPSymbol.h"
#include "PrintDefine.h"
#include "AlarmSymbol.h"
#include "CO2Symbol.h"
#include "SystemManager.h"
#include "NetworkSymble.h"
#include "PrintManager.h"
#include "Version.h"

#define IGNORE_PASSWORD_STR ("******")
/**************************************************************************************************
 * 构造
 *************************************************************************************************/
PrintSupervisorCfgLayout::PrintSupervisorCfgLayout()
    : PrintLayoutIFace(PRINT_LAYOUT_ID_SUPERVISOR_CONFIG)
{

}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
PrintSupervisorCfgLayout::~PrintSupervisorCfgLayout()
{

}

/**************************************************************************************************
 * 开始布局
 *************************************************************************************************/
void PrintSupervisorCfgLayout::start()
{
    _curType = CONFIG_ITEM_START;
    _curItem = 0;
    _content.clear();
}

/**************************************************************************************************
 * 终止
 *************************************************************************************************/
void PrintSupervisorCfgLayout::abort()
{
    if (NULL != _page)
    {
        delete _page;
        _page = NULL;
    }
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintSupervisorCfgLayout::exec(PrintContent */*content*/)
{
    _page = NULL;
    if (printManager.isAbort())
    {
        return _page;
    }

    while(_curType < CONFIG_ITEM_NR)
    {
        _buildPage();
        if (NULL == _page)
        {
            _curType++;
        }
        else
        {
            break;
        }
    }

    return _page;
}

/**************************************************************************************************
 * 获取打印页的最大宽度
 *************************************************************************************************/
int PrintSupervisorCfgLayout::_getPageMaxWidth(const QFont &font)
{
    int fontHigh = fontManager.textHeightInPixels(font);
    int pageMaxWidth = IMAGE_WIDTH - 2 * fontHigh;
    int totalItem = _content.count();
    int maxWidth = 0;
    QVector <int> strLenVec;
    strLenVec.clear();
    int yoffset = fontHigh;

    if (0 == totalItem)
    {
        return 0;
    }

    for (int i = _curItem; (i < totalItem) && (yoffset < IMAGE_HEIGHT); ++i)
    {
        QString text = _content.at(i);
        int width = fontManager.textWidthInPixels(text, font);
        if (width > pageMaxWidth)
        {
            return pageMaxWidth;
        }
        else
        {
            strLenVec.append(width);
            yoffset += fontHigh;
            if (yoffset > IMAGE_HEIGHT)
            {
                break;
            }
        }
    }

    //获取所有字符串的最大长度
    for (int j = 0; j < strLenVec.size(); ++j)
    {
        if (maxWidth < strLenVec.at(j))
        {
            maxWidth = strLenVec.at(j);
        }
    }

    return maxWidth;
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintSupervisorCfgLayout::_buildPage()
{
    int _enable = 0;

    switch (_curType)
    {
        case CONFIG_ITEM_START:
            _buildStartPage();
            break;
        case CONFIG_ITEM_GENERAL:
            _buildGeneralPage();
            break;
        case CONFIG_ITEM_ECG:
            _buildECGPage();
            break;
        case CONFIG_ITEM_RESP:
            if (systemManager.isSupport(CONFIG_RESP))
            {
                _buildRESPPage();
            }
            break;
        case CONFIG_ITEM_ALARM:
            _buildAlarmPage();
            break;
        case CONFIG_ITEM_ALARM_LIMIT:
            _buildAlarmLimitPage();
            break;
        case CONFIG_ITEM_LOCAL:
            _buildLocalPage();
            break;
        case CONFIG_ITEM_NIBP:
            machineConfig.getNumValue("NIBPEnable", _enable);
            if (1 == _enable)
            {
                _buildNIBPPage();
            }
            break;
        case CONFIG_ITEM_RESP_CO2:
            machineConfig.getNumValue("CO2Enable", _enable);
            if (1 == _enable)
            {
                _buildRESPAndCo2Page();
            }
            break;
        case CONFIG_ITEM_DISPLAY:
            _buildDisplayPage();
            break;
        case CONFIG_ITEM_TRIGGER_PRINT:
            _buildTriggerPrintPage();
            break;
        case CONFIG_ITEM_12L:
            machineConfig.getNumValue("ECG12LEADEnable", _enable);
            if (1 == _enable)
            {
                _build12LPage();
            }
            break;
        case CONFIG_ITEM_NETWORK:
            machineConfig.getNumValue("WIFIEnable", _enable);
            if (1 == _enable)
            {
                _buildNetworkPage();
            }
            break;
        case CONFIG_ITEM_SFTP:
            machineConfig.getNumValue("WIFIEnable", _enable);
            if (1 == _enable)
            {
                _buildSFTPPage();
            }
            break;
        case CONFIG_ITEM_DATETIME:
            _buildDateTimePage();
            break;
        case CONFIG_ITEM_CODEMARKER:
            _buildCodeMarkerPage();
            break;
        case CONFIG_ITEM_MAIL:
            machineConfig.getNumValue("WIFIEnable", _enable);
            if (1 == _enable)
            {
                _buildMailPage();
            }
            break;
        default:
            break;
    }

    int totalItem = _content.count();
    if (_curItem >= totalItem)
    {
        return NULL;
    }

    // 设置字体、画笔
    QFont font = printFont();
    int fontHigh = fontManager.textHeightInPixels(font);
    int maxWidth = _getPageMaxWidth(font);
    if (0 == maxWidth)
    {
        return NULL;
    }

    _page = new PrintPage(maxWidth + 2 * fontHigh);
    static int num = 0;
    _page->setID(QString().sprintf("%03d", num++));
    QPainter painter(_page);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.fillRect(_page->rect(), Qt::SolidPattern); // 清空所有的数据。

    // 下面开始绘制内容。
    int yoffset = fontHigh;
    int count = 0;
    int drawMaxLen = 0;

    for (int i = _curItem; i < totalItem; ++i)
    {
        QString text = _content.at(i);
        int curWidth = fontManager.textWidthInPixels(text, font);
        if (curWidth <= maxWidth)
        {
            painter.drawText(fontHigh, yoffset, text);
            yoffset += fontHigh;
        }
        else
        {
            //如果字符串的宽度大于显示宽度，则将字符串切割为几个字符串分别显示（为了解决换行时空格字符被丢失的问题）
            QString preStr = "";
            QString lastStr = "";
            int preIndex = text.indexOf(": ");
            if (-1 != preIndex)
            {
                preStr = text.left(preIndex + 2);                     //获取“： ”前的字符串
                lastStr = text.right(text.size() - preIndex - 2);     //获取“： ”后的字符串
            }
            else
            {
                lastStr = text;
            }

            int preStrLen = fontManager.textWidthInPixels(preStr, font);
            int lastWidth = maxWidth - preStrLen;
            int rowNum = 0;

            //字符串中单个单词的最大长度
            int maxWordLen = 0;
            QStringList wordList = lastStr.split(" ");
            for (int i = 0; i < wordList.count(); ++i)
            {
                int wordLen = fontManager.textWidthInPixels(wordList.at(i), font);
                if (maxWordLen < wordLen)
                {
                    maxWordLen = wordLen;
                }
            }

            QStringList lastStrList;
            lastStrList.clear();
            int spaceWidth = fontManager.textWidthInPixels(" ", font);
            QString tempStr = "";
            int tempStrLen = 0;
            int wordLen = 0;
            //若单个单词的长度比显示宽度大，则按字符宽度逐个累加直到字符总长大于显示宽度时切割为一个字符串
            if ((maxWordLen > lastWidth) || (-1 != text.indexOf(trs("PassWord") + ":")))
            {
                int index = 0;
                char* pStr = NULL;
                QByteArray lastStrBA = lastStr.toLatin1();
                pStr = lastStrBA.data();
                char pVal = *pStr;
                while ('\0' != pVal)
                {
                    wordLen = fontManager.textWidthInPixels(QString(pVal), font);
                    if (tempStrLen + wordLen <= lastWidth)
                    {
                        tempStrLen += wordLen;
                        tempStr += pVal;
                    }
                    else
                    {
                        lastStrList.append(tempStr);
                        if(drawMaxLen < tempStrLen)
                        {
                            drawMaxLen = tempStrLen;
                        }
                        tempStr.clear();
                        tempStrLen = 0;
                        index--;
                    }
                    index++;
                    pVal = *(pStr + index);
                }
            }
            //若单个单词的长度比显示宽度小，则按单词宽度逐个累加直到字符总长大于显示宽度时切割为一个字符串
            else
            {
                for (int i = 0; i < wordList.count();)
                {
                    wordLen = fontManager.textWidthInPixels(wordList.at((i)), font);
                    if((tempStrLen + wordLen) <= lastWidth)
                    {
                        tempStrLen += wordLen;
                        tempStr += wordList.at(i);
                        i++;
                    }
                    else
                    {
                         lastStrList.append(tempStr);
                         if (drawMaxLen < tempStrLen)
                         {
                             drawMaxLen = tempStrLen;
                         }
                         tempStr.clear();
                         tempStrLen = 0;
                         continue;
                    }

                    if(i != wordList.count())
                    {
                        tempStr += " ";
                        tempStrLen += spaceWidth;
                    }
                }
            }

            if(!tempStr.isEmpty())
            {
                lastStrList.append(tempStr);
                if (drawMaxLen < tempStrLen)
                {
                    drawMaxLen = tempStrLen;
                }
                tempStr.clear();
                tempStrLen = 0;
            }

            drawMaxLen += preStrLen + fontHigh;
            rowNum = lastStrList.count();

            //若绘制后的高度要超出绘制范围的话则换页绘制
            if (IMAGE_HEIGHT < ( yoffset + rowNum * fontHigh))
            {
                break;
            }

            //绘制“： ”前的字符串
            painter.drawText(fontHigh, yoffset, preStr);
            //绘制“： ”后的字符串
            for(int i = 0; i < lastStrList.count(); i++)
            {
                painter.drawText(fontHigh + preStrLen, yoffset, lastStrList.at(i));
                yoffset += fontHigh;
            }
        }

        ++count;
        if ((yoffset + fontHigh) >= IMAGE_HEIGHT)
        {
            break;
        }
    }

    painter.end();

    _curItem += count;
    if (_curItem >= totalItem)
    {
        _content.clear();
        ++_curType;
    }

    //page too width, cut blank area
    if (0 < drawMaxLen && drawMaxLen < IMAGE_WIDTH - fontHigh)
    {
        QImage image = _page->copy(0, 0, drawMaxLen + fontHigh, IMAGE_HEIGHT);
        PrintPage *newPage = new PrintPage(image);
        newPage->setID(_page->getID());
        delete _page;
        _page = newPage;
    }

    return _page;
}

/**************************************************************************************************
 * 构建开始打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildStartPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        _content << (trs("SupervisorConfigReport"));

        tmpStr = trs("SerialNum");
        tmpStr += " ";
        machineConfig.getStrValue("SerialNumber", str);
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("SwVersion");
        tmpStr += " ";
        tmpStr += systemManager.getSoftwareVersionNum();
        _content << tmpStr;

        tmpStr = trs("BootVersion");
        tmpStr += " ";
        tmpStr += "V1.0";
        _content << tmpStr;

        tmpStr = trs("FPGAVersion");
        tmpStr += " ";
        tmpStr += "V1.0";
        _content << tmpStr;

        tmpStr = trs("PrintEventTime");
        tmpStr += " ";
        timeDate.getDateTime(str, true, true);
        tmpStr += str;
        _content << tmpStr;

//        tmpStr = trs("LastChangeTime");
//        tmpStr += " ";
//        timeDate.getDateTime(str, true, true);
//        tmpStr += str;
//        _content << tmpStr;

        tmpStr = trs("SupervisorDeviceIdentifier");
        tmpStr += ": ";
        currentConfig.getStrValue("General|DeviceIdentifier", str);
        tmpStr += str;
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建一般配置打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildGeneralPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        int value = 0;
        _content << trs("SupervisorGeneralMenu");

        tmpStr = trs("SupervisorDeviceIdentifier");
        tmpStr += ": ";
        currentConfig.getStrValue("General|DeviceIdentifier", str);
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("PassWord");
        tmpStr += ": ";
        currentConfig.getStrValue("General|Password", str);
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("SupervisorDefaultPatType");
        tmpStr += ": ";
        currentConfig.getNumValue("General|DefaultPatientType", value);
        tmpStr += trs(PatientSymbol::convert((PatientType)value));
        _content << tmpStr;

        tmpStr = trs("DefaultSystemBrightness");
        tmpStr += ": ";
        currentConfig.getNumValue("General|DefaultDisplayBrightness", value);
        tmpStr += QString::number(value + 1);
        _content << tmpStr;

        tmpStr = trs("FullDisclosureRecording");
        tmpStr += ": ";
        currentConfig.getNumValue("General|FullDisclosureRecording", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建ECG打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildECGPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;

        _content << trs("ECGMenu");

        tmpStr = trs("SupervisorPadFrequency");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG|PadsECGBandwidth", value);
        tmpStr += ECGSymbol::convert((ECGBandwidth)value);
        _content << tmpStr;

        tmpStr = trs("SupervisorLimbFrequency");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG|ChestLeadsECGBandwidth", value);
        tmpStr += ECGSymbol::convert((ECGBandwidth)value);
        _content << tmpStr;

        tmpStr = trs("ECGQRSPRToneVolume");
        if (!systemManager.isSupport(CONFIG_SPO2))
        {
            tmpStr = trs("ECGQRSToneVolume");
        }
        tmpStr += ": ";
        currentConfig.getNumValue("ECG|QRSVolume", value);
        if (0 == value)
        {
            tmpStr += trs("Off");
        }
        else
        {
            tmpStr += QString::number(value);
        }
        _content << tmpStr;

        tmpStr = trs("SupervisorLeadNameConvention");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG|ECGLeadConvention", value);
        tmpStr += ECGSymbol::convert((ECGLeadNameConvention)value);
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建RESP打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildRESPPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;

        _content << trs("RESPMenu");

        tmpStr = trs("RESPAutoActivation");
        tmpStr += ": ";
        currentConfig.getNumValue("RESP|AutoActivation", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建NIBP打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildNIBPPage()
{
    if (_content.isEmpty())
    {
        int unit = UNIT_MMHG;
        currentConfig.getNumValue("Local|NIBPUnit", unit);

        QString tmpStr;
        int value = 0;
        _content << trs("NIBPMenu");

        tmpStr = trs("SupervisorNIBPMeasureMode");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|MeasureMode", value);
        tmpStr += trs(NIBPSymbol::convert((NIBPMode)value));
        _content << tmpStr;

        tmpStr = trs("SupervisorNIBPAutoInterval");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|AutoInterval", value);
        tmpStr += trs(NIBPSymbol::convert((NIBPAutoInterval)value));
        _content << tmpStr;

        tmpStr = trs("SupervisorSTATFunction");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|StatFunction", value);
        tmpStr += value ? trs("On") : trs("Off");
        _content << tmpStr;

        tmpStr = trs("SupervisorStartOnMeasureFail");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|AutomaticRetry", value);
        tmpStr += trs(NIBPSymbol::convert((NIBPPRDisplay)value));
        _content << tmpStr;

        tmpStr = trs("SupervisorAdultInitialCuff");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|AdultInitialCuffInflation", value);
        QString str = NIBPSymbol::convert((NIBPAdultInitialCuff)value);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("SupervisorPrediatricInitialCuff");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|PedInitialCuffInflation", value);
        str = NIBPSymbol::convert((NIBPPrediatrictInitialCuff)value);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("SupervisorNeonatalInitialCuff");
        tmpStr += ": ";
        currentConfig.getNumValue("NIBP|NeoInitialCuffInflation", value);
        str = NIBPSymbol::convert((NIBPNeonatalInitialCuff)value);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        tmpStr += str;
//        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建RESP/CO2打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildRESPAndCo2Page()
{
    if (_content.isEmpty())
    {
        int value = 0;
        QString tmpStr;
        QString str;

        _content << trs("CO2Menu");

        tmpStr = trs("CO2Switch");
        tmpStr += ": ";
        currentConfig.getNumValue("CO2|CO2ModeDefault", value);
        tmpStr += value ? trs("On") : trs("Off");
        tmpStr += str;
        _content << tmpStr;

        tmpStr = trs("CO2SweepMode");
        tmpStr += ": ";
        currentConfig.getNumValue("CO2|CO2SweepMode", value);
        tmpStr += trs(CO2Symbol::convert((CO2SweepMode)value));
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建报警限打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildAlarmLimitPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        _content << trs("AlarmLimitMenu");

        QString patStr;
        int type = 0;
        currentConfig.getNumValue("General|DefaultPatientType", type);
        patStr = PatientSymbol::convert((PatientType)type);

        for (int i = 0; i < SUB_PARAM_NR; ++i)
        {
            int isEnable = 0;
            ParamID id = paramInfo.getParamID((SubParamID)i);
            QString paramName = paramInfo.getParamName(id);
            switch (id)
            {
                case PARAM_SPO2:
                case PARAM_NIBP:
                case PARAM_CO2:
                case PARAM_TEMP:
                    machineConfig.getNumValue(paramName + "Enable", isEnable);
                    break;
                case PARAM_DUP_RESP:
                    isEnable = (systemManager.isSupport(CONFIG_CO2) ||
                                systemManager.isSupport(CONFIG_RESP));
                    break;
                default:
                    isEnable = 1;
                    break;
            }

            if (!isEnable)
            {
                continue;
            }

            int type = paramInfo.getUnitOfSubParam((SubParamID)i);
            switch (i)
            {
                case SUB_PARAM_ETCO2:
                case SUB_PARAM_FICO2:
                    currentConfig.getNumValue("Local|CO2Unit", type);
                    break;
                case SUB_PARAM_NIBP_DIA:
                case SUB_PARAM_NIBP_MAP:
                case SUB_PARAM_NIBP_SYS:
                    currentConfig.getNumValue("Local|NIBPUnit", type);
                    break;
                case SUB_PARAM_T1:
                case SUB_PARAM_T2:
                case SUB_PARAM_TD:
                    currentConfig.getNumValue("Local|TEMPUnit", type);
                    break;
                default:
                    break;
            }

            tmpStr = trs(paramInfo.getSubParamName((SubParamID)i));
            tmpStr += " (";
            tmpStr += Unit::localeSymbol((UnitType)type);
            tmpStr += ")";
            tmpStr += ": ";

            QString prefix = "AlarmSource|" + patStr + "|";
            prefix += paramInfo.getSubParamName((SubParamID)i, true);
            tmpStr += trs("AlarmStatus");
            tmpStr += " = ";
            currentConfig.getNumAttr(prefix, "Enable", value);
            tmpStr += value ? trs("On") : trs("Off");
            tmpStr += ", ";

            prefix += "|";
            prefix += Unit::getSymbol((UnitType)type);

            int scale = 1;
            int val;
            currentConfig.getNumValue(prefix + "|Scale", scale);

            currentConfig.getNumValue(prefix + "|High", val);
            tmpStr += trs("High");
            tmpStr += " = ";
            if(scale == 1)
            {
                tmpStr += QString::number(val);
            }
            else
            {
                //precision is ok to be 1, since the scale is always 10.
                tmpStr += QString::number((double)val/scale, 'f', 1);
            }
            tmpStr += ", ";

            currentConfig.getNumValue(prefix + "|Low", val);
            tmpStr += trs("Low");
            tmpStr += " = ";
            if(scale == 1)
            {
                tmpStr += QString::number(val);
            }
            else
            {
                tmpStr += QString::number((double)val/scale, 'f', 1);
            }

            _content << tmpStr;
        }

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建本地化打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildLocalPage()
{
    int _enable = 0;

    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        int value = 0;
        _content << trs("SupervisorLocalMenu");

        tmpStr = trs("SupervisorLanguage");
        tmpStr += ": ";
        currentConfig.getNumAttr("Local|Language", "CurrentOption", value);
        currentConfig.getStrValue("Local|Language|Opt" + QString::number(value), str);
        tmpStr += str;
        _content << tmpStr;

        machineConfig.getNumValue("NIBPEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("SupervisorNIBPUnit");
            tmpStr += ": ";
            currentConfig.getNumValue("Local|NIBPUnit", value);
            tmpStr += Unit::localeSymbol((UnitType)value);
            _content << tmpStr;
        }

        machineConfig.getNumValue("CO2Enable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("SupervisorCO2Unit");
            tmpStr += ": ";
            currentConfig.getNumValue("Local|CO2Unit", value);
            tmpStr += Unit::localeSymbol((UnitType)value);
            _content << tmpStr;
        }

        machineConfig.getNumValue("TEMPEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("SupervisorTEMPUnit");
            tmpStr += ": ";
            currentConfig.getNumValue("Local|TEMPUnit", value);
            tmpStr += Unit::localeSymbol((UnitType)value);
            _content << tmpStr;
        }

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建显示打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildDisplayPage()
{
    int _enable = 0;

    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        _content << trs("SupervisorDisplayMenu");

        tmpStr = trs("ECG");
        tmpStr += ": ";
        currentConfig.getStrValue("Display|ECGColor", str);
        tmpStr += trs(str);
        _content << tmpStr;

        machineConfig.getNumValue("SPO2Enable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("SPO2");
            tmpStr += ": ";
            currentConfig.getStrValue("Display|SPO2Color", str);
            tmpStr += trs(str);
            _content << tmpStr;
        }

        machineConfig.getNumValue("NIBPEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("NIBP");
            tmpStr += ": ";
            currentConfig.getStrValue("Display|NIBPColor", str);
            tmpStr += trs(str);
            _content << tmpStr;
        }

        machineConfig.getNumValue("CO2Enable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("CO2");
            tmpStr += ": ";
            currentConfig.getStrValue("Display|CO2Color", str);
            tmpStr += trs(str);
            _content << tmpStr;
        }

        if (systemManager.isSupport(CONFIG_RESP))
        {
            tmpStr = trs("RESP");
            tmpStr += ": ";
            currentConfig.getStrValue("Display|RESPColor", str);
            tmpStr += trs(str);
            _content << tmpStr;
        }

        machineConfig.getNumValue("TEMPEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("TEMP");
            tmpStr += ": ";
            currentConfig.getStrValue("Display|TEMPColor", str);
            tmpStr += trs(str);
            _content << tmpStr;
        }

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建报警设置打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildAlarmPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        _content << (trs("AlarmSettingMenu"));

//        tmpStr = trs("LTAAlarm");
//        tmpStr += ": ";
//        currentConfig.getNumValue("Alarm|LTAAlarm", value);
//        tmpStr += value ? trs("On") : trs("Off");
//        _content << tmpStr;

        tmpStr = trs("MinimumALarmVolume");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|MinimumAlarmVolume", value);
        tmpStr += QString::number(value + 1);
        _content << tmpStr;

        tmpStr = trs("DefaultAlarmVolume");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|DefaultAlarmVolume", value);
        tmpStr += QString::number(value);
        _content << tmpStr;

        tmpStr = trs("AlarmPauseTime");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|AlarmPauseTime", value);
        tmpStr += trs(AlarmSymbol::convert((AlarmPauseTime)value));
        _content << tmpStr;

        tmpStr = trs("AlarmOffPromptMechanism");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|AlarmOffPrompting", value);
        tmpStr += trs(AlarmSymbol::convert((AlarmClosePromptTime)value));
        _content << tmpStr;

        tmpStr = trs("EnableAlarmAudioOff");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|EnableAlarmAudioOff", value);
        tmpStr += value ? trs("Yes") : trs("No");
        _content << tmpStr;

        tmpStr = trs("EnableAlarmOff");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|EnableAlarmOff", value);
        tmpStr += value ? trs("Yes") : trs("No");
        _content << tmpStr;

        tmpStr = trs("AlarmOffAtPowerOn");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|AlarmOffAtPowerOn", value);
        tmpStr += value ? trs("Yes") : trs("No");
        _content << tmpStr;

        tmpStr = trs("NonAlertsBeepsInNonAED");
        tmpStr += ": ";
        currentConfig.getNumValue("Alarm|NonAlertsBeepsInNonAED", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建summary trigger打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildTriggerPrintPage()
{
    int _enable = 0;

    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        _content << trs("SupervisorPrintMenu");

        tmpStr = trs("ECGRhythm");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|PresentingECG", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("ECGForeAnalysis");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|ECGAnalysis", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("ShockDelivery");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|ShockDelivery", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("CheckPatient");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|CheckPatient", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        machineConfig.getNumValue("PACEEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("PacerStartup");
            tmpStr += ": ";
            currentConfig.getNumValue("Print|PacerStartUp", value);
            tmpStr += value ? trs("Enable") : trs("Disable");
            _content << tmpStr;
        }

        tmpStr = trs("PhyAlarm");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|PhysiologicalAlarm", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("CodeMarker");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|CoderMarker", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        machineConfig.getNumValue("NIBPEnable", _enable);
        if (1 == _enable)
        {
            tmpStr = trs("NIBPReading");
            tmpStr += ": ";
            currentConfig.getNumValue("Print|NIBPReading", value);
            tmpStr += value ? trs("Enable") : trs("Disable");
            _content << tmpStr;
        }

        tmpStr = trs("DiagnosticECG");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|DiagnosticECG", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("SupervisorAutoPrintInAED");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|SummaryReportSnapshotPrintingInAED", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("SupervisorAutoPrint30Jtest");
        tmpStr += ": ";
        currentConfig.getNumValue("Print|Print30JSelfTestReport", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建12L打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_build12LPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        _content << trs("Supervisor12LMenu");

        tmpStr = trs("SupervisorNotchFilter");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|NotchFilter", value);
        tmpStr += ECGSymbol::convert((ECGNotchFilter)value);
        _content << tmpStr;

        tmpStr = trs("Supervisor12lFrequency");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|ECG12LeadBandwidth", value);
        tmpStr += ECGSymbol::convert((ECGBandwidth)(value + ECG_BANDWIDTH_0525_40HZ));
        _content << tmpStr;

        tmpStr = trs("Print12LSnapshotFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|PrintSnapshotFormat", value);
        tmpStr += PrintSymbol::convert((Print12LeadSnapshotFormat)value);
        _content << tmpStr;

        tmpStr = trs("PDFReportFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|PDFReportFormat", value);
        tmpStr += PrintSymbol::convert((Print12LeadPDFFormat)value);
        _content << tmpStr;

        int displayMode = 0;
        tmpStr = trs("ECG12LDisplayFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|DisplayFormat", displayMode);
        tmpStr += trs(ECGSymbol::convert((Display12LeadFormat)displayMode));
        _content << tmpStr;

        tmpStr = trs("SupervisorTimeWindow2x6PDF");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", value);
        switch (value)
        {
            case 0:
                tmpStr += "0-5s";
                break;
            case 1:
                tmpStr += "2.5-7.5s";
                break;
            case 2:
                tmpStr += "5-10s";
                break;
            default:
                break;
        }

        _content << tmpStr;

        tmpStr = trs("SupervisorAutoPrint12L");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|AutoPrinting12LReport", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("SupervisorAutoTransforByWifi");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|AutoTransmission", value);
        tmpStr += value ? trs("Enable") : trs("Disable");
        _content << tmpStr;

        tmpStr = trs("SupervisorTransforFileFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|TransmissionFormat", value);
        tmpStr += value ? trs("JSON") : trs("PDF");
        _content << tmpStr;

        int leadNameConvention = 0;
        currentConfig.getNumValue("ECG|ECGLeadConvention", leadNameConvention);

        tmpStr = trs("RealTimePrintTopLead");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|RealtimePrintTopLead", value);
        tmpStr += ECGSymbol::convert((ECGLead)value,(ECGLeadNameConvention)leadNameConvention,true);
        _content << tmpStr;

        tmpStr = trs("RealTimePrintMidLead");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|RealtimePrintMiddleLead", value);
        tmpStr += ECGSymbol::convert((ECGLead)value,(ECGLeadNameConvention)leadNameConvention,true);
        _content << tmpStr;

        tmpStr = trs("RealTimePrintBottomLead");
        tmpStr += ": ";
        currentConfig.getNumValue("ECG12L|RealtimePrintBottomLead", value);
        tmpStr += ECGSymbol::convert((ECGLead)value,(ECGLeadNameConvention)leadNameConvention,true);
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建网络打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildNetworkPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        QString str;
        _content << (trs("WiFiMenu"));

        tmpStr = trs("Switch");
        tmpStr += ": ";
        currentConfig.getNumValue("WiFi|EnableWifi", value);
        tmpStr += value ? trs("On") : trs("Off");
        _content << tmpStr;

        int count = 0;
        currentConfig.getNumAttr("WiFi|Profiles", "Count", count);
        for (int i = 0; i < count; ++i)
        {
            _content << ("Profile" + QString::number(i));
            QString strPath = "WiFi|Profiles|Profile" + QString::number(i);
            strPath += "|";

            tmpStr = trs("ProfileName");
            tmpStr += ": ";
            currentConfig.getStrValue(strPath + "ProfileName", str);
            _content << (tmpStr + str);

            tmpStr = trs("WiFiAP");
            tmpStr += ": ";
            currentConfig.getStrValue(strPath + "SSID", str);
            _content << (tmpStr + str);

            tmpStr = trs("EncryptType");
            tmpStr += ": ";
            currentConfig.getNumValue(strPath + "AuthType", value);
            tmpStr += NetworkSymble::convert((EncryptType)value);
            _content << tmpStr;

            tmpStr = trs("NetworkSetting");
            tmpStr += ": ";
            currentConfig.getNumValue(strPath + "IsStatic", value);
            tmpStr += trs(NetworkSymble::convert((IpMode)value));
            _content << tmpStr;

            if (IPMODE_STATIC == value)
            {
                tmpStr = trs("StaticIp");
                tmpStr += ": ";
                currentConfig.getStrValue(strPath + "StaticIP", str);
                _content << (tmpStr + str);

                tmpStr = trs("GateWay");
                tmpStr += ": ";
                currentConfig.getStrValue(strPath + "DefaultGateway", str);
                _content << (tmpStr + str);

                tmpStr = trs("SubnetMask");
                tmpStr += ": ";
                currentConfig.getStrValue(strPath + "SubnetMask", str);
                _content << (tmpStr + str);

                tmpStr = trs("PreferrendDNS");
                tmpStr += ": ";
                currentConfig.getStrValue(strPath + "PreferedDNS", str);
                _content << (tmpStr + str);

                tmpStr = trs("AlternateDNS");
                tmpStr += ": ";
                currentConfig.getStrValue(strPath + "AlternateDNS", str);
                _content << (tmpStr + str);
            }
        }

        _curItem = 0;
    }
}

void PrintSupervisorCfgLayout::_buildSFTPPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        _content << trs("Sftp");

        tmpStr = trs("SupervisorServerIP");
        tmpStr += ": ";
        currentConfig.getStrValue("Sftp|ServerIP",str);
        tmpStr += str;
        _content << tmpStr;
        str.clear();

        tmpStr = trs("SupervisorServerPort");
        tmpStr += ": ";
        currentConfig.getStrValue("Sftp|ServerPort", str);
        tmpStr += str;
        _content << tmpStr;
        str.clear();

        tmpStr = trs("Username");
        tmpStr += ": ";
        currentConfig.getStrValue("Sftp|Username", str);
        tmpStr += str;
        _content << tmpStr;
        str.clear();

        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建时间打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildDateTimePage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        int value = 0;
        _content << trs("SupervisorTimeAndDataMenu");

//        tmpStr = trs("SupervisorYear");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getDateYear());
//        _content << tmpStr;

//        tmpStr = trs("SupervisorMonth");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getDateMonth());
//        _content << tmpStr;

//        tmpStr = trs("SupervisorDay");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getDateDay());
//        _content << tmpStr;

//        tmpStr = trs("SupervisorHour");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getTimeHour());
//        _content << tmpStr;

//        tmpStr = trs("SupervisorMinute");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getTimeMinute());
//        _content << tmpStr;

//        tmpStr = trs("SupervisorSecond");
//        tmpStr += ": ";
//        tmpStr += QString::number(timeDate.getTimeSenonds());
//        _content << tmpStr;

        tmpStr = trs("SupervisorDateFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("DateTime|DateFormat", value);
        tmpStr += trs(TimeSymbol::convert((DateFormat)value));
        _content << tmpStr;

        tmpStr = trs("SupervisorTimeFormat");
        tmpStr += ": ";
        currentConfig.getNumValue("DateTime|TimeFormat", value);
        tmpStr += trs(TimeSymbol::convert((TimeFormat)value));
        _content << tmpStr;

        tmpStr = trs("SupervisorDisplaySec");
        tmpStr += ": ";
        currentConfig.getNumValue("DateTime|DisplaySecond", value);
        tmpStr += value ? trs("Yes") : trs("No");
        _content << tmpStr;

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建Codemarker打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildCodeMarkerPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        QStringList codeMarkerList;
        _content << trs("SupervisorCodeMarker");

        int num = 0;
        currentConfig.getNumAttr("Local|Language", "CurrentOption", num);
        QString markerStr = "CodeMarker|SelectMarker|Language";
        markerStr += QString::number(num, 10);
        currentConfig.getStrValue(markerStr, tmpStr);
        codeMarkerList = tmpStr.split(",");
        tmpStr.clear();

        int count = codeMarkerList.count();
        for (int i = 0; i < count; ++i)
        {
            tmpStr += trs(codeMarkerList.at(i));
            tmpStr += "  ";
            if (0 == (i + 1) % 3)
            {
                _content << tmpStr;
                tmpStr.clear();
            }
        }

        if (!tmpStr.isEmpty())
        {
            _content << trs(tmpStr);
        }

        _curItem = 0;
    }
}

/**************************************************************************************************
 * 构建Mail打印页
 *************************************************************************************************/
void PrintSupervisorCfgLayout::_buildMailPage()
{
    if (_content.isEmpty())
    {
        QString tmpStr;
        QString str;
        int value = 0;
        _content << trs("Mail");

        tmpStr = trs("SmtpServer");
        tmpStr += ": ";
        currentConfig.getStrValue("Mail|SmtpServer", str);
        _content << (tmpStr + str);

        tmpStr = trs("SmtpServerPort");
        tmpStr += ": ";
        currentConfig.getStrValue("Mail|SmtpServerPort", str);
        _content << (tmpStr + str);

        tmpStr = trs("Username");
        tmpStr += ": ";
        currentConfig.getStrValue("Mail|SmtpUsername", str);
        _content << (tmpStr + str);

        tmpStr = trs("ConnectionSecurity");
        tmpStr += ": ";
        currentConfig.getNumValue("Mail|ConnectionSecurity", value);
        tmpStr += NetworkSymble::convert((MailConnectSecurity)value);
        _content << tmpStr;

        int count = 0;
        currentConfig.getNumAttr("Mail|Recipients", "Count", count);
        if (count > 0)
        {
            tmpStr = trs("ConfiguredMailRecipients");
            tmpStr += ": ";
            _content << tmpStr;

            tmpStr = "(";
            tmpStr += trs("Name");
            tmpStr += "<";
            tmpStr += trs("EmailAddress");
            tmpStr += ">";
            tmpStr += ")";
            _content << tmpStr;
        }

        for (int i = 0; i < count; ++i)
        {
            QString path = "Mail|Recipients|Recipient" + QString::number(i);
            path += "|";

            currentConfig.getStrValue(path + "Name", tmpStr);
            tmpStr += "<";
            currentConfig.getStrValue(path + "Address", str);
            tmpStr += str;
            tmpStr += ">";
            _content << tmpStr;
        }

        _curItem = 0;
    }
}

