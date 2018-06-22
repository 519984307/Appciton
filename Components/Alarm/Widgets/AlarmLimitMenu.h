#pragma once
#include <QKeyEvent>
#include <QHBoxLayout>
#include "SubMenu.h"
#include "IComboList.h"
#include "ISpinBox.h"
#include "ParamDefine.h"
#include "UnitManager.h"
#include "IBPDefine.h"

class IComboList;
class LButton;
class SetItem : public QWidget
{
public:
    SetItem(ParamID ppid, SubParamID ssid) : QWidget()
    {
        pid = ppid;
        sid = ssid;

        combo = new IComboList("");
        lower = new ISpinBox();
        upper = new ISpinBox();
        lower->enableArrow(false);
        upper->enableArrow(false);
        priority = new IComboList("");
#if 0
        record = new IComboList("");
#endif
        QHBoxLayout *manlayout = new QHBoxLayout();
        manlayout->setSpacing(2);
        manlayout->setMargin(0);

        manlayout->addWidget(combo);
        combo->setSpacing(0);
        manlayout->addWidget(lower);
        manlayout->addWidget(upper);
        manlayout->addWidget(priority);
        priority->setSpacing(0);
#if 0
        manlayout->addWidget(record);
        priority->setSpacing(2);
#endif
        setLayout(manlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(combo);
        setFocusProxy(priority);
       // setFocusProxy(record);
    }
    ~SetItem()
    {

    }

public:
    ParamID pid;
    SubParamID sid;

    IComboList *combo;
    ISpinBox *lower;
    ISpinBox *upper;
    IComboList *priority;
    //IComboList *record;
};

class AlarmLimitMenu : public SubMenu
{
    Q_OBJECT

public:
    static AlarmLimitMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AlarmLimitMenu();
        }
        return *_selfObj;
    }
    static AlarmLimitMenu *_selfObj;
    ~AlarmLimitMenu();

    // 检查参数超限报警状态
    void checkAlarmEnableStatus();

    // IBP报警项设置
    void setIBPAlarmItem(IBPPressureName ibp1, IBPPressureName ibp2);

    //获取聚焦点值
    int getFocusIndex(void);

    //设置聚焦点值
    void setFocusIndex(int index);

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);
    virtual void readyhide(void);

private slots:
    void _limitChange(QString, int);
    void _comboListIndexChanged(int id, int index);
    void _onConfigUpdated(void);
private:
    AlarmLimitMenu();

    // 载入可选项的值。
    void _loadOptions(void);

    QList<SetItem*> _itemList;

    int _focusIndex;
};
#define alarmLimitMenu (AlarmLimitMenu::construction())
