#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "LabelButton.h"

class NIBPCalibrationMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static NIBPCalibrationMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPCalibrationMenu();
        }
        return *_selfObj;
    }
    static NIBPCalibrationMenu *_selfObj;
    ~NIBPCalibrationMenu();
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
    NIBPCalibrationMenu();

    LabelButton *_button;
};
#define nibpCalibrationMenu (NIBPCalibrationMenu::construction())
#define deleteNIBPCalibrationMenu() (delete NIBPCalibrationMenu::_selfObj)
