/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/26
 **/

#pragma once
#include "Framework/UI/Dialog.h"
#include "PatientDefine.h"

class PatientTypeSelectWindowPrivate;
class PatientTypeSelectWindow : public Dialog
{
    Q_OBJECT
public:
    PatientTypeSelectWindow();
    ~PatientTypeSelectWindow();

    /**
     * @brief getConfigInfo 获取配置信息
     * @param patientType 病人类型
     * @param configPath 完整的配置文件路径
     */
    void getConfigInfo(PatientType *patientType, QString *configPath) const;

private slots:
    /**
     * @brief updateBtnStatus  更新按钮状态
     */
    void updateBtnStatus();

    /**
     * @brief onBtnReleased  按钮触发槽函数
     */
    void onBtnReleased();

private:
    PatientTypeSelectWindowPrivate *const d_ptr;
};

