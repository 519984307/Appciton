/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "COTrendWidget.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include <QDebug>
#include "MeasureSettingWindow.h"
#include "BaseDefine.h"
#include "Framework/Utility/Unit.h"


class COTrendWidgetPrivate
{
public:
    COTrendWidgetPrivate()
        :coValue(NULL), ciName(NULL), ciValue(NULL), tbName(NULL), tbValue(NULL),
          coStr(InvStr()), ciStr(InvStr()), tbStr(InvStr())
    {}

    QLabel *coValue;
    QLabel *ciName;
    QLabel *ciValue;
    QLabel *tbName;
    QLabel *tbValue;

    QString coStr;
    QString ciStr;
    QString tbStr;
};

COTrendWidget::COTrendWidget()
    : TrendWidget("COTrendWidget"), pimpl(new COTrendWidgetPrivate())
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CO));
    setPalette(palette);

    setName("C.O.");

    setUnit(trs(Unit::getSymbol(UNIT_LMin)));

    // // 设置报警关闭标志
    // showAlarmOff();

    pimpl->coValue = new QLabel(pimpl->coStr);
    pimpl->coValue->setAlignment(Qt::AlignCenter);
    pimpl->coValue->setPalette(palette);

    pimpl->ciName = new QLabel("C.I.");
    pimpl->ciName->setPalette(palette);
    pimpl->ciName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pimpl->ciValue = new QLabel(pimpl->ciStr);
    pimpl->ciValue->setAlignment(Qt::AlignCenter);
    pimpl->ciValue->setPalette(palette);

    pimpl->tbName = new QLabel("TB");
    pimpl->tbName->setPalette(palette);
    pimpl->tbName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pimpl->tbValue = new QLabel(pimpl->tbStr);
    pimpl->tbValue->setAlignment(Qt::AlignCenter);
    pimpl->tbValue->setPalette(palette);

    QHBoxLayout *ciLayout = new QHBoxLayout();
    ciLayout->setContentsMargins(0, 0, 0, 0);
    ciLayout->setSpacing(8);
    ciLayout->addWidget(pimpl->ciName);
    ciLayout->addWidget(pimpl->ciValue);

    QHBoxLayout *tbLayout = new QHBoxLayout();
    tbLayout->setContentsMargins(0, 0, 0, 0);
    tbLayout->setSpacing(8);
    tbLayout->addWidget(pimpl->tbName);
    tbLayout->addWidget(pimpl->tbValue);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(ciLayout);
    vLayout->addLayout(tbLayout);

    contentLayout->addWidget(pimpl->coValue, 3, Qt::AlignCenter);
    contentLayout->addLayout(vLayout, 3);

    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
COTrendWidget::~COTrendWidget()
{
}

/**************************************************************************************************
 * display C.O. and C.I. data.。
 *************************************************************************************************/
void COTrendWidget::setMeasureResult(short co, short ci)
{
    if (co == InvData() || ci == InvData())
    {
        pimpl->coStr = InvStr();
        pimpl->ciStr = InvStr();
    }
    else
    {
        pimpl->coStr = QString::number(co * 1.0 / 10, 'f', 1);
        pimpl->ciStr = QString::number(co * 1.0 / 10, 'f', 1);
    }
    pimpl->coValue->setText(pimpl->coStr);
    pimpl->ciValue->setText(pimpl->ciStr);

    return;
}

/**************************************************************************************************
 * display temp blood data。
 *************************************************************************************************/
void COTrendWidget::setTb(short tb)
{
    if (tb == InvData())
    {
        pimpl->tbStr = InvStr();
    }
    else
    {
        pimpl->tbStr = QString::number(tb * 1.0 / 10, 'f', 1);
    }

    pimpl->tbValue->setText(pimpl->tbStr);
    return;
}

QList<SubParamID> COTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list << SUB_PARAM_CO_CO << SUB_PARAM_CO_CI << SUB_PARAM_CO_TB;
    return list;
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void COTrendWidget::setTextSize()
{
    QRect r;
    int h = height();
    int w = (width() - unitLabel->width()) / 2;
    r.setSize(QSize(w, h));

    int fontsize = fontManager.adjustNumFontSize(r , true , "2222");
    QFont font = fontManager.numFont(fontsize , true);
    font.setWeight(QFont::Black);

    pimpl->coValue->setFont(font);

    r.setSize(QSize(w / 2, h / 2));
    fontsize = fontManager.adjustNumFontSize(r , true , "2222");
    font = fontManager.numFont(fontsize);
    font.setWeight(QFont::Black);
    pimpl->ciValue->setFont(font);
    pimpl->tbValue->setFont(font);

    font = fontManager.textFont(fontManager.getFontSize(3));
    pimpl->ciName->setFont(font);
    pimpl->tbName->setFont(font);
}

/**************************************************************************************************
 * 趋势槽函数。
 *************************************************************************************************/
void COTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("COMenu"));
}
