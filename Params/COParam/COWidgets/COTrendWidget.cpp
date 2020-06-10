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


class COTrendWidgetPrivate
{
public:
    COTrendWidgetPrivate()
        :coValue(NULL), ciName(NULL), ciValue(NULL),
          tbName(NULL), tbValue(NULL), coStr(InvStr()),
          ciStr(InvStr()), tbStr(InvStr())
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
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    setName("C.O.");

    // 设置报警关闭标志
    showAlarmOff();

    pimpl->coValue = new QLabel();
    pimpl->coValue->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    pimpl->coValue->setPalette(palette);
    pimpl->coValue->setText(InvStr());

    pimpl->ciName = new QLabel("C.I.");
    pimpl->ciName->setPalette(palette);
    pimpl->ciName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pimpl->ciValue = new QLabel();
    pimpl->ciValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    pimpl->ciValue->setPalette(palette);
    pimpl->ciValue->setText(InvStr());

    pimpl->tbName = new QLabel("TB");
    pimpl->tbName->setPalette(palette);
    pimpl->tbName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    pimpl->tbValue = new QLabel();
    pimpl->tbValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    pimpl->tbValue->setPalette(palette);
    pimpl->tbValue->setText(InvStr());

    QHBoxLayout *ciLayout = new QHBoxLayout();
    ciLayout->addWidget(pimpl->ciName, 0, Qt::AlignCenter);
    ciLayout->addWidget(pimpl->ciValue, 0, Qt::AlignCenter);

    QHBoxLayout *tbLayout = new QHBoxLayout();
    tbLayout->addWidget(pimpl->tbName, 0, Qt::AlignCenter);
    tbLayout->addWidget(pimpl->tbValue, 0, Qt::AlignCenter);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(ciLayout);
    vLayout->addLayout(tbLayout);

    contentLayout->addWidget(pimpl->coValue);
    contentLayout->addLayout(vLayout);

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
        u_int16_t coInt = co / 100;
        u_int16_t coDec = co % 100;
        u_int16_t ciInt = ci / 10;
        u_int16_t ciDec = ci % 10;
        pimpl->coStr = QString::number(coInt) + "." + QString::number(coDec);
        pimpl->ciStr = QString::number(ciInt) + "." + QString::number(ciDec);
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
        u_int16_t tbInt = tb / 100;
        u_int16_t tbDec = tb % 100;
        pimpl->tbStr = QString::number(tbInt) + "." + QString::number(tbDec);
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
    int h = ((height()-nameLabel->height()) / 3);
    int w = (width() - unitLabel->width());
    r.setSize(QSize(w, (h * 2)));

    int fontsize = fontManager.adjustNumFontSize(r , true , "2222");
    QFont font = fontManager.numFont(fontsize , true);
    font.setWeight(QFont::Black);

    pimpl->coValue->setFont(font);

    r.setSize(QSize(w, h));
    fontsize = fontManager.adjustNumFontSize(r , true , "2222");
    font = fontManager.numFont(fontsize);
    font.setWeight(QFont::Black);
    pimpl->ciName->setFont(font);
    pimpl->tbName->setFont(font);
    pimpl->ciValue->setFont(font);
    pimpl->tbValue->setFont(font);
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
