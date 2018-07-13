/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include "Button.h"
#include <QWidget>

class PasswordWidgetPrivate;
class PasswordWidget : public QWidget
{
    Q_OBJECT
public:
    PasswordWidget(const QString &name, const QString &password);
    ~PasswordWidget();

    void layoutExec();

signals:
    void correctPassword(void);

protected:
    void changeEvent(QEvent *ev);

private slots:
    void onButtonReleased(void);
    void timeOutSlot(void);

private:
    PasswordWidgetPrivate * const d_ptr;
};
