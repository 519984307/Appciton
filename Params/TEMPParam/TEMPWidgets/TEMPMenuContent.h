/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#pragma once

#include "Framework/UI/MenuContent.h"
#include "TEMPDefine.h"

class TEMPMenuContentPrivate;
class TEMPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    TEMPMenuContent();
    ~TEMPMenuContent();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief readyShow
     */
    virtual void readyShow() {}

signals:
    /**
     * @brief updateTempName 更新温度通道名称信号
     * @param channel 通道号
     * @param TEMPChannelType 通道类型
     */
    void updateTempName(TEMPChannelIndex channel, TEMPChannelType type);

private slots:
    /**
     * @brief onComboIndexUpdated
     * @param index
     */
    void onComboIndexUpdated(int index);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    TEMPMenuContentPrivate *const d_ptr;
};

