/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#pragma once

#include <QWidget>
#include <QScopedPointer>

class COMeasureWidgetPrivate;
class COMeasureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit COMeasureWidget(QWidget *parent = NULL);
    ~COMeasureWidget();

protected:
    void paintEvent(QPaintEvent *ev);

private:
    const QScopedPointer<COMeasureWidgetPrivate> pimpl;
};
