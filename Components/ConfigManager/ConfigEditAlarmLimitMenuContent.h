/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditAlarmLimitMenuContentPrivate;

class ConfigEditAlarmLimitMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditAlarmLimitMenuContent(Config *const config);
    ~ConfigEditAlarmLimitMenuContent();
    /**
     * @brief setItemFocus
     * @param param
     */
    void setItemFocus(const QString &param);

protected:
    /* reimplement */
    void readyShow();

    /* reimplement */
    void layoutExec();

    /* reimplement */
    void setShowParam(const QVariant &param);

private slots:
    void onbtnClick();
    void onRowClicked(int row);
    void onSelectRowChanged(int row);

private:
    ConfigEditAlarmLimitMenuContentPrivate *const d_ptr;
};

