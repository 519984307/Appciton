#include "ElapseTimeWidget.h"
#include "FontManager.h"
#include <QHBoxLayout>
#include <QLabel>

/**************************************************************************************************
 * 功能： 设置显示的内容。
 * 参数：
 *      str：显示的内容。
 *************************************************************************************************/
void ElapseTimeWidget::setText(const QString &str)
{
    _elapseLabel->setText(str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ElapseTimeWidget::ElapseTimeWidget(QWidget *parent) : IWidget("ElapseTimeWidget", parent)
{
    setFocusPolicy(Qt::NoFocus);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::lightGray);

    QFont font = fontManager.numFont(fontManager.getFontSize(5), true);
//    font.setStretch(105);
    font.setWeight(QFont::Black);

    _elapseLabel = new QLabel("", this);
    _elapseLabel->setFont(font);
    _elapseLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _elapseLabel->setPalette(palette);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(_elapseLabel);
    setLayout(hLayout);
}
