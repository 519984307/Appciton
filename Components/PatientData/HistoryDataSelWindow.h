/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#pragma once
#include "Dialog.h"
#include <QScopedPointer>
#include <QModelIndex>

class HistoryDataSelWindowPrivate;
class HistoryDataSelWindow : public Dialog
{
    Q_OBJECT
public:
    HistoryDataSelWindow();
    ~HistoryDataSelWindow();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void itemSelReleased(QModelIndex index);
    void upReleased(void);
    void downReleased(void);

private:
    QScopedPointer<HistoryDataSelWindowPrivate> d_ptr;
};
