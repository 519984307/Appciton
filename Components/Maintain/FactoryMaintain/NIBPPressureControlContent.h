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

    void init(void);

protected:
    virtual void layoutExec(void);
    void LayoutExec(void);

    void timerEvent(QTimerEvent *ev);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void inflateBtnReleased(void);            //  充气、放气控制按钮信号

    /**
     * @brief onOverpressureReleased 过压保护开关
     */
    void onOverpressureReleased(int);

    void enterPressureContrlReleased();

    void onComboBoxIndexChanged(int index);

private:
    NIBPPressureControlContent();

    NIBPPressureControlContentPrivate *const d_ptr;
};

