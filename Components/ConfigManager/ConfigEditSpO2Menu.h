#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditSpO2MenuPrivate;
class ConfigEditSpO2Menu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief ConfigEditSpO2Menu 构造方法
     */
    ConfigEditSpO2Menu();
    ~ConfigEditSpO2Menu();

protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec();
    /**
     * @brief readyShow  装载数据方法
     */
    virtual void readyShow();

private slots:
    /**
     * @brief onComboListConfigChanged  SPO2设置项配置触发方法
     * @param index  选择序列号
     */
    void onComboListConfigChanged(int index);
    void _alarmLbtnSlot(void);

private:
    QScopedPointer<ConfigEditSpO2MenuPrivate> d_ptr;
};
