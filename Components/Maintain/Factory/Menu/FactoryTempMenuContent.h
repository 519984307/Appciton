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
#include "MenuContent.h"

class FactoryTempMenuContentPrivate;
class FactoryTempMenuContent : public MenuContent
{
    Q_OBJECT

public:
    FactoryTempMenuContent();

    ~FactoryTempMenuContent();

    /**
     * @brief showError
     * @param str
     */
    void showError(QString str);

protected:
    void hideEvent(QHideEvent *e);
    virtual void readyShow();

    virtual void layoutExec();

    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief onChannelReleased
     */
    void onChannelReleased(int);
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased();
    /**
     * @brief timeOut
     */
    void timeOut();

private:
    FactoryTempMenuContentPrivate *const d_ptr;
};
