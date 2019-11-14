/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/18
 **/

#pragma once

#include "Framework/UI/MenuWindow.h"

class MeasureSettingWindow : public MenuWindow
{
    Q_OBJECT
public:
    /**
     * @brief getInstanceget the instance of the measure setting window
     * @return
     */
    static MeasureSettingWindow *getInstance();

private:
    MeasureSettingWindow();
};
