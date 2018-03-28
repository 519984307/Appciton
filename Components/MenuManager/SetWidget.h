#pragma once
#include "NumberPanel.h"
#include "UnitManager.h"

// 设置数据的控件。
class SetWidget: public NumberPad
{
    Q_OBJECT

public:
    // 设置范围，最大和最小限。
    void setRange(double min, double max);
    void setRange(int min, int max);

    // 设置浮点数显示的有效位数。
    void setFloatBits(int bits);

    // 设定当前值。
    void setCurrentValue(int v);
    void setCurrentValue(double v);
    void setCurrentValue(QString str);

    //ip类型
    void setIpAddress(bool flag)
    {
        _isIpAddress = flag;
    }

    //设置QString类型的标题
    void setStrTitle(QString str) {_strTitle = str;}

    // 设置管理的单位。
    void setUnit(UnitType t0, UnitType t1 = UNIT_NONE, UnitType t2 = UNIT_NONE);

    // 构造与析构。
    SetWidget();
    ~SetWidget();

private:
    virtual void ClickedBackspace(void);
    virtual void ClickedClear(void);
    virtual void ClickedKey(const QString &key);
    virtual void ClickedEnter(void);
    virtual void ClickedCancel(void);

private:
    void _showValidInfo(bool isHint = false);
    void _realCalc(int curValue);
    void _showValidFloatInfo(bool isHint = false);
    void _realFloatCalc(float curValue);
    void _showValidStrInfo(bool isHint = false);
    void _realStrCalc(void);
    void _check(void);

    bool _isInt;
    bool _isValid;
    bool _isIpAddress;
    bool _isStr;
    float _minFloatValue;
    float _maxFloatValue;
    float _currentFloatValue;
    int _floatBits;

    int _minValue;
    int _maxValue;
    int _currentValue;

    QString _strTitle;

    UnitType _unit0;
    UnitType _unit1;
    UnitType _unit2;
};
