/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

#pragma once
#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QFocusEvent;
class QPaintEvent;
class QResizeEvent;

/***************************************************************************************************
 * 封装QWidget类，提供聚焦、边框、窗口名称。
 **************************************************************************************************/
class IWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IWidget(const QString &name = "IWidget", QWidget *parent = 0);

    // 窗体名称
    const QString &name()
    {
        return _name;
    }

    void setName(const QString &name)
    {
        _name = name;
    }

    // 设置聚焦边框颜色
    void setFocusedBorderColor(QColor color)
    {
        _focusedBorderColor = color;
    }

    virtual void getSubFocusWidget(QList<QWidget *> &/*subWidget*/) const {}

    virtual void resetWaveWidget() {}

    virtual void updateWidgetConfig()
    {
        loadConfig();
    }

signals:
    void clicked(IWidget *w);   // 按键或鼠标单击信号
    void released(IWidget *w);  // 按键或鼠标释放信号
    void released();

protected:
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    int focusedBorderWidth()
    {
        return _focusedBorderWidth;
    }

    /**
     * @brief loadConfig  加载配置
     */
    virtual void loadConfig(){}

private:
    void _updateFocusRegion();

private:
    QString _name;                  // 窗体名称
    QRegion _focusRegion;           // 聚焦边框区域
    QColor _focusedBorderColor;     // 聚焦边框颜色。

    static const int _focusedBorderWidth = 4;     // 聚焦边框宽度
};
