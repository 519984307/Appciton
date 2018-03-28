////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件绘图元素基类
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_WIDGET_ITEM_H
#define WAVE_WIDGET_ITEM_H

#include <QRect>
#include <QFont>
#include <QPalette>

class QPainter;
class WaveWidget;

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件绘图元素基类
////////////////////////////////////////////////////////////////////////////////
class WaveWidgetItem
{
public:
    WaveWidgetItem(WaveWidget *wave, bool isBackground = true);
    virtual ~WaveWidgetItem();

    void resize(const QRect &geometry);
    inline void resize(int x, int y, int w, int h)
    {
        resize(QRect(x, y, w, h));
    }
    inline void move(const QPoint &leftTop)
    {
        resize(leftTop.x(), leftTop.y(), _geometry.width(), _geometry.height());
    }
    inline void move(int x, int y)
    {
        resize(x, y, _geometry.width(), _geometry.height());
    }
    inline const QRect &geometry() const
    {
        return _geometry;
    }
    inline QSize size() const
    {
        return _geometry.size();
    }
    inline int x() const
    {
        return _geometry.x();
    }
    inline int y() const
    {
        return _geometry.y();
    }
    inline int width() const
    {
        return _geometry.width();
    }
    inline int height() const
    {
        return _geometry.height();
    }
    void setFont(const QFont &font);
    inline const QFont &font() const
    {
        return _font;
    }
    void setPalette(const QPalette &color);
    inline const QPalette &palette() const
    {
        return _color;
    }
    void setVisible(bool isVisible);
    inline bool isVisible() const
    {
        return _isVisible;
    }
    void setBackground(bool isBackground);
    inline bool isBackground() const
    {
        return _isBackground;
    }
    void update();
    virtual void paintItem(QPainter &painter) = 0;

private:
    WaveWidget *_wave;             // 绘图元素所归属的波形控件
    QRect _geometry;               // 坐标尺寸
    QFont _font;                   // 字体
    QPalette _color;               // 颜色
    bool _isVisible;               // true表示可见, false表示隐藏
    bool _isBackground;            // true表示融入背景位图, false表示动态绘制
};

#endif
