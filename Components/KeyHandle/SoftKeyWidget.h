#pragma once
#include "IWidget.h"
#include "SoftkeyActionBase.h"
#include <QKeyEvent>

class SoftkeyWidget : public IWidget
{
    Q_OBJECT

public:
    SoftkeyWidget(QWidget *parent = 0);

    // 设置显示内容。
    void setContent(const KeyActionDesc &desc);

    // 外观效果控制。
    void pressedEffect();
    void normalEffect();

    // 默认的背景色。
    static QColor backgroundColor(void);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    // 计算合适的字体大小
    int _adjustFontSize(const QRect &r, QString &txt);

private:
    QPixmap _pixmap;
//    QImage _image;
    QString _text;
    QColor _color;
    QColor _releaseColor;
    QColor _pressColor;
    QColor _borderColor;
    bool _pressed;
    bool _enableBorder;
    bool _isPixmapValid;
};
