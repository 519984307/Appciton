/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#pragma once
#include <QWidget>
#include "IButton.h"

#define TITLE_H (30)           //标题高度
#define HELP_BUTTON_H (30)     //帮助按钮高度
#define ITEM_H (30)            //子项高度
#define BORDER_W (4)           //边宽

class QVBoxLayout;
class QStackedWidget;
class QLabel;
class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    //设置文本及图片
    void setCloseBtnTxt(const QString &txt);
    void setCloseBtnPic(const QImage &pic);
    void setCloseBtnColor(QColor color);

    // 关闭按钮获得焦点
    void closeBtnSetFoucs();

    //获取子菜单高度
    int getListWidth() {return _listWidth;}

    //获取子菜单高度
    int getSubmenuHeight() {return _submenuHeight;}

    //获取子菜单宽度
    int getSubmenuWidth() {return _submenuWidth;}

    bool closeBtnHasFocus() { return closeBtn->hasFocus(); }

    //获取关闭按钮
    IButton *getCloseBtn() {return closeBtn;}

    void popup(int x = 0, int y = 0);

    explicit MenuWidget(const QString &name);
    ~MenuWidget();

protected:
    // 重绘。
    virtual void paintEvent(QPaintEvent *event);

    //按键事件
    void keyPressEvent(QKeyEvent *event);

protected:
    QLabel *titleLabel;

    QVBoxLayout *mainLayout;           //主布局器

    IButton *closeBtn;

private slots:
    void _closeSlot(void);

private:
    //初始化子菜单
    void _addSubMenu();

    int _listWidth;                      //列表宽度
    int _submenuWidth;                   //子菜单宽度
    int _submenuHeight;                  //高度
};
