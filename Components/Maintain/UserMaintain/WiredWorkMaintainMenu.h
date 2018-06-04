#pragma once
#include "SubMenu.h"
#include "LabelButton.h"

class LabelButton;
class IComboList;
class WiredWorkMaintainMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
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
    /**
     * @brief layoutExec 布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  数据加载方法
     */
    virtual void readyShow(void);
private slots:
    /**
     * @brief _addressTypeSlot  地址类型改变方法
     * @param index  选择序列号
     */
    void _addressTypeSlot(int index);
    /**
     * @brief editIpAddress 编辑IP地址方法
     */
    void editIpAddress(void);
private:
    /**
     * @brief WiredWorkMaintainMenu 构造方法
     */
    WiredWorkMaintainMenu();
    IComboList  *_addressType;             //地址类型
    LabelButton *_ipAddress;               //IP地址
    LabelButton *_subnetMask;              //子网掩码
    LabelButton *_gateWay;                 //网关
};
#define wiredWorkMaintainMenu (WiredWorkMaintainMenu::construction())
