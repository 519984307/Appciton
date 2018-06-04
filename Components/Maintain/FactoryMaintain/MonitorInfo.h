#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "LabeledLabel.h"
#include "LabelButton.h"
#include "NumberPanel.h"

class MonitorInfo : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static MonitorInfo &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new MonitorInfo();
        }
        return *_selfObj;
    }
    static MonitorInfo *_selfObj;
    ~MonitorInfo();
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
     * @brief _setSerialNumberSlot 设置电子序列号槽函数
     */
    void _setSerialNumberSlot(void);
private:
    MonitorInfo();

    enum{
        CUMULATIVE_WORKING_TIME=0,
        TEMPERATURE_INSIDE_CASE,
        BATTERY_LEVEL,
        BATTERY_VOLTAGE,
        MACHINE_TYPE,
        MAC_ADDRESS,
        SCREEN_RESOLATION_SIZE,
        ELECTRONIC_SERIAL_NUMBER,
        TYPE_NR
    };

    LabeledLabel *_labeledLabel[TYPE_NR];
    LabelButton  *_setSerialNumber;
    QString       _stringTypeName[TYPE_NR];
};
#define monitorInfo (MonitorInfo::construction())
