#pragma once
#include <QWidget>
#include <QList>
#include <QLineEdit>
#include "PopupWidget.h"

class IButton;
class EnglishPanel: public PopupWidget
{
    Q_OBJECT

public:
    // 设置初始字符串。
    void setInitString(const QString &text);

    // 设置最大输入长度。
    void setMaxInputLength(int len);

    // 获取当前值。
    QString getStrValue(void) const;

    // 设置显示模式
    void setEchoMode(QLineEdit::EchoMode mode);
    
    // 设置显示按钮
//    void setBtnDisable(int alphabet = 0, int nmuber = 0, bool Space = true, const QString symbol = NULL);

    // 设置使能按钮，正则匹配字符串
    void setBtnEnable(const QString &regStr);

    // 设置非使能按钮，正则匹配字符串
    void setBtnDisable(const QString &regStr);

    // 设置空格按键使能
    void setSpaceEnable(bool);

    // 构造与析构。
    EnglishPanel();
    ~EnglishPanel();

    typedef bool (*CheckValue)(QString &);
    void setCheckValueHook(CheckValue hook);

protected:
    virtual void showEvent(QShowEvent *e);

private slots:
    void ClickedKey(const QString &key);
    void ClickedShift(void);
    void ClickedBackspace(void);
    void ClickedClear(void);
    void ClickedEnter(void);

private:
//    void _keySort(const QString key);
    static const int _itemWidth = 34;
    static const int _itemHeight = 34;

    int _maxLength;
    bool _shift;
    bool _isInvalid;
    QList<IButton*> _keys;
    IButton *_space;
    IButton *_enter;
    QLineEdit *_textDisplay;
    CheckValue _checkValue;
//    QStringList _disablekey;
//    QStringList _disableShiftkey;
    void _loadKeyStatus();
    QString _regExpStr;
    bool _btnEnable;
//    int _size;
//    int _shiftsize;
};
