#include <QPainter>
#include <QBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QTimer>
#include "ISpinBox.h"
#include "FloatHandle.h"
#include "Debug.h"


static inline bool lessThan(double v1, double v2)
{
    return !qFuzzyCompare(v1, v2) && v1 < v2;
}

static inline bool largerThan(double v1, double v2)
{
    return !qFuzzyCompare(v1, v2) && v1 > v2;
}

/**************************************************************************************************
 * 功能：真实构造。
 * 参数：
 *      title： 标题。
 *      displayLabel: 是否显示标题。
 *************************************************************************************************/
void ISpinBox::_ispinbox(const QString &title, bool displayLabel)
{
    // 设置可聚焦策略。
    setFocusPolicy(Qt::StrongFocus);

    // 初始化控制成员。
    _mode = ISPIN_MODE_INT;
    _enableCycle = true;
    _fBits = 1;
    _intStep = 1;
    _intValue = 0;
    _intMin = 0;
    _intMax = 1;
    _fStep = 0.1;
    _fValue = 0.0;
    _fMin = 0.0;
    _fMax = 0.1;
    _enableDerectionKeyAction = true;
    _enableEnterAction = true;
    _enableSendRealSignal = false;
    _keyPressCount = 0;
    _operate = NULL;
    _prefix.clear();
    _suffix.clear();
    _id = 0;

    // 构造子控件对象。
    _value = new ISpinBoxValue(ISPIN_BOX_NONE);
    _value->setAlignment(Qt::AlignCenter);

    _layout = new QHBoxLayout();
    _layout->setMargin(0);
    _layout->setSpacing(0);

    _label = NULL;
    if (displayLabel)
    {
        _label = new QLabel();
        _label->setText(title);
        _label->setAlignment(Qt::AlignCenter);
        _layout->addWidget(_label, 1);
        _layout->addWidget(_value, 2);
    }
    else
    {
        _layout->addWidget(_value);
    }

    setLayout(_layout);

    _timer = new QTimer();
    _timer->setInterval(200);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));
}

/**************************************************************************************************
 * 功能：数据增减操作。
 * 参数：
 *      isIncrese： 是否为增加。
 *************************************************************************************************/
void ISpinBox::_valueHandle(bool isIncrese)
{
    if (_mode == ISPIN_MODE_INT)
    {
        if (isIncrese)
        {
            _intValue += _intStep;
            if (_intValue > _intMax)
            {
                _intValue = (_enableCycle) ? _intMin : _intMax;
            }
        }
        else
        {
            _intValue -= _intStep;
            if (_intValue < _intMin)
            {
                _intValue = (_enableCycle) ? _intMax : _intMin;
            }
        }
        _setText(QString::number(_intValue));
    }
    else if (_mode == ISPIN_MODE_FLOAT)
    {
        if (isIncrese)
        {
            _fValue += _fStep;
            if (largerThan(_fValue, _fMax))
            {
                _fValue = (_enableCycle) ? _fMin : _fMax;
            }
        }
        else
        {
            _fValue -= _fStep;
            if (lessThan(_fValue, _fMin))
            {
                _fValue = (_enableCycle) ? _fMax : _fMin;
            }
        }
        QString str;
        QString bits;
        bits.sprintf("%%.%df", _fBits);
        str.sprintf(qPrintable(bits), _fValue);
        _setText(str);
    }
    else
    {
        if (NULL != _operate)
        {
            setValue(_operate->nextValue(_valueStr, isIncrese));
        }
    }
}

/**************************************************************************************************
 * 功能：数据增减操作。
 * 参数：
 *      isIncrese： 是否为增加。
 *************************************************************************************************/
void ISpinBox::_setText(const QString &valueStr)
{
    _valueStr = valueStr;
    QString text = _prefix;
    text += valueStr;
    text += _suffix;

    _value->setValue(text);
}

/**************************************************************************************************
 * 功能：定时时间到。
 *************************************************************************************************/
void ISpinBox::_timeOut()
{
    static int count = 0;
    if (0 != _keyPressCount)
    {
        if (count == _keyPressCount)
        {
            count = 0;
            _keyPressCount = 0;
            emit valueChange(_valueStr, _id);
        }
        else
        {
            count = _keyPressCount;
        }
    }
}

/**************************************************************************************************
 * 功能：用于返回value中的数值。
 *************************************************************************************************/
QString ISpinBox::getText()
{
    return _valueStr;
}

/**************************************************************************************************
 * 功能：。
 *************************************************************************************************/
void ISpinBox::disable(bool flag)
{
    if(flag)
    {
        _value->setStatus(ISPIN_BOX_DISABLE);
    }
    else
    {
        _value->setStatus(ISPIN_BOX_NONE);
    }
}

/**************************************************************************************************
 * 功能：。
 *************************************************************************************************/
void ISpinBox::registerUserOperate(ISpinBoxOperateIFace *operate)
{
    _operate = operate;
}

/**************************************************************************************************
 * 功能：聚焦事件处理。
 * 参数：
 *      e： 事件。
 *************************************************************************************************/
void ISpinBox::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    _value->setStatus(ISPIN_BOX_FOCUS_IN);
    _value->update();
}

/**************************************************************************************************
 * 功能：失焦事件处理。
 * 参数：
 *      e： 事件。
 *************************************************************************************************/
void ISpinBox::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    _value->setStatus(ISPIN_BOX_FOCUS_OUT);
    _value->update();
}

/**************************************************************************************************
 * 功能：处理键盘按下事件。
 * 参数：
 *      e： 事件。
 *************************************************************************************************/
void ISpinBox::keyPressEvent(QKeyEvent *e)
{
    QString str = _valueStr;

    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            if ((_value->status() == ISPIN_BOX_NONE) || (_value->status() == ISPIN_BOX_FOCUS_IN))
            {
                if (_enableDerectionKeyAction)
                {
                    focusPreviousChild();
                    return;
                }
            }
            else if (_value->status() == ISPIN_BOX_EDITABLE)
            {
                _valueHandle(false);

                if (str != _valueStr && _enableSendRealSignal)
                {
                    ++_keyPressCount;
                }
                return;
            }
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            if ((_value->status() == ISPIN_BOX_NONE) || (_value->status() == ISPIN_BOX_FOCUS_IN))
            {
                if (_enableDerectionKeyAction)
                {
                    focusNextChild();
                    return;
                }
            }
            else if (_value->status() == ISPIN_BOX_EDITABLE)
            {
                _valueHandle(true);

                if (str != _valueStr && _enableSendRealSignal)
                {
                    ++_keyPressCount;
                }

                return;
            }
            break;

        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 功能：处理键盘弹起事件。
 * 参数：
 *      e： 事件。
 *************************************************************************************************/
void ISpinBox::keyReleaseEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if (_value->status() == ISPIN_BOX_FOCUS_IN)
        {
            if (!_enableEnterAction)
            {
                return;
            }

            if (_enableSendRealSignal && NULL != _timer)
            {
                _keyPressCount = 0;
                _timer->start();
            }

            _value->setStatus(ISPIN_BOX_EDITABLE);
            emit realReleased();
        }
        else if (_value->status() == ISPIN_BOX_EDITABLE)
        {
            _keyPressCount = 0;

            if (_enableSendRealSignal && NULL != _timer)
            {
                _timer->stop();
            }

            _value->setStatus(ISPIN_BOX_FOCUS_IN);
            emit valueChange(_valueStr, _id);
        }

        _value->update();
    }

    QWidget::keyReleaseEvent(e);
}

/**************************************************************************************************
 * 功能：隐藏事件。
 * 参数：
 *      e： 事件。
 *************************************************************************************************/
void ISpinBox::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    // 防止界面消失后定时器继续运行
    if (NULL != _timer)
    {
        if (_timer->isActive())
        {
            _timer->stop();
        }
    }
}

/**************************************************************************************************
 * 功能：设置ID。
 * 参数：
 *      id 前缀字串。
 *************************************************************************************************/
void ISpinBox::setID(int id)
{
    _id = id;
}

/**************************************************************************************************
 * 功能：设置前缀字串。
 * 参数：
 *      prefix： 前缀字串。
 *************************************************************************************************/
void ISpinBox::setPrefix(const QString &text)
{
    _prefix = text;
}

/**************************************************************************************************
 * 功能：设置后缀字串。
 * 参数：
 *      suffix： 后缀字串。
 *************************************************************************************************/
void ISpinBox::setSuffix(const QString &text)
{
    _suffix = text;
}

/**************************************************************************************************
 * 功能：设置前缀与文本框之间的宽度。
 * 参数：
 *      space：间隙宽度。
 *************************************************************************************************/
void ISpinBox::setPrefixSpace(int space)
{
    if (space <= 0)
    {
        return;
    }

    QString blankStr(space, ' ');
    _prefix += blankStr;
}

/**************************************************************************************************
 * 功能：设置后缀与文本框之间的宽度。
 * 参数：
 *      space：间隙宽度。
 *************************************************************************************************/
void ISpinBox::setSuffixSpace(int space)
{
    if (space <= 0)
    {
        return;
    }

    QString blankStr(space, ' ');
    blankStr += _suffix;
    _suffix = blankStr;
}

/**************************************************************************************************
 * 功能：设置倒角半径。
 * 参数：
 *      radius:半径
 *************************************************************************************************/
void ISpinBox::setRadius(double xradius, double yradius)
{
    _value->setRadius(xradius, yradius);
}

/**************************************************************************************************
 * 功能：设置数据文本对齐方式。
 * 参数：
 *      align：对齐方式。
 *************************************************************************************************/
void ISpinBox::setValueAlignment(Qt::Alignment align)
{
    _value->setAlignment(align);
}

/**************************************************************************************************
 * 功能：设置标签文本对齐方式。
 * 参数：
 *      align：对齐方式。
 *************************************************************************************************/
void ISpinBox::setLabelAlignment(Qt::Alignment align)
{
    if (NULL != _label)
    {
        _label->setAlignment(align);
    }
}

/**************************************************************************************************
 * 功能：设置颜色。
 * 参数：
 *      p:调色板
 *************************************************************************************************/
const QPalette &ISpinBox::palette(void) const
{
    return _value->palette();
}

/**************************************************************************************************
 * 功能：设置颜色。
 * 参数：
 *      p:调色板
 *************************************************************************************************/
void ISpinBox::setPalette(QPalette &p)
{
    _value->setPalette(p);
}

/**************************************************************************************************
 * 功能：设置布局间隙。
 * 参数：
 *      spacing:间隙。
 *************************************************************************************************/
void ISpinBox::setLayoutSpacing(int spacing)
{
    _layout->setSpacing(spacing);
}

/**************************************************************************************************
 * 功能：设置字体。
 * 参数：
 *      font：字体。
 *************************************************************************************************/
void ISpinBox::setValueFont(const QFont &font)
{
    _value->setFont(font);
}

/**************************************************************************************************
 * 功能：设置显示区域的尺寸。
 * 参数：
 *      width：宽度。
 *************************************************************************************************/
void ISpinBox::setValueWidth(int width)
{
    _value->setFixedWidth(width);
}

/**************************************************************************************************
 * 功能：设置标题的尺寸。
 * 参数：
 *      width：宽度。
 *************************************************************************************************/
void ISpinBox::setLabelWidth(int width)
{
    if (NULL != _label)
    {
        _label->setFixedWidth(width);
    }
}

/**************************************************************************************************
 * 功能：设置显示比例。
 *************************************************************************************************/
void ISpinBox::setStretch(int labelStretch, int valueStreth)
{
    if (NULL == _label)
    {
        return;
    }

    _layout->setStretchFactor(_label, labelStretch);
    _layout->setStretchFactor(_value, valueStreth);
}

/**************************************************************************************************
 * 功能：设置边框颜色。
 * 参数：
 *      r:颜色
 *************************************************************************************************/
void ISpinBox::setBorderColor(QColor r)
{
    _value->setBorderColor(r);
}

/**************************************************************************************************
 * 功能：设置边框宽度。
 * 参数：
 *      width:宽度
 *************************************************************************************************/
void ISpinBox::setBorderWidth(int width)
{
    _value->setBorderWidth(width);
}

/**************************************************************************************************
 * 功能：启用边框。
 *************************************************************************************************/
void ISpinBox::enableValueBorder(bool flag)
{
    _value->enableBorder(flag);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      str：需显示的数据。
 *************************************************************************************************/
void ISpinBox::setValue(const QString &str)
{
    _setText(str);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      v：需显示的数据。
 *************************************************************************************************/
void ISpinBox::setValue(int v)
{
    if (_mode == ISPIN_MODE_FLOAT)
    {
        return;
    }
    if ((v < _intMin) || (v > _intMax))
    {
        return;
    }

    if (0 != _keyPressCount)
    {
        return;
    }

    _intValue = v;
    QString str;

    str.sprintf("%d", v);
    _setText(str);
}

/**************************************************************************************************
 * 功能：设置显示的值。
 * 参数：
 *      v：需显示的数据。
 *************************************************************************************************/
void ISpinBox::setValue(double v)
{
    if (_mode == ISPIN_MODE_INT)
    {
        return;
    }
    if (lessThan(v, _fMin) || largerThan(v, _fMax))
    {
        return;
    }

    if (0 != _keyPressCount)
    {
        return;
    }

    _fValue = v;
    QString str;
    QString bits;
    bits.sprintf("%%.%df", _fBits);
    str.sprintf(qPrintable(bits), v);
    _setText(str);
}

/**************************************************************************************************
 * 设置整型工作模式。
 *************************************************************************************************/
void ISpinBox::setMode(ISpinMode mode)
{
    _mode = mode;
}

/**************************************************************************************************
 * 设置范围。
 *************************************************************************************************/
void ISpinBox::setRange(int min, int max)
{
    _intMin = min;
    _intMax = max;
}

/**************************************************************************************************
 * 设置范围。
 *************************************************************************************************/
void ISpinBox::setRange(double min, double max)
{
    _fMin = min;
    _fMax = max;
}

/**************************************************************************************************
 * 获取范围。
 *************************************************************************************************/
void ISpinBox::getRange(int &min, int &max)
{
    min = _intMin;
    max = _intMax;
}

/**************************************************************************************************
 * 获取范围。
 *************************************************************************************************/
void ISpinBox::getRange(double &min, double &max)
{
    min = _fMin;
    max = _fMax;
}

/**************************************************************************************************
 * 设置增量。
 *************************************************************************************************/
void ISpinBox::setStep(int step)
{
    _intStep = step;
}

/**************************************************************************************************
 * 设置增量。
 *************************************************************************************************/
void ISpinBox::setStep(double step)
{
    _fStep = step;
}

/**************************************************************************************************
 * 是否显示箭头。
 *************************************************************************************************/
void ISpinBox::enableArrow(bool enable)
{
    _value->enableArrow(enable);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ISpinBox::ISpinBox() : QWidget()
{
    _ispinbox("", false);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ISpinBox::ISpinBox(const QString &title, bool displayLabel) : QWidget()
{
    _ispinbox(title, displayLabel);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ISpinBox::~ISpinBox()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }

    if (_operate != NULL)
    {
        delete _operate;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ISpinBoxValue::ISpinBoxValue(CtrlStatus status) : QLabel()
{
    // 设置默认的颜色。
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Highlight, QColor(227, 89, 42));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::ToolTipBase, QColor(146, 208, 80));
    palette.setColor(QPalette::ToolTipText, Qt::white);
    setPalette(palette);

    _status = status;
    _borderColor = Qt::black;
    _borderWidth = 1;
    _enableArrow = true;
    _enableValueBorder = true;
    _xradius = 4;
    _yradius = 4;
    _value.clear();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ISpinBoxValue::~ISpinBoxValue()
{

}

/**************************************************************************************************
 * 是否显示箭头。
 *************************************************************************************************/
void ISpinBoxValue::enableArrow(bool enable)
{
    _enableArrow = enable;
}

/**************************************************************************************************
 * 功能：设置边框颜色。
 * 参数：
 *      r:颜色
 *************************************************************************************************/
void ISpinBoxValue::setBorderColor(QColor r)
{
    _borderColor = r;
}

/**************************************************************************************************
 * 功能：设置边框宽度。
 * 参数：
 *      width:宽度
 *************************************************************************************************/
void ISpinBoxValue::setBorderWidth(int width)
{
    _borderWidth = width;
}

/**************************************************************************************************
 * 功能：启用边框。
 *************************************************************************************************/
void ISpinBoxValue::enableBorder(bool flag)
{
    _enableValueBorder = flag;
}

/**************************************************************************************************
 * 功能：设置倒角半径。
 * 参数：
 *      radius:半径
 *************************************************************************************************/
void ISpinBoxValue::setRadius(double xradius, double yradius)
{
    _xradius = xradius;
    _yradius = yradius;
}

/**************************************************************************************************
 * 功能：设置显示字串。
 * 参数：
 *      radius:半径
 *************************************************************************************************/
void ISpinBoxValue::setValue(QString &value)
{
    _value = value;
    update();
}

/**************************************************************************************************
 * 功能：绘制边框。
 * 参数：
 *      painter： 绘制器。
 *************************************************************************************************/
void ISpinBoxValue::_drawBorder(QPainter &painter)
{
    if (!_enableArrow)
    {
        return;
    }

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    QRect r = rect();

    // 绘制指示箭头，首先绘制上箭头。
    int space = 4;
    int arrowWidth = (r.height() - space * 2) / 3;
    int arrowHeight = (r.height() - space * 3) / 2;
    int x0 = r.x() + r.width() - space - arrowWidth;
    int y0 = r.y() + space;
    int x1 = x0 - arrowWidth;
    int y1 = y0 + arrowHeight;
    int x2 = x0 + arrowWidth;
    int y2 = y1;
    QPolygon pts;
    pts.setPoints(3, x0, y0, x1, y1, x2, y2);
    painter.drawConvexPolygon(pts);

    // 绘制下箭头。
    x0 = x1;
    y0 = y1 + space;
    x1 = x2;
    y1 = y0;
    x2 = x0 + arrowWidth;
    y2 = y1 + arrowHeight;

    pts.setPoints(3, x0, y0, x1, y1, x2, y2);
    painter.drawConvexPolygon(pts);
}

/**************************************************************************************************
 * 功能：绘制背景。
 * 参数：
 *      painter： 绘制器。
 *************************************************************************************************/
void ISpinBoxValue::_drawStatus(QPainter &painter)
{
    QPalette palete = this->palette();

    QPen pen;
    pen.setColor(_borderColor);
    pen.setWidth(_borderWidth);

    if (_enableValueBorder)
    {
        painter.setPen(pen);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        if (_status == ISPIN_BOX_FOCUS_IN || _status == ISPIN_BOX_EDITABLE)
        {
            painter.setRenderHint(QPainter::Antialiasing, true);
        }
        else
        {
            painter.setRenderHint(QPainter::Antialiasing, false);
        }
    }

    if (_status == ISPIN_BOX_FOCUS_IN)
    {
        painter.setBrush(palete.highlight());
        palete.setColor(QPalette::WindowText, Qt::white);
     }
    else if (_status == ISPIN_BOX_EDITABLE)
    {
        painter.setBrush(palete.toolTipBase());
    }
    else if (_status == ISPIN_BOX_DISABLE)
    {
        painter.setBrush(palete.dark());
    }
    else
    {
        painter.setBrush(palete.window());
    }

    painter.drawRoundedRect(rect(), _xradius, _yradius);

    // 绘制文字
    painter.setPen(palete.windowText().color());
    painter.drawText(rect(), _value, this->alignment());
}

/**************************************************************************************************
 * 功能：绘制事件。
 * 参数：
 *      e： 绘制事件。
 *************************************************************************************************/
void ISpinBoxValue::paintEvent(QPaintEvent */*e*/)
{
    // 绘图准备。
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 根据状态填充位图矩形框。
    _drawStatus(painter);

    // 绘制边框。
    _drawBorder(painter);
}


