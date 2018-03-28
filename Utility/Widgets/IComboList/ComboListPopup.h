#pragma once
#include <QWidget>
#include "IListWidget.h"
#include "CombolistWidget.h"

enum
{
    POPUP_TYPE_COMBOLIST,
    POPUP_TYPE_DROPLIST,
    POPUP_TYPE_USER,
    POPUP_TYPE_NR
};

class QScrollArea;
class ComboListItem;
class ComboListPopup: public QWidget
{
    Q_OBJECT

public:
    ComboListPopup(QWidget *parent, int type = POPUP_TYPE_COMBOLIST, int count = 0, int curIndex = 0);
    ~ComboListPopup();

    // 设置边框倒角
    void setBorderRadius(double radius);
    void setBorderColor(const QColor &color) {_boardColor = color;}
    void setBorderWidth(int width);

    // 设置子项是否画选中标记
    void setItemDrawMark(bool isDrawMark);

    // 设置子项名称
    void addItemText(const QString &txt);
    void updateItemText(int index, const QString &txt);
    
    int getCurIndex();

    static ComboListPopup *_current;


    static ComboListPopup *current() {return _current;}
    
    bool eventFilter(QObject *obj, QEvent *ev);

    void popupUp(bool popup)
    {
        _isPopup = popup;
    }
    bool isPopupUp() const
    {
        return _isPopup;
    }

    //call to update the update the popup position when the parent position is changed
    void updatePos();

protected:
    void keyPressEvent(QKeyEvent *e);
//    void wheelEvent(QWheelEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void paintEvent(QPaintEvent *e);

private slots:
    void _itemClicked(int index);

private:
    void _showIndex(int index);
    void _relocate();
    int _getCount();
    int _getCurIndex();
    QString _getItemTxt(int index);
    void _setCurIndex(int index);

    QWidget *_parent;                // 关联的组合框控件
    int _type;
    QList<ComboListItem*> _items;     // 菜单项
    ComboListItem *_comboListItem;

    CombolistWidget *_listwidget;

    int _index;                       // 保证显示的选项索引
    int _boardWidth;                  // 边框宽度
    double _radius;                   // 边框倒角
    QColor _boardColor;               // 边框颜色

    int _count;
    int _curIndex;
    QStringList _itemTxt;
    bool _isPopup;
    bool _isErrorClose;              // 异常退出
    int _borderheight;

    static const int _pageSize = 6;   // 弹出菜单显示的最大选项数量
};
