#include "PopupWidget.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "IButton.h"
#include "SystemManager.h"
#include <QPalette>
#include <QPainter>
#include <QPen>
#include <QBitmap>
#include "Debug.h"
#include "ColorManager.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void PopupWidget::paintEvent(QPaintEvent */*event*/)
{
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);

    QPainter barPainter(this);
    barPainter.setRenderHint(QPainter::Antialiasing);
    barPainter.setClipPath(clipPath);

    // 绘制边框。
    QPen pen;
    pen.setColor(colorManager.getBarBkColor());
    pen.setWidth(_borderWidth * 2);
    barPainter.setPen(pen);
    barPainter.setBrush(palette().window());
    barPainter.drawRect(rect());

    // 绘制标题栏。
    if (_titleLabel != NULL)
    {
        QRect r = rect();
        r.setBottom(_titleBarHeight);
        barPainter.fillRect(r, colorManager.getBarBkColor());
    }
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void PopupWidget::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (!_fixedDimension)
    {
        // 移到窗口管理器指定的位置。
        QRect r = windowManager.getMenuArea();
        QWidget *activateModalWidget = QApplication::activeModalWidget();
        if(activateModalWidget && activateModalWidget->height() >= r.height() / 2)
        {
            //avoid the popup widget's title bar overlap with other modal widget's title bar
            move(r.x() + (r.width() - width()) / 2, r.y() + (5 * r.height() / 7 - height()));
        }
        else
        {
            move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
        }
    }
}

/**************************************************************************************************
 * 隐藏事件。
 *************************************************************************************************/
void PopupWidget::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
}

/**************************************************************************************************
 * 退出。
 *************************************************************************************************/
void PopupWidget::exit(void)
{
    hide();
}

/**************************************************************************************************
 * 设置边框宽度。
 *************************************************************************************************/
void PopupWidget::setBorderWidth(int width)
{
    _borderWidth = width;
}

/**************************************************************************************************
 * 隐藏退出按钮。
 *************************************************************************************************/
void PopupWidget::hideExitButton(void)
{
    if (_exitButton != NULL)
    {
        _exitButton->setVisible(false);
    }
}

/**************************************************************************************************
 * 退出按钮触发。
 *************************************************************************************************/
void PopupWidget::exitButtonRealsed(void)
{
    exit();
}

/**************************************************************************************************
 * 设置标题栏的提示文字。
 *************************************************************************************************/
void PopupWidget::setTitleBarText(const QString &str)
{
    if (_titleLabel != NULL)
    {
        _titleLabel->setText(str);
    }
}

/**************************************************************************************************
 * 设置标题栏的字体。
 *************************************************************************************************/
void PopupWidget::setTitleBarFont(const QFont &font)
{
    if (_titleLabel != NULL)
    {
        _titleLabel->setFont(font);
    }
}

/**************************************************************************************************
 * 设置关闭按钮焦点。
 *************************************************************************************************/
void PopupWidget::setCloseBtnFocus(void)
{
    if (NULL != _exitButton)
    {
        _exitButton->setFocus();
    }
}

/**************************************************************************************************
 * 设置关闭按钮显示。
 *************************************************************************************************/
void PopupWidget::setCloseBtnEnable(bool enable)
{
    if (NULL != _exitButton)
    {
        _exitButton->setEnabled(enable);
    }
}

/**************************************************************************************************
 * 自动弹出与关闭，如果窗体显示则关闭，如果关闭则显示。
 *************************************************************************************************/
void PopupWidget::autoShow(void)
{
    if (isVisible())
    {
        hide();
    }
    else
    {
        show();
    }
}

/**************************************************************************************************
 * 自动弹出与关闭，如果窗体显示则关闭，如果关闭则显示。
 * 显示在指定的位置。
 *************************************************************************************************/
void PopupWidget::autoShow(int x, int y)
{
    move(x, y);

    _fixedDimension = true;
    autoShow();
}

/**************************************************************************************************
 * 自动弹出与关闭，如果窗体显示则关闭，如果关闭则显示。
 * 显示在指定的位置，以及指定的尺寸。
 *************************************************************************************************/
void PopupWidget::autoShow(int x, int y, int w, int h)
{
    setFixedWidth(w);
    setFixedHeight(h);
    move(x, y);
//    _exitButton->setFocus();
    _fixedDimension = true;
    autoShow();
}

/**************************************************************************************************
 * 设置文本。
 *************************************************************************************************/
void PopupWidget::setCloseBtnTxt(const QString &txt)
{
    if (NULL != _exitButton)
    {
        _exitButton->setText(txt);
    }

}

/**************************************************************************************************
 * 设置pic。
 *************************************************************************************************/
void PopupWidget::setCloseBtnPic(const QImage &pic)
{
    if (NULL != _exitButton)
    {
        _exitButton->setPicture(pic);
    }
}

/**************************************************************************************************
 * 设置background。
 *************************************************************************************************/
void PopupWidget::setCloseBtnColor(QColor color)
{
    if (NULL != _exitButton)
    {
        _exitButton->setBackgroundColor(color);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PopupWidget::PopupWidget(bool isBlank) : QDialog(0, Qt::FramelessWindowHint)
{
    setModal(true);
    _borderWidth = 4;
    _fixedDimension = false;

    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    setPalette(p);
    setAttribute(Qt::WA_NoSystemBackground); //draw background in paintEvent

    QHBoxLayout *headLayout = NULL;
    _titleLabel = NULL;
    _exitButton = NULL;
    if (!isBlank)
    {
        // 标题栏。
        _titleLabel = new QLabel("");
        _titleLabel->setAlignment(Qt::AlignCenter);
        _titleLabel->setFixedHeight(_titleBarHeight);
        _titleLabel->setFont(fontManager.textFont(17));
        _titleLabel->setWordWrap(true);

        // 设置标题栏的颜色。
        p.setColor(QPalette::Foreground, Qt::black);
        _titleLabel->setPalette(p);

        _exitButton = new IButton("X");
        _exitButton->setFixedSize(_titleBarHeight - 1, _titleBarHeight - 1);
        _exitButton->setAlignment(Qt::AlignCenter);
        _exitButton->setFont(fontManager.textFont(15, false));
        _exitButton->setBorderEnabled(false, true);
        connect(_exitButton, SIGNAL(realReleased()), this, SLOT(exitButtonRealsed()));

        // 布局。
        headLayout = new QHBoxLayout();
        headLayout->setContentsMargins(0, 1, 0, 0);
        headLayout->setSpacing(0);
        headLayout->addWidget(_titleLabel, 1);
        headLayout->addWidget(_exitButton, 1,Qt::AlignVCenter);
    }

    contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(_borderWidth, 0, _borderWidth, _borderWidth);
    contentLayout->setSpacing(0);
    if (headLayout != NULL)
    {
        contentLayout->addLayout(headLayout);
    }
    setLayout(contentLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PopupWidget::~PopupWidget()
{

}

