/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#include <QResizeEvent>
#include <QPainter>
#include "ECGParam.h"
#include "ECGWaveWidget.h"
#include "WaveWidgetLabel.h"
#include "ECGWaveRuler.h"
#include "FontManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ConfigManager.h"
#include "SystemManager.h"
#include "Framework/UI/PopupList.h"
#include "ECGWaveRuler.h"
#include "LayoutManager.h"
#include "ECGDupParam.h"

int ECGWaveWidget::_paceHeight = 5;
/**************************************************************************************************
 * 自动增益计算。
 * 参数:
 *      data： 波形数据。
 *************************************************************************************************/
void ECGWaveWidget::_autoGainHandle(int data)
{
    _autoGainTracePeek = (_autoGainTracePeek < data) ? data : _autoGainTracePeek;
    _autoGainTraveVally = (_autoGainTraveVally > data) ? data : _autoGainTraveVally;

    unsigned t = timeDate->time();
    if (_autoGainTime == 0)
    {
        _autoGainTime = t;
        return;
    }

    if (abs(timeDate->difftime(_autoGainTime, t)) < 6)
    {
        return;
    }

    _autoGainTime = t;

    // 开始寻找最合适的增益。
    int gain = ECG_GAIN_X10;
    for (int i = _gainInfo.count() - 1; i >= 0; --i)
    {
        if (_gainInfo.at(i) == ECG_GAIN_AUTO)
        {
            continue;
        }
        gain = _gainInfo.at(i);
        if ((_autoGainTraveVally >= _autoGainLogicalRange[gain].minRange) &&
                (_autoGainTracePeek <= _autoGainLogicalRange[gain].maxRange))
        {
            break;
        }
    }

    _autoGainTracePeek = -10000000;
    _autoGainTraveVally = 10000000;

    if (gain != static_cast<int>(ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID()))))
    {
        ecgParam.setGain(static_cast<ECGGain>(gain), getID());
    }
}

/**************************************************************************************************
 * 计算每个增益条件下的逻辑范围。
 *************************************************************************************************/
void ECGWaveWidget::_calcGainRange(void)
{
    for (int k = 0; k < _gainInfo.count(); ++k)
    {
        ECGGain gain = _gainInfo.at(k);
        if (gain == ECG_GAIN_AUTO)
        {
            continue;
        }

        int rulerHeight = _calcRulerHeight(gain);

        int rulerTop = qmargins().top()
                       + ((height() - qmargins().top() - qmargins().bottom() - rulerHeight) / 2);
        int rulerBottom = rulerTop + rulerHeight - 1;

        int valueMax = (_p05mV - _n05mV) * (qmargins().top() -
                                            rulerBottom) / (rulerTop - rulerBottom) + _n05mV;

        int valueMin = (_p05mV - _n05mV) * (height() - 1 - qmargins().bottom() -
                                            rulerBottom) / (rulerTop - rulerBottom) + _n05mV;

        _autoGainLogicalRange[gain].minRange = valueMin;
        _autoGainLogicalRange[gain].maxRange = valueMax;
    }
}

void ECGWaveWidget::_adjustLabelLayout()
{
    _name->move(8, 0);

    _gain->move(108, 0);

    _filterMode->move(208, 0);

    _notchInfo->move(358, 0);

    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);
}

/**************************************************************************************************
 * 计算出给定增益对应的实际像素高度
 * 参数:
 *      gain: 波形增益。
 *************************************************************************************************/
double ECGWaveWidget::_calcRulerHeight(ECGGain gain)
{
    double rulerHeight = 0.0;
    switch (gain)
    {
    case ECG_GAIN_X0125:
        rulerHeight = 1.25 / pixelHPitch();
        break;

    case ECG_GAIN_X025:
        rulerHeight = 2.5 / pixelHPitch();
        break;

    case ECG_GAIN_X05:
        rulerHeight = 5 / pixelHPitch();
        break;

    case ECG_GAIN_X10:
        rulerHeight = 10 / pixelHPitch();
        break;

    case ECG_GAIN_X20:
        rulerHeight = 20 / pixelHPitch();
        break;

    case ECG_GAIN_X40:
        rulerHeight = 40 / pixelHPitch();
        break;

    default:
        break;
    }

    return rulerHeight;
}

/**************************************************************************************************
 * 根据校准信息初始化波形数值范围。
 *************************************************************************************************/
void ECGWaveWidget::_initValueRange(ECGGain gain)
{
    if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        gain = get12LGain();
    }

#if 0
    double rulerHeight = _calcRulerHeight(gain);
    double rulerTop = qmargins().top()
                      + ((height() - qmargins().top() - qmargins().bottom() - rulerHeight) / 2);
    double rulerBottom = rulerTop + rulerHeight - 1;

    int valueMax = (_p05mV - _n05mV) * (qmargins().top() - rulerBottom)
                   / (rulerTop - rulerBottom) + _n05mV;

    int valueMin = (_p05mV - _n05mV) * (height() - 1 - qmargins().bottom() - rulerBottom)
                   / (rulerTop - rulerBottom) + _n05mV;
#else
    double rulerHeightPixelPerCM = _calcRulerHeight(gain);
    float baseline = height() / 2;
    double aboveBaselineCM = (baseline - qmargins().top()) / rulerHeightPixelPerCM;
    double belowBaselineCM = (height() - qmargins().bottom() - baseline) / rulerHeightPixelPerCM;
    int waveValueFor1CM = _p05mV - _n05mV;
    /* NOTE: assume wave baseline is 0, should be identical to ecg provider baseline value */
    int valueMax = static_cast<int>(waveValueFor1CM * aboveBaselineCM);
    int valueMin = static_cast<int>(-waveValueFor1CM * belowBaselineCM);
#endif

    setValueRange(valueMin, valueMax);
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void ECGWaveWidget::_popupDestroyed(void)
{
    if (_currentItemIndex == -1)
    {
        return;
    }

    if (_currentItemIndex == ECG_GAIN_AUTO)
    {
        _isAutoGain = true;
    }
    else
    {
        _isAutoGain = false;
    }

    if (layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN)
    {
        for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; i++)
        {
            ecgParam.set12LGain((ECGGain)_currentItemIndex, (ECGLead)i);
        }
    }
    else
    {
        ecgParam.setAutoGain(ecgParam.waveIDToLeadID((WaveformID)getID()), _isAutoGain);

        if (!_isAutoGain)
        {
            // 修改增益后清空R波重绘标记记录
            rMarkList.clear();

            ecgParam.setGain((ECGGain)_currentItemIndex, getID());
        }
        else
        {
            setGain(ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID())));
        }
    }
    _autoGainTime = 0;
    _autoGainTracePeek = -100000000;
    _autoGainTraveVally = 100000000;
    _ruler->setGain(static_cast<ECGGain>(_currentItemIndex));
}


void ECGWaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

void ECGWaveWidget::_updateFilterMode()
{
    if (!this->isVisible())
    {
        return;
    }
    QList<int> waveIdList = layoutManager.getDisplayedWaveformIDs();
    if (!waveIdList.count())
    {
        return;
    }
    if (waveIdList.at(0) != getID())
    {
        return;
    }
    ECGFilterMode  filterMode = ecgParam.getFilterMode();
    _filterMode->setText(trs(ECGSymbol::convert(filterMode)));
    // 2021-04-16DAVID提出要求，ECG监护和手术模式下，ECG波形上可以显示陷波滤波器的信息
//    if (filterMode != ECG_FILTERMODE_DIAGNOSTIC)
//    {
//        _notchInfo->setVisible(false);
//    }
//    else
//    {
//        _notchInfo->setVisible(true);
//    }
}

void ECGWaveWidget::_updateNotchInfo()
{
    if (!this->isVisible())
    {
        return;
    }
    QList<int> waveIdList = layoutManager.getDisplayedWaveformIDs();
    if (!waveIdList.count())
    {
        return;
    }
    if (waveIdList.at(0) != getID())
    {
        return;
    }
    ECGNotchFilter notchFilter = ecgParam.getNotchFilter();
    _notchInfo->setText(QString("%1 %2")
                               .arg(trs("Notch"))
                               .arg(trs(ECGSymbol::convert(notchFilter))));
    _adjustLabelLayout();
}

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void ECGWaveWidget::loadConfig(void)
{
    setSpeed(ecgParam.getSweepSpeed());

    bool is12Lead = ecgParam.getLeadMode() < ECG_LEAD_MODE_12 ? 0 : 1;
    if (layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN)
    {
        _name->setFocusPolicy(Qt::NoFocus);

        _isAutoGain = false;

        // 增益。
        set12LGain(ECG_GAIN_X10);
        ecgParam.setGain(ECG_GAIN_X10);

        _name->setText(ECGSymbol::convert(ecgParam.waveIDToLeadID((WaveformID)getID()),
                                          ecgParam.getLeadConvention(), is12Lead, ecgParam.get12LDisplayFormat()));

        ECGLead lead = ecgParam.waveIDToLeadID((WaveformID)getID());
        // notify
        ecgParam.updateECGNotifyMesg(lead, false);
    }
    else
    {
        _name->setText(ECGSymbol::convert(ecgParam.waveIDToLeadID((WaveformID)getID()),
                                          ecgParam.getLeadConvention(), is12Lead, false));
        _name->setFocusPolicy(Qt::StrongFocus);

        // 增益。
        ECGLead lead = ecgParam.waveIDToLeadID((WaveformID)getID());
        ECGGain gain = ecgParam.getGain(lead);

        _isAutoGain = ecgParam.getAutoGain(lead);

        setGain(gain);

        // notify
        ecgParam.updateECGNotifyMesg(lead, false);
    }
    _adjustLabelLayout();
}

/**************************************************************************************************
 * 获取当前控件的高度，毫秒为单位。
 *************************************************************************************************/
int ECGWaveWidget::getHeightMM(void)
{
    return (height() - 1) * pixelHPitch();
}

/**************************************************************************************************
 * 设置+/-0.5mv对应的数值。
 *
 * 参数:
 *      p05mv: 正0.5mv对应的数值。
 *      n05mv: 负0.5mv对应的数值。
 *************************************************************************************************/
void ECGWaveWidget::set05MV(int p05mv, int n05mv)
{
    _p05mV = p05mv;
    _n05mV = n05mv;
}

/**************************************************************************************************
 * 添加ECG波形数据
 *
 * 参数:
 *      waveData: 波形数据
 *      pace: 起搏开关标志
 *************************************************************************************************/
void ECGWaveWidget::addWaveformData(int waveData, int pace)
{
    addData(waveData, pace);

    if (_isAutoGain && (!(pace & (1 << 14))))
    {
        _autoGainHandle(waveData);
    }
}

/**************************************************************************************************
 * 设置波形增益
 *
 * 参数:
 *      gain: 波形增益
 *************************************************************************************************/
void ECGWaveWidget::setGain(ECGGain gain, bool isAuto)
{
    _initValueRange(gain);
    // 自动增益标志
    if (gain == ECG_GAIN_AUTO)
    {
        _isAutoGain = true;
    }
    // 不是自动增益导致的增益设置
    else if (!isAuto)
    {
        _isAutoGain = false;
    }

    // 不是自动增益导致的增益设置
    if (!isAuto)
    {
        _gain->setText(trs(ECGSymbol::convert(gain)));
    }

    _ruler->setGain(gain);
}

/**************************************************************************************************
 * 设置12导界面下波形增益
 *
 * 参数:
 *      gain: 波形增益
 *************************************************************************************************/
void ECGWaveWidget::set12LGain(ECGGain gain)
{
    _12LGain = gain;
    return;
}

/**************************************************************************************************
 * 获取12导界面下波形增益
 *
 * 参数:
 *      gain: 波形增益
 *************************************************************************************************/
ECGGain ECGWaveWidget::get12LGain()
{
    return _12LGain;
}

/**************************************************************************************************
 * 设置波形速度
 *************************************************************************************************/
void ECGWaveWidget::setSpeed(ECGSweepSpeed speed)
{
    // 波形速度。
    switch (speed)
    {
    case ECG_SWEEP_SPEED_625:
        setWaveSpeed(6.25);
        break;

    case ECG_SWEEP_SPEED_125:
        setWaveSpeed(12.5);
        break;

    case ECG_SWEEP_SPEED_250:
        setWaveSpeed(25.0);
        break;

    case ECG_SWEEP_SPEED_500:
        setWaveSpeed(50);
        break;

    default:
        setWaveSpeed(25.0);
        break;
    }

    layoutManager.resetWave();
}

/**************************************************************************************************
 * 设置自动增益标志
 *************************************************************************************************/
void ECGWaveWidget::setAutoGain(bool flag)
{
    _isAutoGain = flag;
    _autoGainTracePeek = -100000000;
    _autoGainTraveVally = 100000000;
    _autoGainTime = 0;
}

/***************************************************************************************************
 * update the lead display name
 **************************************************************************************************/
void ECGWaveWidget::updateLeadDisplayName(const QString &name)
{
    _name->setText(name);
    setTitle(name);
    update();
}

void ECGWaveWidget::updateWaveConfig()
{
    loadConfig();
}

/**************************************************************************************************
 * set wave notify message
 *************************************************************************************************/
void ECGWaveWidget::setNotifyMesg(ECGWaveNotify mesg)
{
    // demo模式notify下强制更新为正常标志
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        mesg = ECG_WAVE_NOTIFY_NORMAL;
    }

    if (_mesg == mesg)
    {
        return;
    }

    _mesg = mesg;
    switch (_mesg)
    {
    case ECG_WAVE_NOTIFY_LEAD_OFF:
        _notify->setText(trs("LeadOff"));
        break;

    case ECG_WAVE_NOTIFY_CHECK_PATIENT:
    {
        _notify->setText(trs("CheckPatient"));
        break;
    }

    default:
        _notify->setText("");
        break;
    }
    _adjustLabelLayout();
}

/**************************************************************************************************
 * 重绘事件
 *
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void ECGWaveWidget::paintEvent(QPaintEvent *e)
{
    WaveWidget::paintEvent(e);

    if (bufIsEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setFont(fontManager.textFont(14));
    painter.setPen(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, false);
    QMargins margin = qmargins();

    // 记录R波标记,用于重画
    QList<rMark_record> temprMarkList = rMarkList;
    rMarkList.clear();

    QVector<QRect> rects = e->region().rects();
    foreach(QRect rect, rects)
    {
        int leftIndex = qMax(0, xToIndex(rect.left() - 2) - 1);
        int rightIndex = qMin(bufSize() - 1, xToIndex(rect.right() + 2) + 1);
        int beginIndex;
        int endIndex;
        int fontH = fontManager.textHeightInPixels(painter.font());
        if (bufHead() > bufTail())
        {
            // 缓冲连续情形
            beginIndex = qMax(leftIndex, bufTail());
            endIndex = qMin(rightIndex, bufHead() - 1);
            for (int i = beginIndex; i <= endIndex; i++)
            {
                // if (flagBuf(i) & ECG_INTERNAL_FLAG_BIT)//ipace
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))  // ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(&painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)  // epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(&painter, start, end, r, flagBuf(i), this);
                }

//                if (flagBuf(i) & ECG_RWAVE_MARK_FLAG_BIT)//R mark
//                {
//                    QPoint rMarkPoint = getWavePoint(i);
//                    QRect r = this->rect();
//                    drawRMark(painter, rMarkPoint, r, this);
//                }
            }
        }
        else
        {
            // 缓冲不连续情形
            beginIndex = qMax(leftIndex, bufTail());
            endIndex = qMin(rightIndex, bufSize() - 1);
            for (int i = beginIndex; i <= endIndex; i++)
            {
                // if (flagBuf(i) & ECG_INTERNAL_FLAG_BIT)//ipace
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))  // ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(&painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)  // epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(&painter, start, end, r, flagBuf(i), this);
                }

//                if (flagBuf(i) & ECG_RWAVE_MARK_FLAG_BIT)//R mark
//                {
//                    QPoint rMarkPoint = getWavePoint(i);
//                    QRect r = this->rect();
//                    drawRMark(painter, rMarkPoint, r, this);
//                }
            }

            beginIndex = qMax(leftIndex, 0);
            endIndex = qMin(rightIndex, bufHead() - 1);
            for (int i = beginIndex; i <= endIndex; i++)
            {
                // if (flagBuf(i) & ECG_INTERNAL_FLAG_BIT)//ipace
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))  // ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(&painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)  // epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(&painter, start, end, r, flagBuf(i), this);
                }

//                if (flagBuf(i) & ECG_RWAVE_MARK_FLAG_BIT)//R mark
//                {
//                    QPoint rMarkPoint = getWavePoint(i);
//                    QRect r = this->rect();
//                    drawRMark(painter, rMarkPoint, r, this);
//                }
            }
        }
    }

    // 记录R波标记,用于重画，防止R波标记被刷新后只显示一半
    QList<rMark_record>::iterator it;
    for (it = temprMarkList.begin(); it != temprMarkList.end(); ++it)
    {
        QPen oldpen = painter.pen();
        painter.setBrush(Qt::white);

        painter.drawText((*it).s_x, (*it).s_y, "S");

        QPoint ellipsePoint((*it).ellipse_x, (*it).ellipse_y);
        painter.drawEllipse(ellipsePoint, 3, 3);

        painter.setPen(oldpen);
    }
    temprMarkList.clear();
}

/**************************************************************************************************
 * 响应窗体大小调整事件
 *
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void ECGWaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    if (!_name || !_notify)
    {
        return;
    }

    _initValueRange(ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID())));
    _calcGainRange();
    _adjustLabelLayout();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 焦点事件
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void ECGWaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
    _name->setFocus();
}

/**************************************************************************************************
 * 响应窗体大小调整事件
 *
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void ECGWaveWidget::showEvent(QShowEvent *e)
{
    rMarkList.clear();
    WaveWidget::showEvent(e);

    loadConfig();

    if (ECG_DISPLAY_NORMAL == ecgParam.getDisplayMode())
    {
        setMargin(QMargins(WAVE_X_OFFSET, 2, 2, 2));
    }
    else if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
    {
        QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();


        if ((!currentWaveforms.empty()) && (currentWaveforms[0] == name() ||
                                            currentWaveforms[1] == name()))
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, 2, 2, 2));
        }
        else
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, 2, 2, 2));
        }
    }

    // 波形每次显示时需要调用一次
    QList<int> waveIdList = layoutManager.getDisplayedWaveformIDs();
    if (!waveIdList.count())
    {
        return;
    }
    bool isVisible;
    if (waveIdList.at(0) != getID())
    {
        isVisible = false;
    }
    else
    {
        isVisible = true;
    }
    setWaveInfoVisible(isVisible);
}

/**************************************************************************************************
 * hide event
 *************************************************************************************************/
void ECGWaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);
}

/**************************************************************************************************
 * 构造函数
 *
 * 参数:
 *      widgetName: 控件名称
 *      leadName: 导联名称，波形标名
 *************************************************************************************************/
ECGWaveWidget::ECGWaveWidget(WaveformID id, const QString &widgetName, const QString &leadName)
    : WaveWidget(widgetName, leadName)
    , _notify(NULL)
    , _gain(NULL)
    , _filterMode(NULL)
    , _notchInfo(NULL)
    , _ruler(NULL)
    , _p05mV(3185)
    , _n05mV(-3185)
    , _currentItemIndex(-1)
{
    _autoGainTracePeek = -100000000;
    _autoGainTraveVally = 100000000;
    _autoGainTime = 0;
    _isAutoGain = false;
    // Get the gain information supported by the system
    _gainInfo = ecgParam.getSupportGainInfo();
    _mesg = ECG_WAVE_NOTIFY_LEAD_OFF;
    rMarkList.clear();

    setFocusPolicy(Qt::NoFocus);
    setID(id);

    // 设置前景色。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedHeight(fontH);
    _name->setText(leadName);

    // ecg第一道波形显示增益、滤波模式、陷波信息
    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(fontSize));
    _gain->setFixedHeight(fontH);
    _gain->setFocusPolicy(Qt::NoFocus);
    addItem(_gain);
    ECGGain gain = ecgParam.getGain(ecgParam.waveIDToLeadID(id));
    _gain->setText(trs(ECGSymbol::convert(gain)));

    _filterMode = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _filterMode->setFont(fontManager.textFont(fontSize));
    _filterMode->setFixedHeight(fontH);
    _filterMode->setFocusPolicy(Qt::NoFocus);
    addItem(_filterMode);
    connect(&ecgParam, SIGNAL(updateFilterMode()), this, SLOT(_updateFilterMode()));

    _notchInfo = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _notchInfo->setFont(fontManager.textFont(fontSize));
    _notchInfo->setFixedHeight(fontH);
    _notchInfo->setFocusPolicy(Qt::NoFocus);
    addItem(_notchInfo);
    connect(&ecgParam, SIGNAL(updateNotchFilter()), this, SLOT(_updateNotchInfo()));

    _updateNotchInfo();
    _updateFilterMode();

    _notify = new WaveWidgetLabel(trs("LeadOff"), Qt::AlignLeft | Qt::AlignVCenter, this);
    _notify->setFocusPolicy(Qt::NoFocus);
    _notify->setFont(fontManager.textFont(fontSize));
    _notify->setFixedHeight(fontH);
    addItem(_notify);

    _ruler = new ECGWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontSize));
    addItem(_ruler);

    _12LGain = ECG_GAIN_X10;
    _adjustLabelLayout();
}

/**************************************************************************************************
 * 析构函数
 *************************************************************************************************/
ECGWaveWidget::~ECGWaveWidget()
{
}

/**************************************************************************************************
 * 画内部起搏标记
 *************************************************************************************************/
void ECGWaveWidget::drawIPaceMark(QPainter *painter, const QPoint &start, const QPoint &end)
{
    QPen oldPen = painter->pen();

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setDashPattern(darsh);
    painter->setPen(pen);
    painter->drawLine(start, end);

    painter->setPen(oldPen);
}

/**************************************************************************************************
 * 画外部起搏标记
 *************************************************************************************************/
void ECGWaveWidget::drawEPaceMark(QPainter *painter, const QPoint &start, const QPoint &end,
                                  const QRect &r, unsigned flag, ECGWaveWidget *pObj)
{
    QPen oldPen = painter->pen();
    int pStrWidth = fontManager.textWidthInPixels("P", painter->font()) / 2;

    QPen pen(QColor(42, 207, 198));
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    if (flag & ECG_EXTERNAL_SOLD_FLAG_BIT)
    {
        painter->drawLine(start, end);
    }
    else
    {
        QVector<qreal> darsh;
        darsh << 5 << 5;
        QPen dotPen = pen;
        dotPen.setDashPattern(darsh);
        painter->setPen(dotPen);
        QLineF dotLine(start, end);
        painter->drawLine(dotLine);
    }

    painter->setPen(pen);
    int startX = start.x() - pStrWidth;
    int startY = r.top() + r.height() - 10;
    if (NULL != pObj)
    {
        startY = r.height() - 10;
    }

    if (r.width() < pStrWidth * 2)
    {
        startX = r.left();
    }
    else
    {
        if (startX < r.left())
        {
            startX = r.left();
        }
        else if (start.x() + pStrWidth > r.right())
        {
            startX = r.right() - 2 * pStrWidth;
        }
    }

    painter->drawText(startX, startY, "P");
    painter->setPen(oldPen);
}

/**************************************************************************************************
 * 画R波标记
 *************************************************************************************************/
void ECGWaveWidget::drawRMark(QPainter *painter, const QPoint &p, const QRect &r, ECGWaveWidget *pObj)
{
    QPen oldPen = painter->pen();
    int fontH = fontManager.textHeightInPixels(painter->font());
    int sWidth = fontManager.textWidthInPixels("S", painter->font());
    int srH = fontH - painter->fontMetrics().descent();

    painter->setPen(Qt::white);

    QPointF center(p);
    // 圆点的半径为3
    if (p.x() - r.x() < 3)
    {
        center.setX(r.x() + 3);
    }

    if (p.x() + 3 > r.right())
    {
        center.setX(r.width() - 3);
    }

    painter->setBrush(Qt::white);
    painter->drawEllipse(center, (qreal)3, (qreal)3);

    int startY = r.y() + 2 * fontH + 5;
    int startX = center.x() - sWidth / 2;
    if (NULL != pObj)
    {
        if (startX < 0)
        {
            startX = r.x();
        }

        if (center.y() + 3 > startY - srH && startY - srH > center.y() - 3)
        {
            startY = center.y() + 3 + 1 + fontH;
        }
        else if (center.y() - 3 < startY && center.y() + 3 > startY)
        {
            startY = center.y() - 3 - 1;
        }
        else if (center.y() - 3 >= startY - srH && center.y() + 3 <= startY)
        {
            if (r.y() + fontH < center.y() - 3)
            {
                startY = center.y() - 3 - 1;
            }
            else
            {
                startY = center.y() + 3 + 1 + fontH;
            }
        }

        // 记录R波标记,用于重画
        if (0 == pObj->rMarkList.count())
        {
            rMark_record rec;
            rec.ellipse_x = center.x();
            rec.ellipse_y = center.y();
            rec.s_x = startX;
            rec.s_y = startY;
            pObj->rMarkList.append(rec);
        }
        else
        {
            QList<rMark_record>::const_iterator it = pObj->rMarkList.begin();
            for (; it != pObj->rMarkList.end(); ++it)
            {
                if ((*it).ellipse_x == center.x() && (*it).ellipse_y == center.y() &&
                        (*it).s_x ==  startX && (*it).s_y == startY)
                {
                    break;
                }
            }

            if (it == pObj->rMarkList.end())
            {
                rMark_record rec;
                rec.ellipse_x = center.x();
                rec.ellipse_y = center.y();
                rec.s_x = startX;
                rec.s_y = startY;
                pObj->rMarkList.append(rec);
            }
        }
    }
    else
    {
        if (startX < r.left())
        {
            startX = center.x() + 3;
        }
        else if (startX + sWidth > r.right())
        {
            startX = center.x() - 3 - sWidth;
        }

        if (!((center.y() + 3 < startY - srH) || center.y() - 3 > startY))
        {
            if (center.x() + 3 + sWidth > r.right())
            {
                startX = center.x() - 3 - sWidth;
            }
            else
            {
                startX = center.x() + 3;
            }
        }
    }

    painter->drawText(startX, startY, "S");

    painter->setPen(oldPen);
}

void ECGWaveWidget::updatePalette(const QPalette &pal)
{
    _ruler->setPalette(pal);
    setPalette(pal);
    updateBackground();
}

void ECGWaveWidget::setWaveInfoVisible(bool isVisible)
{
    _gain->setVisible(isVisible);
    _filterMode->setVisible(isVisible);
    _notchInfo->setVisible(isVisible);
    if (isVisible)
    {
        _updateNotchInfo();
        _updateFilterMode();
    }
}

void ECGWaveWidget::updateWidgetConfig()
{
    int index = ECG_DISPLAY_NORMAL;
    currentConfig.getNumValue("ECG|DisplayMode", index);
    ECGDisplayMode mode = static_cast<ECGDisplayMode>(index);
    if (ECG_DISPLAY_NORMAL == mode)
    {
        setMargin(QMargins(WAVE_X_OFFSET, 2, 2, 2));
    }
    else if (ECG_DISPLAY_12_LEAD_FULL == mode)
    {
        QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();

        if ((!currentWaveforms.empty()) && (currentWaveforms[0] == name() ||
                                            currentWaveforms[1] == name()))
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, 2, 2, 2));
        }
        else
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, 2, 2, 2));
        }
    }
    // 设置前景色。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);
    _ruler->setPalette(palette);

    // update the lead mode
    int leadMode = ECG_LEAD_MODE_5;
    currentConfig.getNumValue("ECG|LeadMode", leadMode);
    ecgParam.setLeadMode(static_cast<ECGLeadMode>(leadMode));

    // update the filter mode
    int filterMode = ECG_FILTERMODE_MONITOR;
    currentConfig.getNumValue("ECG|FilterMode", filterMode);
    ecgParam.setFilterMode(filterMode);

    ecgParam.updatePacermaker();    // 更新起博标志
    ecgDupParam.updateHRSource();   // 更新HR来源
    ecgParam.updateEditNotchFilter();  // 更新ECG工频陷波

    WaveWidget::updateWidgetConfig();
}

