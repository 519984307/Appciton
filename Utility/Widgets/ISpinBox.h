#pragma once
#include <QWidget>
#include <QLabel>
#include <QEvent>

enum ISpinMode
{
    ISPIN_MODE_INT,
    ISPIN_MODE_FLOAT,
    ISPIN_MODE_SELF_DEFINE,
};

enum CtrlStatus
{
    ISPIN_BOX_NONE,
    ISPIN_BOX_FOCUS_IN,
    ISPIN_BOX_FOCUS_OUT,
    ISPIN_BOX_EDITABLE,
    ISPIN_BOX_DISABLE,
};

class ISpinBoxOperateIFace
{
public:
    //srcStr:原始值，up:加减操作
    virtual const QString nextValue(const QString &srcStr, bool up) = 0;
    virtual ~ISpinBoxOperateIFace(){}
};

class ISpinBoxValue;
class QHBoxLayout;
class QTimer;
class ISpinBox : public QWidget
{
    Q_OBJECT

public:
    // 设置ID
    void setID(int id);

    // 设置前缀和后缀显示的字串。
    void setPrefix(const QString &_prefix);
    void setSuffix(const QString &_suffix);

    // 设置前/后缀与文本框之间的宽度。注意：该接口多次调用后，间隙宽度为多次设置之和。
    void setPrefixSpace(int space);
    void setSuffixSpace(int space);

    // 设置倒角半径。
    void setRadius(double xradius, double yradius);

    // 设置文本对齐方式。
    void setValueAlignment(Qt::Alignment align);
    void setLabelAlignment(Qt::Alignment align);

    // 设置颜色
    const QPalette &palette() const;
    void setPalette(QPalette &p);

    // 设置布局中的控件之间的间隙
    void setLayoutSpacing(int spacing);

    // 设置字体。
    void setValueFont(const QFont &font);

    // 设置边框颜色和宽度。
    void setBorderColor(QColor c);
    void setBorderWidth(int width);
    void enableValueBorder(bool flag);

    // 设置显示控件的尺寸。
    void setValueWidth(int width);
    void setLabelWidth(int width);
    void setStretch(int labelStretch, int valueStreth);

    // 设置显示的小数点显示的位数。
    void setValueBits(int bits);

    // 设置显示的值。
    void setValue(const QString &str);
    void setValue(int v);
    void setValue(double v);

    // 设置整型工作模式。
    void setMode(ISpinMode mode);

    // 设置范围。
    void setRange(int min, int max);
    void setRange(double min, double max);

    // 获取范围
    void getRange(int &min, int &max);
    void getRange(double &min, double &max);

    // 设置是否环回，当数据到达最大最小限时环回。
    inline void enableCycle(bool enable) {_enableCycle = enable;}

    // 设置增量。
    void setStep(int step);
    void setStep(double step);

    // 是否显示箭头。
    void enableArrow(bool enable);

    // 是否响应按键
    void enableDerectionKeyAction(bool flag) {_enableDerectionKeyAction = flag;}
    bool isEnableKeyAction() const {return _enableDerectionKeyAction;}

    void enableEnterKeyAction(bool flag) {_enableEnterAction = flag;}

    // 是否响应连续发送信号
    void enableSendRealSignal(bool flag) {_enableSendRealSignal = flag;}

    // 获取value的数值。
    QString getText(void);

    // 禁用。
    void disable(bool flag);

    // 注册自定义类型操作
    void registerUserOperate(ISpinBoxOperateIFace *operate);

    // 构造与析构。
    ISpinBox();
    ISpinBox(const QString &title, bool displayLabel = true);
    ~ISpinBox();

signals:
    void valueChange(QString, int);
    void realReleased();

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void hideEvent(QHideEvent *e);

private:
    void _ispinbox(const QString &title, bool displayLabel);
    void _valueHandle(bool isIncrese);
    void _setText(const QString &);

private slots:
    void _timeOut();

private:
    QHBoxLayout *_layout;
    QLabel *_label;              // 前缀。
    ISpinBoxValue *_value;       // 显示值。
    QTimer *_timer;

private:
    bool _enableDerectionKeyAction;//是否响应按键
    bool _enableEnterAction;      //是否响应Enter按键
    bool _enableCycle;            //数字是否循环
    bool _enableSendRealSignal;   //是否响应连续发送信号

    int _fBits;  // 显示小数时精确的位数。
    int _id;
    int _intStep;
    int _intValue;
    int _intMin;
    int _intMax;
    int _keyPressCount; //旋钮按键次数，用于连续转动飞梭时发送信号使用

    double _fStep;
    double _fValue;
    double _fMin;
    double _fMax;

    ISpinMode _mode;

    QString _valueStr;  //值
    QString _prefix;    //前缀字串
    QString _suffix;    //后缀字串

    ISpinBoxOperateIFace *_operate;
};

class ISpinBoxValue : public QLabel
{
    Q_OBJECT

public:
    explicit ISpinBoxValue(CtrlStatus status);
    ~ISpinBoxValue();

    // 是否显示箭头。
    void enableArrow(bool enable);

    // 设置边框颜色和宽度。
    void setBorderColor(QColor c);
    void setBorderWidth(int width);
    void enableBorder(bool flag);

    // 设置倒角半径。
    void setRadius(double xradius, double yradius);

    // 设置显示字串
    void setValue(QString &value);

    // 获取/设置状态
    inline void setStatus(CtrlStatus s) {_status = s;}
    CtrlStatus status() const {return _status;}

protected:
    void  paintEvent(QPaintEvent *e);

private:
    void _drawBorder(QPainter &painter);
    void _drawStatus(QPainter &painter);

private:
    bool _enableValueBorder;      // 数值框是否有边框
    bool _enableArrow;            // 是否画上下箭头
    int _borderWidth;             // 边框宽度
    double _xradius;
    double _yradius;

    QColor _borderColor;          // 边框颜色
    CtrlStatus _status;
    QString _value;
};
