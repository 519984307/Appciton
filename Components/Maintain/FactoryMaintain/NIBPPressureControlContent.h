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

protected:
    virtual void layoutExec(void);
    void timerEvent(QTimerEvent *ev);

private slots:
    void inflateBtnReleased(void);            //  充气、放气控制按钮信号

    /**
     * @brief enterPressureContrlReleased 进入退出压力操纵模式
     */
    void enterPressureContrlReleased(void);

private:
    NIBPPressureControlContent();

    NIBPPressureControlContentPrivate *const d_ptr;
};

