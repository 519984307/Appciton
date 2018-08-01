/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2019/7/24
 **/

#pragma once
#include "MenuContent.h"


class NIBPManometerContentPrivate;
class NIBPManometerContent : public MenuContent
{
    Q_OBJECT

public:
    static NIBPManometerContent *getInstance();
    ~NIBPManometerContent();

    /**
     * @brief setCuffPressure    压力
     * @param pressure
     */
    void setCuffPressure(int pressure);
    /**
     * @brief unPacket  模式应答
     * @param flag
     */
    void unPacket(bool flag);

    /**
     * @brief init  初始化
     */
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem() {}
    virtual bool focusNextPrevChild(bool next);

private:
    NIBPManometerContent();

    NIBPManometerContentPrivate *const d_ptr;
};
