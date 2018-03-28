#pragma once
#include "NetworkDefine.h"

class NetworkSymble
{
public:
    NetworkSymble() {}
    ~NetworkSymble() {}

    static const char *convert(NetworkType type)
    {
        const char *networkType[] =
        {
            "wired", "mobiledata", "wifi"
        };

        return networkType[type];
    }

    static const char *convert(NetMode mode)
    {
        const char *networkMode[] =
        {
            "", "cdma2000", "wcdma", "cmnet"
        };

        return networkMode[mode];
    }

    static const char *convert(EncryptType type)
    {
        const char *networkEncryptType[] =
        {
            "None", "WPA PSK", "WPA2 PSK"
        };

        return networkEncryptType[type];
    }

    static const char *convert(IpMode mode)
    {
        const char *ipMode[] =
        {
            "DHCP", "StaticIp"
        };

        return ipMode[mode];
    }

    static const char *convert(MailConnectSecurity type)
    {
        const char *connectSecurity[] =
        {
            "None", "TLS", "TTL"
        };

        return connectSecurity[type];
    }
};

