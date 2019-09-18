/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/18
 **/

#include "MeasureSettingWindow.h"
#include "ECGMenuContent.h"
#include "RESPMenuContent.h"
#include "AGMenuContent.h"
#include "IBPMenuContent.h"
#include "COMenuContent.h"
#include "SPO2MenuContent.h"
#include "NIBPMenuContent.h"
#include "CO2MenuContent.h"
#include "TEMPMenuContent.h"
#include "O2MenuContent.h"
#include "SystemManager.h"
#include "ApneaStimulationMenuContent.h"

MeasureSettingWindow *MeasureSettingWindow::getInstance()
{
    static MeasureSettingWindow *instance = NULL;
    if (!instance)
    {
        instance = new MeasureSettingWindow();
        // initialize the window content
        instance->addMenuContent(new ECGMenuContent);
        if (systemManager.isSupport(CONFIG_RESP))
        {
            instance->addMenuContent(new RESPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_TEMP))
        {
            instance->addMenuContent(new TEMPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_AG))
        {
            instance->addMenuContent(new AGMenuContent);
        }
        if (systemManager.isSupport(CONFIG_IBP))
        {
            instance->addMenuContent(new IBPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_CO))
        {
            instance->addMenuContent(new COMenuContent);
        }
        if (systemManager.isSupport(CONFIG_SPO2))
        {
            instance->addMenuContent(new SPO2MenuContent);
        }
        if (systemManager.isSupport(CONFIG_NIBP))
        {
            instance->addMenuContent(new NIBPMenuContent);
        }
        if (systemManager.isSupport(CONFIG_CO2))
        {
            instance->addMenuContent(new CO2MenuContent);
        }
#ifdef ENABLE_O2_APNEASTIMULATION
        if (systemManager.isSupport(CONFIG_O2))
        {
            instance->addMenuContent(new O2MenuContent);
            instance->addMenuContent(new ApneaStimulationMenuContent());
        }
#endif
    }
    return instance;
}

MeasureSettingWindow::MeasureSettingWindow()
    : MenuWindow()
{
}
