#pragma once
#include <QComboBox>

// 适合触摸屏和飞梭使用的组合框控件
class ComboListPopup;
class ComboList: public QComboBox
{
    Q_OBJECT
public:
    ComboList(QWidget *parent = NULL);
    ~ComboList();

    // 重载。
    virtual void showPopup();

    // 设置边框倒角
    void setBorderRadius(double radius);

    // 设置item的值
    void setItemText(int index, const QString &text);

    // 设置和获取后缀
    void setSuffix(const QString &suffix);
    QString getSuffix();

    //set whether draw check mark in the popup list
    void setDrawCheckMark(bool flag);

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void hideEvent(QHideEvent *e);
    void paintEvent(QPaintEvent *e);

private slots:
    void _popupDestroyed();

private:
    bool _isPopup;                            // 是否弹出下拉菜单
    double _radius;                           // 边框倒角
    ComboListPopup *_popup;                   // 下拉弹出菜单
    QString _suffix;
    bool _drawCheckMark;
};
