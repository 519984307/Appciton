/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#pragma once
#include "IWidget.h"
#include "SoftkeyActionBase.h"
#include <QKeyEvent>

class SoftkeyWidget : public IWidget
{
    Q_OBJECT

public:
    explicit SoftkeyWidget(QWidget *parent = 0);

    // 设置显示内容。
    void setContent(const KeyActionDesc *desc);

    // 外观效果控制。
    void pressedEffect();
    void normalEffect();

    // 默认的背景色。
    static QColor backgroundColor(void);

    // 获取按键标题
    QString hint()
    {
        return _hint;
    }

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    // 计算合适的字体大小
    int _adjustFontSize(const QRect &r, QString *txt);
    int _adjustHintFontSize(const QRect &r, QString *hint);

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
    QString _hint;
};
