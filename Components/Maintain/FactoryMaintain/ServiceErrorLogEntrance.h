#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "LabelButton.h"

class ServiceErrorLogEntrance  : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static ServiceErrorLogEntrance  &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceErrorLogEntrance ();
        }
        return *_selfObj;
    }
    static ServiceErrorLogEntrance  *_selfObj;
    ~ServiceErrorLogEntrance ();
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
    ServiceErrorLogEntrance ();

    LabelButton *_button;
};
#define serviceErrorLogEntrance  (ServiceErrorLogEntrance ::construction())
#define deleteServiceErrorLogEntrance () (delete ServiceErrorLogEntrance ::_selfObj)
