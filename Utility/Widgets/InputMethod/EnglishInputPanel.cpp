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
#include "EnglishInputPanel.h"
#include "Button.h"
#include <QCoreApplication>
#include <QInputMethodEvent>
#include <QLineEdit>
#include "Button.h"
#include <QList>
#include "Debug.h"

#define PATH_ICON_SHIFT "/usr/local/nPM/icons/shift.png"
#define PATH_ICON_CLEAR "/usr/local/nPM/icons/clear.png"
#define PATH_ICON_BACKSPACE "/usr/local/nPM/icons/backspace.png"
#define PATH_ICON_ENTER "/usr/local/nPM/icons/enter.png"
#define ICON_SIZE 32

class EnglishInputPanelPrivate
{
public:
    EnglishInputPanelPrivate();

    static const int itemWidth = 34 * 3 / 2;
    static const int itemHeight = 34 * 3 / 2;

    int maxLength;
    bool shift;
    bool isInvalid;
    QList<Button *> keys;
    Button *space;
    Button *enter;
    QLineEdit *textDisplay;

    QString regExpStr;
    bool btnEnable;
    QString backSpaceStr;
};
EnglishInputPanelPrivate::EnglishInputPanelPrivate()
    : maxLength(0),
      shift(false),
      isInvalid(false),
      space(NULL),
      enter(NULL),
      textDisplay(NULL),
      regExpStr(""),
      btnEnable(false),
      backSpaceStr("")
{
    keys.clear();
}

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
void EnglishInputPanel::ClickedKey()
{
    Button *btn = qobject_cast<Button *>(sender());
    QString key("");
    if (!d_ptr->backSpaceStr.isEmpty())
    {
        key = d_ptr->backSpaceStr;
        d_ptr->backSpaceStr.clear();
    }
    else
    {
        key = btn->text();
    }

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
 * Shift点击。
 *************************************************************************************************/
void EnglishInputPanel::ClickedShift(void)
{
    d_ptr->shift = !d_ptr->shift;

    if (d_ptr->shift)
    {
        for (int i = 0; i < d_ptr->keys.size(); i++)
        {
            d_ptr->keys[i]->setText(_keyShiftSymbol[i]);
        }
    }
    else
    {
        for (int i = 0; i < d_ptr->keys.size(); i++)
        {
            d_ptr->keys[i]->setText(_keySymbol[i]);
        }
    }

    _loadKeyStatus();
}

/**************************************************************************************************
 * Backspace点击。
 *************************************************************************************************/
void EnglishInputPanel::ClickedBackspace(void)
{
    d_ptr->backSpaceStr = QString("\x7f");
    ClickedKey();  // ascii code of 'delete'
}

/**************************************************************************************************
 * Cancel点击。
 *************************************************************************************************/
void EnglishInputPanel::ClickedClear(void)
{
    d_ptr->textDisplay->clear();
}

/**************************************************************************************************
 * Enter点击。
 *************************************************************************************************/
void EnglishInputPanel::ClickedEnter(void)
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
            d_ptr->textDisplay->setText(trs("InvalidInput"));
            return;
        }
    }

    hide();
    done(1);
}

/**************************************************************************************************
 * 加载按键状态。
 *************************************************************************************************/
void EnglishInputPanel::_loadKeyStatus()
{
    if (!d_ptr->regExpStr.isEmpty())
    {
        QRegExp regExp(d_ptr->regExpStr);
        int pos = 0;
        QString str;
        int size = 0;

        if (d_ptr->shift)
        {
            str.clear();
            size = sizeof(_keyShiftSymbol) / sizeof(_keyShiftSymbol[0]);
            if (d_ptr->btnEnable)
            {
                for (int i = 0; i < size; ++i)
                {
                    d_ptr->keys[i]->setEnabled(false);
                    str += _keyShiftSymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    d_ptr->keys[pos]->setEnabled(true);
                    pos += regExp.matchedLength();
                }
            }
            else
            {
                for (int i = 0; i < size; ++i)
                {
                    d_ptr->keys[i]->setEnabled(true);
                    str += _keyShiftSymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    d_ptr->keys[pos]->setEnabled(false);
                    pos += regExp.matchedLength();
                }
            }
        }
        else
        {
            str.clear();
            size = sizeof(_keySymbol) / sizeof(_keySymbol[0]);
            if (d_ptr->btnEnable)
            {
                for (int i = 0; i < size; ++i)
                {
                    d_ptr->keys[i]->setEnabled(false);
                    str += _keySymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    d_ptr->keys[pos]->setEnabled(true);
                    pos += regExp.matchedLength();
                }
            }
            else
            {
                for (int i = 0; i < size; ++i)
                {
                    d_ptr->keys[i]->setEnabled(true);
                    str += _keySymbol[i];
                }

                while (-1 != (pos = regExp.indexIn(str, pos)))
                {
                    d_ptr->keys[pos]->setEnabled(false);
                    pos += regExp.matchedLength();
                }
            }
        }
    }
}

/**************************************************************************************************
 * show事件。
 *************************************************************************************************/
void EnglishInputPanel::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);
    _loadKeyStatus();
    d_ptr->enter->setFocus();
}

/**************************************************************************************************
 * 功能：设置初始字符串。
 * 参数：
 *      text：文本信息。
 *************************************************************************************************/
void EnglishInputPanel::setInitString(const QString &text)
{
    d_ptr->textDisplay->setText(text);
}

/**************************************************************************************************
 * 设置最大输入长度。
 *************************************************************************************************/
void EnglishInputPanel::setMaxInputLength(int len)
{
    d_ptr->maxLength = len;
}

/**************************************************************************************************
 * 获取当前值。
 *************************************************************************************************/
QString EnglishInputPanel::getStrValue(void) const
{
    return d_ptr->textDisplay->text();
}

/**************************************************************************************************
 * 设置显示模式
 *************************************************************************************************/
void EnglishInputPanel::setEchoMode(QLineEdit::EchoMode mode)
{
    d_ptr->textDisplay->setEchoMode(mode);
}


/**************************************************************************************************
 * 设置显示按钮，匹配正则字符串
 *************************************************************************************************/
void EnglishInputPanel::setBtnEnable(const QString &regStr)
{
    if (regStr.isEmpty())
    {
        return;
    }

    d_ptr->btnEnable = true;
    d_ptr->regExpStr = regStr;
}

void EnglishInputPanel::setBtnDisable(const QString &regStr)
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
void EnglishInputPanel::setSpaceEnable(bool enable)
{
    d_ptr->space->setEnabled(enable);
}

/**************************************************************************************************
 * 设置检查函数
 *************************************************************************************************/
void EnglishInputPanel::setCheckValueHook(CheckValue hook)
{
    _checkValue = hook;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EnglishInputPanel::EnglishInputPanel()
    : Dialog(),
      d_ptr(new EnglishInputPanelPrivate)
{
    d_ptr->shift = false;
    d_ptr->maxLength = 90;
    _checkValue = NULL;
    d_ptr->isInvalid = false;
    d_ptr->regExpStr.isNull();

    QFont keyFont = fontManager.textFont(15 * 3 / 2);

    // 设置标题。
    setWindowTitle(trs("EnglishKeyboard"));

    int space = 10;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(space);               // 设置布局器四周预留空间
    mainLayout->setSpacing(space);              // 设置布局器内部空间间隔

    // 显示框。
    d_ptr->textDisplay = new QLineEdit();
    d_ptr->textDisplay->setFont(keyFont);
    d_ptr->textDisplay->setFixedHeight(d_ptr->itemHeight);
    d_ptr->textDisplay->setFocusPolicy(Qt::NoFocus);
    d_ptr->textDisplay->setAlignment(Qt::AlignLeft);
    d_ptr->textDisplay->setEchoMode(QLineEdit::Normal);
    mainLayout->addWidget(d_ptr->textDisplay);

    // 键盘区。
    QHBoxLayout *hLayout = NULL;
    Button *key = NULL;
    int index = 0;
    for (int r = 0; r < 4; r++)
    {
        hLayout = new QHBoxLayout();

        int cn = 0;
        if (r < 2)        // 第一 第二行共13个按钮
        {
            cn = 13;
//            hLayout->setContentsMargins(space, 0, space, 0);
        }
        else if (r == 2)  // 第三行，9个按钮。
        {
            cn = 11;
            hLayout->setContentsMargins((d_ptr->itemWidth + space) * 2 / 2, 0,
                                        2 + (d_ptr->itemWidth + space) * 2 / 2, 0);
        }
        else              // 第四行，7个按钮。
        {
            cn = 10;
            hLayout->setContentsMargins((d_ptr->itemWidth + space) * 3 / 2, 0,
                                        2 + (d_ptr->itemWidth + space) * 3 / 2, 0);
        }

        for (int c = 0; c < cn; c++)
        {
            key = new Button();
            key->setButtonStyle(Button::ButtonTextOnly);
            key->setFixedSize(d_ptr->itemWidth, d_ptr->itemHeight);
            key->setText(_keySymbol[index++]);
            key->setFont(keyFont);
            connect(key, SIGNAL(clicked()), this, SLOT(ClickedKey()));
            hLayout->addWidget(key);
            d_ptr->keys.append(key);
        }
        mainLayout->addLayout(hLayout);
    }

    // 控制区。
    hLayout = new QHBoxLayout();

    // Shift键。
    key = new Button();
    key->setButtonStyle(Button::ButtonIconOnly);
    int height = d_ptr->itemHeight * 2 / 3;
    key->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    key->setIcon(QIcon(PATH_ICON_SHIFT));
    key->setFixedHeight(d_ptr->itemHeight);
    connect(key, SIGNAL(clicked()), this, SLOT(ClickedShift()));
    hLayout->addWidget(key, 1);

    // 清除所有。
    key = new Button();
    key->setButtonStyle(Button::ButtonIconOnly);
    key->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    key->setIcon(QIcon(PATH_ICON_CLEAR));
    key->setFixedHeight(d_ptr->itemHeight);
    connect(key, SIGNAL(released()), this, SLOT(ClickedClear()));
    hLayout->addWidget(key, 1);

    // 空格键。
    d_ptr->space = new Button();
    d_ptr->space->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->space->setText(" ");
    d_ptr->space->setFixedHeight(d_ptr->itemHeight);
    d_ptr->space->setFont(keyFont);
    connect(d_ptr->space, SIGNAL(clicked()), this, SLOT(ClickedKey()));
    hLayout->addWidget(d_ptr->space, 3);

    // Backspace键。
    key = new Button();
    key->setButtonStyle(Button::ButtonIconOnly);
    key->setIcon(QIcon(PATH_ICON_BACKSPACE));
    key->setIconSize(QSize(height, height));
    key->setFixedHeight(d_ptr->itemHeight);
    connect(key, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
    hLayout->addWidget(key, 1);

    // Enter键。
    d_ptr->enter = new Button();
    d_ptr->enter->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->enter->setIconSize(QSize(height, height));
    d_ptr->enter->setIcon(QIcon(PATH_ICON_ENTER));
    d_ptr->enter->setFixedHeight(d_ptr->itemHeight);
    connect(d_ptr->enter, SIGNAL(released()), this, SLOT(ClickedEnter()));
    hLayout->addWidget(d_ptr->enter, 1);

    mainLayout->addLayout(hLayout);
    setWindowLayout(mainLayout);

    // proper size to hold all the keys
    resize(773, 387);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
EnglishInputPanel::~EnglishInputPanel()
{
    delete d_ptr;
}
