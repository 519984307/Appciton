/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/



#pragma once
#include "Window.h"

enum UpgradeWindowType
{
    UPGRADE_WINDOW_TYPE_HOST,
    UPGRADE_WINDOW_TYPE_TE3,
    UPGRADE_WINDOW_TYPE_TN3,
    UPGRADE_WINDOW_TYPE_TN3Daemon,
    UPGRADE_WINDOW_TYPE_TS3,
    UPGRADE_WINDOW_TYPE_TT3,
    UPGRADE_WINDOW_TYPE_PRT72,
    UPGRADE_WINDOW_TYPE_PRT48,
    UPGRADE_WINDOW_TYPE_nPMBoard,
    UPGRADE_WINDOW_TYPE_NR
};

class ServiceUpgradeWindowPrivate;
class ServiceUpgradeWindow : public Window
{
    Q_OBJECT

public:
    static ServiceUpgradeWindow *getInstance();
    ~ServiceUpgradeWindow();

    static const char *convert(UpgradeWindowType index)
    {
        static const char *symbol[UPGRADE_WINDOW_TYPE_NR] =
        {
            "HOST", "TE3", "TN3", "TN3Daemon", "TS3", "TT3", "PRT72", "nPMBoard"
        };
        return symbol[index];
    }

    /**
     * @brief init  初始化
     */
    void init();

    /**
     * @brief setDebugText  显示
     * @param str
     */
    void setDebugText(QString str);

    /**
     * @brief btnShow  按钮的显示与置灰
     * @param flag
     */
    void btnShow(bool flag);

    /**
     * @brief progressBarValue  进步条值
     * @param value
     */
    void progressBarValue(int value);

    /**
     * @brief infoShow  提示信息
     * @param flag
     */
    void infoShow(bool flag);

    /**
     * @brief getType  升级模块名
     * @return
     */
    UpgradeWindowType getType(void);

    /**
     * @brief isUpgrading  是否在升级中
     * @return
     */
    bool isUpgrading(void);

    /**
     * @brief setWaitPDCPLDRunCompletion  PD CPLD完成升级等待标记
     * @param flag
     */
    void setWaitPDCPLDRunCompletion(bool flag);
    /**
     * @brief isWaitPDCPLDRunCompletion
     * @return
     */
    bool isWaitPDCPLDRunCompletion();

protected:
    /**
     * @brief exit
     */
    virtual void exit(void);
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);
    /**
     * @brief showEvent
     */
    virtual void showEvent(QShowEvent *);
private slots:
    /**
     * @brief startBtnReleased
     */
    void startBtnReleased(void);
    /**
     * @brief UpgradeWindowTypeReleased
     * @param index
     */
    void UpgradeWindowTypeReleased(int index);

private:
    ServiceUpgradeWindow();
    ServiceUpgradeWindowPrivate *const d_ptr;
};
