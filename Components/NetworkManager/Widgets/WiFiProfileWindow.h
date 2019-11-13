/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#pragma once

#include "Framework/UI/Dialog.h"
#include <QScopedPointer>
#include "WiFiProfileInfo.h"

class WiFiProfileWindowPrivate;
class WiFiProfileWindow: public Dialog
{
    Q_OBJECT
public:
    ~WiFiProfileWindow();
    WiFiProfileWindow();

    /**
     * @brief getCurrentWifiProfile  获取当前的wifi配置
     * @return
     */
    WiFiProfileInfo getCurrentWifiProfile() const;

    /**
     * @brief readyShow
     */
    void readyShow();
    /**
     * @brief layoutExec
     */
    void layoutExec();

signals:
    void selectProfile(WiFiProfileInfo profile);

protected:
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Q_DECLARE_PRIVATE(WiFiProfileWindow)
    Q_DISABLE_COPY(WiFiProfileWindow)

    Q_PRIVATE_SLOT(d_func(), void onProfileSelect(int))
    Q_PRIVATE_SLOT(d_func(), void onWifiConnected(const QString &ssid))
    Q_PRIVATE_SLOT(d_func(), void updateWifiProfileSlot(bool isEnabled))
    Q_PRIVATE_SLOT(d_func(), void onConfigUpdated(void))

    const QScopedPointer<WiFiProfileWindowPrivate> d_ptr;
};

