/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/21
 **/


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "TrendWidget.h"
#include "FontManager.h"
#include "Debug.h"
#include "BaseDefine.h"
#include "TrendWidgetLabel.h"

/**************************************************************************************************
 * 重绘。
 *************************************************************************************************/
void TrendWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    int borderWidth = focusedBorderWidth();

    // 背景。
    QRect r1 = rect();
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(palette().window());
//    r1.setTop(barHeight);
    r1.adjust(borderWidth, borderWidth, -borderWidth, -borderWidth);
    painter.drawRoundedRect(r1, 4, 4);

    // 绘制标题栏。
//    QRect r2 = rect();
//    r2.adjust(borderWidth, borderWidth, -borderWidth, 0);
//    r2.setBottom(barHeight);
//    painter.setBrush(palette().windowText());
    //    painter.drawRoundedRect(r2, 3, 3);
}

void TrendWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);

    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    nameLabel->setFont(font);

    fontSize = fontManager.getFontSize(1);
    font = fontManager.textFont(fontSize);
    unitLabel->setFont(font);
    upLimit->setFont(font);
    downLimit->setFont(font);

    setTextSize();
}

/**************************************************************************************************
 * 功能： 趋势数据显示颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
QPalette TrendWidget::normalPalette(QPalette psrc)
{
    if (psrc.window().color() != Qt::black)     // 趋势数据正常情况下，背景为黑色
    {
        psrc.setColor(QPalette::Window, Qt::black);
    }
    return psrc;
}

/**************************************************************************************************
 * 功能： 趋势数据报警颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
QPalette TrendWidget::alarmPalette(QPalette psrc, bool isSetName)
{
    if (psrc.window().color() != Qt::white)
    {
        psrc.setColor(QPalette::Window, Qt::white);
    }
    if (isSetName && psrc.windowText().color() != Qt::red)
    {
        psrc.setColor(QPalette::WindowText, Qt::red);
    }
    return psrc;
}

//　报警时状态闪烁＋白底红字
void TrendWidget::showAlarmStatus(QWidget *value, QPalette psrc, bool isSetName)
{
    QPalette alaColor = alarmPalette(palette(), isSetName);
    setPalette(alaColor);
    QPalette p = value->palette();
    alaColor = alarmPalette(p);
    if (p.windowText().color() != alaColor.windowText().color())
    {
        value->setPalette(alaColor);
    }
    else
    {
        value->setPalette(psrc);
    }
}

void TrendWidget::showAlarmParamLimit(const QString &valueStr, QPalette psrc)
{
    QPalette p = upLimit->palette();
    QPalette alaColor = alarmPalette(p);
    double value = valueStr.toDouble();
    double up = upLimit->text().toDouble();
    double down = downLimit->text().toDouble();
    if (value > up)
    {
        if (p.windowText().color() != alaColor.windowText().color())
        {
            upLimit->setPalette(alaColor);
        }
        else
        {
            upLimit->setPalette(psrc);
        }
    }
    else
    {
        upLimit->setPalette(alaColor);
    }

    p = downLimit->palette();
    if (value < down)
    {
        if (p.windowText().color() != alaColor.windowText().color())
        {
            downLimit->setPalette(alaColor);
        }
        else
        {
            downLimit->setPalette(psrc);
        }
    }
    else
    {
        downLimit->setPalette(alaColor);
    }
}

void TrendWidget::showNormalStatus(QWidget *value, QPalette psrc)
{
    setPalette(psrc);
    QPalette p = value->palette();
    if (p.windowText().color() != psrc.windowText().color())
    {
        value->setPalette(psrc);
    }
}

void TrendWidget::showNormalParamLimit(QPalette psrc)
{
    QPalette p = upLimit->palette();
    if (p.windowText().color() != psrc.windowText().color())
    {
        upLimit->setPalette(psrc);
    }

    p = downLimit->palette();
    if (p.windowText().color() != psrc.windowText().color())
    {
        downLimit->setPalette(psrc);
    }
}

/**************************************************************************************************
 * 功能： 设置显示的名称。
 * 参数：
 *      name:名称。
 *************************************************************************************************/
void TrendWidget::updateAlarm(bool alarmFlag)
{
//    QPalette p = nameLabel->palette();
//    if (alarmFlag)
//    {
//        if (p.windowText().color() != Qt::white)
//        {
//            p.setColor(QPalette::WindowText, Qt::white);
//            nameLabel->setPalette(p);
//            unitLabel->setPalette(p);
//        }
//    }
//    else
//    {
//        if (p.windowText().color() != Qt::black)
//        {
//            p.setColor(QPalette::WindowText, Qt::black);
//            nameLabel->setPalette(p);
//            unitLabel->setPalette(p);
//        }
    //    }
    Q_UNUSED(alarmFlag)
}

void TrendWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
}

/**************************************************************************************************
 * 功能： 设置显示的名称。
 * 参数：
 *      name:名称。
 *************************************************************************************************/
void TrendWidget::setName(const QString &name)
{
    _title = name;
    nameLabel->setText(name);
}

/**************************************************************************************************
 * 功能： 设置单位字串。
 * 参数：
 *      unit:单位。
 *************************************************************************************************/
void TrendWidget::setUnit(const QString &unit)
{
    unitLabel->setText(unit);
}

void TrendWidget::setLimit(int up, int down, int scale)
{
    if (scale == 1)
    {
        upLimit->setText(QString::number(up));
        downLimit->setText(QString::number(down));
    }
    else
    {
        upLimit->setText(QString::number(up / scale) + "." + QString::number(up % scale));
        downLimit->setText(QString::number(down / scale) + "." + QString::number(down % scale));
    }
}

/**************************************************************************************************
 * 功能： 设置名称的字体。
 * 参数：
 *      size: 字号。
 *      isBold: 是否加粗
 *************************************************************************************************/
void TrendWidget::setNameFont(int size, bool isBold)
{
    nameLabel->setFont(fontManager.textFont(size, isBold));
}

/**************************************************************************************************
 * 功能： 设置单位区的字体。
 * 参数：
 *      size: 字号。
 *      isBold: 是否加粗
 *************************************************************************************************/
void TrendWidget::setUnitFont(int size, bool isBold)
{
    unitLabel->setFont(fontManager.textFont(size, isBold));
}


/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendWidget::TrendWidget(const QString &widgetName, bool vertical)
    : IWidget(widgetName), nameLabel(NULL),
      unitLabel(NULL), upLimit(NULL), downLimit(NULL)
{
    _title = "";
    nameLabel = new TrendWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    nameLabel->setFocusPolicy(Qt::NoFocus);

    unitLabel = new QLabel("", this);
    unitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    upLimit = new QLabel("", this);
    upLimit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    downLimit = new QLabel("", this);
    downLimit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mLayout = new QHBoxLayout();

    if (!vertical)
    {
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(nameLabel);
        vLayout->addWidget(unitLabel);
        vLayout->addWidget(upLimit);
        vLayout->addWidget(downLimit);
        vLayout->addLayout(mLayout, 1);
        vLayout->addStretch();
        vLayout->setSpacing(0);
        vLayout->setContentsMargins(5, 1, 0, 0);

        contentLayout = new QHBoxLayout();
        contentLayout->setMargin(0);
        contentLayout->setSpacing(0);
        contentLayout->addLayout(vLayout);


        setLayout(contentLayout);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendWidget::~TrendWidget()
{
}
