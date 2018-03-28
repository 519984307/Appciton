#include "CombolistWidget.h"
#include <QKeyEvent>
#include "ColorManager.h"
#include "ComboList.h"
#include "IDropList.h"

/**************************************************************************************************
 * 按键转码。
 *************************************************************************************************/
QModelIndex CombolistWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    // 飞梭只有左右按键，这里进行转码。
    cursorAction = (cursorAction == MoveLeft) ? MoveUp : cursorAction;
    cursorAction = (cursorAction == MoveRight) ? MoveDown : cursorAction;
    return QListWidget::moveCursor(cursorAction, modifiers);
}

/**************************************************************************************************
 * 按键事件处理。
 *************************************************************************************************/
void CombolistWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            if (currentRow() == 0)
            {
                setCurrentRow(0);
                return;
            }
            break;

        case Qt::Key_Right:
        case Qt::Key_Down:
            if (currentRow() == count() - 1)
            {
                setCurrentRow(count() - 1);
                return;
            }
            break;

        default:
            break;
    }

    QListWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CombolistWidget::CombolistWidget() : QListWidget()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CombolistWidget::~CombolistWidget()
{
}
