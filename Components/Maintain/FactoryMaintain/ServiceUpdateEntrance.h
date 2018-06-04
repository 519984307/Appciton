#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "LabelButton.h"

class ServiceUpdateEntrance  : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static ServiceUpdateEntrance  &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceUpdateEntrance ();
        }
        return *_selfObj;
    }
    static ServiceUpdateEntrance  *_selfObj;
    ~ServiceUpdateEntrance ();
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
     * @brief onBtnSlot  按钮槽函数
     */
    void _onBtnSlot();
private:
    ServiceUpdateEntrance ();

    LabelButton *_button;
};
#define serviceUpdateEntrance  (ServiceUpdateEntrance ::construction())
#define deleteServiceUpdateEntrance () (delete ServiceUpdateEntrance ::_selfObj)
