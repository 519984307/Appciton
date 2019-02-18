/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#pragma once
#include "Dialog.h"

class MessageBoxPrivate;
class MessageBox : public Dialog
{
    Q_OBJECT
public:
    explicit MessageBox(const QString &title,
                        const QString &text,
                        bool btn = true,
                        bool wordWrap = false);

    explicit MessageBox(const QString &title,
                        const QPixmap &icon,
                        const QString &text,
                        bool btn,
                        bool wordWrap = false);

    explicit MessageBox(const QString &title,
                        const QString &text,
                        const QStringList &btnNameList,
                        bool wordWrap = false);

    ~MessageBox();
protected:
    virtual void exit();
    virtual void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);
    /**
     * @brief timeOut
     */
    void timeOut(void);
private:
    MessageBoxPrivate *const d_ptr;
};
