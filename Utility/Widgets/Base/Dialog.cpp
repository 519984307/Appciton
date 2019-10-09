/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#include "Dialog.h"
#include <QEvent>
#include <QLabel>
#include "Button.h"
#include <QBoxLayout>
#include <QFrame>
#include <QResizeEvent>
#include "FontManagerInterface.h"
#include "ThemeManager.h"
#include "WindowManagerInterface.h"
#include <QPaintEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>

#define TITLE_BAR_HEIGHT 48

enum ShadowPiece
{
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT
};

class DialogPrivate
{
public:
    DialogPrivate()
        : m_widget(NULL),
          m_titleLbl(NULL),
          m_mask(NULL),
          closeBtn(NULL),
          radialGradient(NULL),
          linearGradient(NULL)
    {}

    QWidget *m_widget;
    QLabel *m_titleLbl;
    QWidget *m_mask;
    Button *closeBtn;
    QRadialGradient *radialGradient;
    QLinearGradient *linearGradient;

    void drawShadow(QPainter &p, QRectF windowRect, QMargins margin, ShadowPiece piece);
};

Dialog::Dialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint),
      d_ptr(new DialogPrivate)
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
    closeBtn->setBorderRadius(themeManger.getBorderRadius());
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

    FontMangerInterface *fontManager = FontMangerInterface::getFontManager();
    if (fontManager)
    {
        setFont(fontManager->textFont(themeManger.defaultFontPixSize()));
    }

    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlWindow, pal);
    setPalette(pal);

    d_ptr->m_mask = new QWidget(this);
    pal = d_ptr->m_mask->palette();
    pal.setColor(QPalette::Window, QColor(20, 20, 20, 80));
    d_ptr->m_mask->setPalette(pal);
    d_ptr->m_mask->setVisible(false);
    d_ptr->m_mask->setAutoFillBackground(true);

    d_ptr->radialGradient = new QRadialGradient();
    d_ptr->radialGradient->setColorAt(0, QColor(0, 0, 0, 255));
    d_ptr->radialGradient->setColorAt(1, QColor(0, 0, 0, 0));
    d_ptr->linearGradient = new QLinearGradient();
    d_ptr->linearGradient->setColorAt(0, QColor(0, 0, 0, 255));
    d_ptr->linearGradient->setColorAt(1, QColor(0, 0, 0, 0));
}

Dialog::~Dialog()
{
    delete d_ptr->radialGradient;
    delete d_ptr->linearGradient;
}

QLayout *Dialog::getWindowLayout()
{
    return d_ptr->m_widget->layout();
}

void Dialog::setWindowLayout(QLayout *layout)
{
    d_ptr->m_widget->setLayout(layout);
}

void Dialog::showMask(bool flag)
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

bool Dialog::isShowingMask() const
{
    return d_ptr->m_mask->isVisible();
}

Button *Dialog::getCloseBtn() const
{
    return d_ptr->closeBtn;
}

int Dialog::exec()
{
    WindowManagerInterface *windowManager = WindowManagerInterface::getWindowManager();
    if (!windowManager)
    {
        return QDialog::exec();
    }
    else if (windowManager->topWindow() == this)
    {
        return QDialog::exec();
    }
    else
    {
        // add this window to the window stack
        windowManager->showWindow(this, WindowManagerInterface::ShowBehaviorModal);
        return this->result();
    }
}

void Dialog::changeEvent(QEvent *ev)
{
    QDialog::changeEvent(ev);
    if (ev->type() == QEvent::WindowTitleChange)
    {
        d_ptr->m_titleLbl->setText(windowTitle());
    }
}

void Dialog::resizeEvent(QResizeEvent *ev)
{
    QDialog::resizeEvent(ev);
    d_ptr->m_mask->resize(ev->size());
}

void Dialog::keyPressEvent(QKeyEvent *ev)
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

void Dialog::keyReleaseEvent(QKeyEvent *ev)
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

void Dialog::hideEvent(QHideEvent *ev)
{
    emit windowHide(this);
    QDialog::hideEvent(ev);
}

void Dialog::paintEvent(QPaintEvent *ev)
{
#ifdef Q_WS_QWS
    Q_UNUSED(ev);
    QRectF r = contentsRect();
    QMargins m = contentsMargins();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // 抗锯齿
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    if (!m.isNull())
    {
        // top left
        d_ptr->drawShadow(p, r, m, TOP_LEFT);

        // top right
        d_ptr->drawShadow(p, r, m, TOP_RIGHT);

        // bottom right
        d_ptr->drawShadow(p, r, m, BOTTOM_RIGHT);

        // bottom left
        d_ptr->drawShadow(p, r, m, BOTTOM_LEFT);

        // top
        d_ptr->drawShadow(p, r, m, TOP);

        // right
        d_ptr->drawShadow(p, r, m, RIGHT);

        // bottom
        d_ptr->drawShadow(p, r, m, BOTTOM);

        // left
        d_ptr->drawShadow(p, r, m, LEFT);
    }

    QPainterPath painterPath;
    painterPath.addRoundedRect(r, themeManger.getBorderRadius(), themeManger.getBorderRadius());
    p.fillPath(painterPath, palette().background());
#else
    QDialog::paintEvent(ev);
#endif
}

int Dialog::getTitleHeight() const
{
    return TITLE_BAR_HEIGHT;
}

void DialogPrivate::drawShadow(QPainter &p, QRectF windowRect, QMargins margin, ShadowPiece piece)
{
    int borderRadius = themeManger.getBorderRadius();
    QRectF shadowRect = windowRect;
    shadowRect.adjust(-margin.left(), -margin.top(), margin.right(), margin.bottom());
    QPainterPath shadowPath;
    shadowPath.addRoundedRect(shadowRect, borderRadius, borderRadius);
    QPainterPath windowPath;
    windowPath.addRoundedRect(windowRect, borderRadius, borderRadius);
    shadowPath = shadowPath.subtracted(windowPath);

    QPainterPath path;
    QPainterPath cutPath;
    QPointF center;

    switch (piece) {
    case TOP_LEFT:
        // 获取区域
        cutPath.addRect(windowRect.left() + borderRadius, shadowRect.top(),
                        shadowRect.width() - margin.left() - borderRadius, margin.top() + borderRadius);
        path = shadowPath.subtracted(cutPath);
        cutPath.addRect(shadowRect.left(), windowRect.top() + borderRadius,
                        shadowRect.width(), shadowRect.height() - margin.top() - borderRadius);
        path = path.subtracted(cutPath);

        // 设置渐变
        center.setX(windowRect.left() + borderRadius);
        center.setY(windowRect.top() + borderRadius);
        radialGradient->setCenter(center);
        radialGradient->setFocalPoint(center);
        radialGradient->setRadius(borderRadius + margin.top());

        // 绘画
        p.fillPath(path, *radialGradient);
        break;

    case TOP_RIGHT:
        // 获取区域
        cutPath.addRect(shadowRect.left(), shadowRect.top(),
                        shadowRect.width() - margin.right() - borderRadius, margin.top() + borderRadius);
        path = shadowPath.subtracted(cutPath);
        cutPath.addRect(shadowRect.left(), windowRect.top() + borderRadius,
                        shadowRect.width(), shadowRect.height() - margin.top() - borderRadius);
        path = path.subtracted(cutPath);

        // 设置渐变
        center.setX(windowRect.right() - borderRadius);
        center.setY(windowRect.top() + borderRadius);
        radialGradient->setCenter(center);
        radialGradient->setFocalPoint(center);
        radialGradient->setRadius(borderRadius + margin.right());

        // 绘画
        p.fillPath(path, *radialGradient);
        break;
    case BOTTOM_RIGHT:
        cutPath.addRect(shadowRect.left(), shadowRect.top(),
                        shadowRect.width(), shadowRect.height() - margin.bottom() - borderRadius);
        path = shadowPath.subtracted(cutPath);
        cutPath.addRect(shadowRect.left(), windowRect.bottom() - borderRadius
                        , shadowRect.width() - margin.right() - borderRadius
                        , shadowRect.height() - margin.bottom() - borderRadius);
        path = path.subtracted(cutPath);

        center.setX(windowRect.right() - borderRadius);
        center.setY(windowRect.bottom() - borderRadius);
        radialGradient->setCenter(center);
        radialGradient->setFocalPoint(center);
        radialGradient->setRadius(borderRadius + margin.bottom());

        p.fillPath(path, *radialGradient);
        break;
    case BOTTOM_LEFT:
        cutPath.addRect(shadowRect.left(), shadowRect.top(),
                        shadowRect.width(), shadowRect.height() - margin.bottom() - borderRadius);
        path = shadowPath.subtracted(cutPath);
        cutPath.addRect(windowRect.left() + borderRadius, windowRect.bottom() - borderRadius
                        , shadowRect.width() - margin.left() - borderRadius
                        , shadowRect.height() - margin.bottom() - borderRadius);
        path = path.subtracted(cutPath);

        center.setX(windowRect.left() + borderRadius);
        center.setY(windowRect.bottom() - borderRadius);
        radialGradient->setCenter(center);
        radialGradient->setFocalPoint(center);
        radialGradient->setRadius(borderRadius + margin.left());

        p.fillPath(path, *radialGradient);
        break;
    case TOP:
        path.addRect(windowRect.left() + borderRadius, shadowRect.top(), windowRect.width() - 2 * borderRadius, margin.top());

        linearGradient->setStart(windowRect.width() / 2 , windowRect.top() + borderRadius);
        linearGradient->setFinalStop(windowRect.width() / 2, shadowRect.top());

        p.fillPath(path, *linearGradient);
        break;
    case RIGHT:
        path.addRect(windowRect.right(), windowRect.top() + borderRadius, margin.right(), windowRect.height() - 2 * borderRadius);

        linearGradient->setStart(windowRect.right() - borderRadius, windowRect.height() / 2);
        linearGradient->setFinalStop(shadowRect.right(), windowRect.height() / 2);

        p.fillPath(path, *linearGradient);
        break;
    case BOTTOM:
        path.addRect(windowRect.left() + borderRadius, windowRect.bottom(), windowRect.width() - 2 * borderRadius, margin.bottom());

        linearGradient->setStart(windowRect.width() / 2 , windowRect.bottom() - borderRadius);
        linearGradient->setFinalStop(windowRect.width() / 2, shadowRect.bottom());

        p.fillPath(path, *linearGradient);
        break;
    case LEFT:
        path.addRect(shadowRect.left(), windowRect.top() + borderRadius, margin.left(), windowRect.height() - 2 * borderRadius);

        linearGradient->setStart(windowRect.left() + borderRadius, windowRect.height() / 2);
        linearGradient->setFinalStop(shadowRect.left(), windowRect.height() / 2);

        p.fillPath(path, *linearGradient);
        break;
    default:
        break;
    }
}
