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
#include "MenuWindow.h"
#define InvStr() ("---")

class NIBPRepairMenuWindowPrivate;
class NIBPRepairMenuWindow : public MenuWindow
{
    Q_OBJECT

public:
    static NIBPRepairMenuWindow* getInstance();
    ~NIBPRepairMenuWindow();

    /**
     * @brief init  初始化
     */
    void init();

    /**
     * @brief unPacket  进入模式的应答
     * @param flag
     */
    void unPacket(bool flag);

    /**
     * @brief returnMenu  返回到菜单列表项。
     */
    void returnMenu(void);
    /**
     * @brief messageBox
     */
    void messageBox(void);
    /**
     * @brief warnShow
     * @param enable
     */
    void warnShow(bool enable);

    /**
     * @brief getRepairError  获取维护模式是否错误
     * @return
     */
    bool getRepairError(void);

signals:
    void foucsChange();

protected:
    /**
     * @brief hideEvent
     * @param event
     */
    virtual void hideEvent(QHideEvent *event);

private slots:
    /**
     * @brief closeSlot  关闭窗口
     */
    void closeSlot();

private:
    NIBPRepairMenuWindow();

    NIBPRepairMenuWindowPrivate *const d_ptr;
};
