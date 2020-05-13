/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#pragma once
#include <QString>
#include <QList>
#include <QColor>
#define ICON_FILE_LEFT              "left.png"
#define ICON_FILE_RIGHT             "right.png"
#define ICON_PATH "/usr/local/nPM/icons/"

#define co2StandbyIcon "standby.png"
#define co2StandbyHint "CO2Standby"
#define co2MeasureIcon "measure.png"
#define co2MeasureHint "CO2Measure"

enum SoftKeyActionType
{
    SOFTKEY_ACTION_NONE = -1,
    SOFTKEY_ACTION_STANDARD,
    SOFTKEY_ACTION_12LEAD,
    SOFTKEY_ACTION_RESCUE_DATA,
    SOFTKEY_ACTION_DEL_RESCUE_DATA,
    SOFTKEY_ACTION_CALCULATE,
    SOFTKEY_ACTION_NR,
};

enum SoftBaseKeyType
{
    SOFT_BASE_KEY_MAIN_SETUP,
    SOFT_BASE_KEY_PAT_INFO,
    SOFT_BASE_KEY_PAT_NEW,
    SOFT_BASE_KEY_PAT_DISCHARGE,
#ifdef VITAVUE_15_INCHES
    SOFT_BASE_KEY_ALARM_RESET,
    SOFT_BASE_KEY_ALARM_PAUSE,
    SOFT_BASE_KEY_RECORD,
    SOFT_BASE_KEY_NIBP_MEASURE,
    SOFT_BASE_KEY_FREEZE,
#endif
    SOFT_BASE_KEY_ECG_LEAD_MODE,
    SOFT_BASE_KEY_ALARM_LIMIT,
    SOFT_BASE_KEY_CODE_MARKER,
    SOFT_BASE_KEY_TREND_GRAPH,
    SOFT_BASE_KEY_EVENT_REVIEW,
    SOFT_BASE_KEY_TREND_TABLE,
    SOFT_BASE_KEY_SYS_MODE,
#ifndef HIDE_SCREEN_LAYOUT
    SOFT_BASE_KEY_WINDOWLAYOUT,
#endif
#ifndef HIDE_PARAM_SWITCH
    SOFT_BASE_KEY_PARA_SWITCH,
#endif
    SOFT_BASE_KEY_SCREEN_BAN,
#ifndef HIDE_STANDBY_FUNCTION
    SOFT_BASE_KEY_STANDBY,
#endif
    SOFT_BASE_KEY_CO2_CALIBRATION,
    SOFT_BASE_KEY_CO2_HANDLE,
    SOFT_BASE_KEY_IBP_ZERO,
    SOFT_BASE_KEY_CALCULATION,
    SOFT_BASE_KEY_KEYBOARD_VOLUMN,
    SOFT_BASE_KEY_SCREEN_BRIGHTNESS,
    SOFT_BASE_KEY_NIGHT_MODE,
    SOFT_BASE_KEY_PRINTER_SET,
    SOFT_BASE_KEY_NR
};

typedef void (*SoftkeyHook)(bool);  // 参数为bool值，按下为1，弹起为0。
struct KeyActionDesc
{
    KeyActionDesc(const QString &txt = QString(),
                  const QString &hint = "",
                  const QString &path = QString(),
                  SoftkeyHook phook = NULL, SoftBaseKeyType type = SOFT_BASE_KEY_NR,
                  bool focus = true,
                  const QColor &releaseColor = QColor(32, 32, 32),
                  const QColor &color = Qt::white,
                  const QColor &pressColor = QColor(0x1C, 0x86, 0xEE),
                  bool border = true
                 )
        : text(txt), iconPath(path), hook(phook), type(type),
          focus(focus), color(color), pressColor(pressColor),
          releaseColor(releaseColor), border(border), hint(hint)
    {
    }

    bool operator !=(const KeyActionDesc &desc)
    {
        if (this->hook != desc.hook || this->iconPath != desc.iconPath ||
                this->type != desc.type)
        {
            return true;
        }

        return false;
    }

    QString text;
    QString iconPath;
    SoftkeyHook hook;
    SoftBaseKeyType type;
    bool focus;
    QColor color;
    QColor pressColor;
    QColor releaseColor;
    bool border;
    QString hint;
};

class SoftkeyActionBase
{
public:    // 一些共有的功能处理。
    static void patientInfo(bool isPressed);
    static void patientNew(bool isPressed);
    static void patientRelieve(bool isPressed);
    static void ecgLeadChange(bool isPressed);
    static void codeMarker(bool isPressed);
    static void limitMenu(bool isPressed);
    static void windowLayout(bool isPressed);
    static void rescueData(bool isPressed);
    static void calculation(bool isPressed);
    static void mainsetup(bool isPressed);
    static void banTouchScreen(bool isPressed);
    static void trendTable(bool isPressed);
    static void switchSystemMode(bool isPressed);
    static void summaryReview(bool isPressed);
    static void eventReview(bool isPressed);
    static void standby(bool isPressed);
    static void CO2Zero(bool isPressed);
    static void CO2Handle(bool isPressed);
    static void IBPZero(bool isPressed);
    static void systemBrightness(bool isPressed);
    static void keyVolume(bool isPressed);
    static void nightMode(bool isPressed);
    static void printSet(bool isPressed);
    static void alarmReset(bool isPressed);
    static void alarmPause(bool isPressed);
    static void manualTirgger(bool isPressed);
    static void nibpMeasure(bool isPressed);
    static void freeze(bool isPressed);

public:
    // 获取动作描述总个数。
    virtual int getActionDescNR(void);

    // 获取动作描述句柄。
    virtual KeyActionDesc *getActionDesc(int index);

    /**
     * @brief getBaseActionDesc get the base key action description
     * @param baseType the base key action
     * @return  the key action
     */
    static KeyActionDesc *getBaseActionDesc(SoftBaseKeyType baseType);

    // 获取类型。
    SoftKeyActionType getType(void);

    // 构造与析构。
    explicit SoftkeyActionBase(SoftKeyActionType t);
    virtual ~SoftkeyActionBase();

protected:  // 处理翻页功能，需要派生类设置这些属性。
    // 翻到上一页。
    void previousPage(void);
    // 翻到下一页。
    void nextPage(void);

private:
    SoftKeyActionType _type;
};
