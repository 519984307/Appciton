/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/30
 **/

#pragma once
#include "MenuContent.h"

class SystemNightModeMenuContentPrivate;
class SystemNightModeMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SystemNightModeMenuContent();
    ~SystemNightModeMenuContent();

protected:
    /*reimplement*/
    void readyShow();

    /*reimplement*/
    void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged  下拉框选项改变槽函数
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief OnBtnReleased  按钮释放槽函数
     */
    void OnBtnReleased(void);

private:
    SystemNightModeMenuContentPrivate *const d_ptr;
};

