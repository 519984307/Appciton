#pragma once
#include "MenuWidget.h"
#include "SubMenu.h"

class LabelButton;
class LButtonEn;
class QLabel;
class FactorySystemInfoMenu : public SubMenu
{
    Q_OBJECT

public:
    static FactorySystemInfoMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactorySystemInfoMenu();
        }

        return *_selfObj;
    }
    static FactorySystemInfoMenu *_selfObj;

    ~FactorySystemInfoMenu();

    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow();
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
private slots:
    void _serialNumReleased();

private:
    FactorySystemInfoMenu();

    LabelButton *_serialNum;       //序列号
    LabelButton *_return;
};
#define factorySystemInfoMenu (FactorySystemInfoMenu::construction())
