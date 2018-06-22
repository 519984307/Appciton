#pragma once
#include "SubMenu.h"
#include "AlarmLimitMenu.h"
#include <QScopedPointer>

class ConfigEditAlarmLimitMenuPrivate;
class ConfigEditAlarmLimitMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditAlarmLimitMenu();
    ~ConfigEditAlarmLimitMenu();

    //获取聚焦点值
    int getFocusIndex(void)const;

    //设置聚焦点值
    void setFocusIndex(int index);
    /**
     * @brief eventFilter 导入导出窗口事件过滤方法
     * @param obj  导入导出窗口对象
     * @param ev   事件
     * @return     返回 false
     */
    bool eventFilter(QObject *obj, QEvent *ev);
protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void _limitChanged(QString, int);
    void _comboListIndexChanged(int id, int index);
private:
    QScopedPointer<ConfigEditAlarmLimitMenuPrivate> d_ptr;
};
