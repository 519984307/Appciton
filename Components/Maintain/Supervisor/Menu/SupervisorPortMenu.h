#pragma once
#include "SubMenu.h"

class LabelButton;
class QTimer;
class QLabel;
class SupervisorPortMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorPortMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorPortMenu();
        }

        return *_selfObj;
    }
    static SupervisorPortMenu *_selfObj;

    ~SupervisorPortMenu();

    enum PortType
    {
        EXPORT_CONFIG,
        IMPORT_CONFIG,
        PORTTYPE_NR
    };

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _exConfigBtnSlot();
    void _imConfigBtnSlot();
    void _USBCheckTimeout();

private:
    SupervisorPortMenu();

    LButtonEx *_exportConfigBtn;        //导出配置
    LButtonEx *_importConfigBtn;        //导入日志
    QLabel *_info;                    //提示信息

private:
    PortType _type;
    QTimer *_USBCheckTimer;
};
#define supervisorPortMenu (SupervisorPortMenu::construction())

