#include <QPainter>
#include "Key.h"

Key::Key(QWidget *parent) : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(_onClicked()));
}

Key::~Key()
{
}

void Key::_onClicked()
{
    emit KeyClicked(_id);
    emit KeyClicked(text());
}

void Key::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
}
