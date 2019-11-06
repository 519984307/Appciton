/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/

#include "WiFiProfileInfo.h"
#include <QRegExp>

WiFiProfileInfo::WiFiProfileInfo():
    staticIp(QString::fromLatin1("0.0.0.0")),
    defaultGateway(QString::fromLatin1("0.0.0.0")),
    subnetMask(QString::fromLatin1("255.255.255.0")),
    preferedDNS(QString::fromLatin1("0.0.0.0")),
    alternateDNS(QString::fromLatin1("0.0.0.0")),
    authType(Wpa_psk),
    isStatic(false)
{
}

WiFiProfileInfo::WiFiProfileInfo(const WiFiProfileInfo &other)
    : profileName(other.profileName), ssid(other.ssid), securityKey(other.securityKey),
      staticIp(other.staticIp), defaultGateway(other.defaultGateway), subnetMask(other.subnetMask),
      preferedDNS(other.preferedDNS), alternateDNS(other.alternateDNS), authType(other.authType),
      isStatic(other.isStatic)
{
}

WiFiProfileInfo &WiFiProfileInfo::operator=(const WiFiProfileInfo &other)
{
    profileName = other.profileName;
    ssid = other.ssid;
    securityKey = other.securityKey;
    staticIp = other.staticIp;
    defaultGateway = other.defaultGateway;
    subnetMask = other.subnetMask;
    preferedDNS = other.preferedDNS;
    alternateDNS = other.alternateDNS;
    authType = other.authType;
    isStatic = other.isStatic;

    return *this;
}

bool WiFiProfileInfo::isValid() const
{
    if (!isStatic)
    {
        return !profileName.isEmpty() && !ssid.isEmpty() && authType >= Open && authType <= Wpa2_psk;
    }

    return !profileName.isEmpty() && !ssid.isEmpty() && authType >= Open && authType <= Wpa2_psk
            && isIpStrValid(staticIp) && isIpStrValid(defaultGateway) && isIpStrValid(subnetMask)
            && isIpStrValid(preferedDNS) && isIpStrValid(alternateDNS);
}

bool WiFiProfileInfo::isIpStrValid(const QString &ipStr)
{
    QRegExp reg("^(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])$");
    return reg.exactMatch(ipStr);
}
