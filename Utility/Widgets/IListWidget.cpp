#include "IListWidget.h"
#include <QKeyEvent>
#include "ColorManager.h"

/**************************************************************************************************
 * 按键转码。
 *************************************************************************************************/
QModelIndex IListWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    // 飞梭只有左右按键，这里进行转码。
    cursorAction = (cursorAction == MoveLeft) ? MoveUp : cursorAction;
    cursorAction = (cursorAction == MoveRight) ? MoveDown : cursorAction;
    return QListWidget::moveCursor(cursorAction, modifiers);
}

/**************************************************************************************************
 * 按键事件处理。
 *************************************************************************************************/
void IListWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            if (currentRow() == 0)
            {
//                setCurrentRow(count() - 1);
                emit exitList();
                emit exitList(true);
                return;
            }
            break;

        case Qt::Key_Right:
        case Qt::Key_Down:
            if (currentRow() == count() - 1)
            {
//                setCurrentRow(0);
                emit exitList();
                emit exitList(false);
                return;
            }
            break;

        default:
            break;
    }

    QListWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 按键事件处理。
 *************************************************************************************************/
void IListWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (!e->isAutoRepeat())
        {
            emit realRelease();
        }
        break;

    default:
        break;
    }

    QListWidget::keyReleaseEvent(e);
}

/**************************************************************************************************
 * 绘画事件处理。
 *************************************************************************************************/
void IListWidget::paintEvent(QPaintEvent *e)
{
    QPalette palette = this->palette();

    if (hasFocus())
    {
        palette.setColor(QPalette::Highlight, colorManager.getHighlight());
        palette.setColor(QPalette::HighlightedText, Qt::white);
    }
    else
    {
        palette.setColor(QPalette::Highlight, QColor(220, 220, 220));
        palette.setColor(QPalette::HighlightedText, Qt::black);
    }

    setPalette(palette);

    QListWidget::paintEvent(e);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IListWidget::IListWidget() : QListWidget()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IListWidget::~IListWidget()
{

}
