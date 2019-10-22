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
#include "Dialog.h"
#include "AlarmDefine.h"
#include <QModelIndex>

class AlarmInfoWindowPrivate;
class AlarmInfoWindow : public Dialog
{
    Q_OBJECT
public:
    AlarmInfoWindow(const QString &title, AlarmType type);
    ~AlarmInfoWindow();

    /**
     * @brief updateData 更新数据
     * @param isShowFirstPage false:更新当前页数据　true:更新到第一页
     */
    void updateData(bool isShowFirstPage = false);

protected:
    /**
     * @brief layout 界面布局
     */
    void layout();

    void showEvent(QShowEvent *ev);

private slots:
    void _onBtnRelease();
    void _accessEventWindow(int index);

private:
    AlarmInfoWindowPrivate *const d_ptr;
};
