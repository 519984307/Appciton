#pragma once

#include <QMetaType>

/***************************************************************************************************
 * 该头文件主要定义了网络管理相关的一些公共的数据结构和枚举类型
 **************************************************************************************************/
#define EMPTY_IP_STR    ("0.0.0.0")

#define SIGNAL_MAX      (5)
#define SIGNAL_MIN      (1)
#define NO_SIGNAL       (0)    // Network disconnected
#define MIN_RSSI        (-100) // Anything worse than or equal to this will show 0 bars.
#define MAX_RSSI        (-55)  // Anything better than or equal to this will show the max bars.


// 定时器相关
#define QUERY_TIMEOUT       (2000)  // 2s
#define OPEN_TIMEOUT        (3000)  // 3s
#define SCAN_TIMEOUT        (10000) // 10s
#define MAX_WAIT_TIME       (20000) // 20s
#define MAX_QUERY_TIMES     (MAX_WAIT_TIME / QUERY_TIMEOUT)

// 网络类型
enum NetworkType
{
    NETWORK_WIRED = 0,      // 有线
    NETWORK_MOBILEDATA,     // 移动数据
    NETWORK_WIFI            // WiFi
};

static QString netTypeStr[] = {"wired", "mobiledata", "wifi"};

// 获取IP地址的方式
enum IpMode
{
    IPMODE_DHCP,            // DHCP
    IPMODE_STATIC ,     // 静态IP
};

// 移动数据网络制式
enum NetMode
{
    UNKNOW = 0,
    CDMA2000,
    WCDMA,
    GPRS,
    MAX_MODE
};

static QString netModeStr[MAX_MODE] = {"", "cdma2000", "wcdma", "cmnet"};

// 网络制式对应的运营商字符串
static QString operatorStr[MAX_MODE] = {
    "Unknow",
    "ChinaTelecom",
    "ChinaUnicom",
    "ChinaMobile"
};

// WiFi接入点的加密类型
enum EncryptType
{
    OPEN_AP = 0,    // 开放WiFi接入点，无密码
    WEP_AP,         // WEP加密的WiFi接入点
    WPA_AP          // WPA/WPA2加密的WiFi接入点
};

// 联网错误码
enum NetworkError
{
    // 无错误
    NO_ERROR = 0,
    // 未检测到无线网卡
    NO_WIRELESS_ADAPTER,
    // 启动wpa_supplicant后台服务进程失败
    WIFI_START_WPA_SUPPLICANT_FAIL,
    // 连接WiFi接入点时，密码认证错误
    WIFI_PWD_ERROR,
    // 连接WiFi接入点时，认证超时
    WIFI_AUTH_TIMEOUT,
    // 连接已保存的网络失败，WiFi 接入点信息可能已变更
    WIFI_AP_PWD_CHANGED,
    // WiFi连接已断开
    WIFI_AP_DISCONNECT,
    // 获取动态IP地址失败
    GET_DYMATIC_IP_FAIL,
    // 移动数据正在重新拨号
    DATA_REDIALING,
    // 未检测到SIM卡
    DATA_NO_SIM_CARD,
    // 未检测到运营商
    DATA_NO_OPERATIR,
    // 移动数据联网需要重启才能拨号
    DATA_NEED_REBOOT,
    // 网络已禁用
    NETWORK_DISABLED
};

// WiFi接入点状态
enum ApStatus
{
    AP_UNCONNECTED = 0,
    AP_CONNECTING,
    AP_CONNECTED
};

// WiFi接入点信息结构体
struct ApInfo
{
    ApInfo()
    {
        signalLevel = 0;
        encType = OPEN_AP;
        status = AP_UNCONNECTED;
        isSaved = false;
        netId = -1;
        updateFlag = -1;
    }

    QString ssid;           // ssid
    QString bssid;          // bssid
    int signalLevel;        // 信号强度, 1-5
    EncryptType encType;    // 加密类型
    ApStatus status;        // 接入点状态
    bool isSaved;           // 是否已保存密码
    int netId;              // 接入点的网络Id,如果连接失败用来删除network
    int updateFlag;         // 接入点更新标志
};

// WiFi接入点列表
typedef QList<ApInfo*>  ApList;

// 正在连接的接入点信息结构体
typedef struct
{
    QString ssid;         // ssid
    QString pwd;          // 认证密码
    EncryptType encType;  // 加密类型
    int netId;            // 当前接入点的网络Id（如果已保存密码）
} ConnectApInfo;

//连接认证类型
enum MailConnectSecurity
{
    MAIL_CONNECT_SECURITY_NONE,
    MAIL_CONNECT_SECURITY_TLS,
    MAIL_CONNECT_SECURITY_TTL
};

// 自定义信号槽传递自定义参数时必须声明
Q_DECLARE_METATYPE(ApInfo)
Q_DECLARE_METATYPE(ApList)
