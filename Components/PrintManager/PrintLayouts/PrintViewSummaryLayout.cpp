#include "PrintViewSummaryLayout.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "FontManager.h"
#include "TimeManager.h"
#include "TimeDate.h"
#include "TimeSymbol.h"
#include "DataStorageDirManager.h"
#include "SummaryRescueDataWidget.h"
#include "SummaryRescueRangePrint.h"
#include "StorageFile.h"
#include "PrintManager.h"
#include "ECGDefine.h"
#include "WaveformCache.h"
#include <QFontMetrics>
#include "ParamInfo.h"
#include "SummaryStorageManager.h"
#include <QDateTime>

/**************************************************************************************************
 * 前台分析回调
 *************************************************************************************************/
void _foregroundCallback(QPainter &painter, const PrintWaveInfo &info)
{
    if (NULL == info.data)
    {
        return;
    }

    if (info.id > WAVE_ECG_V6)
    {
        return;
    }

    SummaryForegroundECGAnalysis *snapshot = (SummaryForegroundECGAnalysis*)(info.data);

    if (NULL == snapshot
            || snapshot->itemDesc.type != SUMMARY_ECG_FOREGROUND_ANALYSIS
            || snapshot->analysisResult == FG_ANALYSIS_HALTED)
    {
        return;
    }

    int rate = waveformCache.getSampleRate(info.id);
    int timeLen = info.waveDataNum / rate; //must be 1s
    int minTime = timeLen * (info.pageNum - 1) * 1000;
    int maxTime = timeLen * info.pageNum * 1000;
    int fontH = fontManager.textHeightInPixels(painter.font());
    int x = 0;
    int y = 2 * fontH;

    int start = 0;
    int end = 0;
    int mid = 0;
    QString text;
    for (int i = 0; i< snapshot->segmentNum; i++)
    {
        start = snapshot->segmentStartTime[i] - snapshot->startTime + 6000; //start form the first 5 seconds
        end = start + 3000; //3 seconds segment
        mid = start + 1500;

        //分析开始
        if (start >= minTime && start <= maxTime)
        {
            x = (start - minTime) * info.pageWidth / (timeLen * 1000);
            painter.drawLine(QPointF(x, y), QPointF(x, y + fontH));
        }

        //分析结束
        if (end >= minTime && end <= maxTime)
        {
            x = (end - minTime) * info.pageWidth / (timeLen * 1000);
            painter.drawLine(QPointF(x, y), QPointF(x, y + fontH));
        }

        //分析结果
        if (mid >= minTime && mid <= maxTime)
        {

            if (snapshot->segmentShockable[i])
            {
                text = "*";
            }
            else
            {
                text = "-";
            }

            x = (mid - minTime) * info.pageWidth / (timeLen * 1000);
            int width = fontManager.textWidthInPixels(text);
            if (x + width > info.pageWidth)
            {
                x = info.pageWidth - width;
            }
            painter.drawText(x, y + fontH, text);
        }
    }
}

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
PrintViewSummaryLayout::PrintViewSummaryLayout() :
    PrintLayoutIFace(PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT),
    PrintSummaryLayout()
{
    _file = NULL;
    _data = NULL;
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
PrintViewSummaryLayout::~PrintViewSummaryLayout()
{
    _filePath.clear();
    _waveInfoList.clear();

    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }

    if (NULL != _data)
    {
        delete[] _data;
        _data = NULL;
    }
}

/**************************************************************************************************
 * 开始布局
 *************************************************************************************************/
void PrintViewSummaryLayout::start()
{
    init();
    _type = summaryRescueRangePrint.getPrintType();

    _filePath.clear();
    summaryRescueRangePrint.getPrintPath(_filePath);
    _checkFlags.clear();
    _checkFlags = summaryRescueDataWidget.getCheckFlags();


    _index = -1;
    _totalData = 0;
    _fileNum = 0;
    _pageTimeLen = 0;
    _pageWidth = 0;

    _waveInfoList.clear();
    _oneSnapShockOK = true;
    _drawWaveCompleteInfoFlag = false;
    _dataLen = 0;
    _timeNum = 0;

    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
}

/**************************************************************************************************
 * 终止
 *************************************************************************************************/
void PrintViewSummaryLayout::abort()
{
    _filePath.clear();
    _waveInfoList.clear();

    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }

    if (NULL != _data)
    {
        delete[] _data;
        _data = NULL;
    }
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintViewSummaryLayout::exec(PrintContent */*content*/)
{
    switch (_type)
    {
        case PRINT_SELECTED_SNAPSHOT:
            return _drawSelectPage();
        case PRINT_SELECTED_HEAD_AND_SNAPSHOTS:
        case PRINT_RESCUE_HEAD_AND_SNAPSHOTS:
        case PRINT_ALL_RESCUE:
            return _drawSelectRescuePage();
        case PRINT_ALL_RESCUE_HEADS:
            return _drawRescueHeadPage();
        case PRINT_RESCUE_LOG:
        case PRINT_SELECTED_LOG:
            return _drawRescueLogPage();
        default:
            return NULL;
    }
}

/**************************************************************************************************
 * 构建选择打印页
 *************************************************************************************************/
PrintPage *PrintViewSummaryLayout::_drawSelectPage()
{
    while(true)
    {
        if (_drawWaveCompleteInfoFlag)
        {
            _drawWaveCompleteInfoFlag = false;
            return _drawWaveCompleteInfo();
        }

        if (!_dataInit())
        {
            return NULL;
        }

        while(_totalData > _checkFlags.size())
        {
            //the total item num might increase after the moment start print select snapshots
            _checkFlags.append(false);
        }

        if (_oneSnapShockOK)
        {
            if (NULL == _file)
            {
                return NULL;
            }

            if (_index + 1 == _totalData)
            {
                SummaryHead head;
                _file->readAdditionalData((char*)&head, sizeof(SummaryHead));
                PrintPage *startpage = buildStartPage((unsigned char*)&head,
                                                      sizeof(SummaryHead), printFont());
                if (NULL != startpage)
                {
                    return startpage;
                }
            }

            if (NULL != _data)
            {
                delete[] _data;
                _data = NULL;
            }

            while (NULL == _data)
            {
                int index = _totalData - _index - 1;
                if (!_checkFlags[index])
                {
                    --_index;

                    if (_index < 0)
                    {
                        break;
                    }
                    continue;
                }

                _dataLen = _file->getBlockDataLen(index);
                _data = new unsigned char[_dataLen];
                _file->readBlockData(index, (char*)_data, _dataLen);
                _oneSnapShockOK = false;
                debug("read index = %d", index);
            }
        }

        if (NULL == _data)
        {
            return NULL;
        }

        PrintPage *page = buildPage(_data, _dataLen, printFont());
        if (NULL != page)
        {
            //if the last snapshot is codemarker or nibp, draw finish infomation
            int itemType = ((SummaryItemDesc*)_data)->type;
            if(_noSelectSnapshotAfterIndex(_totalData - _index)
                    && (itemType == SUMMARY_CODE_MAKER || itemType == SUMMARY_NIBP)
                    && isContentStrPrintFinished())
            {
                _drawCompleteInfo(page, page->getDrawHeight() + fontManager.textHeightInPixels(printFont()) / 2);
            }

            return page;
        }

        page = _drawOneSnapShock(_data, _dataLen);
        if (_oneSnapShockOK)
        {
            delete[] _data;
            _data = NULL;

            bool needToContinue = false;
            if(page == NULL && _timeNum == 0 && _waveInfoList.isEmpty())
            {
                //after print codemarker, we will get here
                needToContinue = true;
            }

            --_index;
            _waveInfoList.clear();
            _timeNum = 0;
            init();

            if(needToContinue)
            {
                continue;
            }

            if (_noSelectSnapshotAfterIndex(_totalData - _index - 1))
            {
                _drawWaveCompleteInfoFlag = true;
            }
        }

        return page;
    }
}

/**************************************************************************************************
 * 构建选择营救打印页
 *************************************************************************************************/
PrintPage *PrintViewSummaryLayout::_drawSelectRescuePage()
{
    while(true)
    {
        if (_drawWaveCompleteInfoFlag)
        {
            _drawWaveCompleteInfoFlag = false;
            return _drawWaveCompleteInfo();
        }

        if (!_dataInit(true))
        {
            return NULL;
        }

        if (_oneSnapShockOK)
        {
            if (_index + 1 == _totalData)
            {
                SummaryHead head;
                _file->readAdditionalData((char*)&head, sizeof(SummaryHead));
                PrintPage *startpage = buildStartPage((unsigned char*)&head,
                                                      sizeof(SummaryHead), printFont());
                if (NULL != startpage)
                {
                    //no snapshots in this incident
                    if (_index < 0 && isStartPageOver())
                    {
                        _drawCompleteInfo(startpage, startpage->getDrawHeight() + fontManager.textHeightInPixels(printFont()));
                    }
                    return startpage;
                }
            }

            _dataLen = _file->getBlockDataLen(_totalData - _index - 1);
            _data = new unsigned char[_dataLen];
            _file->readBlockData(_totalData - _index - 1, (char*)_data, _dataLen);
            _oneSnapShockOK = false;
        }

        if (NULL == _data)
        {
            return NULL;
        }

        PrintPage *page = buildPage(_data, _dataLen, printFont());
        if (NULL != page)
        {
            //if the last snapshot is codemarker, draw finish infomation
            int itemType = ((SummaryItemDesc*)_data)->type;
            if(_index == 0 && (itemType == SUMMARY_CODE_MAKER || itemType == SUMMARY_NIBP)
                    && isContentStrPrintFinished())
            {
                _drawCompleteInfo(page, page->getDrawHeight() + fontManager.textHeightInPixels(printFont()));
            }
            return page;
        }

        page = _drawOneSnapShock(_data, _dataLen);
        if (_oneSnapShockOK)
        {
            if (NULL != _data)
            {
                delete[] _data;
                _data = NULL;
            }

            bool needToContinue = false;

            if(page == NULL && _timeNum == 0 && _waveInfoList.isEmpty())
            {
                needToContinue = true;
            }

            _timeNum = 0;
            --_index;
            _waveInfoList.clear();
            init();

            if(needToContinue)
            {
                //after print codemarker, we get here
                continue;
            }

            if ( _index < 0)
            {
                _drawWaveCompleteInfoFlag = true;
            }
        }


        return page;
    }
}

/**************************************************************************************************
 * 构建log打印页
 *************************************************************************************************/
PrintPage *PrintViewSummaryLayout::_drawRescueLogPage()
{
    if (!_dataInit(true))
    {
        return NULL;
    }

    int fontH = fontManager.textHeightInPixels(printFont());
    int gap = fontH + fontH / 3;
    int tmpH = fontH;
    int width;

    QFontMetrics fm(printFont());
    QStringList strlist;

    QString startTime;
    SummaryHead head;
    _file->readAdditionalData((char *)&head, sizeof(SummaryHead));

    strlist<<QString("%1 %2").arg(trs("RescueStartTime") + ":").arg(startTime);
    width = fm.width(strlist.at(0));
    if (_index != _totalData - 1)
    {
        //not the first page, don't add the rescure start time
        strlist.takeFirst();
    }
    else
    {
        tmpH += gap;
    }


    while (_index >= 0)
    {
        if (NULL == _file)
        {
            return NULL;
        }

        SummaryItemDesc desc;
        _file->readBlockData(_totalData - _index - 1, (char*)&desc, sizeof(SummaryItemDesc));
        QString text;
        int curWidth;
        timeDate.getDateTime(desc.time, text, true, true);
        text += "  ";
        if (desc.type == SUMMARY_PHY_ALARM)
        {
            SummaryPhyAlarm phyAlm;
            _file->readBlockData(_totalData - _index - 1, (char *)&phyAlm, sizeof(SummaryPhyAlarm));
             QString almtext = trs(summaryStorageManager.getPhyAlarmMessage((ParamID)phyAlm.curAlarmInfo.paramid,
                                                       phyAlm.curAlarmInfo.alarmType,
                                                       phyAlm.curAlarmInfo.isOneshot));
            text += almtext;
        }
        else if (desc.type == SUMMARY_CODE_MAKER)
        {
            SummaryCodeMarker codeMarker;
            _file->readBlockData(_totalData - _index - 1, (char*) &codeMarker, sizeof(SummaryCodeMarker));
            text += trs(codeMarker.selectCodeName);
        }
        else if (desc.type == SUMMARY_NIBP)
        {
            SummaryNIBP nibpSnapshot;
            _file->readBlockData(_totalData - _index - 1, (char *) &nibpSnapshot, sizeof(SummaryNIBP));
#if 0 //choose to print actual measure value or not
            QString nibptext = QString("%2/%3 (%4)");
            if(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value == InvData())
            {
                nibptext = nibptext.arg(InvStr()).arg(InvStr()).arg(InvStr());
            }
            else
            {
                UnitType    unit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
                UnitType curUnit = nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].type;
                int mul = nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].mul;
                if (unit == curUnit)
                {
                    nibptext = nibptext.arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value))
                            .arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_DIA].value))
                            .arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_MAP].value));
                }
                else
                {
                    nibptext = nibptext.arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value * 1.0 / mul))
                            .arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_DIA].value * 1.0 / mul))
                            .arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_MAP].value * 1.0 / mul));
                }

            }
            nibptext +=  " " + Unit::localeSymbol(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].type);
            text += nibptext;
#else
            QString nibptext = trs(getSummaryName(SUMMARY_NIBP));
            text += nibptext;
#endif
        }
        else
        {
            text += trs(getSummaryName((SummaryType)desc.type));
        }

        curWidth = fm.width(text);
        if (width < curWidth)
        {
            width = curWidth;
        }
        strlist << text;

        --_index;

        tmpH += gap;
        if (tmpH + fontH > IMAGE_HEIGHT)
        {
            break;
        }
    }

    PrintPage *page = new PrintPage(width + 2 * fontH);
    static int id = 0;
    page->setID("summaryLog" + QString::number(id++));
    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(page->rect(), Qt::SolidPattern);

    int count = strlist.count();
    tmpH = fontH;
    for (int i = 0; i < count; ++i)
    {
        painter.drawText(fontH, tmpH, strlist.at(i));
        tmpH += gap;
    }

    strlist.clear();

    painter.end();

    if(_index < 0)
    {
        _drawCompleteInfo(page, tmpH - fontH);
    }

    return page;
}

/**************************************************************************************************
 * 构建summary head打印页
 *************************************************************************************************/
PrintPage *PrintViewSummaryLayout::_drawRescueHeadPage()
{
    if (!_dataInit(true))
    {
        return NULL;
    }

    SummaryHead head;
    _file->readAdditionalData((char*)&head, sizeof(SummaryHead));
    PrintPage *page = buildStartPage((unsigned char*)&head, sizeof(SummaryHead), printFont());
    if (isStartPageOver())
    {
        _oneSnapShockOK = true;
        init();
        _index = -1;
    }

    _drawCompleteInfo(page, page->getDrawHeight() + fontManager.textHeightInPixels(printFont()));

    return page;
}

bool PrintViewSummaryLayout::_noSelectSnapshotAfterIndex(int index)
{
    while (index < _checkFlags.size()) {
        if(_checkFlags[index])
        {
            return false;
        }
        index ++;
    }
    return true;
}

/**************************************************************************************************
 * 数据初始化
 *************************************************************************************************/
bool PrintViewSummaryLayout::_dataInit(bool drawHeader)
{
    if (isStartPageInit() && !isStartPageOver())
    {
        //we are printing the start page, but haven't finished.
        //This situation happends when the start page has too much contents.
        return true;
    }

    if (_index < 0)
    {
        while (_fileNum < _filePath.count())
        {
            _curIncident = _filePath.at(_fileNum);
            QString fileStr = DATA_STORE_PATH + _filePath.at(_fileNum++)
                    + SUMMARY_DATA_FILE_NAME;

            if (NULL == _file)
            {
                _file = new StorageFile();
            }
            _file->reload(fileStr, QIODevice::ReadOnly);

            if(!_file->isValid())
            {
                return false;
            }

            _totalData = _file->getBlockNR();

            if (_totalData > 0)
            {
                _index = _totalData - 1;
                init();
                break;
            }

            //need to draw the header, make the initialization success.
            if (drawHeader)
            {
                _index = _totalData - 1;
                init();
                return true;
            }
        }

        if (_index < 0)
        {
            return false;
        }
    }

    return true;
}

PrintPage *PrintViewSummaryLayout::_drawOneSnapShock(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return NULL;
    }

    SummaryItemDesc desc;
    if ((unsigned)len <= sizeof(SummaryItemDesc))
    {
        return NULL;
    }

    memcpy(&desc, data, sizeof(SummaryItemDesc));

    SummaryCommonInfo *info = (SummaryCommonInfo*)(data + sizeof(SummaryItemDesc));
    if (NULL == info)
    {
        return NULL;
    }

    PrintWaveInfo waveInfo;
    waveInfo.summaryType = desc.type;

    // 打印页信息，一个打印请求的页宽和速度应该一致
    if (0 == _pageTimeLen)
    {
        PrintSpeed speed= printManager.getPrintSpeed();
        switch (speed)
        {
            case PRINT_SPEED_250:
                _pageTimeLen = 1;
                _pageWidth = 25 * PIXELS_PER_MM;
                break;
            case PRINT_SPEED_500:
                _pageTimeLen = 1;
                _pageWidth = 50 * PIXELS_PER_MM;
                break;
            default:
                break;
        }
    }

    if (_waveInfoList.isEmpty())
    {
        int y = fontManager.textHeightInPixels(printFont());
        int waveHeigh = SUMMARY_WAVE_HEIGHT - y;
        switch (desc.type)
        {
            case SUMMARY_ECG_RHYTHM:
            case SUMMARY_DIAG_ECG:
            case SUMMARY_ECG_FOREGROUND_ANALYSIS:
            case SUMMARY_SHOCK_DELIVERY:
            case SUMMARY_CHECK_PATIENT_ALARM:
            case SUMMARY_PACER_STARTUP:
            case SUMMARY_PRINTER_ACTION:
            {
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.calcLead = (ECGLead)info->ECGLeadName;
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                switch (waveInfo.ecgGain)
                {
                    case ECG_GAIN_X0125:
                    case ECG_GAIN_X025:
                    case ECG_GAIN_X05:
                    case ECG_GAIN_X10:
                    case ECG_GAIN_X15:
                    case ECG_GAIN_X20:
                    case ECG_GAIN_X30:
                    //    waveInfo.startY = y;
                    //    waveInfo.endY = y + SUMMARY_WAVE_HEIGHT / 2;
                    //    waveInfo.medialY = y + SUMMARY_WAVE_HEIGHT / 4;
                    //    break;
                    default:
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh;
                        waveInfo.medialY = y + waveHeigh / 2;
                        break;
                }
                waveInfo.pageWidth = _pageWidth;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;

                if (desc.type == SUMMARY_ECG_FOREGROUND_ANALYSIS)
                {
                    waveInfo.data = (char*)data;
                    waveInfo.callBack = _foregroundCallback;
                }
                else if(desc.type == SUMMARY_DIAG_ECG)
                {
                    SummaryDiagnosticECG *summary = reinterpret_cast<SummaryDiagnosticECG *>(data);
                    waveInfo.diagECGBandWidth = summary->diagBandWidth;
                }
                _waveInfoList.append(waveInfo);
                break;
            }
            case SUMMARY_PHY_ALARM:
            {
                SummaryPhyAlarm *summary = (SummaryPhyAlarm*)data;
                if (len != sizeof(SummaryPhyAlarm))
                {
                    return NULL;
                }
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                switch (waveInfo.ecgGain)
                {
                    case ECG_GAIN_X0125:
                    case ECG_GAIN_X025:
                    case ECG_GAIN_X05:
                    case ECG_GAIN_X10:
                    case ECG_GAIN_X15:
                    case ECG_GAIN_X20:
                    case ECG_GAIN_X30:
                    default:
                    if (summary->waveID != (char)WAVE_NONE)
                    {
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh / 2;
                        waveInfo.medialY = y + waveHeigh / 4;
                        break;
                    }
                    else
                    {
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh;
                        waveInfo.medialY = y + waveHeigh / 2;
                    }
                        break;
                }
                waveInfo.pageWidth = _pageWidth;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                if ((summary->waveID > WAVE_ECG_V6) && (summary->waveID < WAVE_NR))
                {
                    waveInfo.time = desc.time;
                    waveInfo.pageWidth = _pageWidth;
                    waveInfo.waveDataNum = _pageTimeLen *
                            waveformCache.getSampleRate((WaveformID)summary->waveID);
                    waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                    waveInfo.startY = y + waveHeigh / 2;
                    waveInfo.endY = y + waveHeigh;
                    waveInfo.medialY = y + waveHeigh * 3 / 4;
                    waveInfo.id = (WaveformID)summary->waveID;
                    _waveInfoList.append(waveInfo);
                }
                break;
            }

            case SUMMARY_PRINTER_ACTION_4_TRACE:
            {
                SummaryPrinterAction4Trace *printActionSnapshot = (SummaryPrinterAction4Trace*)data;
                if(len != sizeof(SummaryPrinterAction4Trace))
                {
                    return NULL;
                }

                //common info
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                waveInfo.pageWidth = _pageWidth;
                waveInfo.calcLead = (ECGLead) info->ECGLeadName;
                waveInfo.drawAllEcgGain = info->normalEcgGain & 0xFF000000;

                //calculate lead
                int deltaHeight = waveHeigh / 4;
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                //second trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID1;
                waveInfo.ecgGain = info->normalEcgGain & 0xFF;
                waveInfo.ecgBandwidth = info->normalEcgBandWidth;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                //third trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID2;
                waveInfo.ecgGain = (info->normalEcgGain >> 8) & 0xFF;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                //forth trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID3;
                waveInfo.ecgGain = (info->normalEcgGain >> 16) & 0xFF;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);


                break;
            }
            case SUMMARY_PRINTER_ACTION_3_TRACE:
            {
                SummaryPrinterAction3Trace *printActionSnapshot = (SummaryPrinterAction3Trace*)data;
                if(len != sizeof(SummaryPrinterAction3Trace))
                {
                    return NULL;
                }

                //common info
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                waveInfo.pageWidth = _pageWidth;
                waveInfo.calcLead = (ECGLead) info->ECGLeadName;
                waveInfo.drawAllEcgGain = info->normalEcgGain & 0xFF000000;

                //calculate lead
                int deltaHeight = waveHeigh / 3;
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);


                //second trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID1;
                waveInfo.ecgGain = info->normalEcgGain & 0xFF;
                waveInfo.ecgBandwidth = info->normalEcgBandWidth;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                //third trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID2;
                waveInfo.ecgGain = (info->normalEcgGain >> 8) & 0xFF;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);
                break;
            }
            case SUMMARY_PRINTER_ACTION_2_TRACE:
            {
                SummaryPrinterAction2Trace *printActionSnapshot = (SummaryPrinterAction2Trace*)data;
                if(len != sizeof(SummaryPrinterAction2Trace))
                {
                    return NULL;
                }

                //common info
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                waveInfo.pageWidth = _pageWidth;
                waveInfo.calcLead = (ECGLead) info->ECGLeadName;
                waveInfo.drawAllEcgGain = info->normalEcgGain & 0xFF000000;

                //calculate lead
                int deltaHeight = waveHeigh / 2;
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                //second trace
                y += deltaHeight;
                waveInfo.id = (WaveformID)printActionSnapshot->waveID1;
                waveInfo.ecgGain = info->normalEcgGain & 0xFF;
                waveInfo.ecgBandwidth = info->normalEcgBandWidth;
                waveInfo.startY = y;
                waveInfo.endY = y + deltaHeight;
                waveInfo.medialY = (waveInfo.startY + waveInfo.endY) / 2;
                waveInfo.waveDataNum = _pageTimeLen *
                        waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)_pageWidth / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);
                break;
            }
            case SUMMARY_CODE_MAKER:
            case SUMMARY_NIBP:
            _oneSnapShockOK = true;
            default:
                return NULL;
        }
    }

    if (_timeNum < DATA_STORAGE_WAVE_TIME)
    {
        WaveList::iterator waveIter = _waveInfoList.begin();
        switch (desc.type)
        {
            case SUMMARY_ECG_RHYTHM:
            case SUMMARY_DIAG_ECG:
            case SUMMARY_ECG_FOREGROUND_ANALYSIS:
            case SUMMARY_SHOCK_DELIVERY:
            case SUMMARY_CHECK_PATIENT_ALARM:
            case SUMMARY_PACER_STARTUP:
            case SUMMARY_PRINTER_ACTION:
            {
                if (1 != _waveInfoList.count())
                {
                    return NULL;
                }

                PrintWaveInfo &traceInfo = *waveIter;
                int rate = waveformCache.getSampleRate(traceInfo.id);
                memcpy(traceInfo.waveBuff, &info->ECGWave[rate * _timeNum],
                        traceInfo.waveDataNum * sizeof(traceInfo.waveBuff[0]));
                traceInfo.pageNum = _timeNum / _pageTimeLen + 1;
                break;
            }
            case SUMMARY_PHY_ALARM:
            {
                SummaryPhyAlarm *summary = (SummaryPhyAlarm*)data;
                if (len != sizeof(SummaryPhyAlarm))
                {
                    return NULL;
                }

                PrintWaveInfo &traceInfo = *waveIter;
                int rate = waveformCache.getSampleRate(traceInfo.id);
                memcpy(traceInfo.waveBuff, &summary->commonInfo.ECGWave[rate * _timeNum],
                        traceInfo.waveDataNum * sizeof(traceInfo.waveBuff[0]));
                traceInfo.pageNum = _timeNum / _pageTimeLen + 1;

                if (_waveInfoList.count() <= 1)
                {
                    break;
                }
                ++waveIter;
                PrintWaveInfo &traceInfo1 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo1.id);
                memcpy(traceInfo1.waveBuff, &summary->waveBuf[rate * _timeNum],
                        traceInfo1.waveDataNum * sizeof(traceInfo1.waveBuff[0]));
                traceInfo1.pageNum = _timeNum / _pageTimeLen + 1;
                break;
            }

            case SUMMARY_PRINTER_ACTION_4_TRACE:
            {
                SummaryPrinterAction4Trace *summary = (SummaryPrinterAction4Trace*) data;
                if(len != sizeof(SummaryPrinterAction4Trace))
                {
                    return NULL;
                }

                PrintWaveInfo &trace1Info = *waveIter;
                int rate = waveformCache.getSampleRate(trace1Info.id);
                memcpy(trace1Info.waveBuff, &summary->commonInfo.ECGWave[rate * _timeNum],
                        trace1Info.waveDataNum * sizeof(trace1Info.waveBuff[0]));
                trace1Info.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo2 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo2.id);
                memcpy(traceInfo2.waveBuff, &summary->waveBuf1[rate * _timeNum],
                        traceInfo2.waveDataNum * sizeof(traceInfo2.waveBuff[0]));
                traceInfo2.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo3 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo3.id);
                memcpy(traceInfo3.waveBuff, &summary->waveBuf2[rate * _timeNum],
                        traceInfo3.waveDataNum * sizeof(traceInfo3.waveBuff[0]));
                traceInfo3.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo4 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo4.id);
                memcpy(traceInfo4.waveBuff, &summary->waveBuf3[rate * _timeNum],
                        traceInfo4.waveDataNum * sizeof(traceInfo4.waveBuff[0]));
                traceInfo4.pageNum = _timeNum / _pageTimeLen + 1;

                break;
            }
            case SUMMARY_PRINTER_ACTION_3_TRACE:
            {
                SummaryPrinterAction3Trace *summary = (SummaryPrinterAction3Trace*) data;
                if(len != sizeof(SummaryPrinterAction3Trace))
                {
                    return NULL;
                }

                PrintWaveInfo &trace1Info = *waveIter;
                int rate = waveformCache.getSampleRate(trace1Info.id);
                memcpy(trace1Info.waveBuff, &summary->commonInfo.ECGWave[rate * _timeNum],
                        trace1Info.waveDataNum * sizeof(trace1Info.waveBuff[0]));
                trace1Info.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo2 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo2.id);
                memcpy(traceInfo2.waveBuff, &summary->waveBuf1[rate * _timeNum],
                        traceInfo2.waveDataNum * sizeof(traceInfo2.waveBuff[0]));
                traceInfo2.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo3 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo3.id);
                memcpy(traceInfo3.waveBuff, &summary->waveBuf2[rate * _timeNum],
                        traceInfo3.waveDataNum * sizeof(traceInfo3.waveBuff[0]));
                traceInfo3.pageNum = _timeNum / _pageTimeLen + 1;

                break;
            }
            case SUMMARY_PRINTER_ACTION_2_TRACE:
            {
                SummaryPrinterAction2Trace *summary = (SummaryPrinterAction2Trace*) data;
                if(len != sizeof(SummaryPrinterAction2Trace))
                {
                    return NULL;
                }

                PrintWaveInfo &trace1Info = *waveIter;
                int rate = waveformCache.getSampleRate(trace1Info.id);
                memcpy(trace1Info.waveBuff, &summary->commonInfo.ECGWave[rate * _timeNum],
                        trace1Info.waveDataNum * sizeof(trace1Info.waveBuff[0]));
                trace1Info.pageNum = _timeNum / _pageTimeLen + 1;

                waveIter++;

                PrintWaveInfo &traceInfo2 = *waveIter;
                rate = waveformCache.getSampleRate(traceInfo2.id);
                memcpy(traceInfo2.waveBuff, &summary->waveBuf1[rate * _timeNum],
                        traceInfo2.waveDataNum * sizeof(traceInfo2.waveBuff[0]));
                traceInfo2.pageNum = _timeNum / _pageTimeLen + 1;
                break;
            }

            default:
                return NULL;
        }

        _timeNum += _pageTimeLen;
        if (_timeNum >= DATA_STORAGE_WAVE_TIME)
        {
            _oneSnapShockOK = true;
        }

        return buildWavePage(_waveInfoList, printFont());
    }
    else
    {
        return NULL;
    }
}

/**************************************************************************************************
 * 画波形完成信息
 *************************************************************************************************/
PrintPage * PrintViewSummaryLayout::_drawWaveCompleteInfo(void)
{
    static int id = 0;
    QString text = trs("SummaryReportComplete");
    int fontH = fontManager.textHeightInPixels(printFont());
    PrintPage *page = new PrintPage(fontH + fontH / 2);
    page->setID(QString("summaryWaveEnd%1").arg(id++));
    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(page->rect(), Qt::SolidPattern);

    painter.rotate(-90.0);
    QRect r(QPoint(-page->rect().height(), 0),
            QSize(page->rect().height(), page->rect().width()));
    painter.drawText(r, Qt::AlignTop | Qt::AlignHCenter, text);

    return page;
}

/**************************************************************************************************
 * 画完成信息
 * high:本次绘画的y坐标
 *************************************************************************************************/
void PrintViewSummaryLayout::_drawCompleteInfo(PrintPage *page, int high)
{
    if (NULL == page)
    {
        return;
    }

    QRect r = page->rect();
    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);

    QString text = trs("SummaryReportComplete");
    QRect rect(QPoint(0, high), QSize(r.width(), r.height() - high));
    painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, text);
}
