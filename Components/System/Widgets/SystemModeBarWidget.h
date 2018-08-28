/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/28
 **/


#pragma once
#include "IWidget.h"
#include "SystemManager.h"

class QLabel;
/***************************************************************************************************
 * 显示当前的工作模式。
 **************************************************************************************************/
class PopupList;
class SystemModeBarWidget : public IWidget
{
    Q_OBJECT

public:
    explicit SystemModeBarWidget(QWidget *parent = 0);
    void setMode(UserFaceType mode);

private slots:
    void _releaseHandle(IWidget *);
    void _popupListDestroy(void);
    void _getListIndex(int);

private:
    QLabel *_modeLabel;
    PopupList *_systemModeList;
    int _popupListIndex;
    void _popupListAddItem(void);
};
