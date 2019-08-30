/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#include "TrendWaveWidget.h"
#include "TrendSubWaveWidget.h"
#include "TimeDate.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "TrendGraphSetWindow.h"
#include "IBPParam.h"
#include "TrendDataStorageManager.h"
#include "TrendDataSymbol.h"
#include "IConfig.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include "DataStorageDefine.h"
#include <QMouseEvent>
#include "ParamManager.h"
#include "FontManager.h"
#include "TimeDefine.h"
#include "EventDataParseContext.h"
#include "Debug.h"
#include "PatientManager.h"

#define GRAPH_DISPLAY_DATA_NUMBER           4
#define GRAPH_POINT_NUMBER                  120                     // 一屏数据量
#define DATA_INTERVAL_PIXEL                 3                       // 两数据之间的像素点个数
#define GRAPH_DATA_WIDTH                    360                    // 一屏数据所占像素点
#define SCALE_REGION_HEIGHT                 35                      // 时间刻度区域高度
#define INIT_WAVE_NUM                       3                       // 初始化波形数

TrendWaveWidget::TrendWaveWidget() : _hLayoutTrend(NULL),
    _timeInterval(RESOLUTION_RATIO_5_SECOND),
    _waveRegionWidth(0), _oneFrameWidth(0),
    _initTime(0),
    _cursorPosIndex(0), _currentCursorTime(0), _subWidget(NULL),
    _displayGraphNum(3), _totalGraphNum(3), _curVScroller(0),
    _totalPage(0), _currentPage(0),  _leftTime(0), _rightTime(0),
    _curIndex(0), _trendGraphInfo(TrendGraphInfo()), _isHistory(false)
{
    QString prefix = "TrendGraph|";
    int index = 0;
    QString intervalPrefix = prefix + "TimeInterval";
    systemConfig.getNumValue(intervalPrefix, index);
    _timeInterval = (ResolutionRatio)index;
    index = 0;
    QString numberPrefix = prefix + "WaveNumber";
    systemConfig.getNumValue(numberPrefix, index);
    _displayGraphNum = index + 1;

    _initTime = timeDate.time();
    _initTime = _initTime - _initTime % 5;
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::NoFocus);

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);
    _mainLayout->addSpacing(SCALE_REGION_HEIGHT);

    // 初始化三道波形

    for (int i = 0; i < INIT_WAVE_NUM; i++)
    {
        TrendSubWaveWidget *subWidget;
        subWidget = new TrendSubWaveWidget();
        subWidget->setFocusPolicy(Qt::NoFocus);
        _mainLayout->addWidget(subWidget, 1);
        _subWidgetList.append(subWidget);
    }
    setLayout(_mainLayout);
}

TrendWaveWidget::~TrendWaveWidget()
{
    _trendDataPack.clear();
    _subWidgetList.clear();
    _trendGraphInfo.reset();
}

void TrendWaveWidget::setWidgetSize(int w, int h)
{
    setFixedSize(w, h);
    _waveRegionWidth = width() / 4 * 3;
    _oneFrameWidth = GRAPH_DATA_WIDTH / GRAPH_DISPLAY_DATA_NUMBER;
    //确认好窗口大小后初始化子波形窗口
    _initWaveSubWidget();
}

void TrendWaveWidget::leftMoveCoordinate()
{
    if (_currentPage >= _totalPage)
    {
        return;
    }
    else
    {
        _currentPage++;
    }
    updateTimeRange();
    update();
}

void TrendWaveWidget::rightMoveCoordinate()
{
    if (_currentPage <= 1)
    {
        return;
    }
    else
    {
        _currentPage--;
    }
    updateTimeRange();
    update();
}

void TrendWaveWidget::leftMoveCursor()
{
    if (_cursorPosIndex >= _trendGraphInfo.alarmInfo.count() - 1)
    {
        _cursorPosIndex = _trendGraphInfo.alarmInfo.count() - 1;
    }
    else
    {
        _cursorPosIndex++;
    }

    int count = _displayGraphNum;
    for (int i = 0; i < count; i++)
    {
        _subWidgetList.at(i)->cursorMove(_cursorPosIndex);
    }
    update();
}

void TrendWaveWidget::rightMoveCursor()
{
    if (_cursorPosIndex <= 0)
    {
        _cursorPosIndex = 0;
    }
    else
    {
        _cursorPosIndex--;
    }

    int count = _displayGraphNum;
    for (int i = 0; i < count; i++)
    {
        _subWidgetList.at(i)->cursorMove(_cursorPosIndex);
    }
    update();
}

void TrendWaveWidget::leftMoveEvent()
{
    // 遍历找到下一个事件发生时间
    for (int i = _eventList.count() - 1; i >= 0; i --)
    {
        EventInfoSegment event = _eventList.at(i);
        unsigned alarmTime = event.timestamp;
        unsigned curTime = _trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp;
        if (alarmTime < curTime)
        {
            // 事件时间小于当前页最左侧时间时,进行翻页操作
            if (alarmTime < _leftTime)
            {
                if (_currentPage >= _totalPage)
                {
                    return;
                }
                else
                {
                    _currentPage++;
                }
                updateTimeRange();
                _cursorPosIndex = 0;
                leftMoveEvent();
            }
            else
            {
                // 将游标移到事件发生时刻
                _cursorPosIndex = _getTimeIndex(alarmTime);

                int count = _displayGraphNum;
                for (int i = 0; i < count; i++)
                {
                    _subWidgetList.at(i)->cursorMove(_cursorPosIndex);
                }
                update();
            }
            return;
        }
    }
}

void TrendWaveWidget::rightMoveEvent()
{
    for (int i = 0; i < _eventList.count(); i ++)
    {
        EventInfoSegment event = _eventList.at(i);
        unsigned alarmTime = event.timestamp;
        unsigned curTime = _trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp;
        if (alarmTime > curTime)
        {
            if (alarmTime > _rightTime)
            {
                if (_currentPage <= 1)
                {
                    return;
                }
                else
                {
                    _currentPage--;
                }
                updateTimeRange();
                _cursorPosIndex = _trendGraphInfo.alarmInfo.count() - 1;
                rightMoveEvent();
            }
            else
            {
                // 将游标移到事件发生时刻
                _cursorPosIndex = _getTimeIndex(alarmTime);

                int count = _displayGraphNum;
                for (int i = 0; i < count; i++)
                {
                    _subWidgetList.at(i)->cursorMove(_cursorPosIndex);
                }
                update();
            }
            return;
        }
    }
}

void TrendWaveWidget::pageUpParam()
{
    // 参数个数少于显示波形数时不进行翻页操作
    if (_infosList.count() < _displayGraphNum)
    {
        return;
    }
    if ((_curIndex - _displayGraphNum * 2) >= 0)
    {
        _curIndex = _curIndex - _displayGraphNum;
    }
    else
    {
        _curIndex = 0;
    }
    updateTimeRange();
}

void TrendWaveWidget::pageDownParam()
{
    // 参数个数少于显示波形数时不进行翻页操作
    if (_infosList.count() < _displayGraphNum)
    {
        return;
    }

    if ((_curIndex + _displayGraphNum * 2) < _subParams.count())
    {
        _curIndex = _curIndex + _displayGraphNum;
    }
    else
    {
        _curIndex = _subParams.count() - _displayGraphNum;
    }
    updateTimeRange();
}

void TrendWaveWidget::setTimeInterval(ResolutionRatio timeInterval)
{
    _timeInterval = timeInterval;
    _calculationPage();
    _updateEventIndex();
    updateTimeRange();
    update();
}

void TrendWaveWidget::setWaveNumber(int num)
{
    trendWaveReset();
    _displayGraphNum = num;
    updateTimeRange();
}

void TrendWaveWidget::setRulerLimit(int index, int down, int up, int scale)
{
    if (index < _subWidgetList.count())
    {
        _subWidgetList.at(index)->setRulerRange(down, up, scale);
    }
}

void TrendWaveWidget::loadTrendData(SubParamID subID, const int startIndex, const int endIndex)
{
    _trendGraphInfo.reset();
    if (startIndex == InvData() || endIndex == InvData())
    {
        return;
    }

    _trendGraphInfo.startTime = _leftTime;
    _trendGraphInfo.endTime = _rightTime;
    _trendGraphInfo.subParamID = subID;
    unsigned interval = TrendDataSymbol::convertValue(_timeInterval);
    switch (subID)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_SPO2:
    case SUB_PARAM_RR_BR:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    {
        TrendGraphData dataV1;
        AlarmEventInfo alarm;
        unsigned lastTime = _trendDataPack.at(endIndex)->time;
        bool startFlag = true;
        for (int i = endIndex; i >= startIndex; i --)
        {
            if (startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
                    // 处理由于关机引起的数据未记录:向下一个存储数据移动的时间间隔个数
                    int intervalNum = (lastTime - t) / interval + (((lastTime - t) % interval) ? 1 : 0);
                    lastTime = lastTime - interval * intervalNum;
                    dataV1.data = InvData();
                    dataV1.isAlarm = false;
                    dataV1.timestamp = lastTime;
                    alarm.isAlarmEvent = false;
                    alarm.timestamp = lastTime;
                    _trendGraphInfo.alarmInfo.append(alarm);
                    _trendGraphInfo.trendData.append(dataV1);
                    lastTime = lastTime - interval;
                    continue;
                }
                else if (t > lastTime)
                {
                    continue;
                }
            }
            dataV1.data = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV1.isAlarm = _trendDataPack.at(i)->subparamAlarm.value(subID, false);
            dataV1.timestamp = _trendDataPack.at(i)->time;
            alarm.isAlarmEvent = _trendDataPack.at(i)->alarmFlag;
            alarm.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.alarmInfo.append(alarm);
            _trendGraphInfo.trendData.append(dataV1);
            lastTime = lastTime - interval;
        }
        break;
    }
    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
    {
        AlarmEventInfo alarm;
        TrendGraphDataV3 dataV3;
        unsigned lastTime = _trendDataPack.at(endIndex)->time;
        bool startFlag = true;
        for (int i = endIndex; i >= startIndex; i --)
        {
            if (startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
                    // 处理由于关机引起的数据未记录:向下一个存储数据移动的时间间隔个数
                    int intervalNum = (lastTime - t) / interval + (((lastTime - t) % interval) ? 1 : 0);
                    lastTime = lastTime - interval * intervalNum;
                    dataV3.data[0] = InvData();
                    dataV3.data[1] = InvData();
                    dataV3.data[2] = InvData();
                    dataV3.isAlarm = false;
                    dataV3.timestamp = lastTime;
                    alarm.isAlarmEvent = false;
                    alarm.timestamp = lastTime;
                    _trendGraphInfo.alarmInfo.append(alarm);
                    _trendGraphInfo.trendDataV3.append(dataV3);
                    lastTime = lastTime - interval;
                    continue;
                }
                else if (t > lastTime)
                {
                    if (subID == SUB_PARAM_NIBP_SYS)
                    {
                        // NIBP测量标志位保存在后一个时间间隔的数据中.
                        unsigned status = _trendDataPack.at(i)->status;
                        if (status & TrendDataStorageManager::CollectStatusNIBP)
                        {
                            if (_trendGraphInfo.trendDataV3.count())
                            {
                                _trendGraphInfo.trendDataV3.last().status = status;
                            }
                        }
                    }
                    continue;
                }
            }
            dataV3.data[0] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV3.data[1] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 1), InvData());
            dataV3.data[2] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 2), InvData());
            dataV3.isAlarm = _trendDataPack.at(i)->subparamAlarm.value(subID, false) ||
                    _trendDataPack.at(i)->subparamAlarm.value(SubParamID(subID + 1), false) ||
                    _trendDataPack.at(i)->subparamAlarm.value(SubParamID(subID + 2), false);
            dataV3.timestamp = _trendDataPack.at(i)->time;
            dataV3.status = _trendDataPack.at(i)->status;
            alarm.isAlarmEvent = _trendDataPack.at(i)->alarmFlag;
            alarm.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.alarmInfo.append(alarm);
            _trendGraphInfo.trendDataV3.append(dataV3);
            lastTime = lastTime - interval;
        }
        break;
    }
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_T1:
    {
        AlarmEventInfo alarm;
        TrendGraphDataV2 dataV2;
        unsigned lastTime = _trendDataPack.at(endIndex)->time;
        bool startFlag = true;
        for (int i = endIndex; i >= startIndex; i--)
        {
            if (startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
                    // 处理由于关机引起的数据未记录:向下一个存储数据移动的时间间隔个数
                    int intervalNum = (lastTime - t) / interval + (((lastTime - t) % interval) ? 1 : 0);
                    lastTime = lastTime - interval * intervalNum;
                    dataV2.data[0] = InvData();
                    dataV2.data[1] = InvData();
                    dataV2.isAlarm = false;
                    dataV2.timestamp = lastTime;
                    alarm.isAlarmEvent = false;
                    alarm.timestamp = lastTime;
                    _trendGraphInfo.alarmInfo.append(alarm);
                    _trendGraphInfo.trendDataV2.append(dataV2);
                    lastTime = lastTime - interval;
                    continue;
                }
                else if (t > lastTime)
                {
                    continue;
                }
            }
            dataV2.data[0] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV2.data[1] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 1), InvData());
            dataV2.isAlarm = _trendDataPack.at(i)->subparamAlarm.value(subID, false);
            dataV2.timestamp = _trendDataPack.at(i)->time;
            alarm.isAlarmEvent = _trendDataPack.at(i)->alarmFlag;
            alarm.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.alarmInfo.append(alarm);
            _trendGraphInfo.trendDataV2.append(dataV2);
            lastTime = lastTime - interval;
        }
        break;
    }
    default:
        break;
    }

    _cursorPosIndex = 0;
}

void TrendWaveWidget::dataIndex(int &startIndex, int &endIndex)
{
    startIndex = _findIndex(_leftTime);
    endIndex = _findIndex(_rightTime);
}

void TrendWaveWidget::updateTimeRange()
{
    unsigned t;
    unsigned onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
    if (_trendDataPack.length() == 0)
    {
        t = _initTime;
    }
    else
    {
        unsigned lastTime = _trendDataPack.last()->time;
        t = lastTime - onePixelTime * GRAPH_POINT_NUMBER * (_currentPage - 1);
    }
    _rightTime = t;
    _leftTime = t - onePixelTime * GRAPH_POINT_NUMBER;
    _trendLayout();
    update();
}

void TrendWaveWidget::trendWaveReset()
{
    _curIndex = 0;
}

const QList<TrendGraphInfo> TrendWaveWidget::getTrendGraphPrint()
{
    for (int i = 0; i < _infosList.count(); i++)
    {
        int down;
        int up;
        int scale;
        _subWidgetList.at(i)->rulerRange(down, up, scale);
        _infosList[i].unit = _subWidgetList.at(i)->getUnitType();
        _infosList[i].scale.min = down;
        _infosList[i].scale.max = up;
        _infosList[i].scale.scale = scale;
    }
    return _infosList;
}

const QList<EventInfoSegment> TrendWaveWidget::getEventList()
{
    return _eventList;
}

const PatientInfo &TrendWaveWidget::getPatientInfo()
{
    return _patientInfo;
}

void TrendWaveWidget::setHistoryDataPath(QString path)
{
    _historyDataPath = path;
}

void TrendWaveWidget::setHistoryData(bool flag)
{
    _isHistory = flag;
}

QList<SubParamID> TrendWaveWidget::getCurParamList()
{
    return _curDisplaySubList;
}

void TrendWaveWidget::setAllParamAutoRuler()
{
    for (int i = 0; i < _subParams.count(); i++)
    {
        SubParamID sid = _subParams.at(i);
        QString prefix = "TrendGraph|Ruler|";
        prefix += paramInfo.getSubParamName(sid, true);
        systemConfig.setNumAttr(prefix, "Auto", 1);
    }
}

void TrendWaveWidget::paintEvent(QPaintEvent *event)
{
    IWidget::paintEvent(event);

    QPainter barPainter(this);

    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    QRect rectAdjust = rect().adjusted(0, SCALE_REGION_HEIGHT, 0, 0);
    barPainter.drawLine(rectAdjust.topLeft(), rectAdjust.topRight());

    double cursorPos;
    if (_trendGraphInfo.alarmInfo.count() != 0)
    {
        cursorPos = _getCursorPos(_trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp);
    }

    unsigned t = _rightTime;
    QString tStr;
    unsigned onePixelTime;
    onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
    // 坐标刻度
    for (int i = GRAPH_DISPLAY_DATA_NUMBER; i >= 0; i --)
    {
        timeDate.getTime(t, tStr, true);
        int timeFormat = 0;
        systemConfig.getNumValue("DateTime|TimeFormat", timeFormat);
        if (timeFormat == TIME_FORMAT_24)
        {
            QFont font = fontManager.textFont(fontManager.getFontSize(3));
            barPainter.setFont(font);
        }
        else
        {
            QFont font = fontManager.textFont(fontManager.getFontSize(1));
            barPainter.setFont(font);
        }

        barPainter.drawText(rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2 + i * _oneFrameWidth - 40,
                            rectAdjust.topLeft().y() - 5, tStr);
        t = t - onePixelTime * GRAPH_POINT_NUMBER / GRAPH_DISPLAY_DATA_NUMBER;
        barPainter.drawLine(rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y(), rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y() + 5);
    }

    // 游标线
    barPainter.drawLine(cursorPos, rectAdjust.topLeft().y(),
                        cursorPos, rectAdjust.bottomLeft().y());

    // 当前趋势记录的时间
    QRect timeRect = rect().adjusted(_waveRegionWidth + 5, 12, -5, 0);
    if (_cursorPosIndex < _trendGraphInfo.alarmInfo.count() && _cursorPosIndex >= 0)
    {
        timeDate.getDate(_trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp, tStr);
        barPainter.drawText(timeRect, Qt::AlignLeft, tStr);
        timeDate.getTime(_trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp, tStr, true);
        barPainter.drawText(timeRect, Qt::AlignRight, tStr);
    }

    barPainter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    barPainter.drawLine(_waveRegionWidth, 0, _waveRegionWidth, height());

    // 报警事件的标志
    for (int i = 0; i < _eventList.count(); i ++)
    {
        EventInfoSegment event = _eventList.at(i);
        unsigned alarmTime = event.timestamp;
        if (alarmTime <= _rightTime && alarmTime >= _leftTime)
        {
            if (event.type == EventPhysiologicalAlarm)
            {
                barPainter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
            }
            else if (event.type != EventOxyCRG)
            {
                barPainter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
            }
            double pos = _getCursorPos(alarmTime);
            barPainter.drawLine(pos, 0, pos, 5);
        }
    }
}

void TrendWaveWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    _getTrendData();
    _cursorPosIndex = 0;
    updateTimeRange();
    _updateEventIndex();
}

void TrendWaveWidget::mousePressEvent(QMouseEvent *e)
{
    if (_trendGraphInfo.alarmInfo.count() == 0)
    {
        return;
    }
    double waveHead = _getCursorPos(_trendGraphInfo.alarmInfo.last().timestamp);
    double waveTail = _getCursorPos(_trendGraphInfo.alarmInfo.first().timestamp);
    if (e->x() >= waveHead && e->x() <= waveTail)
    {
        double pos = e->x();
        unsigned t = _getCursorTime(pos);
        // 二分查找时间索引
        int lowPos = 0;
        int highPos = _trendGraphInfo.alarmInfo.count() - 1;
        while (lowPos <= highPos)
        {
            int midPos = (lowPos + highPos) / 2;
            unsigned midTime = _trendGraphInfo.alarmInfo.at(midPos).timestamp;
            int timeDiff = qAbs(t - midTime);

            if (t < midTime)
            {
                lowPos = midPos + 1;
            }
            else if (t > midTime)
            {
                highPos = midPos - 1;
            }

            if (timeDiff == 0 || lowPos > highPos)
            {
                _cursorPosIndex = midPos;
                break;
            }
        }
        int count = _displayGraphNum;
        for (int i = 0; i < count; i++)
        {
            _subWidgetList.at(i)->cursorMove(_cursorPosIndex);
        }
        update();
    }
}

void TrendWaveWidget::_trendLayout()
{
    _infosList.clear();
    _curDisplaySubList.clear();
    int subWidgetHeight = (height() - SCALE_REGION_HEIGHT) / _displayGraphNum;
    TrendSubWidgetInfo info;
    info.xHead = (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2;
    info.xTail = (_waveRegionWidth + GRAPH_DATA_WIDTH) / 2;
    info.yTop = subWidgetHeight / 5;
    info.yBottom = subWidgetHeight / 5 * 4;
    _totalGraphNum = 0;
    int startIndex;
    int endIndex;
    dataIndex(startIndex, endIndex);
    for (int i = 0; i < _displayGraphNum; i++)
    {
        int index = _curIndex + i;
        if (index < 0 || index + 1 > _subParams.count())
        {
            continue;
        }
        SubParamID  subId = _subParams.at(index);
        _curDisplaySubList.append(subId);
        _subWidgetList.at(i)->setWidgetParam(subId, getTrendGraphType(subId));
        _subWidgetList.at(i)->setThemeColor(colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(subId))));

        loadTrendData(subId, startIndex, endIndex);
        _subWidgetList.at(i)->trendDataInfo(_trendGraphInfo);
        _subWidgetList.at(i)->setFixedHeight(subWidgetHeight);
        _subWidgetList.at(i)->loadTrendSubWidgetInfo(info);
        _subWidgetList.at(i)->setTimeRange(_leftTime, _rightTime);
        _trendGraphInfo.unit = paramInfo.getUnitOfSubParam(subId);
        _subWidgetList.at(i)->update();
        _subWidgetList.at(i)->getValueLimit(_trendGraphInfo.scale.max, _trendGraphInfo.scale.min, _trendGraphInfo.scale.scale);
        _infosList.append(_trendGraphInfo);
        _totalGraphNum++;
    }
    if (_displayGraphNum == 3)
    {
        _subWidgetList.at(1)->setVisible(true);
        _subWidgetList.at(2)->setVisible(true);
    }
    else if (_displayGraphNum == 2)
    {
        _subWidgetList.at(1)->setVisible(true);
        _subWidgetList.at(2)->setVisible(false);
    }
    else if (_displayGraphNum == 1)
    {
        _subWidgetList.at(1)->setVisible(false);
        _subWidgetList.at(2)->setVisible(false);
    }
}

void TrendWaveWidget::_getTrendData()
{
    IStorageBackend *backend;
    if (_isHistory)
    {
        backend = StorageManager::open(_historyDataPath + TREND_DATA_FILE_NAME, QIODevice::ReadWrite);
        _patientInfo = patientManager.getHistoryPatientInfo(_historyDataPath + PATIENT_INFO_FILE_NAME);
    }
    else
    {
        backend = trendDataStorageManager.backend();
        _patientInfo = patientManager.getPatientInfo();
    }
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    qDeleteAll(_trendDataPack);
    _trendDataPack.clear();
    _eventList.clear();
    // TODO: low efficiency
    for (int i = 0; i < blockNum; i ++)
    {
        TrendDataPackage *pack;
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = reinterpret_cast<TrendDataSegment *>(data.data());
        pack->time = dataSeg->timestamp;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->subparamValue[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].value;
            pack->subparamAlarm[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].alarmFlag;
            pack->alarmFlag = dataSeg->eventFlag;
            pack->status = dataSeg->status;
        }
        _trendDataPack.append(pack);
    }
    _calculationPage();

    // 动态内存释放
    if (_isHistory)
    {
        delete backend;
    }
}

void TrendWaveWidget::_initWaveSubWidget()
{
    QList<ParamID> pids;
    paramManager.getParams(pids);
    // 初始化前三道波形
    int num = 0;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        SubParamID subID = static_cast<SubParamID>(i);
        ParamID pid = paramInfo.getParamID(subID);
        if (!pids.contains(pid))
        {
            continue;
        }

        if (getTrendGraphType(subID) == TREND_GRAPH_TYPE_NR)
        {
            continue;
        }

        _subParams.append(subID);

        if (num < _subWidgetList.count())
        {
            TrendSubWaveWidget *subWidget = _subWidgetList.at(num);
            subWidget->setWidgetParam(subID, getTrendGraphType(subID));
            subWidget->setVisible(true);
            subWidget->setParent(this);
            subWidget->setThemeColor(colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID((SubParamID)i))));
            subWidget->setFocusPolicy(Qt::NoFocus);
            num++;
        }
    }
}

double TrendWaveWidget::_getCursorPos(unsigned t)
{
    double dpos = 0;
    dpos = (t - _leftTime) * GRAPH_DATA_WIDTH / (_rightTime - _leftTime) + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2;

    return dpos;
}

unsigned TrendWaveWidget::_getTimeIndex(unsigned t)
{
    for (int i = 0; i < _trendGraphInfo.alarmInfo.count(); i++)
    {
        if (t == _trendGraphInfo.alarmInfo.at(i).timestamp)
        {
            return i;
        }
    }
    return 0;
}

unsigned TrendWaveWidget::_getCursorTime(double pos)
{
    unsigned t = 0;
    t = (pos - (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2) * (_rightTime - _leftTime) / GRAPH_DATA_WIDTH + _leftTime;
    return t;
}

void TrendWaveWidget::_calculationPage()
{
    int timeInterval = TrendDataSymbol::convertValue(_timeInterval);
    int timeDiff;
    if (_trendDataPack.count() == 0)
    {
        timeDiff = 0;
    }
    else
    {
        timeDiff = _trendDataPack.last()->time - _trendDataPack.first()->time;
    }
    int dataNum = timeDiff / timeInterval;
    _totalPage = (dataNum % GRAPH_POINT_NUMBER) ? ((dataNum / GRAPH_POINT_NUMBER) + 1) : (dataNum / GRAPH_POINT_NUMBER);
    _currentPage = 1;
}

void TrendWaveWidget::_updateEventIndex()
{
    IStorageBackend *backend = NULL;
    if ((_historyDataPath != "") && _isHistory)
    {
        backend =  StorageManager::open(_historyDataPath + EVENT_DATA_FILE_NAME, QIODevice::ReadOnly);
    }
    else
    {
        backend = eventStorageManager.backend();
    }
    int eventNum = backend->getBlockNR();
    EventDataPraseContext ctx;
    _eventList.clear();
    unsigned preTime = 0;
    for (int i = 0; i < eventNum; i++)
    {
        ctx.reset();
        if (ctx.parse(backend, i))
        {
            unsigned t = ctx.infoSegment->timestamp;
            int interval = TrendDataSymbol::convertValue(_timeInterval);
            // 最右边时间 - 最右边时间与事件之间的整数个时间间隔
            t = _rightTime - (_rightTime - t) / interval * interval;
            EventInfoSegment event;
            event.type = ctx.infoSegment->type;
            event.timestamp = t;
            if (i != 0)
            {
                if (preTime == t)
                {
                    continue;       // 相同时间不重复添加进事件列表
                }
            }
            preTime = t;
            _eventList.append(event);

            // 将事件归为前一个时刻后，同时将前一刻的状态也更新
            int curIndex = _findIndex(event.timestamp);
            int lastIndex = _findIndex(ctx.infoSegment->timestamp);
            if (curIndex != lastIndex)
            {
                _trendDataPack.at(curIndex)->status = _trendDataPack.at(lastIndex)->status;
                _trendDataPack.at(curIndex)->subparamValue = _trendDataPack.at(lastIndex)->subparamValue;
                _trendDataPack.at(curIndex)->subparamAlarm = _trendDataPack.at(lastIndex)->subparamAlarm;
            	_trendDataPack.at(curIndex)->alarmFlag = _trendDataPack.at(lastIndex)->alarmFlag;
            }
        }
    }
    if ((_historyDataPath != "") && _isHistory)
    {
        delete backend;
    }
    update();
}

int TrendWaveWidget::_findIndex(unsigned timeStamp)
{
    int lowPos = 0;
    int highPos = _trendDataPack.count() - 1;
    int index = InvData();
    while (lowPos <= highPos)
    {
        int midPos = (lowPos + highPos) / 2;
        int timeDiff = qAbs(timeStamp - _trendDataPack.at(midPos)->time);

        if (timeStamp < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (timeStamp > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            index = midPos;
            break;
        }
    }
    return index;
}

TrendGraphType TrendWaveWidget::getTrendGraphType(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_SPO2:
    case SUB_PARAM_RR_BR:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
        return TREND_GRAPH_TYPE_NORMAL;
    case SUB_PARAM_NIBP_SYS:
        return TREND_GRAPH_TYPE_NIBP;
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_T1:
        return TREND_GRAPH_TYPE_AG_TEMP;
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
        return TREND_GRAPH_TYPE_ART_IBP;
    default:
        return TREND_GRAPH_TYPE_NR;
    }
}
