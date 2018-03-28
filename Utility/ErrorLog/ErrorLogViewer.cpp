#include "ErrorLogViewer.h"
#include <QTextStream>
#include "FontManager.h"
#include <QKeyEvent>
#include <QScrollBar>
#include <QApplication>
#include <QFontMetrics>

#define DEFAULT_WIDGET_WIDTH 500
#define DEFAULT_WIDGET_HEIGHT 310
ErrorLogViewer::ErrorLogViewer()
    :PopupWidget(), _item(NULL)
{
    QFont font = fontManager.textFont(14);
    _textEdit = new QTextEdit;
    this->setFixedSize(DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    this->contentLayout->addWidget(_textEdit);
    _textEdit->setReadOnly(true);
    _textEdit->setFont(font);
    _textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    _textEdit->setTabChangesFocus(true);
    _textEdit->installEventFilter(this);
}

ErrorLogViewer::ErrorLogViewer(ErrorLogItemBase * item)
    :PopupWidget(), _item(item)
{
    QFont font = fontManager.textFont(14);
    _textEdit = new QTextEdit;
    this->setFixedSize(DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    this->setTitleBarText(item->name());
//    QRect r = geometry();
//    int x = r.x() + ((serviceWindowManager.getSubmenuWidth() - DEFAULT_WIDGET_WIDTH) / 2);
//    int y = r.y() + ((serviceWindowManager.getSubmenuHeight() - DEFAULT_WIDGET_HEIGHT) / 2) - 30;
//    this->autoShow(x, y);
    this->contentLayout->addWidget(_textEdit);
    _textEdit->setReadOnly(true);
    _textEdit->setFont(font);
    _textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    _textEdit->setTabChangesFocus(true);
    QString str;
    QTextStream stream(&str);
    item->outputInfo(stream);
    _textEdit->setText(str);
    _textEdit->installEventFilter(this);
}

void ErrorLogViewer::setText(const QString &text)
{
    _textEdit->setText(text);
}

bool ErrorLogViewer::eventFilter(QObject *obj, QEvent *ev)
{

    if(_textEdit == qobject_cast<QObject*>(obj))
    {
        if(ev->type() == QEvent::KeyRelease)
        {
            QKeyEvent *keyEv = static_cast<QKeyEvent *>(ev);
            QScrollBar *scrollBar = _textEdit->verticalScrollBar();

            switch(keyEv->key())
            {
                case Qt::Key_Left:
                case Qt::Key_Up:
                {
                    if (scrollBar->isVisible() && scrollBar->value() != scrollBar->minimum())
                    {
                        scrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
                    }
                    else
                    {
                        focusPreviousChild();
                    }
                }
                break;
                case Qt::Key_Right:
                case Qt::Key_Down:
                {
                    if(scrollBar->isVisible() && scrollBar->value() != scrollBar->maximum())
                    {
                        scrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                    }
                    else
                    {
                        focusNextChild();
                    }
                }
                break;
            }
        }
    }
    return false;
}
