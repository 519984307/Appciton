#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class SystemMenu : public SubMenu
{
    Q_OBJECT

public:
    static SystemMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemMenu();
        }
        return *_selfObj;
    }
    static SystemMenu *_selfObj;
    ~SystemMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _audioVolumeSlot(int index);
    void _errorToneVolumeSlot(int index);
    void _brightnessSlot(int index);
    void _printSpeedSlot(int index);
    void _printWaveformNumSlot(int index);
    void _printCfgSlot(void);
    void _configManagerSlot(void);


    void _supervisorManagerSlot(void);


#ifdef Q_WS_QWS
    void _touchScreenCalSlot(void);
#endif

private:
    SystemMenu();
    void _loadOptions(void);

    IComboList *_audioVolume;                     // 音量。
//    IComboList *_promptVolume;                     // 音量。
//    IComboList *_errorToneVolume;                     // 音量。
    IComboList *_screenBrightness;                // 显示屏亮度。
    IComboList *_printSpeed;                      // 打印机速度。
    IComboList *_printWaveformNum;                // 打印波形数量。
    LabelButton *_printCfg;                       // 打印配置
    LabelButton *_configManager;                  // config Manager
#ifdef Q_WS_QWS
    LabelButton *_touchScreenCal;                 // touch screen calibraiton
#endif
};
#define systemMenu (SystemMenu::construction())
