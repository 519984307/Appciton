/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class MachineConfigModuleContentPrivte;
class MachineConfigModuleContent : public MenuContent
{
    Q_OBJECT
public:
    MachineConfigModuleContent();
    ~MachineConfigModuleContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

    /* reimplment */
    virtual void hideEvent(QHideEvent *e);

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);

private:
    MachineConfigModuleContentPrivte *const d_ptr;
};

