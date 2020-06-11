/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#include "COMeasureWidget.h"
#include <QPainter>

class COMeasureWidgetPrivate
{
public:
    COMeasureWidgetPrivate() {}
};

COMeasureWidget::COMeasureWidget(QWidget *parent)
    : QWidget(parent), pimpl(new COMeasureWidgetPrivate())
{
}

COMeasureWidget::~COMeasureWidget()
{
}

void COMeasureWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(this->contentsRect(), 4, 4);
}
