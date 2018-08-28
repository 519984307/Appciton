/**
 ** This file is part of the nPM project.
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
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "WaveWidgetSelectMenu.h"
#include "TimeDate.h"
#include "WindowManager.h"
#include "SystemManager.h"
#include "PopupList.h"

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

    unsigned t = timeDate.time();
    if (_autoGainTime == 0)
    {
        _autoGainTime = t;
        return;
    }

    if (abs(timeDate.difftime(_autoGainTime, t)) < 6)
    {
        return;
    }

    _autoGainTime = t;

    // 开始寻找最合适的增益。
    int gain = ECG_GAIN_X40;
//    for (; gain >= ECG_GAIN_X05; gain--)
    for (; gain >= ECG_GAIN_X0125; gain--)
    {
        if ((_autoGainTraveVally >= _autoGainLogicalRange[gain].minRange) &&
                (_autoGainTracePeek <= _autoGainLogicalRange[gain].maxRange))
        {
            break;
        }
    }

    _autoGainTracePeek = -10000000;
    _autoGainTraveVally = 10000000;

    // gain为新的增益。
    if (gain < 0)
    {
        gain = ECG_GAIN_X0125;
//        gain = ECG_GAIN_X05;
        // return;
    }

    if (gain != static_cast<int>(ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID()))))
    {
        ecgParam.setGain((ECGGain)gain, getID());
    }
}

/**************************************************************************************************
 * 计算每个增益条件下的逻辑范围。
 *************************************************************************************************/
void ECGWaveWidget::_calcGainRange(void)
{
//    for (int i = ECG_GAIN_X05; i <= ECG_GAIN_X30; i++)
    for (int i = ECG_GAIN_X0125; i <= ECG_GAIN_X40; i++)
    {
        int rulerHeight = _calcRulerHeight((ECGGain)i);

        int rulerTop = qmargins().top()
                       + ((height() - qmargins().top() - qmargins().bottom() - rulerHeight) / 2);
        int rulerBottom = rulerTop + rulerHeight - 1;

        int valueMax = (_p05mV - _n05mV) * (qmargins().top() -
                                            rulerBottom) / (rulerTop - rulerBottom) + _n05mV;

        int valueMin = (_p05mV - _n05mV) * (height() - 1 - qmargins().bottom() -
                                            rulerBottom) / (rulerTop - rulerBottom) + _n05mV;

        _autoGainLogicalRange[i].minRange = valueMin;
        _autoGainLogicalRange[i].maxRange = valueMax;
    }
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
 * 根据波形增益初始化标尺。
 * 参数:
 *      gain: 波形增益。
 *************************************************************************************************/
// void ECGWaveWidget::_initRuler(ECGGain gain)
// {
//    if (!_ruler)
//    {
//        return;
//    }

//    _ruler->setRulerHeight(_calcRulerHeight(gain));
// }

/**************************************************************************************************
 * 根据校准信息初始化波形数值范围。
 *************************************************************************************************/
void ECGWaveWidget::_initValueRange(ECGGain gain)
{
    if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        gain = get12LGain();
    }

    double rulerHeight = _calcRulerHeight(gain);

    double rulerTop = qmargins().top()
                      + ((height() - qmargins().top() - qmargins().bottom() - rulerHeight) / 2);
    double rulerBottom = rulerTop + rulerHeight - 1;

    int valueMax = (_p05mV - _n05mV) * (qmargins().top() - rulerBottom)
                   / (rulerTop - rulerBottom) + _n05mV;

    int valueMin = (_p05mV - _n05mV) * (height() - 1 - qmargins().bottom() - rulerBottom)
                   / (rulerTop - rulerBottom) + _n05mV;

    setValueRange(valueMin, valueMax);
}

/**************************************************************************************************
 * 鼠标释放事件，弹出菜单。
 *************************************************************************************************/
void ECGWaveWidget::_releaseHandle(IWidget *iWidget)
{
    QWidget *p = static_cast<QWidget *>(parent());
    if (p == NULL)
    {
        return;
    }

    QRect prect = p->geometry();
    QRect r = geometry();

    if (ecgParam.getCalcLeadWaveformName() == name())
    {
        waveWidgetSelectMenu.setTopWaveform(true);
    }
    else
    {
        waveWidgetSelectMenu.setTopWaveform(false);
    }

    waveWidgetSelectMenu.setWaveformName(name());
    waveWidgetSelectMenu.setShowPoint(prect.x() + r.x() + 50, prect.y() + r.y());
    waveWidgetSelectMenu.autoShow();
}

/**************************************************************************************************
 * 增益改变。
 *************************************************************************************************/
void ECGWaveWidget::_ecgGain(IWidget *widget)
{
    WaveWidgetLabel *tempGain = NULL;

    if (NULL == _gainList)
    {
        ECGGain gain = ECG_GAIN_X10;
        if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
        {
            gain = get12LGain();

            //这里创建一个临时的增益，属性和监控界面一样，主要是为了弹出增益列表时能够和监控模式下的一致，
            //因为监控列表是根据父控件的属性来设置的
            tempGain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
            tempGain->setFont(fontManager.textFont(14));
            tempGain->setFixedHeight(30);
            tempGain->setFixedWidth(150);
            tempGain->move(_name->rect().width(), 0);

            widget = tempGain;
        }
        else
        {
            gain = ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID()));
        }

        int maxGain = ecgParam.getMaxGain();
        // 12导模式下增益不需要AUTO
        if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
        {
            maxGain = maxGain - 1;
        }
        if (maxGain > ECG_GAIN_AUTO)
        {
            if (_isAutoGain)
            {
                gain = ECG_GAIN_AUTO;
            }
        }
        _gainList = new PopupList(_gain, false);
        for (int i = 0; i < maxGain; i++)
        {
            _gainList->addItemText(ECGSymbol::convert((ECGGain)i));
        }

        int fontSize = fontManager.getFontSize(3);
        _gainList->setFont(fontManager.textFont(fontSize));
        connect(_gainList, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
        connect(_gainList, SIGNAL(selectItemChanged(int)), this , SLOT(_getItemIndex(int)));
    }

    _gainList->show();

    if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
    {
        if (NULL != tempGain)
        {
            delete tempGain;
            tempGain = NULL;
        }
    }
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void ECGWaveWidget::_popupDestroyed(void)
{
    if (_currentItemIndex == -1)
    {
        _gainList = NULL;
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

    if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
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

    _gainList = NULL;
    _autoGainTime = 0;
    _autoGainTracePeek = -100000000;
    _autoGainTraveVally = 100000000;
}

/***************************************************************************************************
 * handle calc lead changed signal
 **************************************************************************************************/
void ECGWaveWidget::_onCalcLeadChanged()
{
    if (UFACE_MONITOR_12LEAD != windowManager.getUFaceType())
    {
        if (ecgParam.getCalcLead() == getID())
        {
            _filter->setVisible(true);
        }
        else
        {
            _filter->setVisible(false);
        }
    }
}

void ECGWaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void ECGWaveWidget::_loadConfig(void)
{
    setSpeed(ecgParam.getSweepSpeed());

    if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
    {
        _name->setFocusPolicy(Qt::NoFocus);
        _gain->setFocusPolicy(Qt::StrongFocus);
        _gain->setVisible(false);

        _isAutoGain = false;

        // 增益。
        set12LGain(ECG_GAIN_X10);
        setGain(_12LGain);

        _filter->setVisible(false);
        // filter.
        QString filter = ECGSymbol::convert(ecgParam.getBandwidth());
        _filter->setText(filter);

        _name->setText(ECGSymbol::convert(ecgParam.waveIDToLeadID((WaveformID)getID()),
                                          ecgParam.getLeadConvention(), true, ecgParam.get12LDisplayFormat()));

        _name->setFixedWidth(70);
        _gain->setFixedWidth(120);
        _notify->setVisible(false);
    }
    else
    {
        _name->setText(ECGSymbol::convert(ecgParam.waveIDToLeadID((WaveformID)getID()),
                                          ecgParam.getLeadConvention(), false, false));
        _name->setFocusPolicy(Qt::StrongFocus);
        _gain->setFocusPolicy(Qt::StrongFocus);
        _gain->setVisible(true);

        _filter->setVisible(false);

        // 增益。
        ECGLead lead = ecgParam.waveIDToLeadID((WaveformID)getID());
        ECGGain gain = ecgParam.getGain(lead);

        _isAutoGain = ecgParam.getAutoGain(lead);

        setGain(gain);

        // filter.
        QString filter;
        filter = ECGSymbol::convert(ecgParam.getBandwidth());
        _filter->setText(filter);

        // notify
        ecgParam.updateECGNotifyMesg(lead, false);

        _name->setFixedWidth(130);
        _gain->setFixedWidth(160);
        _notify->setVisible(true);
    }
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
void ECGWaveWidget::setGain(ECGGain gain)
{
    if (!_gain)
    {
        return;
    }

    _initValueRange(gain);
    QString text;
    if (_isAutoGain)
    {
        text = trs("Automatically");
        text += " ";
    }

    switch (gain)
    {
    case ECG_GAIN_X0125:
        text += "0.125 cm/mV";
        break;

    case ECG_GAIN_X025:
        text += "0.25 cm/mV";
        break;

    case ECG_GAIN_X05:
        text += "0.5 cm/mV";
        break;

    case ECG_GAIN_X10:
        text += "1.0 cm/mV";
        break;

    case ECG_GAIN_X20:
        text += "2.0 cm/mV";
        break;

    case ECG_GAIN_X40:
        text += "4.0 cm/mV";
        break;

    default:
        break;
    }

    _gain->setText(text);
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

    windowManager.resetWave();
}

/**************************************************************************************************
 * 设置波形速度
 *************************************************************************************************/
void ECGWaveWidget::setBandWidth(ECGBandwidth bandwidth)
{
    QString filter;
    QStringList waveName;
    windowManager.getDisplayedWaveform(waveName);

    if (waveName.isEmpty())
    {
        return;
    }

    filter = ECGSymbol::convert(bandwidth);
    _filter->setText(filter);
}

/**************************************************************************************************
 * 获取带宽字符串
 *************************************************************************************************/
QString ECGWaveWidget::getBandWidthStr()
{
    return _filter->text();
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
}

/**************************************************************************************************
 * set wave notify message
 *************************************************************************************************/
void ECGWaveWidget::setNotifyMesg(ECGWaveNotify mesg)
{
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

    if (ECG_DISPLAY_NORMAL == ecgParam.getDisplayMode())
    {
        if (getID() != ecgParam.getCalcLead())
        {
            return;
        }
    }
    else if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
    {
        QStringList currentWaveforms;
        windowManager.getCurrentWaveforms(currentWaveforms);
        if (!((ECG_PACE_ON == (ECGPaceMode)ecgParam.get12LPacermaker()) && (!currentWaveforms.empty())
                && (currentWaveforms[0] == name())))
        {
            return;
        }
    }

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
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))// ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)// epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(painter, start, end, r, flagBuf(i), this);
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
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))// ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))// epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)// epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(painter, start, end, r, flagBuf(i), this);
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
                if ((flagBuf(i) & ECG_INTERNAL_FLAG_BIT) && !(flagBuf(i) & INVALID_WAVE_FALG_BIT))// ipace
                {
                    QPoint start(xBuf(i), margin.top());
                    QPoint end(xBuf(i), this->rect().height() - 10);
                    drawIPaceMark(painter, start, end);
                }

//                if (flagBuf(i) & (ECG_EXTERNAL_DOT_FLAG_BIT|ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flagBuf(i) & ECG_EXTERNAL_SOLD_FLAG_BIT)// epace
                {
                    QPoint start = getWavePoint(i);
                    QPoint end(xBuf(i), this->rect().height() - 10 - fontH);
                    QRect r = this->rect();
                    drawEPaceMark(painter, start, end, r, flagBuf(i), this);
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

    if (!_name || !_gain || !_notify)
    {
        return;
    }

    _name->move(0, 0);
    _gain->move(_name->rect().width(), 0);
    _filter->move(_name->rect().x() + _name->rect().width() +
                  _gain->rect().x() + _gain->rect().width(), 0);

    _notify->setFixedWidth(width() / 2);
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);

    _initValueRange(ecgParam.getGain(ecgParam.waveIDToLeadID((WaveformID)getID())));
    _calcGainRange();

    if (NULL != _gainList)
    {
        _gainList->close();
    }
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

    _loadConfig();

    int fontH = fontManager.textHeightInPixels(fontManager.textFont(14)) + 4;
    if (ECG_DISPLAY_NORMAL == ecgParam.getDisplayMode())
    {
        // 设置带宽显示（为计算导联、PADS为计算导联、12导为计算导联但带宽不一致时都显示带宽）
        if (getID() == ecgParam.getCalcLead())
//                || (this->_filter->text() != ecgParam.getCalBandWidthStr()))
        {
            _filter->setVisible(true);
        }
        else
        {
            _filter->setVisible(false);
        }

        setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
    }
    else if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
    {
        QStringList currentWaveforms;
        windowManager.getCurrentWaveforms(currentWaveforms);

        if ((!currentWaveforms.empty()) && (currentWaveforms[0] == name()))
        {
            _filter->setVisible(true);
            _gain->setVisible(true);
        }
        else
        {
            _filter->setVisible(false);
            _gain->setVisible(false);
        }

        if ((!currentWaveforms.empty()) && (currentWaveforms[0] == name() ||
                                            currentWaveforms[1] == name()))
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, fontH, 2, 2));
        }
        else
        {
            setMargin(QMargins(WAVE_X_OFFSET + 4, 2, 2, 2));
        }
    }
}

/**************************************************************************************************
 * hide event
 *************************************************************************************************/
void ECGWaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
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
    , _gain(NULL)
    , _notify(NULL)
    , _gainList(NULL)
    , _p05mV(3185)
    , _n05mV(-3185)
    , _currentItemIndex(-1)
{
    _autoGainTracePeek = -100000000;
    _autoGainTraveVally = 100000000;
    _autoGainTime = 0;
    _isAutoGain = false;
    _mesg = ECG_WAVE_NOTIFY_NORMAL;
    rMarkList.clear();

    setFocusPolicy(Qt::NoFocus);
    setID(id);

    // 设置前景色。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(1);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
//    _name = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(70, fontH);
    _name->setText(leadName);
    connect(_name, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
//    addItem(_name);

    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(fontSize));
    _gain->setFixedSize(150, fontH);
    _gain->setText("");
    connect(_gain, SIGNAL(released(IWidget *)), this, SLOT(_ecgGain(IWidget *)));
    addItem(_gain);

    _filter = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _filter->setFont(fontManager.textFont(fontSize));
    _filter->setFixedSize(130, fontH);
    _filter->setText("");
    _filter->setFocusPolicy(Qt::NoFocus);
    addItem(_filter);

    _notify = new WaveWidgetLabel("", Qt::AlignCenter, this);
    _notify->setFocusPolicy(Qt::NoFocus);
    _notify->setFont(fontManager.textFont(fontSize));
    _notify->setFixedHeight(fontH);
    addItem(_notify);

    _12LGain = ECG_GAIN_X10;

    connect(&ecgParam, SIGNAL(calcLeadChanged()), this, SLOT(_onCalcLeadChanged()));
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
void ECGWaveWidget::drawIPaceMark(QPainter &painter, QPoint &start, QPoint &end)
{
    QPen oldPen = painter.pen();

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setDashPattern(darsh);
    painter.setPen(pen);
    painter.drawLine(start, end);

    painter.setPen(oldPen);
}

/**************************************************************************************************
 * 画外部起搏标记
 *************************************************************************************************/
void ECGWaveWidget::drawEPaceMark(QPainter &painter, QPoint &start, QPoint &end,
                                  QRect &r, unsigned flag, ECGWaveWidget *pObj)
{
    QPen oldPen = painter.pen();
    int pStrWidth = fontManager.textWidthInPixels("P", painter.font()) / 2;

    QPen pen(QColor(42, 207, 198));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    if (flag & ECG_EXTERNAL_SOLD_FLAG_BIT)
    {
        painter.drawLine(start, end);
    }
    else
    {
        QVector<qreal> darsh;
        darsh << 5 << 5;
        QPen dotPen = pen;
        dotPen.setDashPattern(darsh);
        painter.setPen(dotPen);
        QLineF dotLine(start, end);
        painter.drawLine(dotLine);
    }

    painter.setPen(pen);
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

    painter.drawText(startX, startY, "P");
    painter.setPen(oldPen);
}

/**************************************************************************************************
 * 画R波标记
 *************************************************************************************************/
void ECGWaveWidget::drawRMark(QPainter &painter, QPoint &p, QRect &r, ECGWaveWidget *pObj)
{
    QPen oldPen = painter.pen();
    int fontH = fontManager.textHeightInPixels(painter.font());
    int sWidth = fontManager.textWidthInPixels("S", painter.font());
    int srH = fontH - painter.fontMetrics().descent();

    painter.setPen(Qt::white);

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

    painter.setBrush(Qt::white);
    painter.drawEllipse(center, (qreal)3, (qreal)3);

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

    painter.drawText(startX, startY, "S");

    painter.setPen(oldPen);
}

