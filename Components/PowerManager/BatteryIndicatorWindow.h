/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/23
 **/

#include "Dialog.h"

class BatteryIndicatorWindowPrivate;
class BatteryIndicatorWindow : public Dialog
{
public:
    BatteryIndicatorWindow();
    ~BatteryIndicatorWindow();

private:
    BatteryIndicatorWindowPrivate *d_ptr;
};
