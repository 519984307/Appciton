#pragma once
#include "SubMenu.h"

class LabelButton;
class IComboList;
class SupervisorGeneralSetMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorGeneralSetMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorGeneralSetMenu();
        }

        return *_selfObj;
    }
    static SupervisorGeneralSetMenu *_selfObj;

    ~SupervisorGeneralSetMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _idReleased(void);
    void _passwordReleased(void);
    void _patTypeChange(int);
    void _displayBrightnessChange(int);
    void _recordingFDChange(int);

private:
    SupervisorGeneralSetMenu();

    LabelButton *_deviceID;         // 设备标识符
    LabelButton *_password;         // 密码
    IComboList *_patType;           // 病人类型
    IComboList *_displayBrightness;  // display brightness
    IComboList *_recordingFD;       // full disclosure function
};
#define supervisorGeneralSetMenu (SupervisorGeneralSetMenu::construction())
