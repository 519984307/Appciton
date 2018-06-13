#pragma once
#include "SubMenu.h"
#include "AGDefine.h"
#include "LabelButton.h"

class IComboList;
class AGMenu : public SubMenu
{
    Q_OBJECT

public:
    static AGMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AGMenu();
        }
        return *_selfObj;
    }
    static AGMenu *_selfObj;

public:
    ~AGMenu();
    /**
     * @brief getAGEnabled 获得AG模块是否使能状态位
     * @return  //使能则返回true,反之则返回false;
     */
    bool getAGEnabled(void)const{return _isEnable;}

protected:
    // Show之前的准备工作。
    virtual void readyShow(void);
    /**
     * @brief _loadOptions 装载数据
     */
    void _loadOptions(void);

    // Hide之前的清理工作。
    virtual void readyhide(void);

    //布局
    virtual void layoutExec(void);

private slots:
    void _speedIcombSlot(int index);
    void _isEnableAGComboSlot(int index);
    void _lBtonSlot(void);

private:
    AGMenu();

    IComboList *_isEnableAGCombo; //是否使能AG模块
    LabelButton *_agGasTypeLbtn[AG_TYPE_NR];//AG气体类型
    IComboList *_speedIcomb;             // 波形速度。
    bool _isEnable;//模块使能状态位
};
#define agMenu (AGMenu::construction())
