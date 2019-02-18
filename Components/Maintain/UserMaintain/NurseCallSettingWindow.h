/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/16
 **/
#include "Dialog.h"

class NurseCallSettingWindowPrivate;
class NurseCallSettingWindow : public Dialog
{
    Q_OBJECT
public:
    NurseCallSettingWindow();
    ~NurseCallSettingWindow();

    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    NurseCallSettingWindowPrivate *const d_ptr;
};
