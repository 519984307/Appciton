/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/28
 **/


#include "SystemModeBarWidget.h"
#include "FontManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include "PopupList.h"
#include "FontManager.h"
#include "LayoutManager.h"
#include "LanguageManager.h"

/**************************************************************************************************
 * 功能： 设置显示的内容。
 * 参数：
 *      str：显示的内容。
 *************************************************************************************************/
void SystemModeBarWidget::setMode(UserFaceType mode)
{
    QString str(SystemSymbol::convert(mode));
    _modeLabel->setText(trs(str));
}

void SystemModeBarWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    if (_systemModeList == NULL)
    {
        _systemModeList = new PopupList(this , false);
        connect(_systemModeList, SIGNAL(destroyed()), this, SLOT(_popupListDestroy()));
        connect(_systemModeList, SIGNAL(selectItemChanged(int)), this , SLOT(_getListIndex(int)));
        _popupListAddItem();
        _systemModeList->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    }
    _systemModeList->show();
}

void SystemModeBarWidget::_popupListDestroy()
{
    if (_popupListIndex == -1)
    {
        _systemModeList = NULL;
        return;
    }
    UserFaceType type = static_cast<UserFaceType>(_popupListIndex);
    layoutManager.setUFaceType(type);

    _systemModeList = NULL;
}

void SystemModeBarWidget::_getListIndex(int index)
{
    _popupListIndex = index;
}

void SystemModeBarWidget::_popupListAddItem()
{
    for (int i = 0 ; i < UFACE_NR ; i++)
    {
        _systemModeList->addItemText(trs(SystemSymbol::convert(static_cast<UserFaceType>(i))));
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemModeBarWidget::SystemModeBarWidget(QWidget *parent)
    : IWidget("SystemModeBarWidget", parent)
    , _systemModeList(NULL)
    , _popupListIndex(-1)
{
    setFocusPolicy(Qt::StrongFocus);
    _modeLabel = new QLabel("", this);
    _modeLabel->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    _modeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    _modeLabel->setStyleSheet("color:lightGray;background:rgb(48,48,48);"
//                              "border:1px groove black;border-radius:4px;");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(3);
    layout->addWidget(_modeLabel);
    setLayout(layout);

    // setMode(SYSTEM_MODE_MONITOR);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}
