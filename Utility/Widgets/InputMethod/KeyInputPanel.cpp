/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/26
 **/
#include <QGridLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "KeyInputPanel.h"
#include "Button.h"
#include <QCoreApplication>
#include <QInputMethodEvent>
#include <QLineEdit>
#include "Debug.h"
#include <QIcon>
#include <QPainter>
#include <QColorGroup>
#include "ThemeManager.h"

#define ICON_SIZE 32

class KeyInputPanelPrivate
{
public:
    KeyInputPanelPrivate();

    static const int panelWidth = 640;
    static const int panelHeight = 400;
    static const int keySpace = 12;

    static const int itemWidth = (panelWidth - keySpace * 3 - 44) / 4;

    static const int titleFontSize = 24;
    static const int keyFontSize = 28;

    bool isInvalid;
    bool btnEnable;
    int maxLength;
    int itemHeight;
    int lastFoucsIndex;
    QList<Button *> helpKeys;
    QList<Button *> keys;
    QLineEdit *textDisplay;

    QString regExpStr;
    QString invalidStr;
};

KeyInputPanelPrivate::KeyInputPanelPrivate()
    : isInvalid(false),
      btnEnable(false),
      maxLength(0),
      itemHeight(0),
      lastFoucsIndex(0),
      textDisplay(NULL),
      regExpStr(""),
      invalidStr(trs("InvalidInput"))
{
    helpKeys.clear();
    keys.clear();
}

static const char *_keyLetter[] =
{
    ".,?/", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv",
    "wxyz"
};

static const char *_keyNum[] =
{
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
};

static const char *_keySymbol[] =
{
    ".,?/", ":;'\"", "[]{}", "()<>", "_-+=", "`|\\",
    "#@~", "!$%", "^&*",
};

void KeyInputPanel::onKeyClicked()
{
    Button *button = qobject_cast<Button *>(sender());
    QString key = button->text();
    ClickedKey(key);
}

void KeyInputPanel::onBlankClicked()
{
    ClickedKey(" ");
}

/**************************************************************************************************
 * 一般按键点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedKey(const QString &key)
{
    if (d_ptr->isInvalid)
    {
        d_ptr->textDisplay->setText("");
        d_ptr->isInvalid = false;
    }

    if (d_ptr->textDisplay->text().size() >= d_ptr->maxLength && key != QString("\x7f"))
    {
        return;
    }

    QInputMethodEvent ev;
    if (key == QString("\x7f"))
    {
        // 防止没有內容时多次按删除键，导致再次输入时，内容被选中
        if (!d_ptr->textDisplay->text().isEmpty())
        {
            ev.setCommitString("", -1, 1);
        }
        else
        {
            return;
        }
    }
    else
    {
        ev.setCommitString(key);
    }
    qApp->sendEvent(d_ptr->textDisplay, &ev);
}

/**************************************************************************************************
 * 按钮点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedBtnText()
{
    Button *button = qobject_cast<Button *>(sender());
    QString key = button->text();
    switch (_keytype)
    {
    case KEY_TYPE_NUMBER:
        ClickedKey(key);
        break;

    default:
        _loadHelpBtn(key);
        break;
    }
}

/**************************************************************************************************
 * 按钮点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedBtnId()
{
    if (_keytype == KEY_TYPE_NUMBER)
    {
        return;
    }
    Button *button = qobject_cast<Button *>(sender());
    d_ptr->lastFoucsIndex = button->property("Item").toInt();
}

/**************************************************************************************************
 * 帮助按钮点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedHelpBtn()
{
    Button *button = qobject_cast<Button *>(sender());
    QString key = button->text();

    for (int i = 0; i < d_ptr->helpKeys.count(); ++i)
    {
        d_ptr->helpKeys[i]->setText("");
        d_ptr->helpKeys[i]->setEnabled(false);
    }

    ClickedKey(key);

    if (-1 != d_ptr->lastFoucsIndex && d_ptr->keys.count() > d_ptr->lastFoucsIndex)
    {
        d_ptr->keys[d_ptr->lastFoucsIndex]->setFocus();
        d_ptr->lastFoucsIndex = -1;
    }
}

/**************************************************************************************************
 * Backspace点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedBackspace(void)
{
    ClickedKey(QString("\x7f"));  // ascii code of 'delete'
}

/**************************************************************************************************
 * Cancel点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedClear(void)
{
    d_ptr->textDisplay->clear();
}

/**************************************************************************************************
 * Enter点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedEnter(void)
{
    if (NULL != _checkValue)
    {
        QString text = d_ptr->textDisplay->text();
        if (_checkValue(text))
        {
            hide();
            done(1);
        }
        else
        {
            d_ptr->isInvalid = true;
            d_ptr->textDisplay->setText(d_ptr->invalidStr);
            return;
        }
    }

    hide();
    done(1);
}

/**************************************************************************************************
 * 数字点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedNum(void)
{
    if (d_ptr->keys[KEY_ORDER_NUMBER]->text() == "123")
    {
        d_ptr->keys[KEY_ORDER_NUMBER]->setText("abc");
        _keytype = KEY_TYPE_NUMBER;
    }
    else
    {
        d_ptr->keys[KEY_ORDER_NUMBER]->setText("123");
        _keytype = KEY_TYPE_LETTER;
    }

    _loadKeyStatus();
}

/**************************************************************************************************
 * 符号Cancel点击。
 *************************************************************************************************/
void KeyInputPanel::ClickedSymbol(void)
{
    if (_keytype != KEY_TYPE_SYMBOL)
    {
        _keytype = KEY_TYPE_SYMBOL;
    }
    else if (d_ptr->keys[KEY_ORDER_NUMBER]->text() == "123")
    {
        _keytype = KEY_TYPE_LETTER;
    }
    else
    {
        _keytype = KEY_TYPE_NUMBER;
    }

    _loadKeyStatus();
}

/**************************************************************************************************
 * 加载按键状态。
 *************************************************************************************************/
void KeyInputPanel::_loadKeyStatus()
{
    int numSize = sizeof(_keyNum) / sizeof(_keyNum[0]);
    int symbolSize = sizeof(_keySymbol) / sizeof(_keySymbol[0]);
    int letterSize = sizeof(_keyLetter) / sizeof(_keyLetter[0]);

    int index = 0;
    int count = 0;
    QString displayTxt("    ");
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            displayTxt = "     ";
            switch (_keytype)
            {
            case KEY_TYPE_NUMBER:
                if (numSize > count)
                {
                    displayTxt = _keyNum[count];
                }
                break;

            case KEY_TYPE_SYMBOL:
                if (count < symbolSize)
                {
                    displayTxt = "";
                    displayTxt += QString::number(count + 1);
                    displayTxt += "  ";
                    displayTxt += _keySymbol[count];
                }
                break;

            case KEY_TYPE_LETTER:
                if (count < letterSize)
                {
                    displayTxt = "";
                    displayTxt += QString::number(count + 1);
                    displayTxt += "  ";
                    displayTxt += _keyLetter[count];
                }
                break;

            default:
                break;
            }

            index = i * 4 + j;
            if (d_ptr->keys.count() > index)
            {
                d_ptr->keys[index]->setText(displayTxt);
                if (_keytype == KEY_TYPE_NUMBER)
                {
                    d_ptr->keys[index]->setButtonStyle(Button::ButtonTextOnly);
                }
                else
                {
                    d_ptr->keys[index]->setButtonStyle(Button::ButtonTextOnly);
                }
            }

            ++count;
        }
    }

    for (int i = 0; i < d_ptr->helpKeys.count(); ++i)
    {
        d_ptr->helpKeys[i]->setText("");
        d_ptr->helpKeys[i]->setEnabled(false);
    }
}

/**************************************************************************************************
 * 加载按键状态。
 *************************************************************************************************/
void KeyInputPanel::_loadHelpBtn(const QString &key)
{
    QString str = key;
    str.remove(QChar(' '), Qt::CaseInsensitive);  // 去掉所有空格
    if (_keytype == KEY_TYPE_LETTER)
    {
        if (str.at(0) > '1')
        {
            str += str.right(str.size() - 1).toUpper();
        }
    }
    for (int i = 0; i < d_ptr->helpKeys.count(); ++i)
    {
        if (i < str.count())
        {
            d_ptr->helpKeys[i]->setText(QString(str.at(i)));
            d_ptr->helpKeys[i]->setEnabled(true);
        }
        else
        {
            d_ptr->helpKeys[i]->setText("");
            d_ptr->helpKeys[i]->setEnabled(false);
        }
    }


    if (!d_ptr->regExpStr.isEmpty())
    {
        QRegExp regExp(d_ptr->regExpStr);
        int pos = 0;
        if (d_ptr->btnEnable)
        {
            while (!str.isEmpty())
            {
                Qt::GlobalColor color = Qt::black;

                if (-1 == regExp.indexIn(str))
                {
                    d_ptr->helpKeys[pos]->setEnabled(false);
                    color = Qt::gray;
                }

                QPalette pal;
                pal = d_ptr->helpKeys[pos]->palette();
                pal.setColor(QPalette::Foreground, color);
                d_ptr->helpKeys[pos]->setPalette(pal);

                pos++;
                str.remove(0, 1);
            }
        }
        else
        {
            while (-1 != (pos = regExp.indexIn(str, pos)))
            {
                d_ptr->helpKeys[pos]->setEnabled(false);
                QPalette pal;
                pal = d_ptr->helpKeys[pos]->palette();
                pal.setColor(QPalette::Foreground, Qt::gray);
                d_ptr->helpKeys[pos]->setPalette(pal);
                pos += regExp.matchedLength();
            }
        }
    }

    for (int i = 0; i < d_ptr->helpKeys.count(); ++i)
    {
        if (d_ptr->helpKeys[i]->isEnabled())
        {
            d_ptr->helpKeys[i]->setFocus();
            break;
        }
    }
}

/**************************************************************************************************
 * show事件。
 *************************************************************************************************/
void KeyInputPanel::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);
    _loadKeyStatus();

    if (d_ptr->keys.count() > KEY_ORDER_ENTER)
    {
        d_ptr->keys[KEY_ORDER_ENTER]->setFocus();
    }
}

void KeyInputPanel::setInitString(const QString &text, bool isPlaceHolder)
{
    if (isPlaceHolder)
    {
        d_ptr->textDisplay->setPlaceholderText(text);
        return;
    }
    d_ptr->textDisplay->setText(text);
}

/**************************************************************************************************
 * 设置最大输入长度。
 *************************************************************************************************/
void KeyInputPanel::setMaxInputLength(int len)
{
    d_ptr->maxLength = len;
}

/**************************************************************************************************
 * 获取当前值。
 *************************************************************************************************/
QString KeyInputPanel::getStrValue(void) const
{
    return d_ptr->textDisplay->text();
}

/**************************************************************************************************
 * 设置显示模式
 *************************************************************************************************/
void KeyInputPanel::setEchoMode(QLineEdit::EchoMode mode)
{
    d_ptr->textDisplay->setEchoMode(mode);
}

/**************************************************************************************************
 * 设置显示按钮，匹配正则字符串
 *************************************************************************************************/
void KeyInputPanel::setBtnEnable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    d_ptr->btnEnable = true;
    d_ptr->regExpStr = regStr;
}

void KeyInputPanel::setBtnDisable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    d_ptr->btnEnable = false;
    d_ptr->regExpStr = regStr;
}

/**************************************************************************************************
 * 设置空格按键使能
 *************************************************************************************************/
void KeyInputPanel::setSpaceEnable(bool enable)
{
    if (d_ptr->keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    d_ptr->keys[KEY_ORDER_SPACE]->setEnabled(enable);

    if (enable == true)
    {
        d_ptr->keys[KEY_ORDER_SPACE]->setIcon(QIcon("/usr/local/nPM/icons/blank.png"));
        return;
    }

    d_ptr->keys[KEY_ORDER_SPACE]->setIcon(QIcon("/usr/local/nPM/icons/blankDisable.png"));
}

/**************************************************************************************************
 * 设置符号按键使能
 *************************************************************************************************/
void KeyInputPanel::setSymbolEnable(bool enable)
{
    if (d_ptr->keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    d_ptr->keys[KEY_ORDER_SYMBOL]->setEnabled(enable);
}

/**************************************************************************************************
 * 设置空格按键使能
 *************************************************************************************************/
void KeyInputPanel::setKeytypeSwitchEnable(bool enable)
{
    if (d_ptr->keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    d_ptr->keys[KEY_ORDER_NUMBER]->setEnabled(enable);
}

/**************************************************************************************************
 * 设置检查函数
 *************************************************************************************************/
void KeyInputPanel::setCheckValueHook(CheckValue hook)
{
    _checkValue = hook;
}

void KeyInputPanel::setInvalidHint(const QString &str)
{
    d_ptr->invalidStr = str;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
KeyInputPanel::KeyInputPanel(KeyType type, bool isShowDecimalPoint)
    : Dialog(),
      d_ptr(new KeyInputPanelPrivate)
{
    d_ptr->maxLength = 90;
    _checkValue = NULL;
    d_ptr->isInvalid = false;
    d_ptr->regExpStr.isNull();
    _keytype = type;
    d_ptr->lastFoucsIndex = -1;
    d_ptr->itemHeight = (d_ptr->panelHeight - d_ptr->keySpace * 4) / 6;

    QFont keyFont = fontManager.textFont(d_ptr->titleFontSize);

    // 设置标题。
    setWindowTitle(trs("EnglishKeyboard"));

    int space = d_ptr->keySpace;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(space, space * 2, space, space);  // 设置布局器四周预留空间
    mainLayout->setSpacing(space);                                   // 设置布局器内部空间间隔

    // 显示框。
    d_ptr->textDisplay = new QLineEdit();
    d_ptr->textDisplay->setFont(keyFont);
    d_ptr->textDisplay->setFixedHeight(d_ptr->itemHeight);
    d_ptr->textDisplay->setFocusPolicy(Qt::NoFocus);
    d_ptr->textDisplay->setAlignment(Qt::AlignLeft);
    d_ptr->textDisplay->setEchoMode(QLineEdit::Normal);
    mainLayout->addWidget(d_ptr->textDisplay);

    // 辅助区
    QHBoxLayout *helpLayout = new QHBoxLayout();
    helpLayout->setMargin(0);
    helpLayout->setSpacing(0);
    for (int i = 0; i < 9; ++i)
    {
        Button *subKey = new Button();
        subKey->setButtonStyle((Button::ButtonTextOnly));
        subKey->setFont(fontManager.textFont(d_ptr->keyFontSize));
        subKey->setFixedSize(d_ptr->itemHeight, d_ptr->itemHeight);
        subKey->setBorderWidth(0);
        subKey->setEnabled(false);
        connect(subKey, SIGNAL(clicked()), this, SLOT(ClickedHelpBtn()));
        helpLayout->addWidget(subKey);
        d_ptr->helpKeys.append(subKey);
    }
    mainLayout->addLayout(helpLayout);

    // 键盘区。
    Button *key = NULL;
    keyFont = fontManager.textFont(d_ptr->keyFontSize);
    for (int r = 0; r < 4; r++)
    {
       QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setSpacing(space);
        for (int c = 0; c < 4; c++)
        {
            key = new Button();
            key->setButtonStyle(Button::ButtonTextOnly);
            key->setFixedSize(d_ptr->itemWidth, d_ptr->itemHeight);
            key->setFont(keyFont);

            if (c == 3)
            {
                if (r <= 2)
                {
                    key->setButtonStyle(Button::ButtonIconOnly);
                    key->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
                }
                switch (r)
                {
                case 0:
                    key->setIcon(QIcon("/usr/local/nPM/icons/backspace.png"));
                    connect(key, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
                    break;

                case 1:
                    key->setIcon(QIcon("/usr/local/nPM/icons/clear.png"));
                    connect(key, SIGNAL(released()), this, SLOT(ClickedClear()));
                    break;

                case 2:
                    key->setIcon(QIcon("/usr/local/nPM/icons/enter.png"));
                    connect(key, SIGNAL(released()), this, SLOT(ClickedEnter()));
                    break;

                default:
                    key->setText("123");
                    connect(key, SIGNAL(clicked()), this, SLOT(ClickedNum()));
                    break;
                }
            }
            else if (3 == r)
            {
                switch (c)
                {
                case 0:
                    // 显示小数点，占用原有的空白格按键区域
                    if (isShowDecimalPoint)
                    {
                        key->setText(".");
                        key->setButtonStyle(Button::ButtonTextOnly);
                        connect(key, SIGNAL(clicked()), this, SLOT(onKeyClicked()));
                        break;
                    }
                    key->setButtonStyle(Button::ButtonIconOnly);
                    key->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
                    key->setIcon(QIcon("/usr/local/nPM/icons/blank.png"));
                    connect(key, SIGNAL(clicked()), this, SLOT(onBlankClicked()));
                    break;

                case 1:
                    key->setText("0");
                    connect(key, SIGNAL(clicked()), this, SLOT(onKeyClicked()));
                    break;

                case 2:
//                        key->setBackgroundColor(QColor(185, 190, 194));
                    key->setText("#@~");
                    connect(key, SIGNAL(clicked()), this, SLOT(ClickedSymbol()));
                    break;

                default:
                    break;
                }
            }
            else
            {
                key->setProperty("Item", qVariantFromValue((r * 4 + c)));
                connect(key, SIGNAL(clicked()), this, SLOT(ClickedBtnText()));
                connect(key, SIGNAL(clicked()), this, SLOT(ClickedBtnId()));
            }
            hLayout->addWidget(key);
            d_ptr->keys.append(key);
        }
        mainLayout->addLayout(hLayout);
    }
    setWindowLayout(mainLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
KeyInputPanel::~KeyInputPanel()
{
    delete d_ptr;
}

