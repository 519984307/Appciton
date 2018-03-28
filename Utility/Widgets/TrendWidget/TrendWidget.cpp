#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "TrendWidget.h"
#include "FontManager.h"
#include "Debug.h"
#include "BaseDefine.h"
#include "TrendWidgetLabel.h"
#include "WindowManager.h"
#include "TrendWidgetSelectMenu.h"

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

void TrendWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);

    if (windowManager.getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        nameLabel->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        nameLabel->setFocusPolicy(Qt::NoFocus);
    }
}

void TrendWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);

//    QRect r;
//    r.setHeight((height() / 4));
//    // 字体。
//    int fontsize = fontManager.adjustTextFontSize(r);
//    QFont font = fontManager.textFont(fontsize);
//    nameLabel->setFont(font);
//    font = fontManager.textFont(fontsize - 4);
//    unitLabel->setFont(font);

    int fontSize = fontManager.getFontSize(7);
    QFont font = fontManager.textFont(fontSize);
    nameLabel->setFont(font);
    calcLeadLabel->setFont(font);
    font = fontManager.textFont(fontSize - 4);
    unitLabel->setFont(font);

    setTextSize();
}

/**************************************************************************************************
 * 功能： 趋势数据显示颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
QPalette TrendWidget::normalPalette(QPalette psrc)
{
    if (psrc.windowText().color() == Qt::black)
    {
        psrc.setColor(QPalette::WindowText, Qt::white);
    }
    return psrc;
}

/**************************************************************************************************
 * 功能： 趋势数据报警颜色。
 * 参数：
 *      psrc:趋势数据设置颜色。
 *************************************************************************************************/
QPalette TrendWidget::alarmPalette(QPalette psrc)
{
    if (psrc.windowText().color() == Qt::white)
    {
        psrc.setColor(QPalette::WindowText, Qt::red);
    }
    else
    {
        psrc.setColor(QPalette::WindowText, Qt::white);
    }
    return psrc;
}

/**************************************************************************************************
 * 功能： 设置显示的名称。
 * 参数：
 *      name:名称。
 *************************************************************************************************/
void TrendWidget::updateAlarm(bool /*alarmFlag*/)
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
}

void TrendWidget::_releaseHandle(IWidget *widget)
{
    QPoint prect = widget->mapToGlobal(widget->rect().bottomLeft());
    QRect r = geometry();

    trendWidgetSelectMenu.setWidgetName(name());
    trendWidgetSelectMenu.setShowPoint(prect.x() + r.x() + 50, prect.y() + r.y());
    trendWidgetSelectMenu.autoShow();
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

/**************************************************************************************************
 * 功能： 设置计算导联字串。
 * 参数：
 *      unit:单位。
 *************************************************************************************************/
void TrendWidget::setCalcLeadName(const QString &unit)
{
    if (!calcLeadLabel->isVisible())
    {
        calcLeadLabel->setVisible(true);
    }

    if (unit != calcLeadLabel->text())
    {
        calcLeadLabel->setText(unit);
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
TrendWidget::TrendWidget(const QString &widgetName,bool vertical) : IWidget(widgetName)
{
    _title = "";
    nameLabel = new TrendWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    connect(nameLabel, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    calcLeadLabel = new QLabel("", this);
    calcLeadLabel->setAlignment(Qt::AlignCenter);

    unitLabel = new QLabel("", this);
    unitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    mLayout = new QHBoxLayout();

    if (!vertical)
    {
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(nameLabel);
        vLayout->addWidget(unitLabel);
        vLayout->addLayout(mLayout,1);
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
