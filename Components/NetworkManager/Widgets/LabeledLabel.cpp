#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include "LabeledLabel.h"

LabeledLabel::LabeledLabel(
        const QString &label, const QString &text, QWidget *parent)
    : QWidget(parent)
{
    _label = new QLabel(label, this);
    _label->setFixedWidth(100);
    _label->setAlignment(Qt::AlignRight | Qt::AlignBottom);

//    _colon = new QLabel(":");
//    _colon->setAlignment(Qt::AlignCenter | Qt::AlignBottom);

    _text = new QLabel(text, this);
    _text->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    // 设置不能接受焦点
    this->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(1);
    hLayout->addWidget(_label, 1);
//    hLayout->addWidget(_colon);
    hLayout->addWidget(_text, 1);
    setLayout(hLayout);
}

void LabeledLabel::setText(const QString &text)
{
    _text->setText(text);
}

void LabeledLabel::setSize(const int &width, const int &height)
{
    _label->setFixedSize(width, height);
    _text->setFixedSize(width, height);
    _text->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

}

void LabeledLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().color(QPalette::WindowText));
//    painter.drawLine(_text->mapToParent(_text->rect().bottomLeft()),
//                     _text->mapToParent(_text->rect().bottomRight()));
}
