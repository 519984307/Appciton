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

#include "MenuContent.h"
#include "WiFiProfileEditor.h"
#include <QScopedPointer>

class WiFiProfileMenuContentPrivate;
class WiFiProfileMenuContent: public MenuContent
{
    Q_OBJECT
public:
    WiFiProfileMenuContent();
    ~WiFiProfileMenuContent();

    WiFiProfileInfo getCurrentWifiProfile() const;

    virtual void readyShow();
    virtual void layoutExec();
signals:
    void selectProfile(WiFiProfileInfo profile);

protected:
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Q_DECLARE_PRIVATE(WiFiProfileMenuContent)
    Q_DISABLE_COPY(WiFiProfileMenuContent)

    Q_PRIVATE_SLOT(d_func(), void onProfileSelect(int))
    Q_PRIVATE_SLOT(d_func(), void onWifiConnected(const QString &ssid))
    Q_PRIVATE_SLOT(d_func(), void updateWifiProfileSlot(bool isEnabled))
    Q_PRIVATE_SLOT(d_func(), void onConfigUpdated(void))

    const QScopedPointer<WiFiProfileMenuContentPrivate> d_ptr;
};

