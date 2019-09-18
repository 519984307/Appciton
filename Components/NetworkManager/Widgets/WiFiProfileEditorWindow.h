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
#include "Dialog.h"
#include <QScopedPointer>
#include <QMetaType>


class WiFiProfileWindowInfo {
public:
    enum AuthenticationType{
        Open,
        Wpa_psk,
        Wpa2_psk
    };

    WiFiProfileWindowInfo();
    WiFiProfileWindowInfo(const WiFiProfileWindowInfo &other);
    WiFiProfileWindowInfo& operator= (const WiFiProfileWindowInfo &other);

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

Q_DECLARE_METATYPE(WiFiProfileWindowInfo);

class WiFiProfileEditorWindowPrivate;
class WiFiProfileEditorWindow : public Dialog
{
    Q_OBJECT
public:
    explicit WiFiProfileEditorWindow(const WiFiProfileWindowInfo &profile = WiFiProfileWindowInfo());
     ~WiFiProfileEditorWindow();

     // get the edit result
     WiFiProfileWindowInfo getProfileInfo() const;

 protected:
     void keyPressEvent(QKeyEvent *event);
     const QScopedPointer<WiFiProfileEditorWindowPrivate> d_ptr;

 private:
     Q_PRIVATE_SLOT(d_func(), void onSwitch(int index))
     Q_PRIVATE_SLOT(d_func(), void editIpAddress())
     Q_PRIVATE_SLOT(d_func(), void editNormalText())
     Q_PRIVATE_SLOT(d_func(), void onCancel())
     Q_PRIVATE_SLOT(d_func(), void onCommit())
     Q_DISABLE_COPY(WiFiProfileEditorWindow)
     Q_DECLARE_PRIVATE(WiFiProfileEditorWindow)
};
