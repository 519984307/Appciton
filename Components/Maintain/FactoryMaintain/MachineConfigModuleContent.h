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
#include "MenuContent.h"

enum ModuleStatus
{
    MODULE_DISABLE = 0,
    MODULE_ENABLE,
};

enum CO2ModuleStatus
{
    CO2MODULE_DISABLE = 0,
    CO2mODULE_PHASEIN,
};

enum COModuleStatus
{
    COMODULE_DISABLE = 0,
    COMODULE_M601,
};

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

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);

private:
    MachineConfigModuleContentPrivte *const d_ptr;
};

