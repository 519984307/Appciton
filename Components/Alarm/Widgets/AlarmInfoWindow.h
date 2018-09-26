/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/

#pragma once
#include "Window.h"
#include "AlarmDefine.h"

class AlarmInfoWindowPrivate;
class AlarmInfoWindow : public Window
{
    Q_OBJECT
public:
    AlarmInfoWindow(const QString &title, AlarmType type);
    ~AlarmInfoWindow();

    void updateData();

protected:
    /**
     * @brief layout 界面布局
     */
    void layout();

    void showEvent(QShowEvent *ev);

private slots:
    void _onBtnRelease();

private:
    AlarmInfoWindowPrivate *const d_ptr;
};
