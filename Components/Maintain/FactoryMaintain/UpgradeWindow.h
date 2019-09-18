/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/11
 **/

#pragma once
#include "Dialog.h"
#include "UpgradeManager.h"

class UpgradeWindowPrivate;
class UpgradeWindow : public Dialog
{
    Q_OBJECT
public:
    UpgradeWindow();
    ~UpgradeWindow();

    /* reimplement */
    void exec();

protected:
    void timerEvent(QTimerEvent *ev);
    void hideEvent(QHideEvent *ev);

public slots:
    void upgradeMessageUpdate(const QString &msg);
    void upgradeProgressChanged(int value);
    void onUpgradeFinished(UpgradeManager::UpgradeResult result);

private slots:
    void onStartBtnClick();

private:
    UpgradeWindowPrivate * const d_ptr;
};
