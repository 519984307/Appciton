/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/
#include "BLMEDUpgradeParam.h"
#include "BLMEDUpgradeTime.h"
#include "USBManager.h"
#include "ECGParam.h"
#include "NIBPParam.h"
#include "SPO2Param.h"
#include "TEMPParam.h"
#include "LightManager.h"
#include "md5.h"
#include "IMessageBox.h"
#include "MergeConfig.h"
#include <QTimer>
#include <sys/time.h>
#include <unistd.h>
#include <QCryptographicHash>
#include "RecorderManager.h"
#include "ServiceUpgradeWindow.h"

BLMEDUpgradeParam *BLMEDUpgradeParam::_selfObj = NULL;

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void BLMEDUpgradeParam::run(void)
{
    // 检测USB是否存在
    if (!usbManager.isUSBExist())
    {
        ServiceUpgradeWindow::getInstance()->infoShow(true);
        if (_isUSBExist)
        {
            _isUSBExist = false;
            if (ServiceUpgradeWindow::getInstance()->isUpgrading() && !_isFinish)
            {
                TimeStop();
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("WarningNoUSB"));
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeInterrupt") + "! ");
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
                _btnShow(true);
                if (_file.isOpen())
                {
                    _file.close();
                }
            }
        }
    }
    else
    {
        if (!_isUSBExist)
        {
            ServiceUpgradeWindow::getInstance()->infoShow(false);
        }
    }

    // 等待超时，返回待机模式。
    if (isTimeOut() && _isUSBExist)
    {
        TimeStop();
        if (_upgradeTN3DaemonFlag)
        {
            _upgradeTN3DaemonFlag = false;
        }
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeTimeout") + ", "
                + trs("UpgradeTransferError") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
        _btnShow(true);
    }
}

/**************************************************************************************************
 * 通行连接错误。
 *************************************************************************************************/
void BLMEDUpgradeParam::connectError()
{
    //    TimeStop();
    //    _btnShow(true);
    //    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeCommunicationInterrupt"));
}

/**************************************************************************************************
 * 按钮显示标志。
 *************************************************************************************************/
void BLMEDUpgradeParam::_btnShow(bool flag)
{
    if (flag)
    {
        // 退出TN3透传模式
        if (_type == UPGRADE_WINDOW_TYPE_TN3Daemon)
        {
            _serviceUpgradeTN3Daemon(false);
        }
        ServiceUpgradeWindow::getInstance()->btnShow(true);
    }
    else
    {
        ServiceUpgradeWindow::getInstance()->btnShow(false);
    }
}

/**************************************************************************************************
 * 进入NIBP从芯片升级模式，使NIBP主芯片进入透传模式
 *************************************************************************************************/
void BLMEDUpgradeParam::_serviceUpgradeTN3Daemon(bool flag)
{
    nibpParam.provider().setPassthroughMode(flag);
}

/**************************************************************************************************
 * 1、开始升级，检查U盘和文件。
 *************************************************************************************************/
void BLMEDUpgradeParam::startUpgrade()
{
    _type = ServiceUpgradeWindow::getInstance()->getType();
    _isUSBExist = false;
    _upgradeTN3DaemonFlag = false;
    _USBExist();
}

/**************************************************************************************************
 * 2、检查U盘。
 *************************************************************************************************/
void BLMEDUpgradeParam::_USBExist()
{
    if (!usbManager.isUSBExist())
    {
        _isUSBExist = false;
        ServiceUpgradeWindow::getInstance()->infoShow(true);
        // 警告没插U盘
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        _isUSBExist = true;
        ServiceUpgradeWindow::getInstance()->infoShow(false);
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFindImage"));
        _fileExist();
    }
}

/**************************************************************************************************
 * 3、检查文件。
 *************************************************************************************************/
void BLMEDUpgradeParam::_fileExist()
{
    setTimeOut(10 * 1000);

    // 内核程序不需要MD5检验
    if (_type == UPGRADE_WINDOW_TYPE_HOST)
    {
        QString dstFile(usbManager.getUdiskMountPoint());
        dstFile += "/upgrade/md5sum.txt";

        // 检查文件路径
        if (!QFile::exists(dstFile))
        {
            TimeStop();
            // 导入文件不存在或文件名错误
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeWarningNoImportFile") + ":");
            ServiceUpgradeWindow::getInstance()->setDebugText(dstFile);
            return;
        }
        TimeStop();
        serviceUpgradeStart();
        return;
    }
    else if (_type >= UPGRADE_WINDOW_TYPE_TE3 && _type < UPGRADE_WINDOW_TYPE_NR)
    {
        QString path(usbManager.getUdiskMountPoint());
        path += "/ZOLLImage/";
        QString file;

        if (!_findFile(path, QString(ServiceUpgradeWindow::getInstance()->convert(_type)), file))
        {
            TimeStop();
            return;
        }

        QString dstFile = path + file;
        QString md5 = file.section('.', 1, 1);

        // 检查文件是否可以打开
        _file.setFileName(dstFile);
        if (!_file.isOpen())
        {
            if (!_file.open(QFile::ReadOnly))
            {
                ServiceUpgradeWindow::getInstance()->setDebugText(dstFile + " " + trs("UpgradeOpenFail"));
                TimeStop();
                return;
            }
        }

        _fileSize = _file.size();
        _fileNum = -1;

        QString str = ServiceUpgradeWindow::getInstance()->convert(_type);
        ServiceUpgradeWindow::getInstance()->setDebugText(str + " "  + trs("ModuleUpgradeStart"));

        _md5Check(md5, dstFile);
    }
    else
    {
        TimeStop();
    }
}

/**************************************************************************************************
 * 4、进行MD5检测。
 *************************************************************************************************/
void BLMEDUpgradeParam::_md5Check(const QString &md5Original, const QString &/*fileName*/)
{
//    //对bin文件进行MD5计算
//    char *path = fileName.toLatin1().data();
//    char buffer[128];
//    if (getFileMD5(path, buffer) != 0)
//    {
//        TimeStop();
//        ServiceUpgradeWindow::getInstance()->setDebugText(trs("MD5VerifyError"));
//        _file.close();
//        return;
//    }
//    QString binMD5 = QString(buffer);

////    debug("ORIGINALMD5 %s",qPrintable(md5Original));
////    debug("CALCULATEDMD5 %s",qPrintable(binMD5));

//    TimeStop();
//    if (md5Original == binMD5)
//    {
//        serviceUpgradeStart();
//    }
//    else
//    {
//        ServiceUpgradeWindow::getInstance()->setDebugText(trs("MD5NOTMATCH"));
//        ServiceUpgradeWindow::getInstance()->setDebugText(trs("ORIGINALMD5") + ": "+ md5Original);
//        ServiceUpgradeWindow::getInstance()->setDebugText(trs("CALCULATEDMD5") + ": "+ binMD5);
//        _file.close();
//    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(_file.readAll());
    QByteArray result = hash.result();  // 返回最终的哈希值
    QString strMD5 = result.toHex();

//    QString strMD5 = QString(result.data());
//    strMD5 = strMD5.toAscii();
//    debug("%s",qPrintable(strMD5));

    TimeStop();

    // 将文件指针复位
    if (!_file.seek(0))
    {
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("MD5VerifyError"));
        _file.close();
        return;
    }

    // 不区分大小，全匹配
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (strMD5.compare(md5Original, cs) == 0)
    {
        serviceUpgradeStart();
    }
    else
    {
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("MD5NOTMATCH"));
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("ORIGINALMD5") + ": " + md5Original);
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("CALCULATEDMD5") + ": " + strMD5);
        _file.close();
    }
}

/**************************************************************************************************
 * 5、开始升级指令。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeStart(void)
{
    _btnShow(false);
    if (_type == UPGRADE_WINDOW_TYPE_HOST)
    {
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("SystemWillRestartDuringTheUpgradeProcess"));
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePleasedonotpoweroff"));
        _restartTimer->start(5000);
    }
    else
    {
        _numFile = 0;
        _isFinish = false;

        setTimeOut(5 * 1000);

        if (_type == UPGRADE_WINDOW_TYPE_TN3Daemon)
        {
            _upgradeTN3DaemonFlag = true;
        }
        else
        {
//            if (_type == UPGRADE_WINDOW_TYPE_TN3)
//            {
//                _serviceUpgradeTN3Daemon(false);
//            }
            serviceUpgradeEnter();
        }
    }
}

/**************************************************************************************************
 * host升级重启指令。
 *************************************************************************************************/
void BLMEDUpgradeParam::_restartTimeOut(void)
{
    if (_isUSBExist)
    {
        mergeConfig.backupOldConfig();
        system("fw_setenv maspro_user_mode 1");
        system("reboot");
    }
}

/**************************************************************************************************
 * 6、进入升级程序。发送0xF6
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeEnter(void)
{
    TimeStop();

    // 硬件版本号
//    serviceUpgradeWrite();

    // 读硬件版本号
//    serviceUpgradeRead();

    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        ServiceUpgradeWindow::getInstance()->infoShow(true);
        if (_isUSBExist)
        {
            _isUSBExist = false;
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("WarningNoUSB"));
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeInterrupt") + "! ");
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
            _btnShow(true);
            _file.close();
        }
    }
    else
    {
        _numFile++;
        sendServiceUpgradeEnter();
        if (_numFile < 6)
        {
            blmedUpgradeTime.startEnter(5);
        }
        else
        {
            _numFile = 0;
            blmedUpgradeTime.stopEnter();
            _btnShow(true);
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeTimeout") + "! " + trs("UpgradeEnterFailure"));
            _file.close();
        }
    }
}

/**************************************************************************************************
 * 7、数据发送启动。发送0xF7，开始擦除
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeSend(void)
{
    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        ServiceUpgradeWindow::getInstance()->infoShow(true);
        if (_isUSBExist)
        {
            _isUSBExist = false;
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("WarningNoUSB"));
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeInterrupt") + "! ");
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
            _btnShow(true);
            _file.close();
        }
    }
    else
    {
        _numFile++;
        sendServiceUpgradeSend();
        _find = 0;
        _relive = 0;
        if (_numFile < 6)
        {
            blmedUpgradeTime.startSend(5);
        }
        else
        {
            TimeStop();
            _numFile = 0;
            blmedUpgradeTime.stopSend();
            _btnShow(true);
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeTimeout") + "! " + trs("UpgradeInitFailure"));
            _file.close();
        }
    }
}

/**************************************************************************************************
 * 9、发送文件片段。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendFile(void)
{
    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        ServiceUpgradeWindow::getInstance()->infoShow(true);
        if (_isUSBExist)
        {
            _isUSBExist = false;
            TimeStop();
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("WarningNoUSB"));
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeInterrupt") + "! ");
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
            _btnShow(true);
            _file.close();
        }
    }
    else
    {
        setTimeOut();
        if (!_file.atEnd())
        {
            _fileNum++;
            _fileSum = _fileSize - (_fileNum * 128);
            if (_fileSum > 128)
            {
                _buff[0] = 0x00;
                _buff[1] = (_fileNum & 0xFF);
                _buff[2] = ((_fileNum >> 8) & 0xFF);

                packetData.append(_file.read(128));
                if (packetData.size() < 128)
                {
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFileReadError") + "! ");
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
                    _btnShow(true);
                    return;
                }

                for (int i = 0; i < 128; i++)
                {
                    _buff[i + 3] = packetData.at(0);
                    packetData.remove(0, 1);
                }
            }
            else
            {
                _buff[0] = 0x01;
                _buff[1] = (_fileNum & 0xFF);
                _buff[2] = ((_fileNum >> 8) & 0xFF);

                packetData.append(_file.readAll());
                if (packetData.size() < _fileSum)
                {
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFileReadLastError") + "! ");
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
                    _btnShow(true);
                    return;
                }

                for (int i = 0; i < _fileSum; i++)
                {
                    _buff[i + 3] = packetData.at(0);
                    packetData.remove(0, 1);
                }
                for (int i = _fileSum; i < 128; i++)
                {
                    _buff[i + 3] = 0xFF;
                }
                _file.close();
            }
            serviceUpgradeFile(_buff);
        }
    }
}
/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void BLMEDUpgradeParam::unPacket(unsigned char *packet, int /*len*/, const QString &name)
{
    switch (_type)
    {
    case UPGRADE_WINDOW_TYPE_TE3:
        if (name != "BLM_TE3")
        {
            return;
        }
        break;
    case UPGRADE_WINDOW_TYPE_TN3:
    case UPGRADE_WINDOW_TYPE_TN3Daemon:
        if (name != "BLM_TN3")
        {
            return;
        }
        if ((packet[0] == UPGRADE_PASSTHROUGH_MODE))
        {
            if (packet[1] == 0x01)
            {
                debug("进入透传模式");
                if (_type == UPGRADE_WINDOW_TYPE_TN3Daemon && _sendThroughFlag)
                {
                    serviceUpgradeEnter();
                    _sendThroughFlag = false;
                }
            }
            else
            {
                if (_sendThroughFlag)
                {
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeEnterTN3DaemonModuleFail"));
                    _btnShow(true);
                    _sendThroughFlag = false;
                }
                else
                {
                    debug("退出透传模式");
                }
            }
        }
        break;
    case UPGRADE_WINDOW_TYPE_TS3:
        if (name != "BLM_TS3")
        {
            return;
        }
        break;
    case UPGRADE_WINDOW_TYPE_TT3:
        if (name != "BLM_TT3")
        {
            return;
        }
        break;
    case UPGRADE_WINDOW_TYPE_PRT72:
        if (name != "PRT72")
        {
            return;
        }
        break;
    case UPGRADE_WINDOW_TYPE_nPMBoard:
        if (name != "SystemBoard")
        {
            return;
        }
        break;
    default:
        return;
    }

    // 判断升级是否成功
    if ((packet[0] == UPGRADE_NOTIFY_ALIVE) || (packet[0] == UPGRADE_ALIVE) || (packet[0] == UPGRADE_ECG_NOTIFY_ALIVE))
    {
        // 在升级NIBP从芯片时
        if (_type == UPGRADE_WINDOW_TYPE_TN3Daemon)
        {
            if (_upgradeTN3DaemonFlag)
            {
                // 判断TN3是否在bootLoader层
                // 如果收到NIBP 0x5B则发送进入透传
                if (packet[0] == UPGRADE_NOTIFY_ALIVE)
                {
                    _serviceUpgradeTN3Daemon(true);
                    _sendThroughFlag = true;
                    _upgradeTN3DaemonFlag = false;
                }
                // 如果收到0xFE则说明NIBP需要先升级
                else if (packet[0] == UPGRADE_ALIVE)
                {
                    TimeStop();
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePleaseUpgradeTN3ModuleFirst"));
                    _btnShow(true);
                    _file.close();
                    _upgradeTN3DaemonFlag = false;
                }
            }
        }
        if (packet[0] == UPGRADE_ALIVE)
        {
            if (_isFinish)
            {
                _numFe++;
                if (_numFe > 5)
                {
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFailure") + "! ");
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
                    _btnShow(true);
                    _isFinish = false;
                }
            }
        }
        else
        {
            // 升级完成
            if (_isFinish)
            {
                if (_type == UPGRADE_WINDOW_TYPE_TE3)
                {
                    if (packet[0] == UPGRADE_ECG_NOTIFY_ALIVE)
                    {
                        _isFinish = false;
                    }
                }
                else if (_type < UPGRADE_WINDOW_TYPE_NR)
                {
                    if (packet[0] == UPGRADE_NOTIFY_ALIVE)
                    {
                        _isFinish = false;
                    }
                }
                if (!_isFinish)
                {
                    TimeStop();
                    ServiceUpgradeWindow::getInstance()->progressBarValue(static_cast<int>(_fileNum * 100 / ((_fileSize - 128) / 128)));
                    debug("%s", qPrintable(name));
                    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeSuccess") + "! ");
                    _btnShow(true);
                }
            }
        }
        return;
    }

    // 以下为升级过程协议，若USB不存在，则不需要执行。
    if (!_isUSBExist)
    {
        return;
    }

    // 工作状态
    if (packet[0] == UPGRADE_RSP_STATE)
    {
        TimeStop();
//        blmedUpgradeTime.stopFile();

        isServiceUpgradeFinish(packet);

        return;
    }

    // 升级过程中的判断
    if ((packet[0] == UPGRADE_RESPONSE_NACK) || (packet[0] == UPGRADE_RESPONSE_ACK))
    {
        // 进入升级模式
        if (packet[1] == UPGRADE_RSP_ENTER)
        {
            _numFile = 0;
            blmedUpgradeTime.stopEnter();
            if (!isServiceUpgradeEnter(packet))
            {
                _btnShow(true);
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeEnterFailure") + "! ");
                _file.close();
            }
            else
            {
                serviceUpgradeSend();
            }
        }
        // 数据发送启动
        else if (packet[1] == UPGRADE_RSP_SEND)
        {
            _numFile = 0;
            blmedUpgradeTime.stopSend();
            if (!isServiceUpgradeSend(packet))
            {
                _btnShow(true);
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeInitFailure") + "! ");
                _file.close();
            }
            else
            {
                //  应收到开始擦除的工作状态
                setTimeOut();
            }
        }
        // 升级模式硬件版本号
        else if (packet[1] == UPGRADE_RSP_READ)
        {
            TimeStop();
            if (!isServiceUpgradeReade(packet))
            {
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeReadFailure") + "! ");
                _file.close();
            }
            else
            {
                QString softNumber = "";
                QString hardNumber = "";
                for (int i = 0; i < 31; i++)
                {
                    if (packet[2 + i] == 0x00)
                    {
                        continue;
                    }
                    else
                    {
                        softNumber = softNumber + packet[2 + i];
                    }
                }
                for (int i = 0; i < 15; i++)
                {
                    if (packet[2 + i] == 0x00)
                    {
                        continue;
                    }
                    else
                    {
                        hardNumber = hardNumber + packet[34 + i];
                    }
                }
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeReadSuccess") + "! ");
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeSoftwareNumber") + ": " + softNumber + " " +
                        trs("UpgradeHardwareNumber") + ": " + hardNumber);
            }
        }
        // 升级模式擦除Flash
        else if (packet[1] == UPGRADE_RSP_CLEAR)
        {
            TimeStop();
            if (!isServiceUpgradeClear(packet))
            {
                _btnShow(true);
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADECLEARFAIL") + "! ");
                _file.close();
            }
            else
            {
                ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeClearSuccess") + "! ");
                //                serviceUpgradeSend();
            }
        }
        return;
    }

    // 文件片段传输
    else if (packet[0] == UPGRADE_RSP_FILE)
    {
//        blmedUpgradeTime.stopFile();
        if (!isServiceUpgradeFile(packet))
        {
            TimeStop();
            _btnShow(true);
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeTransferError") + "! ");
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
        }
        else
        {
            _relive++;
            debug("find:%d,  relive:%d", _find, _relive);
            ServiceUpgradeWindow::getInstance()->progressBarValue(static_cast<int>(_fileNum * 100 / ((_fileSize - 128) / 128)));
            sendFile();
        }
        return;
    }
}

/**************************************************************************************************
 * 升级模式擦除Flash。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeClear(void)
{
    sendServiceUpgradeClear();
    setTimeOut(10 * 1000);
    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADECLEARING") + "... ");
}

/**************************************************************************************************
 * 升级模式硬件版本号。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeRead(void)
{
    sendServiceUpgradeRead();
    setTimeOut();
}

/**************************************************************************************************
 * 写设备属性。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeWrite()
{
    // TE3设备属性
    unsigned char cmd[] =
    {
        0x01, 0x54, 0x45, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // TS3设备属性
//    unsigned char cmd[] = {
//        0x01, 0x54, 0x53, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // TN3设备属性
//    unsigned char cmd[] = {
//        0x01, 0x54, 0x4E, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // 前面板设备属性
//    unsigned char cmd[] = {
//        0x01, 0x69, 0x44, 0x4D, 0x42, 0x6F, 0x61, 0x72, 0x64, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // TT3设备属性
//    unsigned char cmd[] = {
//        0x01, 0x54, 0x54, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // PRT72设备属性
//    unsigned char cmd[] = {
//        01, 0x50, 0x52, 0x54, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x41, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    _sendCmdData(UPGRADE_CMD_READ, cmd, 49);
}

/**************************************************************************************************
 * 文件片段传输。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeFile(unsigned char *packet)
{
    _numFile = 0;
    _packet = packet;
    sendServiceUpgradeFile(packet);
    // 文件片段重发
    //    blmedUpgradeTime.startFile(2);
}

/**************************************************************************************************
 * 文件片段未收到应答，再次传输。
 *************************************************************************************************/
void BLMEDUpgradeParam::serviceUpgradeFile(void)
{
    _numFile++;
    sendServiceUpgradeFile(_packet);
    if (_numFile < 4)
    {
        blmedUpgradeTime.startFile(2);
    }
    else
    {
        TimeStop();
        _numFile = 0;
        blmedUpgradeTime.stopFile();
        _btnShow(true);
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeTimeout") + ", "
                + trs("UpgradeTransferError") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
        _file.close();
    }
}

/**************************************************************************************************
 * 关闭文件。
 *************************************************************************************************/
void BLMEDUpgradeParam::fileClose(void)
{
    if (_file.isOpen())
    {
        _file.close();
    }
    TimeStop();
    _isUSBExist = false;
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void BLMEDUpgradeParam::_sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    switch (_type)
    {
    case UPGRADE_WINDOW_TYPE_TE3:
        ecgParam.sendCmdData(cmdId, data, len);
        break;
    case UPGRADE_WINDOW_TYPE_TN3:
    case UPGRADE_WINDOW_TYPE_TN3Daemon:
        nibpParam.provider().sendCmdData(cmdId, data, len);
        break;
    case UPGRADE_WINDOW_TYPE_TS3:
        spo2Param.sendCmdData(cmdId, data, len);
        break;
    case UPGRADE_WINDOW_TYPE_TT3:
        tempParam.sendCmdData(cmdId, data, len);
        break;
    case UPGRADE_WINDOW_TYPE_PRT72:
        // printManager.sendCmdData(cmdId,data,len);
        break;
    case UPGRADE_WINDOW_TYPE_PRT48:
        recorderManager.provider()->sendCmdData(cmdId, data, len);
        break;
    case UPGRADE_WINDOW_TYPE_nPMBoard:
        lightManager.sendCmdData(cmdId, data, len);
        break;
    default:
        return;
    }
}

/**************************************************************************************************
 * 根据type在path目录下找到正确的文件名，找到返回true。
 *************************************************************************************************/
bool BLMEDUpgradeParam::_findFile(const QString &path, const QString &matchStr, QString &fileName)
{
    QDir dir(path);
    if (!dir.exists())
    {
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeWarningNoImportFolder") + ":");
        ServiceUpgradeWindow::getInstance()->setDebugText(path);
        return false;
    }

    dir.setFilter(QDir::Files);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); i++)
    {
        QString str = list[i].fileName().section('-', 0, 0);
//        if (list[i].fileName().indexOf(matchStr) != -1)
        if (str == matchStr)
        {
            debug("str = %s   matchStr = %s", qPrintable(str), qPrintable(matchStr));
            fileName = list[i].fileName();
            return true;
        }
    }

    ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeWarningNoImportFile") + ":");
    ServiceUpgradeWindow::getInstance()->setDebugText(path + matchStr);
    return false;
}

/**************************************************************************************************
 * 设置超时并启动计时。
 *************************************************************************************************/
void BLMEDUpgradeParam::setTimeOut(int timeOut)
{
    _timeOut = timeOut;
    _elapseTime.restart();
    timestart = true;
}

/**************************************************************************************************
 * 是否超时。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isTimeOut(void)
{
    if ((_elapseTime.elapsed() > _timeOut) && timestart)
    {
        timestart = false;
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 获取时间流逝。
 *************************************************************************************************/
int BLMEDUpgradeParam::elapseTime(void)
{
    return _elapseTime.elapsed();
}

/**************************************************************************************************
 * 停止计时。
 *************************************************************************************************/
void BLMEDUpgradeParam::TimeStop()
{
    timestart = false;
}

/**************************************************************************************************
 *进入升级模式。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendServiceUpgradeEnter()
{
    _sendCmdData(UPGRADE_CMD_ENTER, NULL, 0);
}

/**************************************************************************************************
 *进入升级模式的应答。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isServiceUpgradeEnter(unsigned char *packet)
{
    if (packet[1] != UPGRADE_RSP_ENTER)
    {
        return false;
    }

    if ((packet[0] == 0x00) || (packet[0] == 0x01))
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 数据发送启动。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendServiceUpgradeSend()
{
    _sendCmdData(UPGRADE_CMD_SEND, NULL, 0);
}

/**************************************************************************************************
 * 数据发送启动的应答。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isServiceUpgradeSend(unsigned char *packet)
{
    if (packet[1] != UPGRADE_RSP_SEND)
    {
        return false;
    }

    if ((packet[0] == 0x00) || (packet[0] == 0x01))
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 *升级模式擦除Flash。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendServiceUpgradeClear()
{
    _sendCmdData(UPGRADE_CMD_CLEAR, NULL, 0);
}

/**************************************************************************************************
 *升级模式擦除Flash的应答。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isServiceUpgradeClear(unsigned char *packet)
{
    if (packet[1] != UPGRADE_RSP_CLEAR)
    {
        return false;
    }

    if ((packet[0] == 0x00) || (packet[0] == 0x01))
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 *升级模式硬件版本号。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendServiceUpgradeRead()
{
    unsigned char cmd = 0x00;
    _sendCmdData(UPGRADE_CMD_READ, &cmd, 1);
}

/**************************************************************************************************
 *升级模式硬件版本号的应答。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isServiceUpgradeReade(unsigned char *packet)
{
    if (packet[1] != UPGRADE_RSP_READ)
    {
        return false;
    }

    if ((packet[0] == 0x00) || (packet[0] == 0x01))
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 *文件片段传输。
 *************************************************************************************************/
void BLMEDUpgradeParam::sendServiceUpgradeFile(unsigned char *packet)
{
    _find++;
    _sendCmdData(UPGRADE_CMD_FILE, packet, 131);
}

/**************************************************************************************************
 *文件片段传输的应答。
 *************************************************************************************************/
bool BLMEDUpgradeParam::isServiceUpgradeFile(unsigned char *packet)
{
    if (packet[0] != UPGRADE_RSP_FILE)
    {
        return false;
    }

    if (packet[1] != 0x01)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 *升级结果。
 *************************************************************************************************/
void BLMEDUpgradeParam::isServiceUpgradeFinish(unsigned char *packet)
{
    switch (packet[1])
    {
    case 0x00:
        debug("BLMLoader启动了");
        break;
    case 0x01:
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEBLMLOADERLOAD") + "! ");
        debug("BLMLoader加载应用程序");
        _btnShow(true);
        _file.close();
        break;
    case 0x02:
        // 擦除Flash成功，请求发送镜像文件片段
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEIMAGETRANSFERRING") + "...");
        sendFile();
        break;
    case 0x03:
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADESIGNFAIL") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x04:
        // 8、擦除Flash
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADECLEARING") + "... ");
        setTimeOut(15000);
        break;
    case 0x05:
        // FLASH擦除失败
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADECLEARFAIL") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x06:
        // 软件镜像名称不匹配
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEIMAGEISWRONG") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x07:
        // 硬件版本不匹配
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEHARDWAREVERSIONDOESNOTMATCH") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x08:
        // 写设备属性（软件镜像名称+硬件版本）到FLASH失败
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEPROPERTYWRITEFAILED") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x09:
        // 文件片段的序号错误
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFailure") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEFILEORDERNUMBERERROR") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x0A:
        // 写文件片段到FLASH失败
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeFailure") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UPGRADEFILEFLASHFAIL") + "! ");
        ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePowerOffRestart") + "! ");
        _btnShow(true);
        _file.close();
        break;
    case 0x0B:
        debug("升级完成");
        if (_type == UPGRADE_WINDOW_TYPE_nPMBoard)
        {
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradeSuccess") + "! ");
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("SystemWillRestart"));
            ServiceUpgradeWindow::getInstance()->setDebugText(trs("UpgradePleasedonotpoweroff"));
        }
        else
        {
            setTimeOut();
        }
        _isFinish = true;
        _numFe = 0;
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BLMEDUpgradeParam::BLMEDUpgradeParam() : Param(PARAM_UPGRADE)
{
    _isUSBExist = false;
    _find = 0;
    _relive = 0;
    _numFile = 0;
    _isFinish = false;
    _type = UPGRADE_WINDOW_TYPE_NR;
    _upgradeTN3DaemonFlag = false;
    _sendThroughFlag = false;
    TimeStop();

    _restartTimer = new QTimer();
    connect(_restartTimer, SIGNAL(timeout()), this, SLOT(_restartTimeOut()));

    blmedUpgradeTime.construction();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BLMEDUpgradeParam::~BLMEDUpgradeParam()
{
    if (NULL != _restartTimer)
    {
        delete _restartTimer;
        _restartTimer = NULL;
    }

    deleteblmedupgradetime();
}
