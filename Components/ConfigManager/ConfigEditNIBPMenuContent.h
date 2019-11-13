/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/17
 **/

#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditNIBPMenuContentPrivate;
class ConfigEditNIBPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditNIBPMenuContent(Config *const config, PatientType type = PATIENT_TYPE_ADULT);
    ~ConfigEditNIBPMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

    /**
     * @brief onSpinBoxReleased  spinbox释放槽函数
     * @param value  spinbox值
     */
    void onSpinBoxReleased(int value);

private:
    ConfigEditNIBPMenuContentPrivate *const d_ptr;
};

