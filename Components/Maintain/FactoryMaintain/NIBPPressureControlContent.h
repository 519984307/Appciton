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
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include "ComboBox.h"
#include "Button.h"
#include "SpinBox.h"

class NIBPPressureControlContentPrivate;
class NIBPPressureControlContent : public MenuContent
{
    Q_OBJECT
public:
    static NIBPPressureControlContent *getInstance();
    ~NIBPPressureControlContent();

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
     * @brief btnSwitch   充气/放气按钮切换
     * @param inflate
     */
    void btnSwitch(bool inflate);

    /**
     * @brief init   初始化
     */
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem() {}
    virtual bool focusNextPrevChild(bool next);

private slots:
    void inflateBtnReleased(void);            //  充气、放气控制按钮信号
    void pressureChange(int value);             //  充气压力值
    void patientInflate(int index);           //  病人类型信号

private:
    NIBPPressureControlContent();

    void inflateReleased(void);        //  充气指令
    void deflateReleased(void);        //  放气指令
    void changeReleased(void);         //  病人类型，充气值改变指令

    NIBPPressureControlContentPrivate *const d_ptr;
};

