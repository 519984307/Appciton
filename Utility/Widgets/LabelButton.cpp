#include "LabelButton.h"
#include <QHBoxLayout>

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      value：需显示的数据。
 *************************************************************************************************/
void LabelButton::setLabel(const QString &text)
{
    label->setText(text);
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::red);
    label->setPalette(p);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      value：需显示的数据。
 *************************************************************************************************/
void LabelButton::setValue(int value)
{
    QString str;
    str.sprintf("%d", value);
    button->setText(str);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      value：需显示的数据。
 *************************************************************************************************/
void LabelButton::setValue(double value, int bits)
{
    QString format;
    format.sprintf("%s%df", "%.", bits);
    QString text;
    text.sprintf(qPrintable(format), value);
    button->setText(text);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      text：需显示的数据。
 *************************************************************************************************/
void LabelButton::setValue(const QString &text)
{
    button->setText(text);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      text：需显示的数据。
 *************************************************************************************************/
void LabelButton::setValue(const char *text)
{
    if (text == NULL)
    {
        return;
    }

    button->setText(text);
}

/**************************************************************************************************
 * 功能：设置拉伸因子。
 *************************************************************************************************/
void LabelButton::setStretch(int labelStrch, int buttonStrch)
{
    QHBoxLayout *hlayout = (QHBoxLayout *)layout();
    if (NULL == hlayout)
    {
        return;
    }

    hlayout->setStretchFactor(label, labelStrch);
    hlayout->setStretchFactor(button, buttonStrch);
}

/**************************************************************************************************
 * 功能：设置间隙。
 *************************************************************************************************/
void LabelButton::setSpacing(int space)
{
    QHBoxLayout *hlayout = (QHBoxLayout *)layout();
    if (NULL == hlayout)
    {
        return;
    }

    hlayout->setSpacing(space);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
LabelButton::LabelButton(const QString &text) : QWidget()
{
    label = new QLabel(text);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    button = new LButton();

    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(button);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(5);

    layout->addWidget(label);
    layout->addWidget(button);
    setLayout(layout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
LabelButton::~LabelButton()
{

}
