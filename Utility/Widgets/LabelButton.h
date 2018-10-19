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
#include "ColorManager.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QPainter>
#include <QKeyEvent>

class LButton: public QPushButton
{
    Q_OBJECT

public:
    explicit LButton(int id = 0)
    {
        _id = id;
        QPalette p;
        p.setColor(QPalette::Highlight, colorManager.getHighlight());
        setPalette(p);
    }

    void setID(int id)
    {
        _id = id;
    }

    int getID(void)
    {
        return _id;
    }

    ~LButton()
    {
    }

signals:
    void released(int id);
    void realReleased(void);

protected:
    // 响应重绘事件。
    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.setPen(Qt::NoPen);

        if (isDown())
        {
            painter.setBrush(palette().shadow());        // 暗色
        }
        else
        {
            if (hasFocus()) // 聚焦。
            {
                painter.setBrush(palette().highlight()); // 亮色
            }
            else if (!isEnabled())
            {
                // painter.setBrush(Qt::gray);        // 暗色
                painter.setBrush(palette().color(QPalette::Disabled, QPalette::Window));
            }
            else          // 非聚焦。
            {
                painter.setBrush(Qt::white);           // 无画刷
            }
        }

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRoundedRect(rect(), 4, 4);

        // 绘文本。
        painter.setPen(hasFocus() ? palette().color(QPalette::HighlightedText) :
                palette().color(QPalette::WindowText));
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

    // 处理键盘和飞棱事件。
    void keyPressEvent(QKeyEvent *e)
    {
        switch (e->key())
        {
            case Qt::Key_Left:
            case Qt::Key_Right:
                e->ignore();
                return;
            default:
                break;
        }

        QPushButton::keyPressEvent(e);
    }

    // 处理键盘和飞棱事件。
    void keyReleaseEvent(QKeyEvent *e)
    {
        switch (e->key())
        {
            case Qt::Key_Return:
            case Qt::Key_Enter:

                if (!e->isAutoRepeat())
                {
                    emit released(_id);
                    emit realReleased();
                }
                break;
            default:
                break;
        }

        QPushButton::keyReleaseEvent(e);
    }

    // 处理鼠标释放事件。
    void mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            emit released(_id);
            emit realReleased(); // 发出点击消息
        }

        QPushButton::mouseReleaseEvent(e);
    }

private:
    int _id;
};

//返回按钮
class LButtonEx : public LButton
{
    Q_OBJECT
public:
    LButtonEx() : LButton()
    {
    }

    ~LButtonEx()
    {
    }

protected:
    void keyPressEvent(QKeyEvent *e)
    {
        e->ignore();
    }
};

// Label + QPushButton 的组合控件。
class LabelButton: public QWidget
{
    Q_OBJECT

public:
    void setLabel(const QString &text);
    void setValue(int value);
    void setValue(double value, int bits);
    void setValue(const QString &text);
    void setValue(const char *text);

    //设置拉伸因子和间隙
    void setStretch(int labelStrch, int buttonStrch);
    void setSpacing(int space);


    explicit LabelButton(const QString &text);
    ~LabelButton();

    LButton *button;          // 按钮。
    QLabel *label;           // 文本。
};

class PButton: public QPushButton
{
    Q_OBJECT

public:
    explicit PButton(int id = 0)
    {
        _id = id;
        QPalette p;
        p.setColor(QPalette::Highlight, colorManager.getHighlight());
        setPalette(p);
    }

    void setID(int id)
    {
        _id = id;
    }

    int getID(void)
    {
        return _id;
    }

    ~PButton()
    {
    }

    QPixmap pixmap;
    void setPicture(QPixmap pic)
    {
        pixmap = pic;
    }

signals:
    void released(int id);
    void realReleased(void);

protected:
    // 响应重绘事件。
    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.setPen(Qt::NoPen);

        if (isDown())
        {
            painter.setBrush(palette().shadow());        // 暗色
        }
        else
        {
            if (hasFocus()) // 聚焦。
            {
                painter.setBrush(QColor(255, 255, 255, 100)); // 亮色
            }
            else if (!isEnabled())
            {
                painter.setBrush(Qt::gray);        // 暗色
            }
            else          // 非聚焦。
            {
                painter.setBrush(Qt::transparent);           // 无画刷
            }
        }

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRoundedRect(rect(), 4, 4);

        QPixmap scaledpixmap = pixmap.scaled(QSize(64, 64));
        painter.drawPixmap((rect().width() - 64) / 2, 5, scaledpixmap);

        // 绘文本。
        painter.setPen(QColor(255, 255, 255));
        QRect r = rect();
        r.adjust(0, 0, 0, -2);
        painter.drawText(r, Qt::AlignHCenter | Qt::AlignBottom, text());
    }


    // 处理键盘和飞棱事件。
    void keyReleaseEvent(QKeyEvent *e)
    {
        switch (e->key())
        {
            case Qt::Key_Return:
            case Qt::Key_Enter:

                if (!e->isAutoRepeat())
                {
                    emit released(_id);
                    emit realReleased();
                }
                break;
            default:
                break;
        }

        QPushButton::keyReleaseEvent(e);
    }

    // 处理鼠标释放事件。
    void mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            emit released(_id);
            emit realReleased(); // 发出点击消息
        }

        QPushButton::mouseReleaseEvent(e);
    }

private:
    int _id;
};


class LButtonEn: public QPushButton
{
    Q_OBJECT

public:
    explicit LButtonEn(int id = 0)
    {
        _id = id;
        QPalette p;
        p.setColor(QPalette::Highlight, colorManager.getHighlight());
        setPalette(p);
    }

    void setID(int id)
    {
        _id = id;
    }

    int getID(void)
    {
        return _id;
    }

    ~LButtonEn()
    {
    }

signals:
    void released(int id);
    void realReleased(void);

protected:
    // 响应重绘事件。
    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.setPen(Qt::NoPen);

        if (isDown())
        {
            painter.setBrush(palette().shadow());        // 暗色
        }
        else
        {
            if (hasFocus()) // 聚焦。
            {
                painter.setBrush(palette().highlight()); // 亮色
            }
            else if (!isEnabled())
            {
                painter.setBrush(Qt::gray);        // 暗色
            }
            else          // 非聚焦。
            {
                painter.setBrush(Qt::white);           // 无画刷
            }
        }

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRoundedRect(rect(), 4, 4);

        // 绘文本。
        painter.setPen(hasFocus() ? palette().color(QPalette::HighlightedText) :
                palette().color(QPalette::WindowText));
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

    // 处理键盘和飞棱事件。
    void keyReleaseEvent(QKeyEvent *e)
    {
        switch (e->key())
        {
            case Qt::Key_Return:
            case Qt::Key_Enter:

                if (!e->isAutoRepeat())
                {
                    emit released(_id);
                    emit realReleased();
                }
                break;
            default:
                break;
        }

        QPushButton::keyReleaseEvent(e);
    }

    // 处理鼠标释放事件。
    void mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            emit released(_id);
            emit realReleased(); // 发出点击消息
        }

        QPushButton::mouseReleaseEvent(e);
    }

private:
    int _id;
};
