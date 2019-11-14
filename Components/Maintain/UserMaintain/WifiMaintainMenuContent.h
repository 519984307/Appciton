/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#pragma once
#include <QScopedPointer>
#include "Framework/UI/MenuContent.h"

class WifiMaintainMenuContentPrivate;
class WifiMaintainMenuContent: public MenuContent
{
    Q_OBJECT
public:
    WifiMaintainMenuContent();
    ~WifiMaintainMenuContent();

    void focusFirstItem();
protected:
    void layoutExec();
    void readyShow();
    const QScopedPointer<WifiMaintainMenuContentPrivate> d_ptr;
signals:
    void updateWifiProfileSignal(bool isEnabled);

private:
    Q_DECLARE_PRIVATE(WifiMaintainMenuContent)
    Q_DISABLE_COPY(WifiMaintainMenuContent)

    Q_PRIVATE_SLOT(d_func(), void onSwitch(int))
    Q_PRIVATE_SLOT(d_func(), void onBtnClick())
    Q_PRIVATE_SLOT(d_func(), void updateBtnStatus())
};
