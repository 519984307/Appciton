/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/
#pragma once
#include <QMetaType>
#include <QString>


class WiFiProfileInfo {
public:
    enum AuthenticationType{
        Open,
        Wpa_psk,
        Wpa2_psk
    };

    /**
     * @brief WiFiProfileInfo constructor
     */
    WiFiProfileInfo();

    ~WiFiProfileInfo() {}

    /**
     * @brief WiFiProfileInfo copy constructor
     * @param other
     */
    WiFiProfileInfo(const WiFiProfileInfo &other);

    /* assignment override */
    WiFiProfileInfo& operator= (const WiFiProfileInfo &other);

    /**
     * @brief isValid check whether the profile is valie
     * @return true if valid
     */
    bool isValid() const;

    /**
     * @brief isIpStrValid check the ip string is valid or not
     * @param ipStr the input address string
     * @return  true if valid
     */
    static bool isIpStrValid(const QString &ipStr);

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

Q_DECLARE_METATYPE(WiFiProfileInfo)
