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
#include "Framework/UI/MenuContent.h"


class ConfigExportImportMenuContentPrivate;

class ConfigExportImportMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ConfigExportImportMenuContent();
    ~ConfigExportImportMenuContent();

protected:
    /* reimplment */
    void layoutExec();

    /* reimplment */
    void readyShow();

    /* reimplment */
    void showEvent(QShowEvent *ev);

    /* reimplment */
    void hideEvent(QHideEvent *ev);

private slots:
    /**
     * @brief onBtnClick  按钮触发槽函数
     */
    void onBtnClick();

    /**
     * @brief updateBtnStatus  按钮状态更新槽函数
     */
    void updateBtnStatus();

    /**
     * @brief onTimeOut  定时器触发函数
     */
    void onTimeOut(void);

private:
    ConfigExportImportMenuContentPrivate *const d_ptr;
};
