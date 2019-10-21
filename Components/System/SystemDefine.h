/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/10
 **/



#pragma once

// 交直流模式
enum PowerStat
{
    POWER_UNKNOWN,      // 未知
    POWER_AC,           // 交流
    POWER_BAT,          // 电池
    POWER_AC_BAT,       // 交直流
    POWER_BAT_ERROR,    // 电池故障
    POWER_AC_BAT_ERROR, // 交直流故障
    POWER_NR
};

// 界面类型
enum UserFaceType
{
    UFACE_MONITOR_STANDARD,
    UFACE_MONITOR_ECG_FULLSCREEN,
    UFACE_MONITOR_OXYCRG,
    UFACE_MONITOR_TREND,
    UFACE_MONITOR_BIGFONT,
    UFACE_MONITOR_CUSTOM,
    UFACE_MONITOR_UNKNOWN,
    UFACE_MONITOR_SPO2,
    UFACE_NR
};

/**
 * @brief The Backlight Regulation enum
 */
enum BacklightAdjustment
{
    BACKLIGHT_MODE_1,
    BACKLIGHT_MODE_2,
    BACKLIGHT_MODE_NR
};

// 背光亮度等级。
enum BrightnessLevel
{
    BRT_LEVEL_0 = 0,
    BRT_LEVEL_1,
    BRT_LEVEL_2,
    BRT_LEVEL_3,
    BRT_LEVEL_4,
    BRT_LEVEL_5,
    BRT_LEVEL_6,
    BRT_LEVEL_7,
    BRT_LEVEL_8,
    BRT_LEVEL_9,
    BRT_LEVEL_AUTO,
    BRT_LEVEL_NR
};

// 错误警告码
enum ErrorWaringCode
{
    ERR_CODE_NONE,              // 正常，无错误
    ERR_CODE_BAT_COMM_FAULT,    // 电池通讯错误
    ERR_CODE_BAT_ERROR,         // 电池错误
    ERR_CODE_NR
};

enum SystemOneShotAlarm
{
    SOME_LIMIT_ALARM_DISABLED,
    POWERUP_PANEL_RECORD_PRESSED,
    SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP,
    STORAGE_SPACE_FULL,
    SYSTEM_ONE_SHOT_ALARM_AUDIO_OFF,
    SYSTEM_ONE_SHOT_ALARM_OFF,
    SYSTEM_ONE_SHOT_ALARM_SEND_COMMUNICATION_STOP,
    SYSTEM_ONE_SHOT_ALARM_NR
};

class SystemSymbol
{
public:
    static const char *convert(UserFaceType index)
    {
        static const char *symbol[UFACE_NR] =
        {
            "UserFaceStandard", "UserFaceFullScreen", "UserFaceOxyCRG",
            "UserFaceTrend", "UserFaceBigFont", "UserFaceCustom", "UserFaceUnknow",
            "UserFaceSpO2"
        };
        return symbol[index];
    }

    static const char *convert(BacklightAdjustment index)
    {
        static const char *symbol[BACKLIGHT_MODE_NR] =
        {
            "Mode1", "Mode2"
        };
        return symbol[index];
    }
};
