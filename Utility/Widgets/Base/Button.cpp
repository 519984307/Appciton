/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/8
 **/

#include "Button.h"
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>
#include <QKeyEvent>
#include <ThemeManager.h>

#define PADDING 4
#define ICON_TEXT_PADDING 4
#define DEFAULT_BUTTON_HEIGHT (themeManger.getAcceptableControlHeight())

class ButtonPrivate
{
public:
    ButtonPrivate()
        : m_btnStyle(Button::ButtonTextBesideIcon),
          m_borderWidth(themeManger.getBorderWidth()),
          m_borderRadius(themeManger.getBorderRadius()),
          m_press(false)
    {}

    Button::ButtonStyle m_btnStyle;
    int m_borderWidth;
    int m_borderRadius;
    bool m_press;

    /**
     * @brief drawIcon 以转换为Image方式绘画图标
     * @param painter
     * @param ico
     * @param iconRect
     * @param icoMode
     */
    void drawIcon(QPainter &painter, QIcon &ico, QRect iconRect, QIcon::Mode icoMode);
};

Button::Button(const QString &text, const QIcon &icon, QWidget *parent)
    : QAbstractButton(parent),
      d_ptr(new ButtonPrivate)
{
    setText(text);
    setIcon(icon);
    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlButton, pal);
    setPalette(pal);
}

Button::~Button()
{
}

void Button::setBorderWidth(int width)
{
    if (d_ptr->m_borderWidth == width)
    {
        return;
    }

    d_ptr->m_borderWidth = width;
    update();
}

int Button::borderWidth() const
{
    return d_ptr->m_borderWidth;
}

void Button::setBorderRadius(int radius)
{
    if (d_ptr->m_borderRadius == radius)
    {
        return;
    }
    d_ptr->m_borderRadius = radius;
    update();
}

int Button::borderRadius() const
{
    return d_ptr->m_borderRadius;
}

void Button::setButtonStyle(Button::ButtonStyle style)
{
    if (d_ptr->m_btnStyle ==  style)
    {
        return;
    }
    d_ptr->m_btnStyle = style;
    updateGeometry();
}

Button::ButtonStyle Button::buttonStyle() const
{
    return d_ptr->m_btnStyle;
}

QSize Button::sizeHint() const
{
    QString t = text();
    QFontMetrics fm(font());
    QRect textRect = fm.boundingRect(t);
    QSize icoSize = iconSize();
    QSize hint;

    switch (d_ptr->m_btnStyle)
    {
    case ButtonTextOnly:
        hint.setWidth(textRect.width() + PADDING * 2);
        hint.setHeight(textRect.height() + PADDING * 2);
        break;

    case ButtonIconOnly:
        hint.setWidth(icoSize.width() + PADDING * 2);
        hint.setHeight(icoSize.height() + PADDING * 2);
        break;

    case ButtonTextUnderIcon:
    {
        int width = textRect.width();
        if (width < icoSize.width())
        {
            width = icoSize.width();
        }
        hint.setWidth(width + PADDING * 2);
        hint.setHeight(iconSize().height() + textRect.height() + ICON_TEXT_PADDING + PADDING * 2);
    }
    break;

    case ButtonTextBesideIcon:
    default:
    {
        int height = icoSize.height();
        if (height < textRect.height())
        {
            height = textRect.height();
        }
        hint.setWidth(icoSize.width() + textRect.width() + ICON_TEXT_PADDING + PADDING * 2);

        hint.setHeight(height + PADDING * 2);
    }
    break;
    }

    if (hint.height() < DEFAULT_BUTTON_HEIGHT)
    {
        hint.setHeight(DEFAULT_BUTTON_HEIGHT);
    }

    return hint;
}

void Button::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPalette pal = palette();
    QColor bgColor;
    QColor textColor;
    QIcon::Mode icoMode = QIcon::Normal;

    if (!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Window);
        textColor = pal.color(QPalette::Disabled, QPalette::WindowText);
        icoMode = QIcon::Disabled;
    }
    else if ((isCheckable() && isChecked()) || isDown())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Window);
        textColor = pal.color(QPalette::Active, QPalette::WindowText);
        icoMode = QIcon::Selected;
    }
    else if (hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
        icoMode = QIcon::Active;
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Window);
        textColor = pal.color(QPalette::Inactive, QPalette::WindowText);
        icoMode = QIcon::Normal;
    }

    QRect rect = contentsRect();

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    if (d_ptr->m_borderWidth)
    {
        QPen pen(pal.color(QPalette::Inactive, QPalette::Shadow), d_ptr->m_borderWidth);
        if (hasFocus())
        {
            pen.setColor(pal.color(QPalette::Active, QPalette::Shadow));
        }
        else if (!isEnabled())
        {
            pen.setColor(pal.color(QPalette::Disabled, QPalette::Shadow));
        }

        painter.setPen(pen);
        painter.setBrush(bgColor);
        QRect r = rect.adjusted(d_ptr->m_borderWidth / 2, d_ptr->m_borderWidth / 2, -d_ptr->m_borderWidth / 2, -d_ptr->m_borderWidth / 2);
        painter.drawRoundedRect(r, d_ptr->m_borderRadius, d_ptr->m_borderRadius);
    }
    else
    {
        QPainterPath path;
        path.addRoundedRect(rect, d_ptr->m_borderRadius, d_ptr->m_borderRadius);
        painter.fillPath(path, bgColor);
    }

    painter.setPen(textColor);
    QIcon ico = icon();
    switch (d_ptr->m_btnStyle)
    {
    case ButtonIconOnly:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, iconSize(), rect);
        d_ptr->drawIcon(painter, ico, iconRect, icoMode);
    }
    break;
    case ButtonTextOnly:
    {
        painter.drawText(rect, Qt::AlignCenter, text());
    }
    break;
    case ButtonTextUnderIcon:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignTop | Qt::AlignHCenter, iconSize(), rect.adjusted(0,
                                             PADDING, 0, 0));
        d_ptr->drawIcon(painter, ico, iconRect, icoMode);

        QRect textRect = rect;
        textRect.setTop(iconRect.bottom() + ICON_TEXT_PADDING);
        painter.drawText(textRect, Qt::AlignCenter, text());
    }
    break;
    case ButtonTextBesideIcon:
    default:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignVCenter | Qt::AlignLeft, iconSize(),
                                             rect.adjusted(PADDING, 0, 0, 0));
        d_ptr->drawIcon(painter, ico, iconRect, icoMode);

        QRect textRect = rect;
        textRect.setLeft(iconRect.right() + ICON_TEXT_PADDING);
        painter.drawText(textRect, Qt::AlignCenter, text());
    }
    break;
    }
}

void Button::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        break;
    default:
        QAbstractButton::keyPressEvent(ev);
        break;
    }
    d_ptr->m_press = true;
}

void Button::keyReleaseEvent(QKeyEvent *ev)
{
    if (!d_ptr->m_press)
    {
        return;
    }
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        focusNextChild();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        click();
        break;
    default:
        QAbstractButton::keyReleaseEvent(ev);
        break;
    }
    d_ptr->m_press = false;
}

void ButtonPrivate::drawIcon(QPainter &painter, QIcon &ico, QRect iconRect, QIcon::Mode icoMode)
{
    if (!ico.isNull())
    {
        QPixmap pix = ico.pixmap(iconRect.size(), icoMode);
        // 以预乘的格式创建image，目的是绘画图前前进行alpha预乘，使图像纹理能正常进行线性插值
        QImage tmp(iconRect.size(), QImage::Format_ARGB32_Premultiplied);
        QPainter drawImgPainter(&tmp);
        // 以Source的融合方式，使透明且重叠部分使用源图像
        drawImgPainter.setCompositionMode(QPainter::CompositionMode_Source);
        drawImgPainter.drawPixmap(0, 0, iconRect.width(), iconRect.height(), pix);
        drawImgPainter.end();
        painter.drawImage(iconRect, tmp);
    }
}
