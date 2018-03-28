#include "SetWidget.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "FloatHandle.h"
#include "Debug.h"

/**************************************************************************************************
 * 显示无效数据信息。
 *************************************************************************************************/
void SetWidget::_showValidFloatInfo(bool isHint)
{
    QString bits;
    bits.sprintf("(Range:%s%d%s%d%s", "%.", _floatBits, "f~%.", _floatBits, "f");

    // 范围。
    QString text;
    text.sprintf(qPrintable(bits), _minFloatValue, _maxFloatValue);
    text += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text += ")\n";

    QString str;
    if (isHint)
    {
        text += trs("ErrorInput");
        setTitleBarText(text);
        return;
    }

    str += "?";
    str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text.append(str);

    if (_unit1 != UNIT_NONE)
    {
        str.clear();
        str += "    ?";
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit1)));
        text.append(str);
    }

    if (_unit2 != UNIT_NONE)
    {
        str.clear();
        str += "    ?";
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit2)));
        text.append(str);
    }

    setTitleBarText(text);
}

/**************************************************************************************************
 * 实时计算当前的值。
 *************************************************************************************************/
void SetWidget::_realFloatCalc(float curValue)
{
    QString bits;
    bits.sprintf("(Range:%s%d%s%d%s", "%.", _floatBits, "f~%.", _floatBits, "f");

    // 范围。
    QString text;
    text.sprintf(qPrintable(bits), _minFloatValue, _maxFloatValue);
    text += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text += ")\n";

    // 当前值，使用第0个单位。
    bits.sprintf("%s%d%s", "%.", _floatBits, "f");
    QString str;
    str.sprintf(qPrintable(bits), curValue);
    str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text.append(str);

    if (_unit1 != UNIT_NONE)
    {
        str.clear();
        str = "    ";
        str += Unit::convert(_unit1, _unit0, curValue);
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit1)));
        text.append(str);
    }

    if (_unit2 != UNIT_NONE)
    {
        str.clear();
        str = "    ";
        str += Unit::convert(_unit2, _unit0, curValue);
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit2)));
        text.append(str);
    }

    setTitleBarText(text);
}

/**************************************************************************************************
 * 显示无效数据信息。
 *************************************************************************************************/
void SetWidget::_showValidInfo(bool isHint)
{
    QString text;
    text.sprintf("(Range:%d~%d", _minValue, _maxValue);
    text += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text += ")\n";

    QString str;
    if (isHint)
    {
        text += trs("ErrorInput");
        setTitleBarText(text);
        return;
    }

    str += "?";
    str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text.append(str);

    if (_unit1 != UNIT_NONE)
    {
        str.clear();
        str += "    ?";
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit1)));
        text.append(str);
    }

    if (_unit2 != UNIT_NONE)
    {
        str.clear();
        str += "    ?";
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit2)));
        text.append(str);
    }

    setTitleBarText(text);
}

/**************************************************************************************************
 * 实时计算当前的值。
 *************************************************************************************************/
void SetWidget::_realCalc(int curValue)
{
    QString bits;
    bits.sprintf("%s%d%s", "%.", _floatBits, "f");

    // 范围。
    QString text;
    text.sprintf("(Range:%d~%d", _minValue, _maxValue);
    text += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text += ")\n";

    // 当前值，使用第0个单位。
    QString str;
    str.sprintf("%d", curValue);
    str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit0)));
    text.append(str);

    if (_unit1 != UNIT_NONE)
    {
        str.clear();
        str = "    ";
        str += Unit::convert(_unit1, _unit0, curValue);
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit1)));
        text.append(str);
    }

    if (_unit2 != UNIT_NONE)
    {
        str.clear();
        str = "    ";
        str += Unit::convert(_unit2, _unit0, curValue);
        str += trs(QString::fromLocal8Bit(Unit::getSymbol(_unit2)));
        text.append(str);
    }

    setTitleBarText(text);
}

/**************************************************************************************************
 * 显示无效数据信息。
 *************************************************************************************************/
void SetWidget::_showValidStrInfo(bool isHint)
{
    QString text = _strTitle;

    if (isHint)
    {
        text += trs("ErrorInput");
        setTitleBarText(text);
        return;
    }

    setTitleBarText(text);
}

/**************************************************************************************************
 * 实时计算当前的值。
 *************************************************************************************************/
void SetWidget::_realStrCalc()
{
    QString text = _strTitle;
//    text += "\n";

    setTitleBarText(text);
}

/**************************************************************************************************
 * 检查输入的合法性。
 *************************************************************************************************/
void SetWidget::_check(void)
{
    if (_isInt)
    {
        int v;
        bool ok = getIntValue(v);
        if (ok)
        {
            if ((v >= _minValue) && (v <= _maxValue))
            {
                _isValid = true;
            }
            else
            {
                _isValid = false;
            }
            _realCalc(v);
        }
        else
        {
            _isValid = false;
            _showValidInfo();
        }
    }
    else if (_isStr)
    {
        QString str = getStrValue();
        if (_isIpAddress)
        {
            QStringList strlist = str.split('.', QString::SkipEmptyParts);
            int size = strlist.size();
            if (size != 4)
            {
                _isValid = false;
            }
            else
            {
                int p;
                bool ok;
                for (int i = 0; i < size; ++i)
                {
                    p = strlist.at(i).toInt(&ok);
                    if (!ok || p < 0 || p > 255)
                    {
                        _isValid = false;
                        break;
                    }

                    _isValid = true;
                }
            }
        }
        else
        {
            _isValid = true;
            if (getMaxInputLength() < str.size())
            {
                _isValid = false;
            }

            if (getMinInputLength() > str.size())
            {
                _isValid = false;
            }
        }
        _showValidStrInfo();
    }
    else
    {
        float v;
        bool ok = getFloatValue(v);
        if (ok)
        {
            if ((v >= _minFloatValue) && (v <= _maxFloatValue))
            {
                _isValid = true;
            }
            else
            {
                _isValid = false;
            }
            _realFloatCalc(v);
        }
        else
        {
            _isValid = false;
            _showValidFloatInfo();
        }
    }
}

/**************************************************************************************************
 * 重载回退键按下。
 *************************************************************************************************/
void SetWidget::ClickedBackspace(void)
{
    NumberPad::ClickedBackspace();
}

/**************************************************************************************************
 * 重载清除键按下。
 *************************************************************************************************/
void SetWidget::ClickedClear(void)
{
    NumberPad::ClickedClear();
}

/**************************************************************************************************
 * 重载按键按下。
 *************************************************************************************************/
void SetWidget::ClickedKey(const QString &key)
{
    NumberPad::ClickedKey(key);
}

/**************************************************************************************************
 * 重载enter按键。
 *************************************************************************************************/
void SetWidget::ClickedEnter(void)
{
    _check();
    if (!_isValid)
    {
        if (_isInt)
        {
            _showValidInfo(true);
        }
        else if (_isStr)
        {
            _showValidStrInfo(true);
        }
        else
        {
            _showValidFloatInfo(true);
        }
        return;
    }

    _unit0 = UNIT_NONE;
    _unit1 = UNIT_NONE;
    _unit2 = UNIT_NONE;
    NumberPad::ClickedEnter();
}

/**************************************************************************************************
 * 重载Cancel按键。
 *************************************************************************************************/
void SetWidget::ClickedCancel(void)
{
    _unit0 = UNIT_NONE;
    _unit1 = UNIT_NONE;
    _unit2 = UNIT_NONE;
    NumberPad::ClickedCancel();
}

/**************************************************************************************************
 * 设置范围。
 *************************************************************************************************/
void SetWidget::setRange(double min, double max)
{
    _minFloatValue = min;
    _maxFloatValue = max;
    _isInt = false;
}

/**************************************************************************************************
 * 设置浮点的有效位数。
 *************************************************************************************************/
void SetWidget::setFloatBits(int bits)
{
    _floatBits = bits;
}

/**************************************************************************************************
 * 设置范围。
 *************************************************************************************************/
void SetWidget::setRange(int min, int max)
{
    _minValue = min;
    _maxValue = max;
    _isInt = true;
}

/**************************************************************************************************
 * 设定当前值。
 *************************************************************************************************/
void SetWidget::setCurrentValue(double v)
{
    _isValid = true;
    _currentFloatValue = v;
    _realFloatCalc(_currentFloatValue);

    QString format;
    format.sprintf("%s%df", "%.", _floatBits);
    QString text;
    text.sprintf(qPrintable(format), v);
    setInitString(text);
}

/**************************************************************************************************
 * 设定当前值。
 *************************************************************************************************/
void SetWidget::setCurrentValue(int v)
{
    _isValid = true;
    _currentValue = v;
    _realCalc(_currentValue);
    setInitString(QString::number(v));
}

/**************************************************************************************************
 * 设定当前值。
 *************************************************************************************************/
void SetWidget::setCurrentValue(QString str)
{
    _isValid = true;
    _isStr = true;
    _realStrCalc();
    setInitString(str);
}

/**************************************************************************************************
 * 设置管理的单位。
 *************************************************************************************************/
void SetWidget::setUnit(UnitType t0, UnitType t1, UnitType t2)
{
    _unit0 = t0;
    _unit1 = t1;
    _unit2 = t2;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SetWidget::SetWidget() : NumberPad()
{
    setTitleBarFont(fontManager.textFont(fontManager.getFontSize(3)));
    setMaxInputLength(5);

    _unit0 = UNIT_NONE;
    _unit1 = UNIT_NONE;
    _unit2 = UNIT_NONE;
    _isInt = false;
    _isValid = false;
    _isIpAddress = false;
    _isStr = false;
    _minFloatValue = 0.0;
    _maxFloatValue = 0.0;
    _currentFloatValue = 0.0;
    _floatBits = 1;
    _minValue = 0;
    _maxValue = 0;
    _currentValue = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SetWidget::~SetWidget()
{

}
