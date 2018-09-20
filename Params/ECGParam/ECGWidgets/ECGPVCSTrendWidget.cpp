/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#include "ECGPVCSTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "ECGMenu.h"
#include "PublicMenuManager.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "WindowManager.h"
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void ECGPVCSTrendWidget::_releaseHandle(IWidget * iWidget)
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("ECGMenu"));
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGPVCSTrendWidget::setPVCSValue(int16_t pvcs)
{
    if (pvcs < 0)
    {
        _pvcsString = InvStr();
    }
    else
    {
        _pvcsString = QString::number(pvcs);
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGPVCSTrendWidget::isAlarm(bool isAlarm)
{
    _isAlarm = isAlarm;

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGPVCSTrendWidget::showValue(void)
{
    QPalette p = palette();
    if (_isAlarm)
    {
//        if (p.window().color() != Qt::white)
//        {
//            p.setColor(QPalette::Window, Qt::white);
//            p.setColor(QPalette::WindowText, Qt::red);
//            setPalette(p);
//        }

        p = _pvcsValue->palette();
        if (p.windowText().color() != Qt::red)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _pvcsValue->setPalette(p);
        }
    }
    else
    {
        if (p.window().color() != Qt::black)
        {
            p = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
            setPalette(p);
            _pvcsValue->setPalette(p);
        }
    }

    _pvcsValue->setText(_pvcsString);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void ECGPVCSTrendWidget::setTextSize(void)
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r , true);
    QFont font = fontManager.numFont(fontsize , true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _pvcsValue->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGPVCSTrendWidget::ECGPVCSTrendWidget() : TrendWidget("ECGPVCSTrendWidget")
{
    _isAlarm = false;
    _pvcsString = InvStr();

    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_ECG_PVCS)));
    setUnit(Unit::getSymbol(UNIT_BPM));

    _pvcsValue = new QLabel();
    _pvcsValue->setPalette(palette);
    _pvcsValue->setAlignment(Qt::AlignCenter);
    _pvcsValue->setText(InvStr());

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addStretch(1);
    mainLayout->addWidget(_pvcsValue);
    mainLayout->addStretch(1);

    contentLayout->addStretch(1);
    contentLayout->addLayout(mainLayout);
    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _isAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGPVCSTrendWidget::~ECGPVCSTrendWidget()
{
}
