#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "LabeledLabel.h"
#include "LabelButton.h"
#include "NumberPanel.h"
#include "IComboList.h"

class MachineConfigModule : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static MachineConfigModule &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new MachineConfigModule();
        }
        return *_selfObj;
    }
    static MachineConfigModule *_selfObj;
    ~MachineConfigModule();
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow(void);
private slots:
    /**
     * @brief onComboListConfigChanged  配置项触发方法
     * @param index  选择序列号
     */
    void onComboListConfigChanged(int index);
private:
    MachineConfigModule();

    enum ModuleType
    {
        MODULE_TYPE_IBP=0,
        MODULE_TYPE_CO2,
        MODULE_TYPE_ECG12LEADS,
        MODULE_TYPE_PRINTER,
        MODULE_TYPE_HDMI,
        MODULE_TYPE_WIFI,
        MODULE_TYPE_SCREEN,
        MODULE_TYPE_NURSECALLING,
        MODULE_TYPE_ANALOGOUTPUT,
        MODULE_TYPE_SYNC_DEFIBRILLATION,
        MODULE_TYPE_CO,
        MODULE_TYPE_NR
    };
    IComboList   *_icomboList[MODULE_TYPE_NR];
    QString       _stringTypeName[MODULE_TYPE_NR];
    QStringList   _stringItems[MODULE_TYPE_NR];
};
#define machineConfigModule (MachineConfigModule::construction())
