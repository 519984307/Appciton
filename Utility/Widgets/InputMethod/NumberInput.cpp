#include "NumberInput.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QBitmap>
#include <QPainter>
#include <QTime>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IButton.h"

static const char *KEY_SYMBOL[] =
{
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", "0", "."
};

void NumberInput::setInitString(const QString &text)
{
    _textDisplay->setPlaceholderText(text);
}

void NumberInput::setMaxInputLength(int len)
{
    _maxLength = len;
}

void NumberInput::setMinInputLength(int len)
{
    _minLength = len;
}

void NumberInput::setUnitDisplay(QString text)
{
    _unit->setText(text);
}

void NumberInput::disableNumKey(const QString &str)
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

const QString &NumberInput::getStrValue()
{
    return _text;
}

bool NumberInput::getFloatValue(float &v)
{
    bool isOk = false;
    v = _text.toFloat(&isOk);
    return isOk;
}

bool NumberInput::getDoubleValue(double &v)
{
    bool isOk = false;
    v = _text.toDouble(&isOk);
    return isOk;
}

bool NumberInput::getIntValue(int &v)
{
    bool isOk = false;
    v = _text.toInt(&isOk);
    return isOk;
}

NumberInput::NumberInput()
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
    int fontSize = fontManager.getFontSize(1);
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
    back->setFixedSize(_itemWidth, _itemHeight * 2);
    back->setFont(fontManager.textFont(fontSize));
    back->setBorderEnabled(true);
    back->setBorderColor(color);
    back->setPicture(QImage("/usr/local/nPM/icons/backspace.png"));
    connect(back, SIGNAL(clicked()), this, SLOT(ClickedBackspace()));
    grid->addWidget(back, 0, 4, 2, 1);

    // Enter
    _enter = new IButton();
    _enter->setFixedSize(_itemWidth, _itemHeight * 2);
    _enter->setFont(fontManager.textFont(fontSize));
    _enter->setBorderEnabled(true);
    _enter->setBorderColor(color);
    _enter->setPicture(QImage("/usr/local/nPM/icons/enter.png"));
    connect(_enter, SIGNAL(realReleased()), this, SLOT(ClickedEnter()));
    grid->addWidget(_enter, 2, 4, 2, 1);

    // 显示框。
    _textDisplay = new QLineEdit();
    _textDisplay->setFixedSize(_itemWidth * 3, _itemHeight);
    _textDisplay->setReadOnly(true);
    _textDisplay->setFocusPolicy(Qt::NoFocus);
    _textDisplay->setFont(fontManager.textFont(fontSize));

   _unit = new QLabel();
   _unit->setFont(fontManager.textFont(fontSize));
   _unit->setFixedSize(_itemWidth * 2, _itemHeight);
//   _unit->setText("mcg/kg/min");

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_textDisplay);
    hlayout->addWidget(_unit);
    // 布局。
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setMargin(2);
    vLayout->setSpacing(15);
    vLayout->addStretch();
    vLayout->addLayout(hlayout);
    vLayout->addLayout(grid);

    contentLayout->addLayout(vLayout);
}

NumberInput::~NumberInput()
{

}

void NumberInput::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);
    _enter->setFocus();
}

void NumberInput::exit()
{
    hide();
}

void NumberInput::ClickedKey(const QString &key)
{
    if (_text.size() >= _maxLength)
    {
        return;
    }
    _text.append(key);
    _textDisplay->setText(_text);
}

void NumberInput::ClickedBackspace()
{
    if (_text.isEmpty())
    {
        return;
    }

    _text.remove(_text.size() - 1, 1);
    _textDisplay->setText(_text);
}

void NumberInput::ClickedEnter()
{
    done(1);
}
