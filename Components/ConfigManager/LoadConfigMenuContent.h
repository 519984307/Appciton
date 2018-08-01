/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#pragma once
#include "MenuContent.h"

class LoadConfigMenuContentPrivate;
class LoadConfigMenuContent: public MenuContent
{
    Q_OBJECT
public:
    /**
     * @brief LoadConfigMenuContent
     */
    LoadConfigMenuContent();
    ~LoadConfigMenuContent();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief hideEvent
     */
    virtual void hideEvent(QHideEvent *);
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);

signals:
    /**
     * @brief configUpdated
     */
    void configUpdated();
private slots:
    /**
     * @brief onConfigClick
     */
    void onConfigClick();
    /**
     * @brief onBtnClick
     */
    void onBtnClick();
private:
    LoadConfigMenuContentPrivate * const d_ptr;
};
