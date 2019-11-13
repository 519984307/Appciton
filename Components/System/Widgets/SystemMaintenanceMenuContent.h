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

#include "Framework/UI/MenuContent.h"

class SystemMaintenanceMenuContentPrivate;
class SystemMaintenanceMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SystemMaintenanceMenuContent();
    ~SystemMaintenanceMenuContent();

protected:
    /* reimplement */
    void layoutExec();

    /* reimplement */
    void readyShow();

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    SystemMaintenanceMenuContentPrivate *const d_ptr;
};


