/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#pragma once
#include "IWidget.h"

class QLabel;
class DateTimeWidget : public IWidget
{
    Q_OBJECT
public:
    explicit DateTimeWidget(QWidget *parent = 0);
    void setText(const QString &str);

protected:
    void mousePressEvent(QMouseEvent *e);

private slots:
    void _release(IWidget *);

private:
    QLabel *_datetimeLabel;
};
