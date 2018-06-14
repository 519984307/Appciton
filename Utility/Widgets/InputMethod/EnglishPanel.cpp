#include <QGridLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "EnglishPanel.h"
#include "IButton.h"
#include <QCoreApplication>
#include <QInputMethodEvent>
#include "Debug.h"


static const char *_keySymbol[] =
{
    "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\\",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'",
    "z", "x", "c", "v", "b", "n", "m", ",", ".", "/",
};

static const char *_keyShiftSymbol[] =
{
    "~", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "|",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"",
    "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?",
};

/**************************************************************************************************
 * 一般按键点击。
 *************************************************************************************************/
void EnglishPanel::ClickedKey(const QString &key)
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
 * Shift点击。
 *************************************************************************************************/
void EnglishPanel::ClickedShift(void)
{
    _shift = !_shift;

     if (_shift)
     {
         for (int i = 0; i < _keys.size(); i++)
         {
             _keys[i]->setText(_keyShiftSymbol[i]);
         }
     }
     else
     {
         for (int i = 0; i < _keys.size(); i++)
         {
             _keys[i]->setText(_keySymbol[i]);
         }
     }

     _loadKeyStatus();
}

/**************************************************************************************************
 * Backspace点击。
 *************************************************************************************************/
void EnglishPanel::ClickedBackspace(void)
{
    ClickedKey(QString("\x7f")); //ascii code of 'delete'
}

/**************************************************************************************************
 * Cancel点击。
 *************************************************************************************************/
void EnglishPanel::ClickedClear(void)
{
    _textDisplay->clear();
//    hide();
//    done(0);
}

/**************************************************************************************************
 * Enter点击。
 *************************************************************************************************/
void EnglishPanel::ClickedEnter(void)
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
 * 加载按键状态。
 *************************************************************************************************/
void EnglishPanel::_loadKeyStatus()
{
    if (!_regExpStr.isEmpty())
    {
        QRegExp regExp(_regExpStr);
        int pos = 0;
        QString str;
        int size = 0;

        if (_shift)
        {
            str.clear();
            size = sizeof(_keyShiftSymbol) / sizeof(_keyShiftSymbol[0]);
            if (_btnEnable)
            {
                for (int i = 0; i < size; ++i)
                {
                    _keys[i]->setEnabled(false);
                    str += _keyShiftSymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    _keys[pos]->setEnabled(true);
                    pos += regExp.matchedLength();
                }
            }
            else
            {
                for (int i = 0; i < size; ++i)
                {
                    _keys[i]->setEnabled(true);
                    str += _keyShiftSymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    _keys[pos]->setEnabled(false);
                    pos += regExp.matchedLength();
                }
            }
        }
        else
        {
            str.clear();
            size = sizeof(_keySymbol) / sizeof(_keySymbol[0]);
            if (_btnEnable)
            {
                for (int i = 0; i < size; ++i)
                {
                    _keys[i]->setEnabled(false);
                    str += _keySymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    _keys[pos]->setEnabled(true);
                    pos += regExp.matchedLength();
                }
            }
            else
            {
                for (int i = 0; i < size; ++i)
                {
                    _keys[i]->setEnabled(true);
                    str += _keySymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    _keys[pos]->setEnabled(false);
                    pos += regExp.matchedLength();
                }
            }
        }
    }
}

/**************************************************************************************************
 * show事件。
 *************************************************************************************************/
void EnglishPanel::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);
    _loadKeyStatus();
    _enter->setFocus();
}

/**************************************************************************************************
 * 功能：设置初始字符串。
 * 参数：
 *      text：文本信息。
 *************************************************************************************************/
void EnglishPanel::setInitString(const QString &text)
{
    _textDisplay->setText(text);
}

/**************************************************************************************************
 * 设置最大输入长度。
 *************************************************************************************************/
void EnglishPanel::setMaxInputLength(int len)
{
    _maxLength = len;
}

/**************************************************************************************************
 * 获取当前值。
 *************************************************************************************************/
QString EnglishPanel::getStrValue(void) const
{
    return _textDisplay->text();
}

/**************************************************************************************************
 * 设置显示模式
 *************************************************************************************************/
void EnglishPanel::setEchoMode(QLineEdit::EchoMode mode)
{
    _textDisplay->setEchoMode(mode);
}

/**************************************************************************************************
 * 按_keySymbol、_keyShiftSymbol数组排序
 *************************************************************************************************/
//void EnglishPanel::_keySort(const QString key)
//{

//    _disablekey.clear();
//    _disableShiftkey.clear();
//    _size = 0;
//    _shiftsize = 0;

//    QStringList _keyList;
//    for (int i = 0; i < key.length(); i++)
//    {
//        _keyList.append(key.at(i));
//    }

//    for (int i = 0; i < _keys.size(); i++)
//    {
//        for (int k = 0; k < _keyList.size(); k++)
//        {
//            if ((_keyList.at(k)) == _keySymbol[i])
//            {
//                _disablekey.append(_keySymbol[i]);
//                _keyList.removeAt(k);
//                break;
//            }

//        }
//        if (_keyList.size() == 0)
//        {
//            _size = _disablekey.size();
//            _shiftsize = _disableShiftkey.size();
//            return;
//        }
//    }
//    for (int i = 0; i < _keys.size(); i++)
//    {
//        for (int k = 0; k < _keyList.size(); k++)
//        {
//            if ((_keyList.at(k)) == _keyShiftSymbol[i])
//            {
//                _disableShiftkey.append(_keyShiftSymbol[i]);
//                _keyList.removeAt(k);
//                break;
//            }
//        }
//        if (_keyList.size() == 0)
//        {
//            _size = _disablekey.size();
//            _shiftsize = _disableShiftkey.size();
//            return;
//        }
//    }
//    _size = _disablekey.size();
//    _shiftsize = _disableShiftkey.size();
//}

/**************************************************************************************************
 * 设置显示按钮
 * alphabet为大小写禁用；
 * alphabet：0为大小写可用；
 *           1为大写禁用；
 *           2为小写禁用；
 *           3为大小写禁用；
 * nmuber为数字禁用；
 * nmuber：0为数字可用；
 *         1为数字禁用；
 * symbol为符号禁用；
 *
 * QString _key = "`-=[]\\;',./ ";
 * englishPanel.setBtnDisable(2,1,false,_key);
 *************************************************************************************************/
//void EnglishPanel::setBtnDisable(int alphabet, int nmuber, bool Space, const QString symbol)
//{
//    QString str = NULL;
//    if (alphabet == 1)
//    {
//        _shift = false;
//        str += "QWERTYUIOPASDFGHJKLZXCVBNM";
//    }
//    else if (alphabet == 2)
//    {
//        _shift = true;
//        str += "qwertyuiopasdfghjklzxcvbnm";
//    }
//    else if (alphabet == 3)
//    {
//        str += "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
//    }

//    if (nmuber == 1)
//    {
//        if (alphabet == 2 || alphabet == 3)
//        {
//            _shift = true;
//        }
//        str += "1234567890";
//    }
//    str += symbol;
//    _keySort(str);

//    _space->setEnabled(Space);

//    int n = 0;
//    if (_shift)
//    {
//        for (int i = 0; i < _keys.size(); i++)
//        {
//            _keys[i]->setText(_keyShiftSymbol[i]);
//            _keys[i]->setEnabled(true);
//            if (_shiftsize == n)
//            {
//                continue;
//            }
//            if ((_disableShiftkey.at(n)) == _keyShiftSymbol[i])
//            {
//                _keys[i]->setDisabled(true);
//                n++;
//            }
//        }
//    }
//    else
//    {
//        for (int i = 0; i < _keys.size(); i++)
//        {
//            _keys[i]->setText(_keySymbol[i]);
//            _keys[i]->setEnabled(true);
//            if (_size == n)
//            {
//                continue;
//            }
//            if (_disablekey.at(n) == _keySymbol[i])
//            {
//                _keys[i]->setDisabled(true);
//                n++;

//            }
//        }
//    }
//}

/**************************************************************************************************
 * 设置显示按钮，匹配正则字符串
 *************************************************************************************************/
void EnglishPanel::setBtnEnable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    _btnEnable = true;
    _regExpStr = regStr;
}

void EnglishPanel::setBtnDisable(const QString &regStr)
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
void EnglishPanel::setSpaceEnable(bool enable)
{
    _space->setEnabled(enable);
}

/**************************************************************************************************
 * 设置检查函数
 *************************************************************************************************/
void EnglishPanel::setCheckValueHook(CheckValue hook)
{
    _checkValue = hook;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EnglishPanel::EnglishPanel() : PopupWidget()
{
    _shift = false;
    _maxLength = 90;
    _checkValue = NULL;
    _isInvalid = false;
    _regExpStr.isNull();
//    _disablekey.clear();
//    _disableShiftkey.clear();
//    _size = 0;
//    _shiftsize = 0;

    QFont keyFont = fontManager.textFont(15*3/2);

    // 设置标题。
    setTitleBarText(trs("EnglishKeyboard"));

    int space = 5;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(space);

    // 显示框。
    _textDisplay = new QLineEdit();
    _textDisplay->setFont(keyFont);
    _textDisplay->setFixedHeight(_itemHeight);
    _textDisplay->setFocusPolicy(Qt::NoFocus);
    _textDisplay->setAlignment(Qt::AlignLeft);
    _textDisplay->setEchoMode(QLineEdit::Normal);
    mainLayout->addWidget(_textDisplay);

    // 键盘区。
    QColor color(120, 120, 120);
    QHBoxLayout *hLayout = NULL;
    IButton *key = NULL;
    int index = 0;
    for (int r = 0; r < 4; r++)
    {
        hLayout = new QHBoxLayout();
        hLayout->setSpacing(space);

        int cn = 0;
        if (r == 0)
        {
            cn = 13;
            hLayout->setContentsMargins(space, 0, space, 0);
        }
        else if (r == 1)       // 第一、二行，10个按钮。
        {
            cn = 13;
            hLayout->setContentsMargins(space, 0, space, 0);
        }
        else if(r == 2)  // 第三行，9个按钮。
        {
            cn = 11;
            hLayout->setContentsMargins(space + (_itemWidth + space) * 2 / 2, 0,
                                        2 + (_itemWidth + space) * 2 / 2, 0);
        }
        else             // 第四行，7个按钮。
        {
            cn = 10;
            hLayout->setContentsMargins(space + (_itemWidth + space) * 3 / 2, 0,
                                        2 + (_itemWidth + space) * 3 / 2, 0);
        }

        for (int c = 0; c < cn; c++)
        {
            key = new IButton();
            key->setFixedSize(_itemWidth, _itemHeight);
            key->setText(_keySymbol[index++]);
            key->setFont(keyFont);
            key->setBorderEnabled(true);
            key->setBorderColor(color);
            connect(key, SIGNAL(clicked(const QString &)), this, SLOT(ClickedKey(const QString &)));
            hLayout->addWidget(key);
            _keys.append(key);
        }
        mainLayout->addLayout(hLayout);
    }

    // 控制区。
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(space, 0, space, 0);

    // Shift键。
    key = new IButton();
    key->setBorderEnabled(true);
    key->setBorderColor(color);
    key->setText("Shift");
    key->setFixedHeight(_itemHeight);
    key->setFont(keyFont);
    connect(key, SIGNAL(clicked(QString)), this, SLOT(ClickedShift()));
    hLayout->addWidget(key, 1);

    // 清除所有。
    key = new IButton();
    key->setBorderEnabled(true);
    key->setBorderColor(color);
    key->setText("Clear All");
    key->setFixedHeight(_itemHeight);
    key->setFont(keyFont);
    connect(key, SIGNAL(realReleased()), this, SLOT(ClickedClear()));
    hLayout->addWidget(key, 1);

    // 空格键。
    _space = new IButton();
    _space->setBorderEnabled(true);
    _space->setBorderColor(color);
    _space->setText(" ");
    _space->setFixedHeight(_itemHeight);
    _space->setFont(keyFont);
    connect(_space, SIGNAL(clicked(const QString &)), this, SLOT(ClickedKey(const QString &)));
    hLayout->addWidget(_space, 3);

    // Backspace键。
    key = new IButton();
    key->setBorderEnabled(true);
    key->setBorderColor(color);
    key->setText("Backspace");
    key->setFixedHeight(_itemHeight);
    key->setFont(keyFont);
    connect(key, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
    hLayout->addWidget(key, 1);

    // Enter键。
    _enter = new IButton();
    _enter->setBorderEnabled(true);
    _enter->setBorderColor(color);
    _enter->setText("Enter");
    _enter->setFixedHeight(_itemHeight);
    _enter->setFont(keyFont);
    connect(_enter, SIGNAL(realReleased()), this, SLOT(ClickedEnter()));
    hLayout->addWidget(_enter, 1);

    mainLayout->addLayout(hLayout);
    contentLayout->addLayout(mainLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
EnglishPanel::~EnglishPanel()
{

}
