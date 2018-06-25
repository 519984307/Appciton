#include "Button.h"
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>

#define PADDING 4
#define ICON_TEXT_PADDING 4

class ButtonPrivate
{
public:
    ButtonPrivate()
        :m_btnStyle(Button::ButtonTextBesideIcon),
          m_borderWidth(1),
          m_borderRadius(0)
    {}

    Button::ButtonStyle m_btnStyle;
    int m_borderWidth;
    int m_borderRadius;
};

Button::Button(const QString &text, const QIcon &icon, QWidget *parent)
    :QAbstractButton(parent),
      d_ptr(new ButtonPrivate)
{
    setText(text);
    setIcon(icon);
}

Button::~Button()
{

}

void Button::setBorderWidth(int width)
{
    if(d_ptr->m_borderWidth == width)
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
    if(d_ptr->m_borderRadius == radius)
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

void Button::buttonStyle(Button::ButtonStyle style)
{
    if(d_ptr->m_btnStyle ==  style)
    {
        return;
    }
    d_ptr->m_btnStyle = style;
    updateGeometry();
}

Button::ButtonStyle Button::getButtonStyle() const
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

    switch(d_ptr->m_btnStyle)
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
        if(width < icoSize.width())
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
        if(height < textRect.height())
        {
            height = textRect.height();
        }
        hint.setWidth(icoSize.width() + textRect.width() + ICON_TEXT_PADDING + PADDING * 2);

        hint.setHeight(height + PADDING * 2);
    }
        break;
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

    if(!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Button);
        textColor = pal.color(QPalette::Disabled, QPalette::ButtonText);
        icoMode = QIcon::Disabled;
    }
    else if((isCheckable() && isChecked()) || isDown())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Button);
        textColor = pal.color(QPalette::Active, QPalette::ButtonText);
        icoMode = QIcon::Selected;
    }
    else if(hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
        icoMode = QIcon::Active;
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Button);
        textColor = pal.color(QPalette::Inactive, QPalette::ButtonText);
        icoMode = QIcon::Normal;
    }

    QRect rect = contentsRect();

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    if(d_ptr->m_borderWidth)
    {
        QPen pen(pal.color(QPalette::Inactive, QPalette::WindowText), d_ptr->m_borderWidth);
        if(hasFocus())
        {
            pen.setColor(pal.color(QPalette::Active, QPalette::WindowText));
        }
        else if(!isEnabled())
        {
            pen.setColor(pal.color(QPalette::Disabled, QPalette::WindowText));
        }

        painter.setPen(pen);
        painter.setBrush(bgColor);
        QRect r = rect.adjusted(d_ptr->m_borderWidth, d_ptr->m_borderWidth, -d_ptr->m_borderWidth, -d_ptr->m_borderWidth);
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
    switch (d_ptr->m_btnStyle) {
    case ButtonIconOnly:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, iconSize(), rect);
        ico.paint(&painter, iconRect, Qt::AlignCenter, icoMode);
    }
        break;
    case ButtonTextOnly:
    {
        painter.drawText(rect, Qt::AlignCenter, text());
    }
        break;
    case ButtonTextUnderIcon:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignTop|Qt::AlignHCenter, iconSize(), rect.adjusted(0, PADDING, 0, 0));
        ico.paint(&painter, iconRect, Qt::AlignCenter, icoMode);

        QRect textRect = rect;
        textRect.setTop(iconRect.bottom() + ICON_TEXT_PADDING);
        painter.drawText(textRect, Qt::AlignCenter, text());
    }
        break;
    case ButtonTextBesideIcon:
    default:
    {
        QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignTop|Qt::AlignHCenter, iconSize(), rect.adjusted(PADDING, 0, 0, 0));
        ico.paint(&painter, iconRect, Qt::AlignCenter, icoMode);

        QRect textRect = rect;
        textRect.setLeft(iconRect.right() + ICON_TEXT_PADDING);
        painter.drawText(textRect, Qt::AlignCenter, text());
    }
        break;
    }
}
