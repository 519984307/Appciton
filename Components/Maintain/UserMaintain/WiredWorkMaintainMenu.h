#pragma once
#include "SubMenu.h"
#include "LabelButton.h"

class LabelButton;
class IComboList;
class WiredWorkMaintainMenu : public SubMenu
{
    Q_OBJECT

public:
    static WiredWorkMaintainMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new WiredWorkMaintainMenu();
        }

        return *_selfObj;
    }
    static WiredWorkMaintainMenu *_selfObj;

    ~WiredWorkMaintainMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _addressTypeSlot(int);
    void editIpAddress(void);

private:
    WiredWorkMaintainMenu();

    IComboList  *_addressType;             //地址类型
    LabelButton *_ipAddress;               //IP地址
    LabelButton *_subnetMask;              //子网掩码
    LabelButton *_gateWay;                 //网关

};
#define wiredWorkMaintainMenu (WiredWorkMaintainMenu::construction())
