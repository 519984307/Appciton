/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#pragma once
#include "MenuContent.h"

class NIBPZeroPointContentPrivate;
class NIBPZeroPointContent : public MenuContent
{
    Q_OBJECT

public:
    static NIBPZeroPointContent *getInstance();
    ~NIBPZeroPointContent();

public:
    /**
     * @brief setCuffPressure  压力计压力。
     * @param pressure
     */
    void setCuffPressure(int pressure);

    /**
     * @brief unPacket  进入模式应答
     * @param flag
     */
    void unPacket(bool flag);

    /**
     * @brief startSwitch  压力值
     * @param flag
     */
    void startSwitch(bool flag);

    /**
     * @brief init  初始化
     */
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem() {}
    virtual bool focusNextPrevChild(bool next);
    virtual void showEvent(QShowEvent *ev);
private slots:
    /**
     * @brief onBtnReleased  关闭气阀、校零按钮信号槽
     */
    void onBtnReleased(void);
private:
    NIBPZeroPointContent();

    /**
     * @brief onStartReleased  关闭气阀指令
     */
    void onStartReleased(void);
    /**
     * @brief onZeroReleased  校零指令
     */
    void onZeroReleased(void);

    NIBPZeroPointContentPrivate *const d_ptr;
};



