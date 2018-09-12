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
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "WindowManager.h"
#include "Debug.h"
#include "IBPManualRuler.h"
#include "TimeDate.h"
#include "PopupList.h"

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
 * 自动设置标尺弹出。
 *************************************************************************************************/
void IBPWaveWidget::displayManualRuler()
{
    ibpManualRuler.setWaveWidget(this);
    ibpManualRuler.setHighLowRuler(_highLimit, _lowLimit);
    ibpManualRuler.autoShow();
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void IBPWaveWidget::setLimit(int low, int high)
{
    _lowLimit = low;
    _highLimit = high;
    _ruler->setRuler(high, (high + low) / 2, low);
    low = low * 10 + 1000;
    high = high * 10 + 1000;
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPWaveWidget::setEntitle(IBPPressureName entitle)
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
IBPPressureName IBPWaveWidget::getEntitle()
{
    return _entitle;
}

void IBPWaveWidget::setRulerLimit(IBPRulerLimit ruler)
{
    if (ruler == -1)
    {
        return;
    }

    if (ruler == IBP_RULER_LIMIT_AOTU)
    {
        _isAutoRuler = true;
    }
    else if (ruler == IBP_RULER_LIMIT_MANUAL)
    {
        _isAutoRuler = false;
        displayManualRuler();
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
IBPWaveWidget::IBPWaveWidget(WaveformID id, const QString &waveName, const IBPPressureName &entitle)
    : WaveWidget(waveName, IBPSymbol::convert(entitle)),
      _entitle(entitle)
{
    _autoRulerTracePeek = -10000;
    _autoRulerTraveVally = 10000;
    _autoRulerTime = 0;
    _isAutoRuler = false;

    setFocusPolicy(Qt::NoFocus);
    setID(id);
//    setValueRange(64, 192);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);

    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(getTitle());
    connect(_name, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _ruler = new IBPWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);

    _leadSta = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _leadSta->setFont(fontManager.textFont(infoFont));
    _leadSta->setFixedSize(120, fontH);
    _leadSta->setText(trs("LeadOff"));
    _leadSta->setFocusPolicy(Qt::NoFocus);
    addItem(_leadSta);

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));

    // 加载配置
//    _loadConfig();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPWaveWidget::~IBPWaveWidget()
{
}

void IBPWaveWidget::paintEvent(QPaintEvent *e)
{
    WaveWidget::paintEvent(e);
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void IBPWaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(0, 0);
    _leadSta->move(_name->rect().x() + _name->rect().width(), 0);
    _ruler->resize(qmargins().left(), qmargins().top(),
                   width() - qmargins().left() - qmargins().right(),
                   height() - qmargins().top() - qmargins().bottom());
    WaveWidget::resizeEvent(e);
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

void IBPWaveWidget::_releaseHandle(IWidget *w)
{
    Q_UNUSED(w);
    QWidget *p = qobject_cast<QWidget *>(parent());
    if (p == NULL)
    {
        return;
    }

    QRect prect = p->geometry();
    QRect r = geometry();

    waveWidgetSelectMenu.setTopWaveform(false);
    waveWidgetSelectMenu.setWaveformName(name());
    waveWidgetSelectMenu.setShowPoint(prect.x() + r.x() + 50, prect.y() + r.y());
    windowManager.showWindow(&waveWidgetSelectMenu, WindowManager::ShowBehaviorModal);
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

    unsigned t = timeDate.time();
    if (_autoRulerTime == 0)
    {
        _autoRulerTime = t;
        return;
    }

    if (abs(timeDate.difftime(_autoRulerTime, t)) < 6)
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
