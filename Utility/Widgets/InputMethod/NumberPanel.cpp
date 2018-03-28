#include <QGridLayout>
#include <QLineEdit>
#include <QBitmap>
#include <QPainter>
#include "FontManager.h"
#include "LanguageManager.h"
#include "NumberPanel.h"
#include "IButton.h"

static const char *KEY_SYMBOL[] =
{
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", "0", "."
};


/**************************************************************************************************
 * 一般按键点击。
 *************************************************************************************************/
void NumberPad::ClickedKey(const QString &key)
{
    if (_text.size() >= _maxLength)
    {
        return;
    }
    _text.append(key);
    _textDisplay->setText(_text);
}

/**************************************************************************************************
 * Backspace点击。
 *************************************************************************************************/
void NumberPad::ClickedBackspace(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    _text.remove(_text.size() - 1, 1);
    _textDisplay->setText(_text);
}

/**************************************************************************************************
 * Clear点击。
 *************************************************************************************************/
void NumberPad::ClickedClear(void)
{
    _text.clear();
    _textDisplay->setText(_text);
}

/**************************************************************************************************
 * Enter点击。
 *************************************************************************************************/
void NumberPad::ClickedEnter(void)
{
//    hide();
    done(1);
}

/**************************************************************************************************
 * Cancel点击。
 *************************************************************************************************/
void NumberPad::ClickedCancel(void)
{
    hide();
//    done(0);
}

/**************************************************************************************************
 * 退出。
 *************************************************************************************************/
void NumberPad::exit(void)
{
    ClickedCancel();
}

/**************************************************************************************************
 * show事件。
 *************************************************************************************************/
void NumberPad::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);
    _enter->setFocus();
}

/**************************************************************************************************
 * 功能：设置初始字符串。
 * 参数：
 *      text：文本信息。
 *************************************************************************************************/
void NumberPad::setInitString(const QString &text)
{
    _text = text;
    _textDisplay->setText(_text);
}

/**************************************************************************************************
 * 得到文本型数据。
 *************************************************************************************************/
const QString &NumberPad::getStrValue(void)
{
    return _text;
}

/**************************************************************************************************
 * 得到浮点型数据。
 *************************************************************************************************/
bool NumberPad::getFloatValue(float &v)
{
    bool isOk = false;
    v = _text.toFloat(&isOk);
    return isOk;
}

/**************************************************************************************************
 * 得到浮点型数据。
 *************************************************************************************************/
bool NumberPad::getDoubleValue(double &v)
{
    bool isOk = false;
    v = _text.toDouble(&isOk);
    return isOk;
}

/**************************************************************************************************
 * 得到整型数据。
 *************************************************************************************************/
bool NumberPad::getIntValue(int &v)
{
    bool isOk = false;
    v = _text.toInt(&isOk);
    return isOk;
}

/**************************************************************************************************
 * 最大输入长度。
 *************************************************************************************************/
void NumberPad::setMaxInputLength(int len)
{
    _maxLength = len;
}

/**************************************************************************************************
 * 最小输入长度。
 *************************************************************************************************/
void NumberPad::setMinInputLength(int len)
{
    _minLength = len;
}

/**************************************************************************************************
 * 功能: 禁用数字弹出框上的按键
 * 参数:
 *    str:按键字符
 *************************************************************************************************/
void NumberPad::disableNumKey(QString str)
{
    IButton *btn = NULL;
    int i = 0;
    int size = _keys.size();

    while (i != size)
    {
        btn = _keys.at(i);
        if (btn->text() == str)
        {
            btn->setEnabled(false);
            break;
        }

        ++i;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NumberPad::NumberPad() : PopupWidget()
{
    _maxLength = 20;
    _minLength = 0;
    setTitleBarText(trs("NumberKeyboard"));

    QGridLayout *grid = new QGridLayout();
    grid->setMargin(0);
    grid->setSpacing(3);
    grid->setVerticalSpacing(1);
    grid->setHorizontalSpacing(5);

    int i = 0;
    int fontSize = 15;
    QColor color(120, 120, 120);
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            IButton *key = new IButton();
            key->setFixedSize(_itemWidth, _itemHeight);
            key->setText(KEY_SYMBOL[i++]);
            key->setFont(fontManager.textFont(fontSize));
            key->setBorderEnabled(true);
            key->setBorderColor(color);
            connect(key, SIGNAL(clicked(QString)), this, SLOT(ClickedKey(QString)));
            grid->addWidget(key, r, c);
            _keys.append(key);
        }
    }

    // Backspace
    IButton *back = new IButton();
    back->setFixedHeight(_itemHeight);
    back->setText("Backspace");
    back->setFont(fontManager.textFont(fontSize));
    back->setBorderEnabled(true);
    back->setBorderColor(color);
    connect(back, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
    grid->addWidget(back, 0, 4);

    // Clear
    IButton *clear = new IButton();
    clear->setFixedHeight(_itemHeight);
    clear->setText("Clear");
    clear->setFont(fontManager.textFont(fontSize));
    clear->setBorderEnabled(true);
    clear->setBorderColor(color);
    connect(clear, SIGNAL(clicked()), this, SLOT(ClickedClear()));
    grid->addWidget(clear, 1, 4);

    // Return
    IButton *ret = new IButton();
    ret->setFixedHeight(_itemHeight);
    ret->setText("Cancel");
    ret->setFont(fontManager.textFont(fontSize));
    ret->setBorderEnabled(true);
    ret->setBorderColor(color);
    connect(ret, SIGNAL(realReleased()), this, SLOT(ClickedCancel()));
    grid->addWidget(ret, 2, 4);

    // Enter
    _enter = new IButton();
    _enter->setFixedHeight(_itemHeight);
    _enter->setText("Enter");
    _enter->setFont(fontManager.textFont(fontSize));
    _enter->setBorderEnabled(true);
    _enter->setBorderColor(color);
    connect(_enter, SIGNAL(realReleased()), this, SLOT(ClickedEnter()));
    grid->addWidget(_enter, 3, 4);

    // 显示框。
    _textDisplay = new QLineEdit();
    _textDisplay->setFixedHeight(_itemHeight);
    _textDisplay->setReadOnly(true);
    _textDisplay->setFocusPolicy(Qt::NoFocus);
    _textDisplay->setFont(fontManager.textFont(15));

    // 布局。
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setMargin(2);
    vLayout->setSpacing(15);
    vLayout->addStretch();
    vLayout->addWidget(_textDisplay);
    vLayout->addLayout(grid);

    contentLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NumberPad::~NumberPad()
{

}
