#pragma once
#include "QListWidget"

//enum
//{
//    POPUP_TYPE_COMBOLIST,
//    POPUP_TYPE_DROPLIST,
//    POPUP_TYPE_USER,
//    POPUP_TYPE_NR
//};

// 重新定义ListWidget便于处理飞梭操作。
class CombolistWidget : public QListWidget
{
    Q_OBJECT

public:
    CombolistWidget();
    virtual ~CombolistWidget();

    // 设置子项名称
//    void addItemText(const QString &txt);

//    int getCurIndex();

signals:
    void realRelease();
    void exitList();
    void exitList(bool backTab);

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual void keyPressEvent(QKeyEvent *e);
//    virtual void showEvent(QShowEvent *e);
//    virtual void keyReleaseEvent(QKeyEvent *e);
//    virtual void paintEvent(QPaintEvent *e);

private slots:
//    void _itemClicked(int index);

private:
    void _showIndex(int index, bool isFocus = false);
    int _getCount();
    int _getCurIndex();
    QString _getItemTxt(int index);
    void _setCurIndex(int index);
    QWidget *_parent;                // 关联的组合框控件
    int _type;
    int _curIndex;
    int _count;
    int _index;                       // 保证显示的选项索引
    static const int _pageSize = 5;   // 弹出菜单显示的最大选项数量
};
