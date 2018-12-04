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


#include "RESPWaveWidget.h"
#include "RESPParam.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ComboListPopup.h"
#include "WindowManager.h"
#include "PopupList.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 增益改变。
 *************************************************************************************************/
void RESPWaveWidget::_respZoom(IWidget *widget)
{
    Q_UNUSED(widget);
    if (NULL == _gainList)
    {
        _gainList = new PopupList(_gain, false);
        for (int i = 0; i < RESP_ZOOM_NR; i++)
        {
            _gainList->addItemText(RESPSymbol::convert(RESPZoom(i)));
        }
        int fontSize = fontManager.getFontSize(3);
        _gainList->setFont(fontManager.textFont(fontSize));
        connect(_gainList, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
        connect(_gainList , SIGNAL(selectItemChanged(int)), this , SLOT(_getItemIndex(int)));
    }
    _gainList->show();
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void RESPWaveWidget::_popupDestroyed(void)
{
    if (_currentItemIndex == -1)
    {
        _gainList = NULL;
        return;
    }

    respParam.setZoom((RESPZoom)_currentItemIndex);

    _gainList = NULL;
}

void RESPWaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
RESPWaveWidget::RESPWaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title), _notify(NULL), _gain(NULL), _gainList(NULL)
    , _currentItemIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_RESP);

    int infoFont = fontManager.getFontSize(4);;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
    _name->setFixedSize(130, fontH);
    _name->setFont(fontManager.textFont(infoFont));
    _name->setText(title);

    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(infoFont));
    _gain->setFixedSize(120, fontH);
    _gain->setText("");
    addItem(_gain);
    connect(_gain, SIGNAL(released(IWidget *)), this, SLOT(_respZoom(IWidget *)));

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(infoFont));
    _notify->setFixedHeight(fontH);
    _notify->setFocusPolicy(Qt::NoFocus);
    addItem(_notify);

    // 加载配置
    _loadConfig();

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
RESPWaveWidget::~RESPWaveWidget()
{
}

/**************************************************************************************************
 * 设置波形放大倍数。
 *************************************************************************************************/
void RESPWaveWidget::setZoom(int zoom)
{
    switch (zoom)
    {
    case RESP_ZOOM_X100:
        setValueRange(-0x4000, 0x3FFF);
        break;

    case RESP_ZOOM_X200:
        setValueRange(-0x2000, 0x1FFF);
        break;

    case RESP_ZOOM_X300:
        setValueRange(-0x1555, 0x1554);
        break;

    case RESP_ZOOM_X400:
        setValueRange(-0x1000, 0xFFF);
        break;

    case RESP_ZOOM_X500:
        setValueRange(-0xCCC, 0xCCB);
        break;

    default:
        zoom = RESP_ZOOM_X100;
        break;
    }
    _gain->setText(RESPSymbol::convert((RESPZoom)zoom));
}

/**************************************************************************************************
 * 电极脱落
 *************************************************************************************************/
void RESPWaveWidget::leadoff(bool flag)
{
    // demo模式notify下强制更新为正常标志
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        flag = false;
    }

    if (flag)
    {
        _notify->setText(trs("RESPCheckElectrodes"));
    }
    else
    {
        _notify->setText("");
    }
}

/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool RESPWaveWidget::waveEnable()
{
    return respParam.isEnabled();
}

void RESPWaveWidget::updateWidgetConfig()
{
   _loadConfig();
   WaveWidget::updateWidgetConfig();
}

/**************************************************************************************************
 * 加载配置。
 *************************************************************************************************/
void RESPWaveWidget::_loadConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);

    RESPSweepSpeed speed = respParam.getSweepSpeed();
    if (speed == RESP_SWEEP_SPEED_6_25)
    {
        setWaveSpeed(6.25);
    }
    else if (speed == RESP_SWEEP_SPEED_12_5)
    {
        setWaveSpeed(12.5);
    }
    else if (speed == RESP_SWEEP_SPEED_25_0)
    {
        setWaveSpeed(25.0);
    }

    setZoom(static_cast<int>(respParam.getZoom()));
}

/**************************************************************************************************
 * 功能： resize事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void RESPWaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    _name->move(0, 0);
    _gain->move(_name->rect().width(), 0);

    // 居中显示。
    _notify->setFixedWidth(width() / 2);
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void RESPWaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    _name->setFocus();
}

/**************************************************************************************************
 * 功能： hide event。
 *************************************************************************************************/
void RESPWaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
}
