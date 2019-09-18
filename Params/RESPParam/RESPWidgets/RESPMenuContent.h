/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/9
 **/

#pragma once
#include "MenuContent.h"

class RESPMenuContentPrivate;
class RESPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    RESPMenuContent();
    ~RESPMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);


    void updateBRSource();

private:
    RESPMenuContentPrivate * const d_ptr;
};
