/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/10
 **/

#pragma once
#include <QObject>
#include "BLMProvider.h"
#include "BLMCO2Provider.h"

class UpgradeManagerPrivate;
class UpgradeManager : public QObject, public BLMProviderUpgradeIface
{
    Q_OBJECT
public:
    enum UpgradeModuleType
    {
        UPGRADE_MOD_NONE,
        UPGRADE_MOD_HOST,
        UPGRADE_MOD_E5,
        UPGRADE_MOD_S5,
        UPGRADE_MOD_N5,
        UPGRADE_MOD_N5DAEMON,
        UPGRADE_MOD_T5,
        UPGRADE_MOD_CO2,
        UPGRADE_MOD_NEONATE,
        UPGRADE_MOD_PRT48,
        UPGRADE_MOD_nPMBoard,
        UPGRADE_MOD_LOGO,
        UPGRADE_MOD_TYPE_NR,
    };

    enum UpgradeResult {
        UPGRADE_SUCCESS,
        UPGRADE_REBOOT,     // need reboot to complete upgrade
        UPGRADE_FAIL,       // upgrade failed, need to check the error
    };

    /**
     * @brief getInstance get the upgrade manager instance
     * @return the point the upgrade manager
     */
    static UpgradeManager *getInstance();

    /**
     * @brief getUpgradeModuleName get the upgrade module name
     * @param type the upgrade module type
     * @return name of the upgrade module or the empty string when type is invalid
     */
    static QString getUpgradeModuleName(UpgradeModuleType type);

    /**
     * @brief startModuleUpgrade start the module upgrade
     * @param type the module type
     */
    void startModuleUpgrade(UpgradeModuleType type);

    /* reimplement */
    void handlePacket(unsigned char *data, int len);

    /**
     * @brief enterPassthroughMode 切换透传模式成功后进入升级
     */
    void n5DaemonEnterUpgradeMode();

    /**
     * @brief exitPassthroughMode N5Daemon退出升级
     */
    void n5DaemonExitUpgradeMode();

signals:
    void upgradeResult(UpgradeManager::UpgradeResult result);
    void upgradeInfoChanged(const QString &info);
    void upgradeProgressChanged(int value);
    void reboot();
    /**
     * @brief upgradeT5ModuleCompleted  and the signal of upgrading the T5 module when completed
     */
    void upgradeT5ModuleCompleted();

private slots:
    /**
     * @brief upgradeProcess handle the upgrade stuff
     */
    void upgradeProcess();

    /**
     * @brief noResponseTimeout timeout for command response
     */
    void noResponseTimeout();

private:
    Q_DECLARE_PRIVATE(UpgradeManager)
    UpgradeManagerPrivate * const d_ptr;
    UpgradeManager();
    ~UpgradeManager();
};
