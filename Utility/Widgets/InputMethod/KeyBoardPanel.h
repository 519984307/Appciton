#pragma once
#include <QWidget>
#include <QList>
#include <QLineEdit>
#include "PopupWidget.h"

class IButton;
class KeyBoardPanel: public PopupWidget
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

    // 设置使能按钮，正则匹配字符串
    void setBtnEnable(const QString &regStr);

    // 设置非使能按钮，正则匹配字符串
    void setBtnDisable(const QString &regStr);

    // 设置空格按键使能
    void setSpaceEnable(bool);
    void setSymbolEnable(bool);
    void setKeytypeSwitchEnable(bool);

    typedef bool (*CheckValue)(const QString &);
    void setCheckValueHook(CheckValue hook);

    enum KeyOrder
    {
        KEY_ORDER_BACKSPACE = 0X03,
        KEY_ORDER_CLEAR_ALL = 0X07,
        KEY_ORDER_ENTER     = 0X0B,
        KEY_ORDER_SPACE     = 0X0C,
        KEY_ORDER_SYMBOL    = 0X0E,
        KEY_ORDER_NUMBER    = 0X0F
    };

    enum KeyType
    {
        KEY_TYPE_NUMBER,
        KEY_TYPE_SYMBOL,
        KEY_TYPE_LETTER
    };

    // 构造与析构。
    KeyBoardPanel(KeyType type = KEY_TYPE_LETTER);
    ~KeyBoardPanel();

protected:
    virtual void showEvent(QShowEvent *e);

private slots:
    void ClickedHelpBtn(const QString &key);
    void ClickedBtn(const QString &key);
    void ClickedBtn(int id);
    void ClickedKey(const QString &key);
    void ClickedBackspace(void);
    void ClickedClear(void);
    void ClickedEnter(void);
    void ClickedNum(void);
    void ClickedSymbol(void);

private:
    void _loadKeyStatus();
    void _loadHelpBtn(const QString &key);

private:
    static const int _itemWidth = 100;
    static const int _itemHeight = 34;

    bool _isInvalid;
    bool _btnEnable;
    int _maxLength;
    int _lastFoucsIndex;
    QList<IButton*> _helpKeys;
    QList<IButton*> _keys;
    QLineEdit *_textDisplay;
    CheckValue _checkValue;
    QString _regExpStr;
    KeyType _keytype;
};

