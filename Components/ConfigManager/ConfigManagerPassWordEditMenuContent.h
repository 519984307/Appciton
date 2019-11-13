/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/17
 **/

#pragma once

#include "Framework/UI/MenuContent.h"

class ConfigManagerPassWordEditMenuContentPrivate;
class ConfigManagerPassWordEditMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ConfigManagerPassWordEditMenuContent();
    ~ConfigManagerPassWordEditMenuContent();

protected:
    /* reimplment */
    void layoutExec();

private slots:
    /**
     * @brief onBtnReleased  按钮槽函数触发
     */
    void onBtnReleased(void);

private:
    ConfigManagerPassWordEditMenuContentPrivate *const d_ptr;
};
