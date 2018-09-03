/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/3
 **/
#pragma once

#include "MenuContent.h"

class SystemMaintenancePrivate;
class SystemMaintenance : public MenuContent
{
    Q_OBJECT
public:
    SystemMaintenance();
    ~SystemMaintenance();

protected:
    /* reimplement */
    void readyShow();
    /* reimplement */
    void layoutExec();

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    SystemMaintenancePrivate *const d_ptr;
};


