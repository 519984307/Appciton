/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/18
 **/



#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include "LanguageManager.h"
#include "FontManager.h"
#include "Debug.h"
#include "SystemManager.h"

#define ITEM_H (30)                       //子项高度
#define SCROLL_BAR_WIDTH (17)             //滚动条宽度

class LButtonEx;
class QStackedWidget;
class SubMenu : public QWidget
{
    Q_OBJECT

public:
    // 菜单名称。
    const QString &name(void)
    {
        return _menuName;
    }

    // 菜单的描述，该描述将在菜单栏显示。
    const QString &desc(void)
    {
        return _menuDesc;
    }

    // 设置描述字串。
    void setDesc(const QString &desc);

    //设置窗体大小
    void setMenuSize(int width, int height);

    //获取窗体宽度
    int getMenuWidth()
    {
        return _menuWidth;
    }

    //获取窗体高度
    int getMenuHeight()
    {
        return _menuHeight;
    }

    QFont defaultFont(void)
    {
        return fontManager.textFont(fontManager.getFontSize(2));
    }

    // 聚焦第一个item
    virtual void focusFirstItem(void);

    // 聚焦最后一个item
    virtual void focusLastItem(void);

    // 构造与析构。
    explicit SubMenu(const QString &name);
    virtual ~SubMenu();

    void startLayout(void);
    QVBoxLayout *mainLayout;         //当设置菜单有标题时

    // 设置是否有标题。
    void setTitleEnable(bool enable)
    {
        _enableTitle = enable;
    }

signals:
    // 返回到菜单列表。
    void returnItemList();

    // 关闭菜单。
    void closeMenu(void);

    // 标题更改。
    void titleChanged(void);

    // 滚动区域变化
    void scrollAreaChange(int index);

protected:
    // 重载一些函数。
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void keyPressEvent(QKeyEvent *e);

    // Show之前的准备工作。
    virtual void readyShow(void);

    // Hide之前的清理工作。
    virtual void readyhide(void);

    //布局
    virtual void layoutExec(void);

private slots:
    void _returnBtnSlot();

private:
    int _menuWidth;                      //窗体宽度
    int _menuHeight;                     //窗体高度

    bool _enableTitle;                   //是否有标题

    QString _menuName;
    QString _menuDesc;
};
