#pragma once
#include "QListWidget"

// 重新定义ListWidget便于处理飞梭操作。
class IListWidget : public QListWidget
{
    Q_OBJECT

public:
    IListWidget();
    virtual ~IListWidget();

signals:
    void realRelease();
    void exitList();
    void exitList(bool backTab);

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void paintEvent(QPaintEvent *e);
};
