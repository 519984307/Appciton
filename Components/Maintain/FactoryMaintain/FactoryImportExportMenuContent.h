/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/

#pragma once
#include "MenuContent.h"


class FactoryImportExportMenuContentPrivate;
class FactoryImportExportMenuContent : public MenuContent
{
    Q_OBJECT
public:
    FactoryImportExportMenuContent();
    ~FactoryImportExportMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);

public:
    /**
     * @brief eventFilter 导入导出窗口事件过滤方法
     * @param obj  导入导出窗口对象
     * @param ev   事件
     * @return     返回 false
     */
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();
    /**
     * @brief onConfigUpdated
     */
    void onConfigUpdated();

private:
    FactoryImportExportMenuContentPrivate *const d_ptr;
};


