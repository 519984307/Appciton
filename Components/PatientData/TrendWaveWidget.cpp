#include "TrendWaveWidget.h"
#include "TrendSubWaveWidget.h"
#include "TimeDate.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "TrendGraphSetWidget.h"
#include "IBPParam.h"
#include "TrendDataStorageManager.h"
#include "TrendDataSymbol.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>

#define GRAPH_DISPLAY_DATA_NUMBER           4
#define GRAPH_POINT_NUMBER                  120                     // 一屏数据量
#define DATA_INTERVAL_PIXEL                 5                       // 两数据之间的像素点个数
#define GRAPH_DATA_WIDTH                    600                     // 一屏数据所占像素点

TrendWaveWidget::TrendWaveWidget() :
    _timeInterval(RESOLUTION_RATIO_5_SECOND),
    _initTime(0),
    _cursorPosIndex(0), _currentCursorTime(0),
    _displayGraphNum(3), _totalGraphNum(3)
{
    _initTime = timeDate.time();
    _initTime = _initTime - _initTime % 5;
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::NoFocus);

    _subWidget = new IWidget();
    _subWidget->setFocusPolicy(Qt::NoFocus);
    _subWidgetScrollArea = new QScrollArea();

    // 重新调整子部件的大小
    _subWidgetScrollArea->setWidgetResizable(true);

    // 滚动区域透明处理
    _subWidgetScrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
    _subWidgetScrollArea->viewport()->setStyleSheet("background-color:transparent;");

    _subWidgetScrollArea->setFocusPolicy(Qt::NoFocus);

    _subWidgetScrollArea->verticalScrollBar()->setStyleSheet(
                                                             "QScrollBar:vertical"
                                                             "{"
                                                             "width:12px;"
                                                             "}"
                                                             );

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);

    _hLayoutTrend = new QVBoxLayout();
    _hLayoutTrend->setMargin(0);
    _hLayoutTrend->setSpacing(0);
    _subWidget->setLayout(_hLayoutTrend);

    _subWidgetScrollArea->setWidget(_subWidget);

    _mainLayout->addSpacing(30);
    _mainLayout->addWidget(_subWidgetScrollArea);

    setLayout(_mainLayout);
}

TrendWaveWidget::~TrendWaveWidget()
{
    _trendDataPack.clear();
    _subWidgetMap.clear();
    _trendGraphInfo.reset();
}

void TrendWaveWidget::setWidgetSize(int w, int h)
{
    setFixedSize(w, h);
    _waveRegionWidth = width()/5 * 4;
    _oneFrameWidth = GRAPH_DATA_WIDTH/GRAPH_DISPLAY_DATA_NUMBER;
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
        _currentPage ++;
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
        _currentPage --;
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
    else{
        _cursorPosIndex ++;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

void TrendWaveWidget::rightMoveCursor()
{
    if (_cursorPosIndex <= 0)
    {
        _cursorPosIndex = 0;
    }
    else{
        _cursorPosIndex --;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

void TrendWaveWidget::leftMoveEvent()
{
    bool flag = true;
    int index = _cursorPosIndex;
    while (flag || !_trendGraphInfo.alarmInfo.at(_cursorPosIndex).isAlarmEvent)
    {
        flag = false;        
        if (_cursorPosIndex >= _trendGraphInfo.alarmInfo.count() - 1)
        {
            unsigned time = _trendGraphInfo.alarmInfo.at(index).timestamp;
            if (!_alarmTimeList.isEmpty())
            {
                if (time <= _alarmTimeList.first())
                {
                    _cursorPosIndex = index;
                    return;
                }
                else
                {
                    if (_currentPage >= _totalPage)
                    {
                        return;
                    }
                    else
                    {
                        _currentPage ++;
                    }
                    updateTimeRange();
                    _cursorPosIndex = 0;
                    leftMoveEvent();
                }
            }
            else
            {
                _cursorPosIndex = index;
                return;
            }
        }
        else{
            _cursorPosIndex ++;
        }
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

void TrendWaveWidget::rightMoveEvent()
{
    bool flag = true;
    int index = _cursorPosIndex;
    while (flag || !_trendGraphInfo.alarmInfo.at(_cursorPosIndex).isAlarmEvent)
    {
        flag = false;
        if (_cursorPosIndex <= 0)
        {
            unsigned time = _trendGraphInfo.alarmInfo.at(index).timestamp;
            if (!_alarmTimeList.isEmpty())
            {
                if (time >= _alarmTimeList.last())
                {
                    _cursorPosIndex = index;
                    return;
                }
                else
                {
                    if (_currentPage <= 1)
                    {
                        return;
                    }
                    else
                    {
                        _currentPage --;
                    }
                    updateTimeRange();
                    _cursorPosIndex = _trendGraphInfo.alarmInfo.count() - 1;
                    rightMoveEvent();
                }
            }
            else
            {
                _cursorPosIndex = index;
                return;
            }
        }
        else{
            _cursorPosIndex --;
        }
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

void TrendWaveWidget::pageUpParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller > 0)
    {
        QScrollBar *scrollBar = _subWidgetScrollArea->verticalScrollBar();
        // 当前位置　－　最大位置乘以总行数除以看不见的行数
        int positon = _curVScroller - (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum);
        scrollBar->setSliderPosition(positon);
    }

}

void TrendWaveWidget::pageDownParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller < maxValue && _totalGraphNum != _displayGraphNum)
    {
        QScrollBar *scrollBar = _subWidgetScrollArea->verticalScrollBar();
        int positon = _curVScroller + (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum);
        scrollBar->setSliderPosition(positon);
    }
}

void TrendWaveWidget::setTimeInterval(ResolutionRatio timeInterval)
{
    _timeInterval = timeInterval;
    _calculationPage();
    updateTimeRange();
    update();
}

void TrendWaveWidget::setWaveNumber(int num)
{
    _displayGraphNum = num;    
    updateTimeRange();
}

void TrendWaveWidget::setRulerLimit(SubParamID id, int down, int up)
{
    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i ++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = qobject_cast<TrendSubWaveWidget *>(item->widget());
        if (widget->getSubParamID() == id)
        {
            widget->setRulerRange(down, up);
            return;
        }
    }
}

void TrendWaveWidget::loadTrendData(SubParamID subID, const int startIndex, const int endIndex)
{
    if (startIndex == InvData() || endIndex == InvData())
    {
        return;
    }

    _trendGraphInfo.reset();
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
        for(int i = endIndex; i >= startIndex; i --)
        {
            if(startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
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
        for(int i = endIndex; i >= startIndex; i --)
        {
            if(startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
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
                    continue;
                }
            }
            dataV3.data[0] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV3.data[1] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 1), InvData());
            dataV3.data[2] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 2), InvData());
            dataV3.isAlarm = _trendDataPack.at(i)->subparamAlarm.value(subID, false);
            dataV3.timestamp = _trendDataPack.at(i)->time;
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
        for(int i = endIndex; i >= startIndex; i--)
        {
            if(startFlag)
            {
                startFlag = false;
            }
            else
            {
                unsigned t = _trendDataPack.at(i)->time;
                if (t < lastTime)
                {
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
    // 开始和结尾的索引查找
    startIndex = InvData();
    endIndex = InvData();

    // 二分查找时间索引
    int lowPos = 0;
    int highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(_leftTime - _trendDataPack.at(midPos)->time);

        if (_leftTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (_leftTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            startIndex = midPos;
            break;
        }
    }

    lowPos = 0;
    highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(_rightTime - _trendDataPack.at(midPos)->time);

        if (_rightTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (_rightTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            endIndex = midPos;
            break;
        }
    }
}

void TrendWaveWidget::updateTimeRange()
{
    unsigned t;
    unsigned onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
//    int intervalNum = onePixelTime/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);
    if (_trendDataPack.length() == 0)
    {
        t = _initTime;
    }
    else
    {
        unsigned lastTime = _trendDataPack.last()->time;
        t = lastTime - onePixelTime * GRAPH_POINT_NUMBER * (_currentPage - 1);

//        int index = _trendDataPack.length() - 1 - intervalNum * GRAPH_POINT_NUMBER * (_currentPage - 1);
//        t = _trendDataPack.at(index)->time;
    }
    _rightTime = t;
    _leftTime = t - onePixelTime * GRAPH_POINT_NUMBER;
    _trendLayout();
}

void TrendWaveWidget::paintEvent(QPaintEvent *event)
{
    IWidget::paintEvent(event);

    QPainter barPainter(this);

    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    QRect rectAdjust =rect().adjusted(0,30,0,0);
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
        barPainter.drawText(rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth - 30,
                            rectAdjust.topLeft().y() - 5, tStr);
        t = t - onePixelTime * GRAPH_POINT_NUMBER/GRAPH_DISPLAY_DATA_NUMBER;
        barPainter.drawLine(rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y(), rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y() + 5);
    }

    // 游标线
    barPainter.drawLine(cursorPos, rectAdjust.topLeft().y(),
                        cursorPos, rectAdjust.bottomLeft().y());

    // 当前趋势记录的时间
    if (_cursorPosIndex < _trendGraphInfo.alarmInfo.count())
    {
        timeDate.getDate(_trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp, tStr);
        barPainter.drawText(_waveRegionWidth + 5, rectAdjust.topLeft().y() - 5, tStr);
        timeDate.getTime(_trendGraphInfo.alarmInfo.at(_cursorPosIndex).timestamp, tStr, true);
        barPainter.drawText(_waveRegionWidth + 120, rectAdjust.topLeft().y() - 5, tStr);
    }

    barPainter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    barPainter.drawLine(_waveRegionWidth, 0, _waveRegionWidth, height());

    // 报警事件的标志
    barPainter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
    for (int i = 0; i < _trendGraphInfo.alarmInfo.count(); i ++)
    {
        if (_trendGraphInfo.alarmInfo.at(i).isAlarmEvent)
        {
            double pos = _getCursorPos(_trendGraphInfo.alarmInfo.at(i).timestamp);
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
}

void TrendWaveWidget::_trendLayout()
{
    _infosList.clear();
    int subWidgetHeight = (height() - 30)/_displayGraphNum;
    TrendSubWidgetInfo info;
    info.xHead = (_waveRegionWidth -GRAPH_DATA_WIDTH)/2;
    info.xTail = (_waveRegionWidth + GRAPH_DATA_WIDTH)/2;
    info.yTop = subWidgetHeight/5;
    info.yBottom = subWidgetHeight/5*4;
    _totalGraphNum = 0;
    int startIndex;
    int endIndex;
    dataIndex(startIndex, endIndex);
    QMap<SubParamID, TrendSubWaveWidget *>::iterator it = _subWidgetMap.begin();
    for (; it != _subWidgetMap.end(); it ++)
    {
        SubParamID subId = it.key();
        if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_RESP)
        {
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_RR_BR:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:                
                loadTrendData(subId, startIndex, endIndex);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }
        else if(trendGraphSetWidget.getTrendGroup() == TREND_GROUP_IBP)
        {
            SubParamID ibp1;
            SubParamID ibp2;
            ibpParam.getSubParamID(ibp1, ibp2);
            if (subId != ibp1 && subId != ibp2 &&
                    subId != SUB_PARAM_HR_PR && subId != SUB_PARAM_SPO2 &&
                    subId != SUB_PARAM_NIBP_MAP && subId != SUB_PARAM_T1)
            {
                it.value()->setVisible(false);
                continue;
            }
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_CVP_MAP:
            case SUB_PARAM_LAP_MAP:
            case SUB_PARAM_RAP_MAP:
            case SUB_PARAM_ICP_MAP:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_T1:
            case SUB_PARAM_ART_SYS:
            case SUB_PARAM_PA_SYS:
            case SUB_PARAM_AUXP1_SYS:
            case SUB_PARAM_AUXP2_SYS:
                loadTrendData(subId, startIndex, endIndex);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }
        else if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_AG)
        {
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
            case SUB_PARAM_T1:
                loadTrendData(subId, startIndex, endIndex);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }

        it.value()->setFixedHeight(subWidgetHeight);
        it.value()->loadTrendSubWidgetInfo(info);
        it.value()->setTimeRange(_leftTime, _rightTime);
        it.value()->setVisible(true);
        _trendGraphInfo.unit = paramInfo.getUnitOfSubParam(subId);
        it.value()->getValueLimit(_trendGraphInfo.scale.max, _trendGraphInfo.scale.min);
        _infosList.append(_trendGraphInfo);
        _totalGraphNum ++;
    }

}

void TrendWaveWidget::_getTrendData()
{
    IStorageBackend *backend;
    backend = trendDataStorageManager.backend();
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    TrendDataPackage *pack;
    qDeleteAll(_trendDataPack);
    _trendDataPack.clear();
    _alarmTimeList.clear();
    //TODO: low efficiency
    for (int i = 0; i < blockNum; i ++)
    {
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = (TrendDataSegment*)data.data();
        pack->time = dataSeg->timestamp;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->subparamValue[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].value;
            pack->subparamAlarm[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].alarmFlag;
            if (!pack->alarmFlag && dataSeg->values[j].alarmFlag)
            {
                pack->alarmFlag = dataSeg->values[j].alarmFlag;
            }
        }
        if (pack->alarmFlag)
        {
            _alarmTimeList.append(pack->time);
        }
        _trendDataPack.append(pack);
    }
    _calculationPage();
}

void TrendWaveWidget::_initWaveSubWidget()
{
    TrendGraphType type;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        switch (i)
        {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_SPO2:
        case SUB_PARAM_RR_BR:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
            type = TREND_GRAPH_TYPE_NORMAL;
            break;
        case SUB_PARAM_NIBP_SYS:
            type = TREND_GRAPH_TYPE_NIBP;
            break;
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
        case SUB_PARAM_T1:
            type = TREND_GRAPH_TYPE_AG_TEMP;
            break;
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
            type = TREND_GRAPH_TYPE_ART_IBP;
            break;
        default:
            continue;
        }

        TrendSubWaveWidget *subWidget = new TrendSubWaveWidget((SubParamID)i, type);
        subWidget->setVisible(true);
        subWidget->setParent(this);
        subWidget->setThemeColor(colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID((SubParamID)i))));
        subWidget->setFocusPolicy(Qt::NoFocus);
        _subWidgetMap.insert((SubParamID)i, subWidget);
        _hLayoutTrend->addWidget(subWidget);
    }
}

double TrendWaveWidget::_getCursorPos(unsigned t)
{
    double dpos = 0;
    dpos = (t - _leftTime) * GRAPH_DATA_WIDTH / (_rightTime - _leftTime) + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2;

    return dpos;
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
