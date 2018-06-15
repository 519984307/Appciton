#include <QGridLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "KeyBoardPanel.h"
#include "IButton.h"
#include <QCoreApplication>
#include <QInputMethodEvent>
#include "Debug.h"

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

/**************************************************************************************************
 * 一般按键点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedKey(const QString &key)
{
    if (_isInvalid)
    {
        _textDisplay->setText("");
        _isInvalid = false;
    }

    if(_textDisplay->text().size() >= _maxLength && key != QString("\x7f"))
    {
        return;
    }

    QInputMethodEvent ev;
    if(key == QString("\x7f"))
    {
        // 防止没有內容时多次按删除键，导致再次输入时，内容被选中
        if (!_textDisplay->text().isEmpty())
        {
            ev.setCommitString("", -1, 1);
        }
        else
        {
            return;
        }
    }
    else {
        ev.setCommitString(key);
    }
    qApp->sendEvent(_textDisplay, &ev);
}

/**************************************************************************************************
 * 按钮点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedBtn(const QString &key)
{
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
void KeyBoardPanel::ClickedBtn(int id)
{
    if (_keytype != KEY_TYPE_NUMBER)
    {
        _lastFoucsIndex = id;
    }
}

/**************************************************************************************************
 * 帮助按钮点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedHelpBtn(const QString &key)
{
    for (int i = 0; i < _helpKeys.count(); ++i)
    {
        _helpKeys[i]->setText("");
        _helpKeys[i]->setEnabled(false);
    }

    ClickedKey(key);

    if (-1 != _lastFoucsIndex && _keys.count() > _lastFoucsIndex)
    {
        _keys[_lastFoucsIndex]->setFocus();
        _lastFoucsIndex = -1;
    }
}

/**************************************************************************************************
 * Backspace点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedBackspace(void)
{
    ClickedKey(QString("\x7f")); //ascii code of 'delete'
}

/**************************************************************************************************
 * Cancel点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedClear(void)
{
    _textDisplay->clear();
}

/**************************************************************************************************
 * Enter点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedEnter(void)
{
    if (NULL != _checkValue)
    {
        QString text = _textDisplay->text();
        if (_checkValue(text))
        {
            hide();
            done(1);
        }
        else
        {
            _isInvalid = true;
            _textDisplay->setText(trs("InvalidInput"));
            return;
        }
    }

    hide();
    done(1);
}

/**************************************************************************************************
 * 数字点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedNum(void)
{
    if (_keys[KEY_ORDER_NUMBER]->text() == "123")
    {
        _keys[KEY_ORDER_NUMBER]->setText("abc");
        _keytype = KEY_TYPE_NUMBER;
    }
    else
    {
        _keys[KEY_ORDER_NUMBER]->setText("123");
        _keytype = KEY_TYPE_LETTER;
    }

    _loadKeyStatus();
}

/**************************************************************************************************
 * 符号Cancel点击。
 *************************************************************************************************/
void KeyBoardPanel::ClickedSymbol(void)
{
    if (_keytype != KEY_TYPE_SYMBOL)
    {
        _keytype = KEY_TYPE_SYMBOL;
    }
    else if (_keys[KEY_ORDER_NUMBER]->text() == "123")
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
void KeyBoardPanel::_loadKeyStatus()
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
                        displayTxt += QString::number(count + 1);
                        displayTxt += "  ";
                        displayTxt += _keySymbol[count];
                    }
                    break;

                case KEY_TYPE_LETTER:
                    if (count < letterSize)
                    {
                        displayTxt += QString::number(count + 1);
                        displayTxt += "  ";
                        displayTxt += _keyLetter[count];
                    }
                    break;

                default:
                    break;
            }

            index = i * 4 + j;
            if (_keys.count() > index)
            {
                _keys[index]->setText(displayTxt);
                if (_keytype == KEY_TYPE_NUMBER)
                {
                    _keys[index]->setAlignment(Qt::AlignCenter);
                }
                else
                {
                    _keys[index]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                }
            }

            ++count;
        }
    }

    for (int i = 0; i < _helpKeys.count(); ++i)
    {
        _helpKeys[i]->setText("");
        _helpKeys[i]->setEnabled(false);
    }
}

/**************************************************************************************************
 * 加载按键状态。
 *************************************************************************************************/
void KeyBoardPanel::_loadHelpBtn(const QString &key)
{
    QString str = key;
    str.remove(QChar(' '), Qt::CaseInsensitive);//去掉所有空格
    if (_keytype == KEY_TYPE_LETTER)
    {
        if (str.at(0) > '1')
        {
            str += str.right(str.size() - 1).toUpper();
        }
    }

    for (int i = 0; i < _helpKeys.count(); ++i)
    {
        if (i < str.count())
        {
            _helpKeys[i]->setText(QString(str.at(i)));
            _helpKeys[i]->setEnabled(true);
            _helpKeys[i]->setTextColor(Qt::black);
        }
        else
        {
            _helpKeys[i]->setText("");
            _helpKeys[i]->setEnabled(false);
        }
    }

    if (!_regExpStr.isEmpty())
    {
        QRegExp regExp(_regExpStr);
        int pos = 0;
        if (_btnEnable)
        {
            while (!str.isEmpty())
            {
                if (-1 == regExp.indexIn(str))
                {
                    _helpKeys[pos]->setEnabled(false);
                }

                pos++;
                str.remove(0, 1);
            }
        }
        else
        {
            while (-1 != (pos = regExp.indexIn(str, pos)))
            {
                _helpKeys[pos]->setEnabled(false);
                pos += regExp.matchedLength();
            }
        }
    }

    for (int i = 0; i < _helpKeys.count(); ++i)
    {
        if (_helpKeys[i]->isEnabled())
        {
            _helpKeys[i]->setFocus();
            break;
        }
    }
}

/**************************************************************************************************
 * show事件。
 *************************************************************************************************/
void KeyBoardPanel::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);
    _loadKeyStatus();

    if (_keys.count() > KEY_ORDER_ENTER)
    {
        _keys[KEY_ORDER_ENTER]->setFocus();
    }
}

/**************************************************************************************************
 * 功能：设置初始字符串。
 * 参数：
 *      text：文本信息。
 *************************************************************************************************/
void KeyBoardPanel::setInitString(const QString &text)
{
    _textDisplay->setText(text);
}

/**************************************************************************************************
 * 设置最大输入长度。
 *************************************************************************************************/
void KeyBoardPanel::setMaxInputLength(int len)
{
    _maxLength = len;
}

/**************************************************************************************************
 * 获取当前值。
 *************************************************************************************************/
QString KeyBoardPanel::getStrValue(void) const
{
    return _textDisplay->text();
}

/**************************************************************************************************
 * 设置显示模式
 *************************************************************************************************/
void KeyBoardPanel::setEchoMode(QLineEdit::EchoMode mode)
{
    _textDisplay->setEchoMode(mode);
}

/**************************************************************************************************
 * 设置显示按钮，匹配正则字符串
 *************************************************************************************************/
void KeyBoardPanel::setBtnEnable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    _btnEnable = true;
    _regExpStr = regStr;
}

void KeyBoardPanel::setBtnDisable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    _btnEnable = false;
    _regExpStr = regStr;
}

/**************************************************************************************************
 * 设置空格按键使能
 *************************************************************************************************/
void KeyBoardPanel::setSpaceEnable(bool enable)
{
    if (_keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    _keys[KEY_ORDER_SPACE]->setEnabled(enable);
}

/**************************************************************************************************
 * 设置空格按键使能
 *************************************************************************************************/
void KeyBoardPanel::setSymbolEnable(bool enable)
{
    if (_keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    _keys[KEY_ORDER_SYMBOL]->setEnabled(enable);
}

/**************************************************************************************************
 * 设置空格按键使能
 *************************************************************************************************/
void KeyBoardPanel::setKeytypeSwitchEnable(bool enable)
{
    if (_keys.count() <= KEY_ORDER_NUMBER)
    {
        return;
    }

    _keys[KEY_ORDER_NUMBER]->setEnabled(enable);
}

/**************************************************************************************************
 * 设置检查函数
 *************************************************************************************************/
void KeyBoardPanel::setCheckValueHook(CheckValue hook)
{
    _checkValue = hook;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
KeyBoardPanel::KeyBoardPanel(KeyType type) : PopupWidget()
{
    _maxLength = 90;
    _checkValue = NULL;
    _isInvalid = false;
    _regExpStr.isNull();
    _keytype = type;
    _lastFoucsIndex = -1;
    _itemHeight = (_panelHeight-PopupWidget::getTitleBarhight()-_keySpace*4)/6;

    QFont keyFont = fontManager.textFont(_titleFontSize);

    // 设置标题。
    setTitleBarText(trs("EnglishKeyboard"));

    int space = _keySpace;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(2);
    mainLayout->setSpacing(space);

    // 显示框。
    _textDisplay = new QLineEdit();
    _textDisplay->setFont(keyFont);
    _textDisplay->setFixedHeight(_itemHeight);
    _textDisplay->setFocusPolicy(Qt::NoFocus);
    _textDisplay->setAlignment(Qt::AlignLeft);
    _textDisplay->setEchoMode(QLineEdit::Normal);
    mainLayout->addWidget(_textDisplay);

    // 辅助区
    QHBoxLayout *helpLayout = new QHBoxLayout();
    helpLayout->setMargin(0);
    helpLayout->setSpacing(0);
    for (int i = 0; i < 9; ++i)
    {
        IButton *subKey = new IButton();
        subKey->setFont(fontManager.textFont(_keyFontSize));
        subKey->setFixedSize(_itemHeight, _itemHeight);
        subKey->setBorderEnabled(false);
        subKey->setEnabled(false);
        subKey->setInactivityTextColor(Qt::gray);
        subKey->setBackgroundColor(this->palette().window().color());
        subKey->setShdowColor(this->palette().window().color());
        connect(subKey, SIGNAL(clicked(const QString &)), this, SLOT(ClickedHelpBtn(const QString &)));
        helpLayout->addWidget(subKey);
        _helpKeys.append(subKey);
    }
    mainLayout->addLayout(helpLayout);

    // 键盘区。
    QColor color(120, 120, 120);
    QHBoxLayout *hLayout = NULL;
    IButton *key = NULL;
    keyFont = fontManager.textFont(_keyFontSize);
    for (int r = 0; r < 4; r++)
    {
        hLayout = new QHBoxLayout();
        hLayout->setSpacing(space);

        for (int c = 0; c < 4; c++)
        {
            key = new IButton();
            key->setFixedSize(_itemWidth, _itemHeight);
            key->setFont(keyFont);
            key->setBorderEnabled(true);
            key->setBorderColor(color);

            if (c == 3)
            {
                switch (r)
                {
                    case 0:
                        key->setBackgroundColor(QColor(185, 190, 194));
                        key->setPicture(QImage("/usr/local/nPM/icons/backspace.png"));
                        connect(key, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
                        break;

                    case 1:
                        key->setBackgroundColor(QColor(185, 190, 194));
                        key->setPicture(QImage("/usr/local/nPM/icons/clear.png"));
                        connect(key, SIGNAL(realReleased()), this, SLOT(ClickedClear()));
                        break;

                    case 2:
                        key->setBackgroundColor(QColor(185, 190, 194));
                        key->setPicture(QImage("/usr/local/nPM/icons/enter.png"));
                        connect(key, SIGNAL(realReleased()), this, SLOT(ClickedEnter()));
                        break;

                    default:
                        key->setBackgroundColor(QColor(185, 190, 194));
                        key->setText("123");
                        connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedNum()));
                        break;
                }
            }
            else if (3 == r)
            {
                switch (c)
                {
                    case 0:
                        key->setText(" ");
                        key->setPicture(QImage("/usr/local/nPM/icons/blank.png"));
                        connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedKey(const QString &)));
                        break;

                    case 1:
                        key->setText("0");
                        connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedKey(const QString &)));
                        break;

                    case 2:
                        key->setBackgroundColor(QColor(185, 190, 194));
                        key->setText("#@~");
                        connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedSymbol()));
                        break;

                    default:
                        break;
                }
            }
            else
            {
                key->setId(r * 4 + c);
                connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedBtn(const QString &)));
                connect(key, SIGNAL(clicked(int)), this, SLOT(ClickedBtn(int)));
            }
            hLayout->addWidget(key);
            _keys.append(key);
        }
        mainLayout->addLayout(hLayout);
    }
    contentLayout->addLayout(mainLayout);
    contentLayout->addStretch(2);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
KeyBoardPanel::~KeyBoardPanel()
{

}

