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

#include "AGWaveWidget.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "AGWaveRuler.h"
#include "AGSymbol.h"
#include "WaveWidgetSelectMenu.h"
#include "AGParam.h"
#include "WindowManager.h"
#include "PopupList.h"

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGWaveWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        _name->setText(getTitle());
    }
    else
    {
        _name->setText(AGSymbol::convert(type));
    }
}

/**************************************************************************************************
 * 添加波形数据。
 *************************************************************************************************/
void AGWaveWidget::addWaveformData(short data, int flag)
{
    addData(data, flag);
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void AGWaveWidget::setLimit(int low, int high)
{
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标尺的标签值。
 *************************************************************************************************/
void AGWaveWidget::setRuler(AGDisplayZoom zoom)
{
    float zoomValue = 0;
    switch (zoom)
    {
    case AG_DISPLAY_ZOOM_4:
        _ruler->setRuler(4.0, 2.0, 0);
        zoomValue = 4.0;
        break;
    case AG_DISPLAY_ZOOM_8:
        _ruler->setRuler(8.0, 4.0, 0);
        zoomValue = 8.0;
        break;
    case AG_DISPLAY_ZOOM_15:
        _ruler->setRuler(15.0, 7.5, 0);
        zoomValue = 15.0;
        break;
    default:
        break;
    }

    QString str;
    str = QString("0.0~%1").arg(QString::number(zoomValue, 'f', 1));
//    str = QString("0.0~%1").number(zoomValue, 'f', 1);
    str += " ";
    str += trs("percent");
    _zoom->setText(str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGWaveWidget::AGWaveWidget(WaveformID id, const QString &waveName, const AGTypeGas gasType)
    : WaveWidget(waveName, AGSymbol::convert(gasType))
    , _currentItemIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setID(id);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(getTitle());
    connect(_name, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _ruler = new AGWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);

    _zoom = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _zoom->setFont(fontManager.textFont(infoFont));
    _zoom->setFixedSize(120, fontH);
    addItem(_zoom);
    connect(_zoom, SIGNAL(released(IWidget *)), this, SLOT(_zoomChangeSlot(IWidget *)));

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGWaveWidget::~AGWaveWidget()
{
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void AGWaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(0, 0);
    _zoom->move(_name->rect().width(), 0);
    _ruler->resize(qmargins().left(), qmargins().top(),
                   width() - qmargins().left() - qmargins().right(),
                   height() - qmargins().top() - qmargins().bottom());
    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 焦点进入。
 *************************************************************************************************/
void AGWaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    if (Qt::NoFocus != _name->focusPolicy())
    {
        _name->setFocus();
    }
}

void AGWaveWidget::_releaseHandle(IWidget *w)
{
    Q_UNUSED(w);
    QWidget *p = static_cast<QWidget*>(parent());
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
 * 标尺改变。
 *************************************************************************************************/
void AGWaveWidget::_zoomChangeSlot(IWidget *widget)
{
    Q_UNUSED(widget);
    if (NULL == _gainList)
    {
        AGDisplayZoom zoom = AG_DISPLAY_ZOOM_4;
        int maxZoom = AG_DISPLAY_ZOOM_NR;
        _gainList = new PopupList(_zoom , false);
        float zoomArray[] = {4.0, 8.0, 15.0};
        QString str;
        for (int i = 0; i < maxZoom; i ++)
        {
            str.clear();
            str = QString("0.0~%1").arg(QString::number(zoomArray[i], 'f', 1));
            str += " ";
            str += trs("percent");
            _gainList->addItemText(str);
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
void AGWaveWidget::_popupDestroyed()
{
    if (_currentItemIndex == -1)
    {
        _gainList = NULL;
        return;
    }

    agParam.setDisplayZoom((AGDisplayZoom)_currentItemIndex);
    _gainList = NULL;
}

void AGWaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}
