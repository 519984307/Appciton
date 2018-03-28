#pragma once
#include <QWidget>
#include <QList>
#include <QLabel>
#include "PopupWidget.h"

class IButton;
class QLineEdit;
class NumberInput: public PopupWidget
{
    Q_OBJECT

public:
    // 设置初始字符串。
    void setInitString(const QString &text);

    // 设置最大输入长度。
    void setMaxInputLength(int len);
    void setMinInputLength(int len);

    // 设置单位显示
    void setUnitDisplay(QString text);

    // 获取最大输入长度。
    int getMaxInputLength(void) {return _maxLength;}
    int getMinInputLength(void) {return _minLength;}

    // 禁用数字弹出框上的按键
    void disableNumKey(const QString &str);

    // 获取当前值。
    const QString &getStrValue(void);
    bool getFloatValue(float &v);
    bool getDoubleValue(double &v);
    bool getIntValue(int &v);

    // 构造与析构。
    NumberInput();
    ~NumberInput();

protected:
    virtual void showEvent(QShowEvent *e);
    virtual void exit(void);

protected slots:
    virtual void ClickedKey(const QString &key);
    virtual void ClickedBackspace(void);
    virtual void ClickedEnter(void);

private:
    static const int _titleBarHeight = 35;
    static const int _itemWidth = 40;
    static const int _itemHeight = 40;

    int _maxLength;
    int _minLength;
    IButton *_enter;
    QLineEdit *_textDisplay;
    QLabel *_unit;
    QList<IButton*> _keys;
    QString _text;
};
