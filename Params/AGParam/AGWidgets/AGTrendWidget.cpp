/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "AGTrendWidget.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "AGSymbol.h"
#include "WindowManager.h"
#include "PublicMenuManager.h"
#include <QDebug>
#include "MainMenuWindow.h"
#include "AGMenuContent.h"

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGTrendWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        setName("Et" + (QString)AGSymbol::convert(_gasType));
        _fiName->setText("Fi" + (QString)AGSymbol::convert(_gasType));
    }
    else
    {
        setName("Et" + (QString)AGSymbol::convert(type));
        _fiName->setText("Fi" + (QString)AGSymbol::convert(type));
    }
}

/**************************************************************************************************
 * 设置et结果数据。
 *************************************************************************************************/
void AGTrendWidget::setEtData(int16_t etValue)
{
    if (etValue == InvData())
    {
        _etStr = InvStr();
    }
    else
    {
        if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
        {
            _etStr = QString::number(etValue);
        }
        else
        {
            int16_t etValueInt = etValue / 10;
            int16_t etValueDes = etValue % 10;
            _etStr = QString::number(etValueInt) + "." + QString::number(etValueDes);
        }
    }

    _etValue->setText(_etStr);
}

/**************************************************************************************************
 * 设置fi结果数据。
 *************************************************************************************************/
void AGTrendWidget::setFiData(int16_t fiValue)
{
    if (fiValue == InvData())
    {
        _fiStr = InvStr();
    }
    else if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
    {
        _fiStr = QString::number(fiValue);
    }
    else
    {
        unsigned char fiValueInt = fiValue / 10;
        unsigned char fiValueDes = fiValue % 10;
        _fiStr = QString::number(fiValueInt) + "." + QString::number(fiValueDes);
    }

    _fiValue->setText(_fiStr);
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void AGTrendWidget::isAlarm(int id, bool flag)
{
    SubParamID subID = (SubParamID)id;
    switch (subID)
    {
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
        _etAlarm = flag;
        break;
    case SUB_PARAM_FIN2O:
    case SUB_PARAM_FIAA1:
    case SUB_PARAM_FIAA2:
    case SUB_PARAM_FIO2:
        _fiAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_etAlarm || _fiAlarm);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void AGTrendWidget::showValue()
{
    QPalette p;
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    QPalette fgColor = normalPalette(psrc);
    QPalette alaColor = alarmPalette(psrc);
    if (_fiAlarm || _etAlarm)
    {
        p = _etValue->palette();
        if (_etAlarm)
        {
            if (p.windowText().color() != alaColor.windowText().color())
            {
                _etValue->setPalette(alaColor);
            }
            else
            {
                _etValue->setPalette(fgColor);
            }
        }
        else
        {
            _etValue->setPalette(fgColor);
        }

        p = _fiValue->palette();
        if (_fiAlarm)
        {
            if (p.windowText().color() != alaColor.windowText().color())
            {
                _fiValue->setPalette(alaColor);
            }
            else
            {
                _fiValue->setPalette(fgColor);
            }
        }
        else
        {
            _fiValue->setPalette(fgColor);
        }
    }
    else
    {
        p = _etValue->palette();
        if (p.windowText().color() != fgColor.windowText().color())
        {
            _etValue->setPalette(fgColor);
        }

        p = _fiValue->palette();
        if (p.windowText().color() != fgColor.windowText().color())
        {
            _fiValue->setPalette(fgColor);
        }
    }

    _etValue->setText(_etStr);

    if (!_fiValue->isHidden())
    {
        _fiValue->setText(_fiStr);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGTrendWidget::AGTrendWidget(const QString &trendName, const AGTypeGas gasType)
    : TrendWidget(trendName), _gasType(gasType), _etAlarm(false), _fiAlarm(false)
{
    _etStr = InvStr();
    _fiStr = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    setName("Et" + (QString)AGSymbol::convert(gasType));
    setUnit("%");

    // 构造资源。
    _etValue = new QLabel();
    _etValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _etValue->setPalette(palette);
    _etValue->setText(InvStr());

    _fiName = new QLabel();
    _fiName->setPalette(palette);
    _fiName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _fiName->setText("Fi" + (QString)AGSymbol::convert(gasType));

    _fiValue = new QLabel();
    _fiValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _fiValue->setPalette(palette);
    _fiValue->setText(InvStr());

    QHBoxLayout *fiLayout = new QHBoxLayout();
    fiLayout->addStretch();
    fiLayout->addWidget(_fiName);
    fiLayout->addStretch();
    fiLayout->addWidget(_fiValue);
    fiLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addWidget(_etValue);
    vLayout->addStretch();
    vLayout->addLayout(fiLayout);
    vLayout->addStretch();

    contentLayout->addLayout(vLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGTrendWidget::~AGTrendWidget()
{
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void AGTrendWidget::setTextSize()
{
    QRect r;
    int h = ((height() - nameLabel->height()) / 3);
    int w = (width() - unitLabel->width());
    r.setSize(QSize(w, (h * 2)));

    int fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _etValue->setFont(font);

    r.setSize(QSize(w, h));
    fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    font = fontManager.numFont(fontsize - 5);
    font.setWeight(QFont::Black);
    _fiName->setFont(font);

    font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _fiValue->setFont(font);
}


/**************************************************************************************************
 * 趋势槽函数。
 *************************************************************************************************/
void AGTrendWidget::_releaseHandle(IWidget *iWidget)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    MainMenuWindow *mainMenu =  MainMenuWindow::getInstance();
    mainMenu->popup(AGMenuContent::getInstace() , x , y);
}
