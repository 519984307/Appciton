#include "IMoveButton.h"
#include <QKeyEvent>

IMoveButton::IMoveButton(const QString &text, QWidget *parent) : IButton(text, parent),
    _status(BUTTON_FOCUS_IN)
{

}

IMoveButton::~IMoveButton()
{

}

void IMoveButton::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        if (_status == BUTTON_FOCUS_IN)
        {
            focusPreviousChild();
            return;
        }
        else if (_status == BUTTON_FOCUS_OUT)
        {
            emit leftMove();
            return;
        }
        break;

    case Qt::Key_Down:
    case Qt::Key_Right:
        if (_status == BUTTON_FOCUS_IN)
        {
            focusNextChild();
            return;
        }
        else if (_status == BUTTON_FOCUS_OUT)
        {
            emit rightMove();
            return;
        }
        break;
    default:
        break;
    }
}

void IMoveButton::keyReleaseEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if (_status == BUTTON_FOCUS_IN)
        {
            _status = BUTTON_FOCUS_OUT;
        }
        else if (_status == BUTTON_FOCUS_OUT)
        {
            _status = BUTTON_FOCUS_IN;
        }
    }
}
