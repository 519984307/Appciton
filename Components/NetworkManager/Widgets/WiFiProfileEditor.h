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
#include "PopupWidget.h"
#include <QScopedPointer>
#include <QMetaType>


class WiFiProfileInfo {
public:
    enum AuthenticationType{
        Open,
        Wpa_psk,
        Wpa2_psk
    };

    WiFiProfileInfo();
    WiFiProfileInfo(const WiFiProfileInfo &other);
    WiFiProfileInfo& operator= (const WiFiProfileInfo &other);

    bool isValid() const;

    QString profileName;
    QString ssid;
    QString securityKey;
    QString staticIp;
    QString defaultGateway;
    QString subnetMask;
    QString preferedDNS;
    QString alternateDNS;
    AuthenticationType authType;
    bool isStatic;
};

Q_DECLARE_METATYPE(WiFiProfileInfo);

class WiFiProfileEditorPrivate;
class WiFiProfileEditor: public PopupWidget
{
    Q_OBJECT
public:
   explicit WiFiProfileEditor(const WiFiProfileInfo &profile = WiFiProfileInfo());
    ~WiFiProfileEditor();

    // get the edit result
    WiFiProfileInfo getProfileInfo() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    const QScopedPointer<WiFiProfileEditorPrivate> d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void onSwitch(int index))
    Q_PRIVATE_SLOT(d_func(), void editIpAddress())
    Q_PRIVATE_SLOT(d_func(), void editNormalText())
    Q_PRIVATE_SLOT(d_func(), void onCancel())
    Q_PRIVATE_SLOT(d_func(), void onCommit())
    Q_DISABLE_COPY(WiFiProfileEditor)
    Q_DECLARE_PRIVATE(WiFiProfileEditor)
};
