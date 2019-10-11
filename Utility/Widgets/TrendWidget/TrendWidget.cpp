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
#include <QTimer>
#include "AlarmConfig.h"

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

void TrendWidget::showAlarmOff()
{
    QList<SubParamID> subParams = getShortTrendSubParams();
    if (subParams.count() == 0)
    {
        return;
    }
    bool alarmOffVisabled = false;
    QList<SubParamID>::ConstIterator iter = subParams.constBegin();
    for (; iter != subParams.end(); iter++)
    {
        if (!alarmConfig.isLimitAlarmEnable(*iter))
        {
            alarmOffVisabled = true;
        }
    }
    alarmOffIcon->setVisible(alarmOffVisabled);
}

/**************************************************************************************************
 * 功能： 趋势数据显示颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
void TrendWidget::normalPalette(QPalette &psrc)
{
    if (psrc.window().color() != Qt::black)     // 趋势数据正常情况下，背景为黑色
    {
        psrc.setColor(QPalette::Window, Qt::black);
    }
}

/**************************************************************************************************
 * 功能： 趋势数据报警颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
QPalette TrendWidget::alarmPalette(QPalette psrc)
{
    if (psrc.windowText().color() != Qt::black)
    {
        psrc.setColor(QPalette::WindowText, Qt::black);
    }
    return psrc;
}

//　报警时状态闪烁＋白底红字
void TrendWidget::showAlarmStatus(QWidget *value)
{
    QPalette p = value->palette();
    QPalette alaColor  = alarmPalette(p);
    value->setPalette(alaColor);
}

void TrendWidget::showAlarmParamLimit(QWidget *valueWidget, const QString &valueStr, QPalette psrc)
{
    normalPalette(psrc);
    double value = valueStr.toDouble();
    double up = upLimit->text().toDouble();
    double down = downLimit->text().toDouble();
    if (value > up)
    {
        upLimit->setPalette(valueWidget->palette());
    }
    else
    {
        upLimit->setPalette(psrc);
    }

    if (value < down)
    {
        downLimit->setPalette(valueWidget->palette());
    }
    else
    {
        downLimit->setPalette(psrc);
    }
    darkParamLimit();
}

// 将控件下的全部控件都刷新颜色
void setWidgetPalette(QLayout *layout, QPalette psrc)
{
    for (int i = 0; i < layout->count(); i++)
    {
        if (layout->itemAt(i)->layout())
        {
            setWidgetPalette(layout->itemAt(i)->layout(), psrc);
        }
        else if (layout->itemAt(i)->widget())
        {
            QWidget *item = layout->itemAt(i)->widget();
            if (item->palette().windowText().color() != psrc.windowText().color())
            {
                layout->itemAt(i)->widget()->setPalette(psrc);
            }
        }
    }
}

void TrendWidget::showNormalStatus(QPalette psrc)
{
    normalPalette(psrc);
    setWidgetPalette(contentLayout, psrc);
    darkParamLimit();
}

void TrendWidget::showNormalStatus(QLayout *layout, QPalette psrc)
{
    normalPalette(psrc);
    setWidgetPalette(layout, psrc);
    darkParamLimit();
}

void TrendWidget::showNormalStatus(QWidget *value, QPalette psrc)
{
    normalPalette(psrc);
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

    darkParamLimit();
}

void TrendWidget::darkParamLimit()
{
    QPalette pal = upLimit->palette();
    QColor c = pal.color(QPalette::WindowText);
    if (c.alpha() != 0)
    {
        c.setAlpha(150);
        pal.setColor(QPalette::WindowText, c);
        upLimit->setPalette(pal);
    }


    pal = downLimit->palette();
    c = pal.color(QPalette::WindowText);
    if (c.alpha() != 0)
    {
        c.setAlpha(150);
        pal.setColor(QPalette::WindowText, c);
        downLimit->setPalette(pal);
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

void TrendWidget::restoreNormalStatusLater()
{
    QTimer::singleShot(500, this, SLOT(doRestoreNormalStatus()));
}

void TrendWidget::doAlarmOff(SubParamID subParamId)
{
    QList<SubParamID> subParams = getShortTrendSubParams();
    if (subParams.contains(subParamId))
    {
        showAlarmOff();
    }
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

void TrendWidget::loadConfig()
{
    updateLimit();
    showAlarmOff();
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

    alarmOffIcon = new QLabel(this);
    QPixmap icon("/usr/local/nPM/icons/AlarmOff.png");
    icon = icon.scaled(32, 32);
    alarmOffIcon->setPixmap(icon);
    alarmOffIcon->setVisible(false);

    mLayout = new QHBoxLayout();

    if (!vertical)
    {
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(nameLabel);
        vLayout->addWidget(unitLabel);

        QVBoxLayout *vLayoutLimit = new QVBoxLayout();
        vLayoutLimit->addWidget(upLimit);
        vLayoutLimit->addWidget(downLimit);

        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->addWidget(alarmOffIcon, 1);
        hLayout->addLayout(vLayoutLimit, 1);

        vLayout->addLayout(hLayout);
        vLayout->addLayout(mLayout, 1);
        vLayout->addStretch();
        vLayout->setSpacing(0);
        vLayout->setContentsMargins(5, 1, 0, 0);

        contentLayout = new QHBoxLayout();
        contentLayout->setMargin(0);
        contentLayout->setSpacing(0);
        contentLayout->addLayout(vLayout, 1);


        setLayout(contentLayout);
    }

    connect(&alarmConfig, SIGNAL(alarmOff(SubParamID)), this, SLOT(doAlarmOff(SubParamID)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendWidget::~TrendWidget()
{
}
