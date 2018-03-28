#include <QFocusEvent>
#include <QScrollBar>
#include <QKeyEvent>
#include <QMouseEvent>
#include "TableWidget.h"
#include "Debug.h"
#include <QApplication>

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
    , _isMoveStart(false)
    , _isMoved(false)
    , _pressedRow(0)
{
}

/***************************************************************************************************
 * 失去焦点事件处理
 **************************************************************************************************/
void TableWidget::focusOutEvent(QFocusEvent *)
{
    // 清除选中
    this->clearSelection();
}

void TableWidget::focusInEvent(QFocusEvent *e)
{
    if(this->selectionBehavior() == QAbstractItemView::SelectRows && this->selectionMode() == QAbstractItemView::SingleSelection )
    {
        if(this->rowCount() == 0)
        {
            QKeyEvent *keyevent = NULL;
            if(e->reason() == Qt::TabFocusReason)
            {
                keyevent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
            }
            else if(e->reason() == Qt::BacktabFocusReason){
                keyevent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
            }
            QApplication::postEvent(this, keyevent);
        }
        else {
            if(e->reason() == Qt::TabFocusReason)
            {
                this->selectRow(0);
            }
            else if(e->reason() == Qt::BacktabFocusReason){
                this->selectRow(this->rowCount() - 1);
            }
        }
    }
}

/***************************************************************************************************
 * 键盘和飞梭点击事件，触发cellClicked
 **************************************************************************************************/
void TableWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            if(this->selectionBehavior() == QAbstractItemView::SelectRows && this->selectionMode() == QAbstractItemView::SingleSelection )
            {
                if(this->currentRow() <= 0) {
                    e->ignore(); //let parent widget handle the event
                    return;
                }
                this->selectRow(this->currentRow() - 1);
            }
            else {
                focusPreviousChild();
            }
            e->accept();
            return;
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            if(this->selectionBehavior() == QAbstractItemView::SelectRows && this->selectionMode() == QAbstractItemView::SingleSelection )
            {
                if(this->currentRow() >= this->rowCount() - 1) {
                    e->ignore(); //let parent widget handle the event
                    return;
                }
                this->selectRow(this->currentRow() + 1);
            }
            else {
                focusNextChild();
            }
            e->accept();
            return;
            break;
        default:
            break;
    }

    QTableWidget::keyPressEvent(e);
}

void TableWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            emit cellClicked(currentRow(), currentColumn());
            break;
        default:
            break;
    }
    QTableWidget::keyReleaseEvent(e);
}

/***************************************************************************************************
 * 触摸屏或鼠标点击事件。
 **************************************************************************************************/
void TableWidget::mousePressEvent(QMouseEvent *e)
{
    _isMoved = false;
    if(!_isMoveStart && Qt::LeftButton == e->button())
    {
        _isMoveStart = true;
        _lastMousePos = e->globalPos();
        // itemAt()中接受的坐标是TreeWidget坐标系的,
        // 因此需要将全局的坐标映射到TreeWidget上才可以
        QPoint pos = mapFromGlobal(_lastMousePos);
        // 因为有表头占了一行，造成每次取到的Item总是会向下错位一行，因此行数需要减1
        _pressedRow = row(itemAt(pos)) - 1;
    }
}

/***************************************************************************************************
 * 触摸屏或鼠标点击释放事件
 **************************************************************************************************/
void TableWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        _isMoveStart = false;
        // 鼠标发生移动，是拖动动作，直接返回，忽略掉事件消息
        if(_isMoved)
        {
            return;
        }
        // 如果鼠标没有移动，是正常的点击操作，设置当前Item为鼠标按下时的Item
        if(_pressedRow >= 0 && _pressedRow < rowCount())
        {
            selectRow(_pressedRow);
            _pressedRow = 0;
        }

        emit cellClicked(currentRow(), currentColumn());
    }
}

/***************************************************************************************************
 * 鼠标移动事件
 * 默认情况下，mouseMoveEvent响应你按下鼠标的某个键的鼠标移动（拖动，但不局限于左键拖动）
 **************************************************************************************************/
void TableWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint pos = e->globalPos();

    if(_isMoveStart)
    {
        int offset = pos.y() - _lastMousePos.y();

        if(!_isMoved)
        {
            _isMoved = (abs(offset) >= 5);
        }

        QScrollBar *scrollBar = verticalScrollBar();
        if(scrollBar)
        {
            scrollBar->setValue(scrollBar->value() - offset);
        }

        _lastMousePos = pos;
    }
}
