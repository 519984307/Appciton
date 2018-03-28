#include "PromptInfoBarWidget.h"
#include <QHBoxLayout>
#include <QPainter>
#include <LanguageManager.h>
#include "FontManager.h"

static PromptInfoBarWidget *_selfObj = NULL;

/**************************************************************************************************
 * 获取对象。
 *************************************************************************************************/
PromptInfoBarWidget &PromptInfoBarWidget::getSelf()
{
    return *_selfObj;
}

/**************************************************************************************************
 * 设置报警提示信息。
 *************************************************************************************************/
void PromptInfoBarWidget::display(QString &info)
{
    _stringList.append(info);
    _timer->start();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PromptInfoBarWidget::PromptInfoBarWidget(const QString &name) :
    IWidget(name)
{
//    setFocusPolicy(Qt::NoFocus);
    _timer = new QTimer(this);
    _timer->setInterval(1500);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PromptInfoBarWidget::~PromptInfoBarWidget()
{

}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void PromptInfoBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    _drawText();
}

/**************************************************************************************************
 * 超时槽函数。
 *************************************************************************************************/
void PromptInfoBarWidget::onTimeout()
{
    if(_stringList.isEmpty())
    {
        _text.clear();
        _timer->stop();
        update();
        return;
    }

    QString t = _stringList.takeFirst();
    _text = t;
    update();
}

/**************************************************************************************************
 * 绘制文本。
 *************************************************************************************************/
void PromptInfoBarWidget::_drawText()
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::lightGray);

    QRect r = this->rect();
    r.adjust(focusedBorderWidth() + 6, 0, 0, 0);

    QRect SizeFont;
    SizeFont.setHeight(height() * 0.66);
    int fontSize = fontManager.adjustTextFontSize(SizeFont);
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, _text);

}
