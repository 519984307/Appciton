/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
 **/
#pragma once
#include "MenuWindow.h"
#include "Config.h"
#include "PatientDefine.h"

class ConfigEditMenuWindowPrivate;
class ConfigEditMenuWindow : public MenuWindow
{
    Q_OBJECT
public:
    ConfigEditMenuWindow();
    ~ConfigEditMenuWindow();

    /**
     * @brief initializeSubMenu initialize the sub menu
     */
    void initializeSubMenu();

    /**
     * @brief setCurrentEditConfigInfo and set the current edit config object, patient type
     * @param config
     */
    void setCurrentEditConfigInfo(Config *config, PatientType type = PATIENT_TYPE_ADULT);

    /**
     * @brief getCurrentEditConfig get the current edit config object
     * @return  the config object
     */
    Config *getCurrentEditConfig() const;

    /**
     * @brief getCurrentConfigType and get current config type
     * @return
     */
    PatientType getCurrentConfigType() const;

    /**
     * @brief setCurrentEditConfigName set the current edit config name
     * @param name
     */
    void setCurrentEditConfigName(const QString &name);

    /**
     * @brief getCurrentEditConfigName get teh current edit config name
     * @return return the edit config name
     */
    QString getCurrentEditConfigName() const;

    /**
     * @brief getCurrentEditConfigItem  获得当前的配置项
     * @return
     */
    QMap <QString, MenuContent *> getCurrentEditConfigItem()const;


private:
    ConfigEditMenuWindowPrivate *const d_ptr;
};
