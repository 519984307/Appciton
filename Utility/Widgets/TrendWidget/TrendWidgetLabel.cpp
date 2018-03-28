#include <QPainter>
#include "TrendWidgetLabel.h"
#include "TrendWidget.h"
#include <QHBoxLayout>

TrendWidgetLabel::TrendWidgetLabel(const QString &text, int flags,
        TrendWidget *trend) :
        IWidget("", trend),
        _flags(flags), _isFixedWidth(false)
{
    nameLabel = new QLabel("", this);
    nameLabel->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
    nameLabel->setText(text);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(nameLabel);

    setLayout(mainLayout);
}

TrendWidgetLabel::~TrendWidgetLabel()
{
}

void TrendWidgetLabel::setText(const QString &text)
{
    nameLabel->setText(text);
}

void TrendWidgetLabel::setFlags(int flags)
{
    if (flags != _flags)
    {
        _flags = flags;
        update();
    }
}

void TrendWidgetLabel::setFont(const QFont font)
{
    nameLabel->setFont(font);
}
