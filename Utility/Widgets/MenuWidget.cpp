/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#include "MenuWidget.h"
#include "WindowManager.h"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include "ColorManager.h"
#include "FontManager.h"
#include "MenuManager.h"

/**************************************************************************************************
 * 设置文本。
 *************************************************************************************************/
void MenuWidget::setCloseBtnTxt(const QString &txt)
{
    closeBtn->setText(txt);
}

/**************************************************************************************************
 * 设置pic。
 *************************************************************************************************/
void MenuWidget::setCloseBtnPic(const QImage &pic)
{
    closeBtn->setPicture(pic);
}

/**************************************************************************************************
 * 设置background Color。
 *************************************************************************************************/
void MenuWidget::setCloseBtnColor(QColor color)
{
    closeBtn->setBackgroundColor(color);
}

void MenuWidget::closeBtnSetFoucs()
{
    closeBtn->setFocus();
}

void MenuWidget::popup(int x, int y)
{
    menuManager.popupWidegt(this, x, y);
}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void MenuWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);

    QPainter barPainter(this);
    barPainter.setRenderHint(QPainter::Antialiasing);
    barPainter.setClipPath(clipPath);

    // 绘制边框。
    QPen pen;
    pen.setColor(colorManager.getBarBkColor());
    pen.setWidth(BORDER_W * 2);
    barPainter.setPen(pen);
    barPainter.setBrush(palette().window());
    barPainter.drawRect(rect());

    QRect r = rect();
    r.setBottom(TITLE_H);
    barPainter.fillRect(r, colorManager.getBarBkColor());
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void MenuWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;

        default:
            break;
    }

    QWidget::keyPressEvent(event);
}

void MenuWidget::_closeSlot(void)
{
    menuManager.returnPrevious();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MenuWidget::MenuWidget(const QString &name)
    : QWidget(0, Qt::FramelessWindowHint)
    , _listWidth(0)
    , _submenuWidth(0)
    , _submenuHeight(0)
{
    // 设定为模态窗口。
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setWindowModality(Qt::WindowModal);

    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
    _listWidth = windowManager.getPopWindowWidth() / 4;
    _submenuWidth = windowManager.getPopWindowWidth() - _listWidth - 20;
    _submenuHeight = windowManager.getPopWindowHeight() - 30 - 30 - 20;

    // 创建资源。
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    setAttribute(Qt::WA_NoSystemBackground); // draw background in paintEvent

    int fontSize = fontManager.getFontSize(1);
    // 标题栏。
    titleLabel = new QLabel(name);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(TITLE_H);
    titleLabel->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    titleLabel->setPalette(p);

    //关闭按钮
    closeBtn = new IButton("X");
    closeBtn->setEnableKeyAction(false);
    closeBtn->setFixedSize(TITLE_H - 1, TITLE_H - 1);
    closeBtn->setFont(fontManager.textFont(fontSize));
    connect(closeBtn, SIGNAL(realReleased()), this, SLOT(_closeSlot()));

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 1, 0, 0);
    titleLayout->addWidget(titleLabel, 1);
    titleLayout->addWidget(closeBtn, 1, Qt::AlignVCenter);

    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(BORDER_W, 0, BORDER_W, BORDER_W);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(titleLayout);

    setLayout(mainLayout);
}

/**************************************************************************************************
* 功能: 添加子菜单
*************************************************************************************************/
void MenuWidget::_addSubMenu(void)
{
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MenuWidget::~MenuWidget()
{
}

