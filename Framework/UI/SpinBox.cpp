/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/19
 **/

#include "SpinBox.h"
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>
#include <QKeyEvent>
#include <ThemeManager.h>
#include "PopupNumEditor.h"
#include "Utility.h"

#define PADDING 4
#define ICON_TEXT_PADDING 4
#define DEFAULT_BUTTON_HEIGHT (themeManager.getAcceptableControlHeight())

class SpinBoxPrivate
{
public:
    SpinBoxPrivate()
        : m_borderWidth(themeManager.getBorderWidth()),
          m_borderRadius(themeManager.getBorderRadius()),
          arrow(false),
          status(SpinBox::SPIN_BOX_FOCUS_IN)
    {}

    // get the value
    QString value() const;

    int m_borderWidth;
    int m_borderRadius;

    ItemEditInfo info;
    bool arrow;
    SpinBox::CtrlStatus status;
};

QString SpinBoxPrivate::value() const
{
    if (info.type == ItemEditInfo::VALUE)
    {
        if (info.curValue >= info.lowLimit && info.curValue <= info.highLimit)
        {
            return Util::convertToString(info.curValue, info.scale);
        }
        else
        {
            // invaild display blank
            return QString("");
        }
    }
    else
    {
        return info.list.at(info.curValue);
    }
    return QString();
}

SpinBox::SpinBox(QWidget *parent)
    : QAbstractButton(parent),
      d_ptr(new SpinBoxPrivate)

{
    QPalette pal = palette();
    themeManager.setupPalette(ThemeManager::ControlSpinBox, &pal);
    setPalette(pal);
}

SpinBox::~SpinBox()
{
}

void SpinBox::setArrow(bool arrow)
{
    d_ptr->arrow = arrow;
}

bool SpinBox::isArrow() const
{
    return d_ptr->arrow;
}

void SpinBox::setValue(int value)
{
    if (value == d_ptr->info.curValue)
    {
        return;
    }

    d_ptr->info.curValue = value;
    update();
    emit valueChange(d_ptr->info.curValue, d_ptr->info.scale);
}

int SpinBox::getValue()
{
    return d_ptr->info.curValue;
}

void SpinBox::setRange(int min, int max)
{
    if (d_ptr->info.type == ItemEditInfo::LIST)
    {
        // 当格式为字符串时，设置范围的值不可超出范围
        if (max > d_ptr->info.highLimit || min < d_ptr->info.lowLimit)
        {
            return;
        }
    }
    d_ptr->info.lowLimit = min;
    d_ptr->info.highLimit = max;
    if (d_ptr->info.startValue > d_ptr->info.highLimit || d_ptr->info.startValue < d_ptr->info.lowLimit)
    {
        // 如果起始值不在范围内，设置起始值为最小值
        d_ptr->info.startValue = min;
    }
}

void SpinBox::getRange(int *min, int *max)
{
    if (min)
    {
        *min = d_ptr->info.lowLimit;
    }
    if (max)
    {
        *max = d_ptr->info.highLimit;
    }
}

void SpinBox::setScale(int scale)
{
    d_ptr->info.scale = scale;
}

int SpinBox::getScale()
{
    return d_ptr->info.scale;
}

void SpinBox::setStep(int step)
{
    d_ptr->info.step = step;
}

QSize SpinBox::sizeHint() const
{
    QString t = text();
    QFontMetrics fm(font());
    QRect textRect = fm.boundingRect(t);
    QSize hint;

    hint.setWidth(textRect.width() + PADDING * 2);
    hint.setHeight(textRect.height() + PADDING * 2);

    if (hint.height() < DEFAULT_BUTTON_HEIGHT)
    {
        hint.setHeight(DEFAULT_BUTTON_HEIGHT);
    }

    return hint;
}

void SpinBox::setSpinBoxStyle(SpinBox::SpinBoxStyle spinBoxStyle)
{
    if (spinBoxStyle == SPIN_BOX_STYLE_NUMBER)
    {
        d_ptr->info.type = ItemEditInfo::VALUE;
    }
    else if (spinBoxStyle == SPIN_BOX_STYLE_STRING)
    {
        d_ptr->info.type = ItemEditInfo::LIST;
    }
}

void SpinBox::setStringList(QStringList strs)
{
    if (d_ptr->info.type != ItemEditInfo::LIST)
    {
        return;
    }
    d_ptr->info.list = strs;
    d_ptr->info.highLimit = strs.count() - 1;
    d_ptr->info.lowLimit = 0;
    d_ptr->info.scale = 1;
}

void SpinBox::setStartValue(int value)
{
    d_ptr->info.startValue = value;
}

void SpinBox::onPopupDestroy()
{
    emit valueChange(d_ptr->info.curValue, d_ptr->info.scale);
}

void SpinBox::onEditValueUpdate(int value)
{
    d_ptr->info.curValue = value;
    update();
}

void SpinBox::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPalette pal = palette();
    QColor bgColor;
    QColor textColor;

    if (!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Window);
        textColor = pal.color(QPalette::Disabled, QPalette::WindowText);
    }
    else if ((isCheckable() && isChecked()) || isDown() || hasFocus())
    {
        if (d_ptr->status == SPIN_BOX_EDITABLE)
        {
            bgColor = pal.color(QPalette::Active, QPalette::Highlight);
            textColor = pal.color(QPalette::Active, QPalette::WindowText);
        }
        else
        {
            bgColor = pal.color(QPalette::Active, QPalette::Highlight);
            textColor = pal.color(QPalette::Active, QPalette::WindowText);
        }
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Window);
        textColor = pal.color(QPalette::Inactive, QPalette::WindowText);
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
        QRect r = rect.adjusted(d_ptr->m_borderWidth, d_ptr->m_borderWidth, -d_ptr->m_borderWidth,
                                -d_ptr->m_borderWidth);
        painter.drawRoundedRect(r, d_ptr->m_borderRadius, d_ptr->m_borderRadius);
    }
    else
    {
        QPainterPath path;
        path.addRoundedRect(rect, d_ptr->m_borderRadius, d_ptr->m_borderRadius);
        painter.fillPath(path, bgColor);
    }

    painter.setPen(textColor);
    if (d_ptr->arrow)
    {
        if (d_ptr->info.curValue < d_ptr->info.highLimit)
        {
            QIcon upIcon = themeManager.getIcon(ThemeManager::IconUp);
            QRect upRect = QStyle::alignedRect(layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter,
                                               iconSize(), rect.adjusted(width() / 6 * 5, 0, 0, -(height() / 2)));
            upIcon.paint(&painter, upRect);
        }

        if (d_ptr->info.curValue > d_ptr->info.lowLimit)
        {
            QIcon downIcon = themeManager.getIcon(ThemeManager::IconDown);
            QRect downRect = QStyle::alignedRect(layoutDirection(), Qt::AlignVCenter | Qt::AlignHCenter,
                                                 iconSize(), rect.adjusted(width() / 6 * 5, height() / 2, 0, 0));
            downIcon.paint(&painter, downRect);
        }
    }
    painter.drawText(rect, Qt::AlignCenter, d_ptr->value());
}

void SpinBox::keyPressEvent(QKeyEvent *ev)
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
}

void SpinBox::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        if (d_ptr->status != SPIN_BOX_EDITABLE)
        {
            d_ptr->status = SPIN_BOX_FOCUS_OUT;
            focusPreviousChild();
        }
        else
        {
            if (d_ptr->info.curValue > d_ptr->info.lowLimit)
            {
                d_ptr->info.curValue -= d_ptr->info.step;
                update();
            }
        }
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        if (d_ptr->status != SPIN_BOX_EDITABLE)
        {
            d_ptr->status = SPIN_BOX_FOCUS_OUT;
            focusNextChild();
        }
        else
        {
            if (d_ptr->info.curValue < d_ptr->info.highLimit)
            {
                d_ptr->info.curValue += d_ptr->info.step;
                update();
            }
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QRect vrect = rect();
        QRect rect(mapToGlobal(vrect.topLeft()),
                   mapToGlobal(vrect.bottomRight()));
        QPalette pal = palette();
        PopupNumEditor *editor = new PopupNumEditor();
        editor->setEditInfo(d_ptr->info);
        QFontInfo info = fontInfo();
        editor->setFont(QFont(info.family(), info.pixelSize(), info.weight(), info.italic()));
        editor->setPalette(pal);
        editor->setEditValueGeometry(rect);
        QObject::connect(editor, SIGNAL(valueChanged(int)), this, SLOT(onEditValueUpdate(int)));
        QObject::connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(onPopupDestroy()));
        editor->show();
        break;
    }
    default:
        QAbstractButton::keyReleaseEvent(ev);
        break;
    }
}

void SpinBox::mouseReleaseEvent(QMouseEvent *ev)
{
    QAbstractButton::mouseReleaseEvent(ev);
    QRect vrect = rect();
    QRect rect(mapToGlobal(vrect.topLeft()),
               mapToGlobal(vrect.bottomRight()));
    QPalette pal = palette();
    PopupNumEditor *editor = new PopupNumEditor();
    editor->setEditInfo(d_ptr->info);
    QFontInfo info = fontInfo();
    editor->setFont(QFont(info.family(), info.pixelSize(), info.weight(), info.italic()));
    editor->setPalette(pal);
    editor->setEditValueGeometry(rect);
    QObject::connect(editor, SIGNAL(valueChanged(int)), this, SLOT(onEditValueUpdate(int)));
    QObject::connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(onPopupDestroy()));
    editor->show();
}

void SpinBox::focusInEvent(QFocusEvent *ev)
{
    QAbstractButton::focusInEvent(ev);
    d_ptr->status = SPIN_BOX_FOCUS_IN;
}

void SpinBox::focusOutEvent(QFocusEvent *ev)
{
    QAbstractButton::focusOutEvent(ev);
    d_ptr->status = SPIN_BOX_FOCUS_OUT;
}

void SpinBox::mousePressEvent(QMouseEvent *e)
{
    QAbstractButton::mousePressEvent(e);
    // 触屏点击播放按键音
    themeManager.playClickSound();
}
