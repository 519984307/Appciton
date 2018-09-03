/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件绘图元素基类
////////////////////////////////////////////////////////////////////////////////
#include "WaveWidgetItem.h"
#include "WaveWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造一个绘图元素对象
//
// 参数:
// parent: 绘图元素所归属的波形控件
// isBackground: true表示将绘图元素融入背景位图, false表示绘图元素动态绘制
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveWidgetItem::WaveWidgetItem(WaveWidget *wave, bool isBackground)
    : _wave(wave), _geometry(), _font(), _color(), _isVisible(true),
      _isBackground(isBackground)
{
    if (!_wave)
    {
        return;
    }

    _font = _wave->font();
    _color = _wave->palette();
    _wave->addItem(this);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构一个绘图元素对象
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveWidgetItem::~WaveWidgetItem()
{
}
#include "Debug.h"
////////////////////////////////////////////////////////////////////////////////
// 说明:
// 改变绘图元素尺寸
//
// 参数:
// geometry: 新的坐标及尺寸
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::resize(const QRect &geometry)
{
    if (!geometry.isValid())
    {
        return;
    }

    QRegion region(geometry);
    if (_geometry.isValid())
    {
        region += _geometry;
    }

    _geometry = geometry;

    if (_wave)
    {
        if (_isBackground)
        {
            _wave->updateBackground();
        }

        _wave->update(region);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 改变绘图元素的字体
//
// 参数:
// font: 新字体
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::setFont(const QFont &font)
{
    if (font == _font)
    {
        return;
    }

    _font = font;

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 改变绘图元素颜色
//
// 参数:
// color: 新颜色
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::setPalette(const QPalette &color)
{
    if (color == _color)
    {
        return;
    }

    _color = color;

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 显示或隐藏绘图元素
//
// 参数:
// isVisible: true表示可见, false表示隐藏
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::setVisible(bool isVisible)
{
    if (isVisible == _isVisible)
    {
        return;
    }

    _isVisible = isVisible;

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定是否将绘图元素融入背景位图, 出于性能考虑, 相对静态的绘图元素应该融入背景位图, 频繁变动
// 的绘图元素应该动态绘制
//
// 参数:
// isBackground: true表示融入背景位图, false表示动态绘制
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::setBackground(bool isBackground)
{
    if (!isBackground)
    {
        return;
    }

    if (_wave)
    {
        _wave->updateBackground();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 通知绘图元素所属的波形控件进行必要的重绘
////////////////////////////////////////////////////////////////////////////////
void WaveWidgetItem::update()
{
    if (_wave && _geometry.isValid())
    {
        if (_isBackground)
        {
            _wave->updateBackground();
        }
        else
        {
            _wave->update(_geometry);
        }
    }
}
