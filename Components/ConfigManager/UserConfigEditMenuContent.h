/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#pragma once
#include "MenuContent.h"

class UserConfigEditMenuContentPrivate;
class UserConfigEditMenuContent: public MenuContent
{
    Q_OBJECT
public:
    UserConfigEditMenuContent();
    ~UserConfigEditMenuContent();

    /**
     * @brief eventFilter
     * @param obj
     * @param ev
     * @return
     */
    bool eventFilter(QObject *obj, QEvent *ev);

protected:
    /**
     * @brief layoutExec
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow
     */
    virtual void readyShow(void);
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);
    /**
     * @brief hideEvent
     */
    virtual void hideEvent(QHideEvent *);

private slots:
    /**
     * @brief onExitList
     */
    void onExitList(bool);
    /**
     * @brief onConfigClick
     */
    void onConfigClick();
    /**
     * @brief onBtnClick
     */
    void onBtnClick();
    /**
     * @brief onEditFinished
     */
    void onEditFinished();

private:
    UserConfigEditMenuContentPrivate *const d_ptr;
};
