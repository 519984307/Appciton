/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/10
 **/

#pragma once
#include "Dialog.h"
#include "ErrorLogItem.h"
#include <QScopedPointer>

class ErrorLogViewerWindowPrivate;
class ErrorLogViewerWindow : public Dialog
{
    Q_OBJECT
public:
    ErrorLogViewerWindow();
    explicit ErrorLogViewerWindow(ErrorLogItemBase *item);
    ~ErrorLogViewerWindow();

    void setText(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    QScopedPointer<ErrorLogViewerWindowPrivate> d_ptr;
};
