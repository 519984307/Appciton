/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/



#pragma once
#include <QObject>
#include <QFile>
#include <QTime>
#include <QDir>
#include "Param.h"
#include "Provider.h"
#include "ServiceUpgradeWindow.h"
#include "ServiceUpgrade.h"


// 收到的数据帧类型。
enum UpgradePacketType
{
    UPGRADE_RESPONSE_NACK                            = 0x00,       // NACK应答
    UPGRADE_RESPONSE_ACK                             = 0x01,       // ACK应答

    UPGRADE_PASSTHROUGH_MODE                         = 0x26,       // 透传模式

    UPGRADE_NOTIFY_ALIVE                             = 0x5B,       // 保活帧。
    UPGRADE_ECG_NOTIFY_ALIVE                         = 0xB0,       // ECG保活帧。

    UPGRADE_CMD_ENTER                                = 0xF6,       // 进入升级模式
    UPGRADE_RSP_ENTER                                = 0xF6,       //
    UPGRADE_CMD_SEND                                 = 0xF7,       // 数据发送启动
    UPGRADE_RSP_SEND                                 = 0xF7,       //
    UPGRADE_CMD_READ                                 = 0xF8,       // 升级模式硬件版本号
    UPGRADE_RSP_READ                                 = 0xF8,       //
    UPGRADE_CMD_CLEAR                                = 0xF9,       // 升级模式擦除Flash
    UPGRADE_RSP_CLEAR                                = 0xF9,       //
    UPGRADE_CMD_FILE                                 = 0xFB,       // 文件片段传输
    UPGRADE_RSP_FILE                                 = 0xFC,       //
    UPGRADE_RSP_STATE                                = 0xFD,       // 工作状态
    UPGRADE_ALIVE                                    = 0xFE,       // 升级保活帧
};

/**************************************************************************************************
 * OneShot事件定义，其实也是NIBP的错误码定义。
 *************************************************************************************************/
enum UpgradeState
{
    UPGRADE_BLMLOADER_START,                // BLMLoader启动了
    UPGRADE_BLMLOADER_LOAD,                 // BLMLoader加载应用程序
    UPGRADE_BLMLOADER_UPGRADE,              // BLMLoader请求发送镜像文件片段
    UPGRADE_SIGN_FAIL,                      // 写升级标志失败
    UPGRADE_CLEAR_START,                    // 开始擦除FLASH
    UPGRADE_CLEAR_FAIL,                     // FLASH擦除失败
    UPGRADE_VERSION_SOFTWARE_ERROR,         // 软件镜像名称不匹配
    UPGRADE_VERSION_HARDWARE_ERROR,         // 硬件版本不匹配
    UPGRADE_VERSION_WRITE_FAIL,             // 写设备属性（软件镜像名称+硬件版本）到FLASH失败
    UPGRADE_FILE_NUMBER_ERROR,              // 文件片段的序号错误
    UPGRADE_FILE_FLASH_FAIL,                // 写文件片段到FLASH失败
    UPGRADE_SUCCEESS,                       // 升级完成
    UPGRADE_NR,
};

class QTimer;
class BLMEDUpgradeParam : public Param
{
    #ifdef CONFIG_UNIT_TEST
    friend class TestUpgradeParam;
    #endif
    Q_OBJECT

public:
    static BLMEDUpgradeParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new BLMEDUpgradeParam();
        }
        return *_selfObj;
    }
    static BLMEDUpgradeParam *_selfObj;
    ~BLMEDUpgradeParam();

public:
    void serviceUpgradeStart(void);
    void serviceUpgradeEnter(void);
    void serviceUpgradeSend(void);
    void serviceUpgradeClear(void);
    void serviceUpgradeRead(void);
    void serviceUpgradeWrite(void);
    void serviceUpgradeFile(unsigned char *packet);
    void serviceUpgradeFile(void);
    void sendFile(void);
    void fileClose(void);

    void run(void);
    void connectError(void);
    void unPacket(unsigned char *packet, int len, const QString &name);
    // 进入升级模式
    void sendServiceUpgradeEnter(void);
    bool isServiceUpgradeEnter(unsigned char *packet);

    // 数据发送启动
    void sendServiceUpgradeSend(void);
    bool isServiceUpgradeSend(unsigned char *packet);

    // 升级模式擦除Flash
    void sendServiceUpgradeClear(void);
    bool isServiceUpgradeClear(unsigned char *packet);

    // 升级模式硬件版本号
    void sendServiceUpgradeRead(void);
    bool isServiceUpgradeReade(unsigned char *packet);

    // 文件片段传输
    void sendServiceUpgradeFile(unsigned char *packet);
    bool isServiceUpgradeFile(unsigned char *packet);

    // 升级完成
    void isServiceUpgradeFinish(unsigned char *packet);

    // 开始升级
    void startUpgrade(void);

protected:
    // 设置超时时限，ms单位。
    void setTimeOut(int t = 2000);

    // 是否超时。
    bool isTimeOut(void);

    // 自启动倒计时的时间流逝。
    int elapseTime(void);

    void TimeStop(void);
    bool timestart;
    bool _isUSBExist;                // USB存在标志

    int _find;
    int _relive;

private slots:
    void _restartTimeOut();

private:
    BLMEDUpgradeParam();

    bool _isReturn;
    bool _isEnterSuccess;
    bool _isFinish;
    bool _upgradeTN3DaemonFlag;          // 升级TN3Daemon之前判断TN3是否在bootLoader层
    bool _sendThroughFlag;               // 发送透传命令判断
    unsigned char *_packet;
    int _numFile;
    int _numFe;

    QTime _elapseTime;
    QTimer *_restartTimer;
    int _timeOut;

    QByteArray packetData;
    QFile _file;

    int _fileSize;
    int _fileSum;
    int _fileNum;

    unsigned char _buff[131];
    UpgradeWindowType _type;

    void _USBExist(void);
    void _fileExist(void);

    //按钮显示标志
    void _btnShow(bool flag);

    // 发送协议命令
    void _sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 根据type在path目录下找到正确的文件名，找到返回true。
    bool _findFile(const QString &path, const QString &matchStr, QString &fileName);

    // MD5校验，MD5匹配返回true。
    void _md5Check(const QString &md5, const QString &file);

    // 进入NIBP从芯片升级模式
    void _serviceUpgradeTN3Daemon(bool flag);

    NIBPProviderIFace *_providerNIBP;
};
#define blmedUpgradeParam (BLMEDUpgradeParam::construction())
#define deleteblmedUpgradeParam() (delete BLMEDUpgradeParam::_selfObj)
