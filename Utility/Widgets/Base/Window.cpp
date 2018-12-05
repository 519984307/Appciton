/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#include "Window.h"
#include <QEvent>
#include <QLabel>
#include "Button.h"
#include <QBoxLayout>
#include <QFrame>
#include <QResizeEvent>
#include "FontManager.h"
#include "ThemeManager.h"
#include "WindowManager.h"
#include <QPaintEvent>
#include <QPainter>

#define TITLE_BAR_HEIGHT 48

class WindowPrivate
{
public:
    WindowPrivate()
        : m_widget(NULL),
          m_titleLbl(NULL),
          m_mask(NULL),
          closeBtn(NULL)
    {}

    QWidget *m_widget;
    QLabel *m_titleLbl;
    QWidget *m_mask;
    Button *closeBtn;
};

Window::Window(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint),
      d_ptr(new WindowPrivate)
{
    QLabel *titleLbl = new QLabel();
    titleLbl->setFixedHeight(TITLE_BAR_HEIGHT);
    titleLbl->setText(this->windowTitle());
    titleLbl->setAlignment(Qt::AlignCenter);
    d_ptr->m_titleLbl = titleLbl;
    Button *closeBtn = new Button();
    d_ptr->closeBtn = closeBtn;
    closeBtn->setIconSize(QSize(24, 24));
    closeBtn->setBorderWidth(0);
    closeBtn->setFixedSize(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
    closeBtn->setIcon(QIcon(":/ui/close.svg"));
    closeBtn->setButtonStyle(Button::ButtonIconOnly);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(TITLE_BAR_HEIGHT, 0, 0, 0);
    hlayout->addWidget(titleLbl, 1);
    hlayout->addWidget(closeBtn);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->addLayout(hlayout);
    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    vLayout->addWidget(line);


#ifdef Q_WS_QWS
    this->setContentsMargins(8, 8, 8, 8);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    d_ptr->m_widget = new QWidget();
    vLayout->addWidget(d_ptr->m_widget, 1);
    connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(close()));

    setFont(fontManager.textFont(themeManger.defaultFontPixSize()));

    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlWindow, pal);
    setPalette(pal);

    d_ptr->m_mask = new QWidget(this);
    pal = d_ptr->m_mask->palette();
    pal.setColor(QPalette::Window, QColor(20, 20, 20, 80));
    d_ptr->m_mask->setPalette(pal);
    d_ptr->m_mask->setVisible(false);
    d_ptr->m_mask->setAutoFillBackground(true);
}

Window::~Window()
{
}

QLayout *Window::getWindowLayout()
{
    return d_ptr->m_widget->layout();
}

void Window::setWindowLayout(QLayout *layout)
{
    d_ptr->m_widget->setLayout(layout);
}

void Window::showMask(bool flag)
{
    if (flag)
    {
        d_ptr->m_mask->raise();
        d_ptr->m_mask->setVisible(true);
    }
    else
    {
        d_ptr->m_mask->lower();
        d_ptr->m_mask->setVisible(false);
    }
}

bool Window::isShowingMask() const
{
    return d_ptr->m_mask->isVisible();
}

Button *Window::getCloseBtn() const
{
    return d_ptr->closeBtn;
}

int Window::exec()
{
    if (windowManager.topWindow() == this)
    {
        return QDialog::exec();
    }
    else
    {
        // add this window to the window stack
        windowManager.showWindow(this, WindowManager::ShowBehaviorModal);
        return this->result();
    }
}

void Window::changeEvent(QEvent *ev)
{
    QDialog::changeEvent(ev);
    if (ev->type() == QEvent::WindowTitleChange)
    {
        d_ptr->m_titleLbl->setText(windowTitle());
    }
}

void Window::resizeEvent(QResizeEvent *ev)
{
    QDialog::resizeEvent(ev);
    d_ptr->m_mask->resize(ev->size());
}

void Window::keyPressEvent(QKeyEvent *ev)
{
    QDialog::keyPressEvent(ev);
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Right:
        break;
    default:
        QDialog::keyPressEvent(ev);
        break;
    }
}

void Window::keyReleaseEvent(QKeyEvent *ev)
{
    QDialog::keyReleaseEvent(ev);
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        focusPreviousChild();
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        focusNextChild();
        break;
    default:
        QDialog::keyReleaseEvent(ev);
        break;
    }
}

void Window::hideEvent(QHideEvent *ev)
{
    QDialog::hideEvent(ev);
    emit windowHide(this);
}

void Window::paintEvent(QPaintEvent *ev)
{
#ifdef Q_WS_QWS
    Q_UNUSED(ev);
    QRect r = contentsRect();
    QMargins m = contentsMargins();
    QPainter p(this);
    p.fillRect(r, palette().background());
    if (!m.isNull())
    {
        QRect shadowRect(0, 0, m.left(), m.top()); // left top
        QPixmap pm = themeManger.getShadowElement(ThemeManager::ShadowElementLeftTop, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // top
        shadowRect.setLeft(r.left());
        shadowRect.setRight(r.right());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementTop, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // right top
        shadowRect.setLeft(r.right() + 1);
        shadowRect.setWidth(m.right());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementRightTop, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // right
        shadowRect.setTop(r.top());
        shadowRect.setBottom(r.bottom());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementRight, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // right bottom
        shadowRect.setTop(r.bottom() + 1);
        shadowRect.setHeight(m.bottom());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementRightBottom, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // bottom
        shadowRect.setLeft(r.left());
        shadowRect.setRight(r.right());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementBottom, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // left bottom
        shadowRect.setLeft(0);
        shadowRect.setWidth(m.left());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementLeftBottom, shadowRect.size());
        p.drawPixmap(shadowRect, pm);

        // left
        shadowRect.setTop(r.top());
        shadowRect.setBottom(r.bottom());
        pm = themeManger.getShadowElement(ThemeManager::ShadowElementLeft, shadowRect.size());
        p.drawPixmap(shadowRect, pm);
    }
#else
    QDialog::paintEvent(ev);
#endif
}
