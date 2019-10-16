/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/10/16
 **/


////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件中的文本标签
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "WaveWidgetLabel.h"
#include "WaveWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造一个文本标签
//
// 参数:
// text: 标签文本字符串
// flags: 文本对齐标志, 接收如下标志用于控制字符串的绘制:
//      Qt::AlignLeft
// 		Qt::AlignRight
// 		Qt::AlignHCenter
// 		Qt::AlignJustify
// 		Qt::AlignTop
// 		Qt::AlignBottom
// 		Qt::AlignVCenter
// 		Qt::AlignCenter
// 		Qt::TextDontClip
// 		Qt::TextSingleLine
// 		Qt::TextExpandTabs
// 		Qt::TextShowMnemonic
// 		Qt::TextWordWrap
// 		Qt::TextIncludeTrailingSpaces
// parent: 标签所归属的波形控件
// isBackground: true表示将标签融入背景位图, false表示标签动态绘制
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveWidgetLabel::WaveWidgetLabel(const QString &text, int flags,
                                 WaveWidget *wave) :
    IWidget("", wave),
    _text(text), _flags(flags), _isFixedWidth(false)
{
    if (NULL != wave)
    {
        _waveName = wave->name();
    }
    else
    {
        _waveName = "";
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构一个标签对象
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveWidgetLabel::~WaveWidgetLabel()
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 设置标签文本
//
// 参数:
// text: 标签文本字符串
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetLabel::setText(const QString &text)
{
    if (text != _text)
    {
        _text = text;
        resizeToFitText();
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 设置标签对齐标志
//
// 参数:
// flags: 文本对齐标志, 接收如下标志用于控制字符串的绘制:
// 		Qt::AlignLeft
// 		Qt::AlignRight
// 		Qt::AlignHCenter
// 		Qt::AlignJustify
// 		Qt::AlignTop
// 		Qt::AlignBottom
// 		Qt::AlignVCenter
// 		Qt::AlignCenter
// 		Qt::TextDontClip
// 		Qt::TextSingleLine
// 		Qt::TextExpandTabs
// 		Qt::TextShowMnemonic
// 		Qt::TextWordWrap
// 		Qt::TextIncludeTrailingSpaces
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetLabel::setFlags(int flags)
{
    if (flags != _flags)
    {
        _flags = flags;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 根据当前的字体和字号修改标签的尺寸
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetLabel::resizeToFitText()
{
    if (_isFixedWidth)
    {
        return;
    }

    QFontMetrics fm(font());
    resize(fm.width(text()), fm.height());
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制函数
//
// 参数:
// painter: 绘图对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////

void WaveWidgetLabel::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter painter(this);
    painter.setFont(font());
    painter.setPen(palette().color(QPalette::WindowText));

    QRect r = this->rect();
    painter.drawText(r, _flags, _text);
}
