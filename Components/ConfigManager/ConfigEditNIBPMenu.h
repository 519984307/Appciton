#pragma once
#include "SubMenu.h"
#include <QScopedPointer>
#include "ConfigEditAlarmLimitMenu.h"
class ConfigEditNIBPMenuPrivate;
class ConfigEditNIBPMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief ConfigEditNIBPMenu  构造方法
     */
    ConfigEditNIBPMenu();
    ~ConfigEditNIBPMenu();

protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec();
    /**
     * @brief readyShow  装载数据
     */
    virtual void readyShow();

private slots:
    /**
     * @brief onComboListConfigChanged  NIBP设置项配置触发方法
     * @param index  选择序列号
     */
    void onComboListConfigChanged(int index);
    void _alarmLbtnSlot(void);

private:
    QScopedPointer<ConfigEditNIBPMenuPrivate> d_ptr;
};
