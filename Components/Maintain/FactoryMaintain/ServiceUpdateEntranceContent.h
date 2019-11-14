/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class ServiceUpdateEntranceContent  : public MenuContent
{
    Q_OBJECT
public:
    ServiceUpdateEntranceContent();
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow(void);
private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased();
};
