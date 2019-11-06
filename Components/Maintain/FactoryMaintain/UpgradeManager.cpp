/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/10
 **/

#include "UpgradeManager.h"
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QCryptographicHash>
#include <stdio.h>
#include "USBManager.h"
#include "ParamManager.h"
#include "Debug.h"
#include "Framework/Utility/Utility.h"
#include <QFile>
#include <QImage>
#include <QApplication>
#include <QDesktopWidget>
#include "LanguageManager.h"
#include "BLMCO2Provider.h"
#include "IConfig.h"

#define UPGRADE_FILES_DIR "/upgrade/"
#define DEFAULT_HW_VER_STR "1.0A"
#define DEFAULT_SW_VER_STR "1.0A"
#define LOGO_BIN_FILE "logo.bin"

enum UpgradeState
{
    STATE_IDLE,
    STATE_CHECK_UDISK,
    STATE_CHECK_FILE,
    STATE_CHECKSUM,
    STATE_REQUEST_ENTER_UPGRADE_MODE,
    STATE_REQUEST_SEND_DATA,
    STATE_WAIT_FOR_REQUEST_IMAGE,
    STATE_WRITE_HARDWARE_ATTRIBUTE,
    STATE_WRITE_IMAGE_SEGMENT,
    STATE_WAIT_FOR_COMPLETE_MSG,
    STATE_REBOOT,
    STATE_UPGRADE_LOGO,
    STATE_UPGRADE_ENTER_PASSTHROUGH_MODE,
    STATE_UPGRADE_EXIT_PASSTHROUGH_MODE
};

enum UpgradePacketType
{
    UPGRADE_RSP_NACK    = 0x00,         // nack response
    UPGRADE_RSP_ACK     = 0x01,         // ack response

    UPGRADE_N5_CMD_PASSTHROUGH_MODE = 0x25,  // 透传模式。
    UPGRADE_N5_RSP_PASSTHROUGH_MODE = 0x26,

    UPGRADE_NOTIFY_ALIVE = 0x5B,        // module alive packet
    UPGRADE_E5_NOTIFY_ALIVE = 0xB0,     // E5 module alive packet

    UPGRADE_CMD_REQUEST_ENTER  = 0xF6,  // enter upgrade mode
    UPGRADE_RSP_ENTER   = 0xF6,         // response of enter upgrade mode

    UPGRADE_CMD_REQUEST_SEND = 0xF7,    // send data;
    UPGRADE_RSP_SEND    = 0xF7,         // response of send data

    UPGRADE_CMD_HW_ATTR = 0xF8,         // read or write module attritube
    UPGRADE_RSP_HW_ATTR = 0xF8,         // response

    UPGRADE_CMD_ERASE   = 0xF9,         // Erase flash
    UPGRADE_RSP_ERASE   = 0xF9,         // response of erase flash

    UPGRADE_CMD_SEGMENT = 0xFB,         // send upgrade segment
    UPGRADE_RSP_SEGMENT = 0xFC,         // response of upgrade segment

    UPGRADE_NOTIFY_STATE = 0xFD,        // module upgrade state

    UPGRADE_NOTIFY_UPGRADE_ALIVE = 0xFE,  // upgrade moudle alive packet
};

enum UpgradeErrorType
{
    UPGRADE_ERR_NONE                    = 0,
    UPGRADE_ERR_NO_UDISK                = 1,
    UPGRADE_ERR_NO_UPGRADE_FILE         = 2,
    UPGRADE_ERR_NO_CHECKSUM_FILE        = 3,
    UPGRADE_ERR_READ_FILE_FAIL          = 4,
    UPGRADE_ERR_CHECKSUM_FAIL           = 5,
    UPGRADE_ERR_MODULE_NOT_FOUND        = 6,
    UPGRADE_ERR_MODULE_INITIALIZE_FAIL  = 7,
    UPGRADE_ERR_MODULE_ERASE_FLASH_FAIL = 8,
    UPGRADE_ERR_IMAGE_FILE_UNMATCH      = 9,
    UPGRADE_ERR_HARDWARE_VERION_UNMATCH = 10,
    UPGRADE_ERR_WRITE_ATTR_FAIL         = 11,
    UPGRADE_ERR_WRITE_SEGMENT_FAIL      = 12,
    UPGRADE_ERR_COMMUNICATION_FAIL      = 13,
    UPGRADE_ERR_WRITE_FAIL              = 14,
    UPGRADE_ERR_PASSTHROUGH_MODE_FAIL   = 15,
    UPGRADE_ERR_FLASH_CRC               = 16,
    UPGRADE_ERR_NR,
};

static const char *errorString(UpgradeErrorType errorType)
{
    static const char *errors[UPGRADE_ERR_NR] =
    {
        "",
        "UDiskNotFound",
        "UpgradeFileNotFound",
        "ChecksumFileNotFound",
        "ReadFileFailed",
        "ChecksumFailed",
        "ModuleNotFound",
        "ModuleInitializeFail",
        "ModuleEraseFlashFail",
        "ImageFileUnmatch",
        "HardwareVersionUnmatch",
        "WriteDeviceAttributeFail",
        "WriteSegmentFail",
        "CommunicationFail",
        "WriteFail",
        "SwitchPassthroughModeFail",
        "FlashCRCError",
    };
    return errors[errorType];
}

enum ModuleState
{
    MODULE_STAT_BOOTLOADER_START = 0,       // bootloader start
    MODULE_STAT_APPLICANT_START,            // bootloader start the applicantion
    MODULE_STAT_REQUEST_IMAGE_SEGMENT,      // booloader request receive data segment
    MODULE_STAT_WRITE_UPGRADE_FLAG_FAIL,    // write upgrade flag failed
    MODULE_STAT_ERASE_FLASH,                // bootloader is erasing the flash
    MODULE_STAT_ERASE_FLASH_FAIL,           // bootloader erase the flash failed
    MODULE_STAT_IMAGE_NAME_UNMATCH,         // the image name is unmatch
    MODULE_STAT_HARDWARE_VER_UNMATCH,       // hardware version unmatch
    MODULE_STAT_WRITE_DEVICE_ATTR_FAIL,          // write device attribute
    MODULE_STAT_IMAGE_SEQ_ERROR,            // image sequence number error
    MODULE_STAT_WRITE_IMAGE_SEGMENT_FAIL,   // write image segment failed
    MODULE_STAT_WRITE_UPGRADE_COMPLETE,     // upgrade complete
    MODULE_STAT_ENTER_PASSTHROUGH_MODE,     // nibp enter passthrough mode
    MODULE_STAT_EXIT_PASSTHROUGH_MODE,      // nibp exit passthrough mode
    MODULE_STAT_CRC16_CHECK_ERROR,          // flash crc16 校验错误
};

// 透传模式
enum NIBPPassthrough
{
    NIBP_PASSTHROUGH_OFF,
    NIBP_PASSTHROUGH_ON,
};

class UpgradeManagerPrivate
{
    Q_DECLARE_PUBLIC(UpgradeManager)
public:
    explicit UpgradeManagerPrivate(UpgradeManager *const q_ptr)
        : q_ptr(q_ptr),
          type(UpgradeManager::UPGRADE_MOD_NONE),
          state(STATE_IDLE),
          provider(NULL),
          co2Provider(NULL),
          noResponseTimer(NULL),
          noResponeCount(0),
          segmentSeq(0),
          isPassthroughMode(false)
    {}

    /**
     * @brief checkUpgradeFile check the upgrade file is exist or not
     * @return true if the upgrade file is exist
     */
    bool checkUpgradeFile();

    /**
     * @brief getUpgradeFile find the upgrade file in the usb disk base on the upgrade module name
     * @return the upgrade file name
     */
    QString getUpgradeFile() const;

    /**
     * @brief calcChecksum calcuate the md5 checksum
     * @param content the content need to calcualte the md5
     * @return the md5 reuslt
     */
    static QString calcChecksum(const QByteArray &content);

    /**
     * @brief getProviderName get the provder name base on the upgrade module type
     * @param type the upgrade module type
     * @return the name of the provider
     */
    static QString getProviderName(UpgradeManager::UpgradeModuleType type);

    /**
     * @brief upgradeExit exit upgrade stat
     * @param result the upgrade result
     * @param error the upgrade error
     */
    void upgradeExit(UpgradeManager::UpgradeResult result, UpgradeErrorType error);

    /**
     * @brief handleAck handle the command ack response
     * @param data the ack packet data field
     * @param len  the data field length
     */
    void handleAck(unsigned char *data, int len);

    /**
     * @brief hanleNack handle the command nack response
     * @param data the nack packet data field
     * @param len the data field length
     */
    void hanleNack(unsigned char *data, int len);

    /**
     * @brief handleSegmentRsp handle the image segment response
     * @param data the data field of the response packet
     * @param len the data field length
     */
    void handleSegmentRsp(unsigned char *data, int len);

    /**
     * @brief handleStateChanged handle the module statte changed
     * @param modState
     */
    void handleStateChanged(ModuleState modState);

    /**
     * @brief handleUpgradeAlivePacket handle the module upgrade alive packet
     */
    void handleUpgradeAlivePacket();

    /**
     * @brief resetTimer no response timer reset
     */
    void resetTimer()
    {
        noResponseTimer->stop();
        noResponeCount = 0;
    }

    /**
     * @brief getImageSegment get the image segment base on the sequence number
     * @note Each segment contains 131 bytes, if the first byte is 1, this segment is the last segment;
     *       otherwise, the first byte is 0. the second and third byte represent the sequence number.
     *       the last 128 represent the image data.
     *       Specially, when the sequence number is 0, the segmnet represent the image infomation, see
     *       code for detail
     * @param seqNum the sequence number
     * @return the image segment
     */
    QByteArray getImageSegment(int seqNum);

    UpgradeManager *const q_ptr;
    UpgradeManager::UpgradeModuleType type;
    UpgradeState state;
    QByteArray fileContent;
    BLMProvider *provider;
    BLMCO2Provider *co2Provider;
    QTimer *noResponseTimer;
    int noResponeCount;
    int segmentSeq;     // the file segment sequence number
    bool isPassthroughMode;  // NIBP透传模式
};

bool UpgradeManagerPrivate::checkUpgradeFile()
{
    QString dirname = usbManager.getUdiskMountPoint() + UPGRADE_FILES_DIR;
    if (type == UpgradeManager::UPGRADE_MOD_HOST)
    {
        /**
         * special handling for the host upgrade file
         * the host upgrade is handling in the uboot, we only check the upgrade
         * file is exist or not.
         * the upgrade files' name and checksum info is in the md5sum.txt
         */
        QString checksumFileName = dirname + "md5sum.txt";
        if (!QFile::exists(checksumFileName))
        {
            qdebug("Host upgrade checksum file not exist.");
            return false;
        }

        QFile checksumFile(checksumFileName);
        if (!checksumFile.open(QIODevice::ReadOnly))
        {
            qdebug("Open host checksum file failed.");
            return false;
        }

        QString line;
        bool foundUpgradeFile = false;
        do
        {
            line = checksumFile.readLine();
            QStringList l = line.trimmed().split(" ", QString::SkipEmptyParts);
            if (!l.isEmpty())
            {
                if (QFile::exists(dirname + l.last()))
                {
                    // exist checksum file and upgrade file, we can perform upgrade
                    foundUpgradeFile = true;
                    break;
                }
            }
        }while (!line.isEmpty());

        return foundUpgradeFile;
    }
    else if (type >= UpgradeManager::UPGRADE_MOD_E5 && type < UpgradeManager::UPGRADE_MOD_TYPE_NR)
    {
        QString filename = getUpgradeFile();
        if (filename.isEmpty())
        {
            qDebug("No upgrade file for %s", qPrintable(q_ptr->getUpgradeModuleName(type)));
            return false;
        }
        return true;
    }

    return false;
}

QString UpgradeManagerPrivate::getUpgradeFile() const
{
    QString dirname = usbManager.getUdiskMountPoint() + UPGRADE_FILES_DIR;
    QString moduleName = q_ptr->getUpgradeModuleName(type);
    QDir dir(dirname);
    if (!dir.exists())
    {
        return QString();
    }

    QStringList nameFilters;
    if (type == UpgradeManager::UPGRADE_MOD_LOGO)
    {
        nameFilters << moduleName + ".*";
        dir.setSorting(QDir::Name | QDir::IgnoreCase);
    }
    else
    {
        // the upgrade file should end with md5 hex code and .bin suffix
        nameFilters << moduleName + "*.????????????????????????????????.bin";
        dir.setSorting(QDir::Name);
    }

    dir.setNameFilters(nameFilters);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.isEmpty())
    {
        return QString();
    }

    return fileList.first().fileName();
}

QString UpgradeManagerPrivate::calcChecksum(const QByteArray &content)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(content);
    return hash.result().toHex();
}

QString UpgradeManagerPrivate::getProviderName(UpgradeManager::UpgradeModuleType type)
{
    switch (type)
    {
    case UpgradeManager::UPGRADE_MOD_E5:
        return "BLM_E5";
    case UpgradeManager::UPGRADE_MOD_S5:
        return "BLM_S5";
    case UpgradeManager::UPGRADE_MOD_N5:
    case UpgradeManager::UPGRADE_MOD_N5DAEMON:
        return "BLM_N5";
    case UpgradeManager::UPGRADE_MOD_T5:
        return "BLM_T5";
    case UpgradeManager::UPGRADE_MOD_CO2:
        return "BLM_CO2";
    case UpgradeManager::UPGRADE_MOD_PRT48:
        return "PRT48";
    case UpgradeManager::UPGRADE_MOD_NEONATE:
        return "NEONATE_O2";
    case UpgradeManager::UPGRADE_MOD_nPMBoard:
        return "SystemBoard";
    default:
        break;
    }

    return QString();
}

void UpgradeManagerPrivate::upgradeExit(UpgradeManager::UpgradeResult result, UpgradeErrorType error)
{
    qdebug("Upgrade exit, result:%d, error:%d", result, error);

    if (error != UPGRADE_ERR_NONE)
    {
        emit q_ptr->upgradeInfoChanged(trs(errorString(error)));
    }

    emit q_ptr->upgradeResult(result);

    resetTimer();

    segmentSeq = 0;

    QString moduleStr;
    machineConfig.getStrValue("NIBP", moduleStr);
    if (type == UpgradeManager::UPGRADE_MOD_N5DAEMON && moduleStr == "BLM_N5")
    {
        handleStateChanged(MODULE_STAT_EXIT_PASSTHROUGH_MODE);
        state = STATE_UPGRADE_EXIT_PASSTHROUGH_MODE;
    }
    else
    {
        if (provider)
        {
            provider->setUpgradeIface(NULL);
            if (provider->getName() == "BLM_T5")
            {
                QString curSpO2Module;
                machineConfig.getStrValue("SPO2", curSpO2Module);
                Provider *spo2Provider = paramManager.getProvider(curSpO2Module);
                if (spo2Provider)
                {
                    spo2Provider->stopCheckConnect(false);
                }
            }
            else if (provider->getName() == "SystemBoard")
            {
                QString curModule;
                machineConfig.getStrValue("SPO2", curModule);
                Provider *provider = paramManager.getProvider(curModule);
                if (provider)
                {
                    provider->stopCheckConnect(false);
                }
                machineConfig.getStrValue("NIBP", curModule);
                provider = paramManager.getProvider(curModule);
                if (provider)
                {
                    provider->stopCheckConnect(false);
                }
                machineConfig.getStrValue("TEMP", curModule);
                provider = paramManager.getProvider(curModule);
                if (provider)
                {
                    provider->stopCheckConnect(false);
                }
            }
            provider = NULL;
        }
        if (co2Provider)
        {
            co2Provider->setUpgradeIface(NULL);
            co2Provider = NULL;
        }
        state = STATE_IDLE;
    }

    type = UpgradeManager::UPGRADE_MOD_NONE;
}

void UpgradeManagerPrivate::handleAck(unsigned char *data, int len)
{
    Q_UNUSED(len)
    qdebug("ack:0x%02x", data[0]);
    switch (state)
    {
    case STATE_REQUEST_ENTER_UPGRADE_MODE:
        if (data[0] == UPGRADE_CMD_REQUEST_ENTER)
        {
            state = STATE_REQUEST_SEND_DATA;
            resetTimer();
            q_ptr->upgradeProcess();
        }
        break;
    case STATE_REQUEST_SEND_DATA:
        if (data[0] == UPGRADE_CMD_REQUEST_SEND)
        {
            state = STATE_WAIT_FOR_REQUEST_IMAGE;
            resetTimer();
            q_ptr->upgradeProcess();
        }
        break;
    case STATE_WRITE_HARDWARE_ATTRIBUTE:
        if (data[0] == UPGRADE_CMD_HW_ATTR)
        {
            state = STATE_WRITE_IMAGE_SEGMENT;
            resetTimer();
            q_ptr->upgradeProcess();
        }
        break;
    default:
        break;
    }
}

void UpgradeManagerPrivate::hanleNack(unsigned char *data, int len)
{
    Q_UNUSED(len)
    qdebug("nack:0x%02x", data[0]);
    switch (state)
    {
    case STATE_REQUEST_ENTER_UPGRADE_MODE:
        if (data[0] == UPGRADE_CMD_REQUEST_ENTER)
        {
            upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;
    case STATE_REQUEST_SEND_DATA:
        if (data[0] == UPGRADE_CMD_REQUEST_SEND)
        {
            upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;
    case STATE_WRITE_HARDWARE_ATTRIBUTE:
        if (data[0] == UPGRADE_CMD_HW_ATTR)
        {
            upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;
    default:
        break;
    }
}

void UpgradeManagerPrivate::handleSegmentRsp(unsigned char *data, int len)
{
    Q_UNUSED(len)
    short seq = data[1] | (data[2] << 8);
    if (state == STATE_WRITE_IMAGE_SEGMENT)
    {
        if (data[0] == 1)
        {
            qdebug("Segment %d is written", seq);
            if (segmentSeq * 128 >= fileContent.length())
            {
                // all segment is sent, not wait for state change
                state = STATE_WAIT_FOR_COMPLETE_MSG;
            }
            else if (seq == segmentSeq)
            {
                segmentSeq++;
            }

            resetTimer();
            q_ptr->upgradeProcess();
        }
        else
        {
            qdebug("segment %d send failed.", seq);
        }
    }
}

void UpgradeManagerPrivate::handleStateChanged(ModuleState modState)
{
    qdebug("Module state: %d", modState);
    switch (modState)
    {
    case MODULE_STAT_BOOTLOADER_START:
        emit q_ptr->upgradeInfoChanged(trs("BootloaderStart"));
        qDebug() << q_ptr->getUpgradeModuleName(type) << "bootlaoder start";
        break;
    case MODULE_STAT_APPLICANT_START:
        emit q_ptr->upgradeInfoChanged(trs("BootloaderStartApplication"));
        qDebug() << q_ptr->getUpgradeModuleName(type) << "application start";
        break;
    case MODULE_STAT_REQUEST_IMAGE_SEGMENT:
        emit q_ptr->upgradeInfoChanged(trs("RequestSendImageSegment"));
        qDebug() << q_ptr->getUpgradeModuleName(type) << "request image segment";
        if (state == STATE_WAIT_FOR_REQUEST_IMAGE)
        {
            state = STATE_WRITE_IMAGE_SEGMENT;
            resetTimer();
            q_ptr->upgradeProcess();
        }
        break;
    case MODULE_STAT_WRITE_UPGRADE_FLAG_FAIL:
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_MODULE_INITIALIZE_FAIL);
        break;
    case MODULE_STAT_ERASE_FLASH:
        emit q_ptr->upgradeInfoChanged(trs("EraseFlash"));
        qDebug() << q_ptr->getUpgradeModuleName(type) << "Start earse flash";
        break;
    case MODULE_STAT_ERASE_FLASH_FAIL:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "Erase flash fail";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_MODULE_ERASE_FLASH_FAIL);
        break;
    case MODULE_STAT_IMAGE_NAME_UNMATCH:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "Image name unmatch";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_IMAGE_FILE_UNMATCH);
        break;
    case MODULE_STAT_HARDWARE_VER_UNMATCH:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "hardware version unmatch";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_HARDWARE_VERION_UNMATCH);
        break;
    case MODULE_STAT_WRITE_DEVICE_ATTR_FAIL:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "write device attritube fail";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_WRITE_ATTR_FAIL);
        break;
    case MODULE_STAT_IMAGE_SEQ_ERROR:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "image sequence number error";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        break;
    case MODULE_STAT_WRITE_IMAGE_SEGMENT_FAIL:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "write image segment fail";
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_WRITE_SEGMENT_FAIL);
        break;
    case MODULE_STAT_WRITE_UPGRADE_COMPLETE:
        qDebug() << q_ptr->getUpgradeModuleName(type) << "upgrade complete";
        if (state == STATE_WAIT_FOR_COMPLETE_MSG || state == STATE_WRITE_IMAGE_SEGMENT)
        {
            emit  q_ptr->upgradeInfoChanged(trs("WriteImageComplete"));
            // 当前面板升级成功后启动重启操作
            if (type == UpgradeManager::UPGRADE_MOD_nPMBoard)
            {
                state = STATE_REBOOT;
                q_ptr->upgradeProcess();
            }
            else
            {
                if (type == UpgradeManager::UPGRADE_MOD_T5)
                {
                    emit q_ptr->upgradeT5ModuleCompleted();
                }
                upgradeExit(UpgradeManager::UPGRADE_SUCCESS, UPGRADE_ERR_NONE);
            }
        }
        break;
    case MODULE_STAT_ENTER_PASSTHROUGH_MODE:
    {
        emit q_ptr->upgradeInfoChanged(trs("EnterPassthroughMode"));
        unsigned char cmd = NIBP_PASSTHROUGH_ON;
        provider->sendCmd(UPGRADE_N5_CMD_PASSTHROUGH_MODE, &cmd, 1);
        noResponseTimer->start(2000);
        break;
    }
    case MODULE_STAT_EXIT_PASSTHROUGH_MODE:
    {
        emit q_ptr->upgradeInfoChanged(trs("ExitPassthroughMode"));
        unsigned char cmd = NIBP_PASSTHROUGH_OFF;
        provider->sendCmd(UPGRADE_N5_CMD_PASSTHROUGH_MODE, &cmd, 1);
        noResponseTimer->start(2000);
        break;
    }
    case MODULE_STAT_CRC16_CHECK_ERROR:
    {
        upgradeExit(UpgradeManager::UPGRADE_FAIL, UPGRADE_ERR_FLASH_CRC);
        break;
    }
    default:
        break;
    }
}

void UpgradeManagerPrivate::handleUpgradeAlivePacket()
{
}

QByteArray UpgradeManagerPrivate::getImageSegment(int seqNum)
{
    QByteArray segment(131, 0);
    char *ptr = segment.data();
    ptr[0] = 0;
    ptr[1] = seqNum & 0xFF;
    ptr[2] = (seqNum >> 8) & 0xFF;

    char *content = ptr + 3;

    int index = seqNum * 128;
    if (index + 128 < fileContent.length())
    {
        ::memcpy(content, fileContent.data() + index, 128);
    }
    else if (index < fileContent.length())
    {
        ptr[0] = 1;
        int size = fileContent.length() - index;
        ::memcpy(content, fileContent.data() + index, size);
        ::memset(content + size, 0xff, 128 - size);
    }
    return segment;
}

UpgradeManager *UpgradeManager::getInstance()
{
    static UpgradeManager *instance = NULL;

    if (instance == NULL)
    {
        instance = new UpgradeManager();
    }

    return instance;
}

QString UpgradeManager::getUpgradeModuleName(UpgradeManager::UpgradeModuleType type)
{
    switch (type)
    {
    case UPGRADE_MOD_HOST:
        return "Host";
    case UPGRADE_MOD_E5:
        return "E5";
    case UPGRADE_MOD_S5:
        return "S5";
    case UPGRADE_MOD_N5:
        return "N5";
    case UPGRADE_MOD_N5DAEMON:
        return "N5Daemon";
    case UPGRADE_MOD_T5:
        return "T5";
    case UPGRADE_MOD_CO2:
        return "CO2";
    case UPGRADE_MOD_NEONATE:
        return "Neonate";
    case UPGRADE_MOD_PRT48:
        return "PRT48";
    case UPGRADE_MOD_nPMBoard:
        return "nPMBoard";
    case UPGRADE_MOD_LOGO:
        return "Logo";
    default:
        break;
    }

    return QString();
}

void UpgradeManager::startModuleUpgrade(UpgradeManager::UpgradeModuleType type)
{
    if (d_ptr->type != UPGRADE_MOD_NONE)
    {
        debug("upgarde in process, start failed");
        return;
    }

    d_ptr->type = type;
    d_ptr->state = STATE_CHECK_UDISK;
    if (d_ptr->type == UPGRADE_MOD_CO2)
    {
        d_ptr->co2Provider = static_cast<BLMCO2Provider *>(paramManager.getProvider("BLM_CO2"));
        if (d_ptr->co2Provider)
        {
            d_ptr->co2Provider->setUpgradeIface(this);
        }
    }
    else
    {
        if (d_ptr->type == UPGRADE_MOD_T5)
        {
            // T5升级时，先停止血氧的检测
            QString curSpO2Module;
            machineConfig.getStrValue("SPO2", curSpO2Module);
            Provider *spo2Provider = paramManager.getProvider(curSpO2Module);
            if (spo2Provider)
            {
                spo2Provider->stopCheckConnect(true);
            }
        }
        else if (d_ptr->type == UPGRADE_MOD_nPMBoard)
        {
            // nPMBoaed升级时，先停止spo2和NIBP的检测
            QString curModule;
            machineConfig.getStrValue("SPO2", curModule);
            Provider *provider = paramManager.getProvider(curModule);
            if (provider)
            {
                provider->stopCheckConnect(true);
            }
            machineConfig.getStrValue("NIBP", curModule);
            provider = paramManager.getProvider(curModule);
            if (provider)
            {
                provider->stopCheckConnect(true);
            }
            machineConfig.getStrValue("TEMP", curModule);
            provider = paramManager.getProvider(curModule);
            if (provider)
            {
                provider->stopCheckConnect(true);
            }
        }
        d_ptr->provider = BLMProvider::findProvider(d_ptr->getProviderName(d_ptr->type));
        if (d_ptr->provider)
        {
            d_ptr->provider->setUpgradeIface(this);
        }
    }

    QTimer::singleShot(0, this, SLOT(upgradeProcess()));
}

void UpgradeManager::handlePacket(unsigned char *data, int len)
{
    switch (data[0])
    {
    case UPGRADE_RSP_NACK:
        d_ptr->hanleNack(data + 1, len - 1);
        break;
    case UPGRADE_RSP_ACK:
        d_ptr->handleAck(data + 1, len - 1);
        break;
    case UPGRADE_RSP_SEGMENT:
        d_ptr->handleSegmentRsp(data + 1, len - 1);
        break;
    case UPGRADE_NOTIFY_STATE:
        d_ptr->handleStateChanged(static_cast<ModuleState>(data[1]));
        break;
    case UPGRADE_NOTIFY_UPGRADE_ALIVE:
        break;
    case UPGRADE_N5_RSP_PASSTHROUGH_MODE:
        if (data[1] == NIBP_PASSTHROUGH_OFF)
        {
            n5DaemonExitUpgradeMode();
        }
        else if (data[1] == NIBP_PASSTHROUGH_ON)
        {
            n5DaemonEnterUpgradeMode();
        }
        break;
    default:
        break;
    }
}

void UpgradeManager::n5DaemonEnterUpgradeMode()
{
    d_ptr->state = STATE_REQUEST_ENTER_UPGRADE_MODE;
    d_ptr->resetTimer();
    upgradeProcess();
}

void UpgradeManager::n5DaemonExitUpgradeMode()
{
    d_ptr->resetTimer();
    if (d_ptr->provider)
    {
        d_ptr->provider->setUpgradeIface(NULL);
        d_ptr->provider = NULL;
    }
}

void UpgradeManager::upgradeProcess()
{
    switch (d_ptr->state)
    {
    case STATE_IDLE:
        d_ptr->type = UPGRADE_MOD_NONE;
        break;

    case STATE_CHECK_UDISK:
        if (usbManager.isUSBExist())
        {
            d_ptr->state = STATE_CHECK_FILE;
            QTimer::singleShot(0, this, SLOT(upgradeProcess()));
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_NO_UDISK);
        }
        break;

    case STATE_CHECK_FILE:
        emit upgradeInfoChanged(trs("CheckUpgradeFile"));
        if (d_ptr->checkUpgradeFile())
        {
            if (d_ptr->type == UPGRADE_MOD_HOST)
            {
                system("fw_setenv maspro_user_mode 1 && sync");
                d_ptr->state = STATE_REBOOT;
            }
            else if (d_ptr->type == UPGRADE_MOD_LOGO)
            {
                d_ptr->state = STATE_UPGRADE_LOGO;
            }
            else
            {
                d_ptr->state = STATE_CHECKSUM;
            }

            QTimer::singleShot(0, this, SLOT(upgradeProcess()));
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_NO_UPGRADE_FILE);
        }
        break;

    case STATE_CHECKSUM:
    {
        emit upgradeInfoChanged(trs("CalculateChecksum"));
        QString dirname = usbManager.getUdiskMountPoint() + UPGRADE_FILES_DIR;
        QString filename = dirname + d_ptr->getUpgradeFile();
        QFile f(filename);
        if (!f.open(QIODevice::ReadOnly))
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_READ_FILE_FAIL);
            break;
        }

        d_ptr->fileContent = f.readAll();

        // get the md5sum from the file name
        QString md5 = filename.section('.', -2, -2);

        QString calcMd5 = d_ptr->calcChecksum(d_ptr->fileContent);

        if (md5.compare(calcMd5, Qt::CaseInsensitive))
        {
            // checksum failed
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_CHECKSUM_FAIL);
            break;
        }

        if (d_ptr->type == UPGRADE_MOD_N5DAEMON)
        {
            QString moduleStr;
            machineConfig.getStrValue("NIBP", moduleStr);
            if (moduleStr == "BLM_N5")
            {
                d_ptr->handleStateChanged(MODULE_STAT_ENTER_PASSTHROUGH_MODE);
                d_ptr->state = STATE_UPGRADE_ENTER_PASSTHROUGH_MODE;
                break;
            }
            else
            {
                d_ptr->state = STATE_REQUEST_ENTER_UPGRADE_MODE;
            }
        }
        else
        {
            d_ptr->state = STATE_REQUEST_ENTER_UPGRADE_MODE;
        }
        QTimer::singleShot(0, this, SLOT(upgradeProcess()));
    }
    break;

    case STATE_REQUEST_ENTER_UPGRADE_MODE:
    {
        emit upgradeInfoChanged(trs("RequestEnterUpgradeMode"));
        if (!d_ptr->provider && !d_ptr->co2Provider)
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_MODULE_NOT_FOUND);
            break;
        }

        if (d_ptr->type == UPGRADE_MOD_CO2)
        {
            d_ptr->co2Provider->enterUpgradeMode();
            d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
        }
        else
        {
            d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
        }
        d_ptr->noResponseTimer->start(2000);
    }
    break;

    case STATE_REQUEST_SEND_DATA:
    {
        emit upgradeInfoChanged(trs("RequestSendData"));
        if (d_ptr->type == UPGRADE_MOD_CO2)
        {
            d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
        }
        else
        {
            d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
        }
        d_ptr->noResponseTimer->start(1000);
    }
    break;

    case STATE_WAIT_FOR_REQUEST_IMAGE:
    {
        // after UPGRADE_CMD_REQUEST_SEND command, the moudle will erase flash,
        // need to wait longer enough for the flash erase complete.
        d_ptr->noResponseTimer->start(100000);
    }
    break;

    case STATE_WRITE_HARDWARE_ATTRIBUTE:
    {
        emit upgradeInfoChanged(trs("WriteDeviceAttribute"));
        char data[49];
        data[0] = 0x1;  // write attr
        // module name
        ::snprintf(data + 1, 32, "%s", qPrintable(getUpgradeModuleName(d_ptr->type)));  // NOLINT
        // module hardware version
        ::snprintf(data + 33, 16, DEFAULT_HW_VER_STR); // NOLINT
        if (d_ptr->type == UPGRADE_MOD_CO2)
        {
            d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
        }
        else
        {
            d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
        }
        d_ptr->noResponseTimer->start(2000);
    }
    break;

    case STATE_WRITE_IMAGE_SEGMENT:
    {
        if (d_ptr->segmentSeq == 0)
        {
            emit upgradeInfoChanged(trs("WriteImageData"));
        }

        QByteArray segment = d_ptr->getImageSegment(d_ptr->segmentSeq);
        if (d_ptr->type == UPGRADE_MOD_CO2)
        {
            d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_SEGMENT,
                                               reinterpret_cast<const unsigned char *>(segment.constData()),
                                               segment.length());
        }
        else
        {
            d_ptr->provider->sendCmd(UPGRADE_CMD_SEGMENT,
                                     reinterpret_cast<const unsigned char *>(segment.constData()),
                                     segment.length());
        }
        d_ptr->noResponseTimer->start(1000);

        int count = (d_ptr->fileContent.size() + 127) / 128;
        // d_ptr->segmentSeq是从０开始的序列．
        emit upgradeProgressChanged(d_ptr->segmentSeq * 100 / (count - 1));

        qdebug("write segment %d", d_ptr->segmentSeq);
    }
    break;

    case STATE_WAIT_FOR_COMPLETE_MSG:
    {
        d_ptr->noResponseTimer->start(1000);
    }
    break;

    case STATE_REBOOT:
        emit upgradeResult(UPGRADE_REBOOT);
        d_ptr->noResponseTimer->start(3000);
        break;

    case STATE_UPGRADE_LOGO:
    {
        QString filename = d_ptr->getUpgradeFile();
        QImage image(usbManager.getUdiskMountPoint() + UPGRADE_FILES_DIR + filename);

        if (image.isNull())
        {
            upgradeInfoChanged(trs("InvalidImageFormat"));

            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_NONE);
            break;
        }


        emit upgradeInfoChanged(trs("FoundLogoFile") + filename);
        QApplication::processEvents();

        QRect screenRect = QApplication::desktop()->screenGeometry();
        if (image.width() > screenRect.width() || image.height() > screenRect.height())
        {
            QString s = QString("%1 %2*%3").arg(trs("ImageTooLarge")).arg(screenRect.width()).arg(screenRect.height());
            upgradeInfoChanged(s);

            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_NONE);
            break;
        }

        QString bootDir = "/mnt/boot/";
        if (!QFile::exists(bootDir))
        {
            if (!QDir().mkpath(bootDir))
            {
                qdebug("Fail to create boot directory.");
                d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_WRITE_FAIL);
            }
        }

        qdebug("mount boot partition.");
        // mount the boot partition
        if (system("mount /dev/mmcblk0p1 /mnt/boot/") != 0)
        {
            qdebug("Fail to mount boot partition.");
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_NONE);
            break;
        }

        QFile logoFile(bootDir + LOGO_BIN_FILE);
        if (!logoFile.open(QIODevice::WriteOnly))
        {
            qdebug("Fail to open %s", LOGO_BIN_FILE);
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_WRITE_FAIL);
            break;
        }

        emit upgradeInfoChanged(trs("UpdateSystemFile"));
        QApplication::processEvents();
        if (!Util::generateKernelLogo(&logoFile, image))
        {
            qdebug("generate logo fail");
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_WRITE_FAIL);
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_SUCCESS, UPGRADE_ERR_NONE);
        }

        // close the logo file an umount boot partition
        logoFile.close();
        system("umount /mnt/boot");
        qdebug("boot partition umount.");
    }
    break;

    default:
        break;
    }
}

void UpgradeManager::noResponseTimeout()
{
    qdebug("state:%d", d_ptr->state);
    d_ptr->noResponeCount++;
    switch (d_ptr->state)
    {
    case STATE_REQUEST_ENTER_UPGRADE_MODE:
        if (d_ptr->noResponeCount < 2)
        {
            if (d_ptr->type == UPGRADE_MOD_CO2)
            {
                d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
            }
            else
            {
                d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
            }
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;

    case STATE_REQUEST_SEND_DATA:
        if (d_ptr->noResponeCount < 5)
        {
            if (d_ptr->type == UPGRADE_MOD_CO2)
            {
                d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
            }
            else
            {
                d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_SEND, NULL, 0);
            }
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;

    case STATE_WAIT_FOR_REQUEST_IMAGE:
        d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        break;

    case STATE_WRITE_HARDWARE_ATTRIBUTE:
        if (d_ptr->noResponeCount < 2)
        {
            char data[49];
            data[0] = 0x1;   // write attr
            // module name
            ::snprintf(data + 1, 32, "%s", qPrintable(getUpgradeModuleName(d_ptr->type))); // NOLINT
            // module hardware version
            ::snprintf(data + 33, 16, DEFAULT_HW_VER_STR); // NOLINT
            if (d_ptr->type == UPGRADE_MOD_CO2)
            {
                d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
            }
            else
            {
                d_ptr->provider->sendCmd(UPGRADE_CMD_REQUEST_ENTER, NULL, 0);
            }
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;
    case STATE_WRITE_IMAGE_SEGMENT:
        if (d_ptr->noResponeCount < 5)
        {
            QByteArray segment = d_ptr->getImageSegment(d_ptr->segmentSeq);
            if (d_ptr->type == UPGRADE_MOD_CO2)
            {
                d_ptr->co2Provider->sendUpgradeCmd(UPGRADE_CMD_SEGMENT,
                                                   reinterpret_cast<const unsigned char *>(segment.constData()),
                                                   segment.length());
            }
            else
            {
                d_ptr->provider->sendCmd(UPGRADE_CMD_SEGMENT,
                                         reinterpret_cast<const unsigned char *>(segment.constData()),
                                         segment.length());
            }
        }
        else
        {
            d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        }
        break;
    case STATE_WAIT_FOR_COMPLETE_MSG:
        // too long to receive the complete message
        d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_COMMUNICATION_FAIL);
        break;
    case STATE_REBOOT:
        emit reboot();
        system("reboot");
        break;
    case STATE_UPGRADE_ENTER_PASSTHROUGH_MODE:
        d_ptr->upgradeExit(UPGRADE_FAIL, UPGRADE_ERR_PASSTHROUGH_MODE_FAIL);
        break;
    case STATE_UPGRADE_EXIT_PASSTHROUGH_MODE:
        emit upgradeInfoChanged(trs(errorString(UPGRADE_ERR_PASSTHROUGH_MODE_FAIL)));
        n5DaemonExitUpgradeMode();
        break;
    default:
        break;
    }
}

UpgradeManager::UpgradeManager()
    : QObject(), d_ptr(new UpgradeManagerPrivate(this))
{
    d_ptr->noResponseTimer = new QTimer(this);

    connect(d_ptr->noResponseTimer, SIGNAL(timeout()), this, SLOT(noResponseTimeout()));
}

UpgradeManager::~UpgradeManager()
{
    delete d_ptr;
}
