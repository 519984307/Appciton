#pragma once
#include <QAbstractButton>
#include "ComboListPopup.h"

/**************************************************************************************************
 * 组合框弹出菜单的菜单项
**************************************************************************************************/
class ComboListItem: public QAbstractButton
{
    Q_OBJECT

public:
    // 菜单项类型
    enum ItemType
    {
        IT_MASK = 0x70,          // 掩码
        IT_TEXT = 0x10,          // 文本选项
        IT_ARROW_UP = 0x20,      // 上翻页键
        IT_ARROW_DOWN = 0x40     // 下翻页键
    };

    // 菜单项边角倒圆
    enum RoundedCorner
    {
        RC_MASK = 0x0F,          // 掩码
        RC_LEFT_TOP = 0x01,      // 左上角倒圆
        RC_LEFT_BOTTOM = 0x02,   // 左下角倒圆
        RC_RIGHT_TOP = 0x04,     // 右上角倒圆
        RC_RIGHT_BOTTOM = 0x08   // 右下角倒圆
    };

    enum Item
    {
        Item_First = 0x01,      // 第一个
        Item_Last = 0x02,       // 最后一个
    };

    ComboListItem(int flags);
    ~ComboListItem();
    void setUserData(int data)
    {
        _userData = data;
    }
    int userData() const
    {
        return _userData;
    }
    void setChecked(bool checked)
    {
        _isChecked = checked;
    }
    bool isChecked() const
    {
        return _isChecked;
    }

    // 设置绘画选中标记
    void setDrawCheckedMark(bool drawMark)
    {
        _isDrawCheckedMark = drawMark;
    }

    signals:
    void clicked(int userData);

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);

private slots:
    void _itemClicked();

private:
    ComboListPopup *_comboListPopup;
    int _flags;            // ItemType与RoundedCorner的位组合
    int _userData;         // 与菜单项关联的额外数据
    bool _isChecked;       // 是否钩选
    bool _isDrawCheckedMark;// 是否绘画选中标记
};
