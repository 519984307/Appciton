/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#pragma once
#include "Window.h"
#include <QScopedPointer>
#include <QModelIndex>

class ErrorLogWindowPrivate;
class ErrorLogWindow : public Window
{
    Q_OBJECT
public:
    static ErrorLogWindow *getInstance();
    ~ErrorLogWindow();

    void init(void);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void itemClickSlot(QModelIndex index);
    void summaryReleased();
    void exportReleased();
    void eraseReleased();
    void USBCheckTimeout();

private:
    ErrorLogWindow();
    QScopedPointer<ErrorLogWindowPrivate> d_ptr;
};
