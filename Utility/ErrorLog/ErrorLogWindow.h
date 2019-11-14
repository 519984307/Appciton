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
#include "Framework/UI/Dialog.h"
#include <QScopedPointer>
#include <QModelIndex>

class ErrorLogWindowPrivate;
class ErrorLogWindow : public Dialog
{
    Q_OBJECT
public:
    static ErrorLogWindow *getInstance();
    ~ErrorLogWindow();

    void init(void);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief itemClickSlot
     * @param row
     */
    void itemClickSlot(int row);

    void summaryReleased();
    void exportReleased();
    void eraseReleased();
    void USBCheckTimeout();

    /**
     * @brief onPageInfoUpdated  页码更新槽函数
     * @param curPage 当前页
     * @param totalPage 总页
     */
    void onPageInfoUpdated(int curPage, int totalPage);

private:
    ErrorLogWindow();
    QScopedPointer<ErrorLogWindowPrivate> d_ptr;
};
