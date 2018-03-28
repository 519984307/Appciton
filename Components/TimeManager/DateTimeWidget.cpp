#include "DateTimeWidget.h"
#include "FontManager.h"
#include <QHBoxLayout>
#include <QLabel>

/**************************************************************************************************
 * 功能： 设置显示的内容。
 * 参数：
 *      str：显示的内容。
 *************************************************************************************************/
void DateTimeWidget::setText(const QString &str)
{
    _datetimeLabel->setText(str);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
DateTimeWidget::DateTimeWidget(QWidget *parent)
        : IWidget("DateTimeWidget", parent)
{
    setFocusPolicy(Qt::NoFocus);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::lightGray);

    _datetimeLabel = new QLabel("", this);
    _datetimeLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    _datetimeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _datetimeLabel->setPalette(palette);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(_datetimeLabel);
    hLayout->setContentsMargins(1, 1, 1, 1);
    setLayout(hLayout);
}
