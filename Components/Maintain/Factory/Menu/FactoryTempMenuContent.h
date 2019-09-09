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

    void calibrateValueOnTime(int time);

protected:
    /* reimplment */
    void hideEvent(QHideEvent *e);

    /* reimplment */
    virtual void readyShow();

    /* reimplment */
    virtual void layoutExec();

    /* reimplment */
    void timerEvent(QTimerEvent *ev);

    bool eventFilter(QObject *obj, QEvent *event);
private slots:
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
