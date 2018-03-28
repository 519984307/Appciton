#include <QIcon>
#include <QHeaderView>
#include <QKeyEvent>
#include "ITableWidget.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "QCoreApplication"
#include "Debug.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ITableWidget::ITableWidget(bool autoCheckWhenEnterPress) : QTableWidget(), _index(-1), _autoCheckWhenEnterPress(autoCheckWhenEnterPress)
{
    int fontSize = fontManager.getFontSize(1);

    horizontalHeader()->setStretchLastSection(true);              // 使最后一列和右边界对齐。
    setEditTriggers(QAbstractItemView::NoEditTriggers);           // 不能编辑。
    setFrameShape(QFrame::NoFrame);                               // 设置边框。
    horizontalHeader()->setFont(fontManager.textFont(fontSize));  // 列首字体。
    verticalHeader()->setFont(fontManager.textFont(fontSize));    // 行首字体。
    setFont(fontManager.textFont(fontSize));                      // 文本字体。
    setSelectionBehavior(QAbstractItemView::SelectRows);          // 选择整行。
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);         // 不显示横向滚动条

    _pic = new QIcon("/usr/local/nPM/icons/select.png");
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ITableWidget::~ITableWidget()
{
    if (NULL != _pic)
    {
        delete _pic;
        _pic = NULL;
    }
}

/**************************************************************************************************
 * 设置选择标志。
 *************************************************************************************************/
void ITableWidget::setCheck(QTableWidgetItem *item)
{
    if (NULL == item)
    {
        return;
    }

    item->setIcon(*_pic);
}

/**************************************************************************************************
 * 清除选择标志。
 *************************************************************************************************/
void ITableWidget::clearCheck(QTableWidgetItem *item)
{
    if (NULL == item)
    {
        return;
    }

    item->setIcon(QIcon());
}

/***************************************************************************************************
 * isEmpty : check whether the table has not content(all the cell is empty)
 **************************************************************************************************/
bool ITableWidget::isEmpty() const
{
    //Note:Just check item(0,0), it might not very accurate.
    QTableWidgetItem * item = this->item(0,0);
    if(item)
    {
        return item->text().trimmed().isEmpty();
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void ITableWidget::keyPressEvent(QKeyEvent *event)
{
    QTableWidgetItem *item1 = NULL;
    QTableWidgetItem *item2 = NULL;
    int column = columnCount();

    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            if (0 < _index)
            {
                for (int i = 0; i < column; ++i)
                {
                    item1 = item(_index, i);
                    item2 = item(_index - 1, i);
                    if (NULL != item2)
                    {
                        item2->setBackgroundColor(colorManager.getHighlight());
                    }

                    if (NULL != item1)
                    {
                        item1->setBackgroundColor(Qt::white);
                    }
                }
                --_index;
            }
            else
            {
                event->ignore();
            }
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
        {
            int lastLowIndex = rowCount() - 1;
            while(lastLowIndex >= 0)
            {
                item1 = item(lastLowIndex, 0);
                if(item1 && !item1->text().trimmed().isEmpty())
                {
                    break;
                }
                lastLowIndex --;
            }

            for(int i = 0; i < column; i++)
            {
                item1 = item(_index, i);
                if(NULL != item1)
                {
                    item1->setBackgroundColor(Qt::white);
                }
            }

            ++_index;
            if(_index > lastLowIndex)
            {
                _index = -1;
                event->ignore();
            }
            else
            {
                for(int i = 0; i < column; i++)
                {
                    item1 = item(_index, i);
                    if(NULL != item1)
                    {
                        item1->setBackgroundColor(colorManager.getHighlight());
                    }
                }
            }
        }
            return;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            item1 = item(_index, column - 1);
            if (NULL != item1)
            {
                if(_autoCheckWhenEnterPress)
                {
                    if (item1->icon().isNull())
                    {
                        item1->setIcon(*_pic);
                    }
                    else
                    {
                        item1->setIcon(QIcon());
                    }
                }

                emit itemEnter(_index);
            }
            return;
        default:
            break;
    }

    QTableWidget::keyPressEvent(event);
}

/**************************************************************************************************
 * 聚焦事件。
 *************************************************************************************************/
void ITableWidget::focusInEvent(QFocusEvent *event)
{
    QTableWidgetItem *item = this->item(0,0);
    int column = columnCount();

    if(!item) //no item in the table, ignore focus in event
    {
        event->ignore();
        return;
    }

    if(event->reason() == Qt::ActiveWindowFocusReason)
    {
        //select last select item
        if(_index >= 0 && _index < rowCount())
        {
            for (int i = 0; i < column; ++i)
            {
                item = this->item(_index, i);
                if (NULL != item)
                {
                    item->setBackgroundColor(colorManager.getHighlight());
                }
            }
        }
        return;
    }

    switch (event->type())
    {
        case QEvent::FocusIn:
        if(event->reason() == Qt::TabFocusReason)
        {
            if(item->text().trimmed().isEmpty())
            {
                //no connent, ignore focus in event
                _index = -1;
            }
            else
            {
                _index = 0;
            }
        }
        else   //backTab focus
        {
            //find a non-empty row
            _index = rowCount() - 1;
            while(_index >= 0)
            {
                item  = this->item(_index, 0);
                if(item && !item->text().trimmed().isEmpty())
                {
                    break;
                }
                else
                {
                    _index --;
                }
            }
        }

        if(_index < 0)
        {
            event->ignore();
            QKeyEvent *keyevent;
            if(event->reason() == Qt::TabFocusReason)
            {
                keyevent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
            }
            else
            {
                keyevent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
            }
            qApp->postEvent(this, keyevent);
        }
        else
        {
            for (int i = 0; i < column; ++i)
            {
                item = this->item(_index, i);
                if (NULL != item)
                {
                    item->setBackgroundColor(colorManager.getHighlight());
                }
            }
        }
            return;
        default:
            break;
    }

    QTableWidget::focusInEvent(event);
}

/**************************************************************************************************
 * 聚焦事件。
 *************************************************************************************************/
void ITableWidget::focusOutEvent(QFocusEvent *event)
{
    QTableWidgetItem *item = NULL;
    int column = columnCount();

    switch (event->type())
    {
        case QEvent::FocusOut:
            for (int i = 0; i < column; ++i)
            {
                item = this->item(_index, i);
                if (NULL != item)
                {
                    item->setBackgroundColor(Qt::white);
                }
            }
            return;
        default:
            break;
    }

    QTableWidget::focusInEvent(event);
}
