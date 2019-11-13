/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#pragma once
#include "Framework/UI/Dialog.h"

class AlarmLimitWindowPrivate;

class AlarmLimitWindow : public Dialog
{
    Q_OBJECT
public:
    explicit AlarmLimitWindow(const QString &param);
    AlarmLimitWindow();
    ~AlarmLimitWindow();


private slots:
    /**
     * @brief setItemFocus
     * @param param
     */
    void setItemFocus();

protected:
    /* reimplment */
    void showEvent(QShowEvent *ev);
    /**
     * @brief readyShow
     */
    void readyShow();

    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    /**
     * @brief onbtnClick
     */
    void onbtnClick();

    /**
     * @brief onRowClicked
     * @param row
     */
    void onRowClicked(int row);

    /**
     * @brief onSelectRowChanged
     * @param row
     */
    void onSelectRowChanged(int row);

    /**
     * @brief onDefaultsClick
     */
    void onDefaultsClick();

private:
    void restoreDefaults();

private:
    AlarmLimitWindowPrivate *const d_ptr;
};
