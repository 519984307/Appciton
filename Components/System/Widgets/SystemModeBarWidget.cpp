#include "SystemModeBarWidget.h"
#include "SystemModeSelectWidget.h"
#include "FontManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

/**************************************************************************************************
 * 绘画。
 *************************************************************************************************/
void SystemModeBarWidget::paintEvent(QPaintEvent */*e*/)
{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setPen(Qt::black);
//    painter.setBrush(QColor(48,48,48));

//    int border =  focusedBorderWidth() - 1;
//    QRect tempRect = rect().adjusted(border, border, -border, -border);
//    painter.drawRoundedRect(tempRect, 4, 4);
}

/**************************************************************************************************
 * 功能： 设置显示的内容。
 * 参数：
 *      str：显示的内容。
 *************************************************************************************************/
void SystemModeBarWidget::setMode(UserFaceType mode)
{
    QString str(SystemSymbol::convert(mode));
    _modeLabel->setText(trs(str));
}

void SystemModeBarWidget::_releaseHandle(IWidget *)
{
    QPoint p = this->mapToGlobal(this->rect().bottomLeft());
    QRect r = geometry();
    _systemModeSelectWidget->autoShow(p.x(), p.y() + 1, r.width() + 10, 3 * r.height() + 18);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemModeBarWidget::SystemModeBarWidget(QWidget *parent) : IWidget("SystemModeBarWidget", parent)
{
//    _systemModeSelectWidget = new SystemModeSelectWidget();
    setFocusPolicy(Qt::NoFocus);

    _modeLabel = new QLabel("", this);
    _modeLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    _modeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _modeLabel->setFixedWidth(150);
    _modeLabel->setStyleSheet("color:lightGray;background:rgb(48,48,48);"
            "border:1px groove black;border-radius:4px;");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 5, 0);
    layout->addWidget(_modeLabel);
    setLayout(layout);

    //setMode(SYSTEM_MODE_MONITOR);

    // 释放事件。
//    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}
