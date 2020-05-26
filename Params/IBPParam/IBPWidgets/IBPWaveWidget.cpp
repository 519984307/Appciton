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
#include "IBPWaveWidget.h"
#include "IBPParam.h"
#include "IBPWaveRuler.h"
#include "IBPDefine.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "Framework/UI/PopupList.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 添加波形数据。
 *************************************************************************************************/
void IBPWaveWidget::addWaveformData(short wave, int flag)
{
    addData(wave, flag);

    if (_isAutoRuler)
    {
        _autoRulerHandle(wave);
    }
}

/**************************************************************************************************
 * 设置导联状态信息。
 *************************************************************************************************/
void IBPWaveWidget::setLeadSta(int info)
{
    // demo模式staIBP1/2下强制更新为false
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        info = 0;
    }

    if (info)
    {
        _leadSta->setText(trs("LeadOff"));
    }
    else
    {
        _leadSta->setText("");
    }
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void IBPWaveWidget::setLimit(int low, int high)
{
    IBPScaleInfo scale;
    scale.low = low;
    scale.high = high;
    ibpParam.setScaleInfo(scale, _entitle);

    _lowLimit = low;
    _highLimit = high;
    _ruler->setRuler(high, (high + low) / 2, low);

    low *= 10;
    high *= 10;
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPWaveWidget::setEntitle(IBPLabel entitle)
{
    _name->setText(IBPSymbol::convert(entitle));
    QString zoomStr = QString::number(ibpParam.getIBPScale(entitle).low) + "-" +
                      QString::number(ibpParam.getIBPScale(entitle).high) + "mmHg";
    _entitle = entitle;
    setLimit(ibpParam.getIBPScale(getEntitle()).low, ibpParam.getIBPScale(getEntitle()).high);
}

/**************************************************************************************************
 * 获取标名。
 *************************************************************************************************/
IBPLabel IBPWaveWidget::getEntitle()
{
    return _entitle;
}

void IBPWaveWidget::setRulerLimit(IBPRulerLimit ruler)
{
    if (ruler == -1)
    {
        return;
    }

    if (ruler == IBP_RULER_LIMIT_AUTO)
    {
        _isAutoRuler = true;
    }
    else if (ruler == IBP_RULER_LIMIT_MANUAL)
    {
        _isAutoRuler = false;
    }
    else
    {
        _isAutoRuler = false;
        setLimit(ibpParam.ibpScaleList.at(ruler).low, ibpParam.ibpScaleList.at(ruler).high);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPWaveWidget::IBPWaveWidget(WaveformID id, const QString &waveName, const IBPChannel &ibpChn)
    : WaveWidget(waveName, IBPSymbol::convert(ibpParam.getEntitle(ibpChn))),
      _ibpChn(ibpChn),
      _entitle(ibpParam.getEntitle(ibpChn))
{
    _autoRulerTracePeek = -10000;
    _autoRulerTraveVally = 10000;
    _autoRulerTime = 0;
    _isAutoRuler = false;
    _lowLimit = 0;
    _highLimit = 300;

    setFocusPolicy(Qt::NoFocus);
    setID(id);
//    setValueRange(64, 192);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, fontH);
    _name->setText(getTitle());

    _ruler = new IBPWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontSize - 2));
    addItem(_ruler);

    _leadSta = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _leadSta->setFont(fontManager.textFont(fontSize));
    _leadSta->setFixedSize(120, fontH);
    _leadSta->setFocusPolicy(Qt::NoFocus);
    addItem(_leadSta);

    setMargin(QMargins(WAVE_X_OFFSET, 2, 2, 2));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPWaveWidget::~IBPWaveWidget()
{
}

void IBPWaveWidget::updatePalette(const QPalette &pal)
{
    _ruler->setPalette(pal);
    setPalette(pal);
    updateBackground();
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void IBPWaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);
    _name->move(0, 0);
    _leadSta->setFixedWidth(width() / 2);
    _leadSta->move((width() - _leadSta->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _leadSta->height() - 1);

    _ruler->resize(RULER_X_OFFSET, 0,
                   width() - RULER_X_OFFSET,
                   height());
}

void IBPWaveWidget::showEvent(QShowEvent *e)
{
    WaveWidget::showEvent(e);
}

void IBPWaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    if (Qt::NoFocus != _name->focusPolicy())
    {
        _name->setFocus();
    }
}

void IBPWaveWidget::loadConfig()
{
    const QPalette &palette = colorManager.getPalette(paramInfo->getParamName(PARAM_IBP));
    setPalette(palette);
    _ruler->setPalette(palette);

    setEntitle(ibpParam.getEntitle(_ibpChn));
}

/**************************************************************************************************
 * 自动标尺计算。
 * 参数:
 *      data： 波形数据。
 *************************************************************************************************/
void IBPWaveWidget::_autoRulerHandle(short data)
{
    _autoRulerTracePeek = (_autoRulerTracePeek < data) ? data : _autoRulerTracePeek;
    _autoRulerTraveVally = (_autoRulerTraveVally > data) ? data : _autoRulerTraveVally;

    unsigned t = timeDate->time();
    if (_autoRulerTime == 0)
    {
        _autoRulerTime = t;
        return;
    }

    if (abs(timeDate->difftime(_autoRulerTime, t)) < 6)
    {
        return;
    }

    _autoRulerTime = t;

    // 开始寻找最合适的标尺。
    int ruler;
    for (ruler = 1; ruler < ibpParam.ibpScaleList.count() - 1; ruler ++)
    {
        if ((_autoRulerTracePeek <= ibpParam.ibpScaleList.at(ruler).high * 10 + 1000))
        {
            if ((ruler == 7) || (ruler == 8))
            {
                if (_autoRulerTraveVally >= ibpParam.ibpScaleList.at(ruler).low * 10 + 1000)
                {
                    break;
                }
                else if (ruler == 8)
                {
                    ruler = 9;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    // ruler为新的增益。
    if (ruler > 13)
    {
        ruler = 13;
    }

    _autoRulerTracePeek = -10000;
    _autoRulerTraveVally = 10000;

    setLimit(ibpParam.ibpScaleList.at(ruler).low, ibpParam.ibpScaleList.at(ruler).high);
}
