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

#include "MenuContent.h"

#define PRINT_WAVE_NUM 3

class PrintSettingMenuContentPrivate;
class PrintSettingMenuContent : public MenuContent
{
    Q_OBJECT
public:
    PrintSettingMenuContent();
    ~PrintSettingMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    /**
     * @brief onComboxIndexChanged
     * @param index
     */
    void onComboxIndexChanged(int index);

    /**
     * @brief onSelectWaveChanged
     * @param waveName
     */
    void onSelectWaveChanged(const QString &waveName);

private:
    PrintSettingMenuContentPrivate *const d_ptr;
};
