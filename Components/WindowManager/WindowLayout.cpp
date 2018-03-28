#include <QHBoxLayout>
#include <QVBoxLayout>
#include "WindowLayout.h"
#include "WindowManager.h"
#include "PatientManager.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "IButton.h"
#include "EnglishPanel.h"
#include "KeyBoardPanel.h"
#include "RelievePatientWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"

WindowLayout *WindowLayout::_selfObj = NULL;

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void WindowLayout::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight());

    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("WindowLayout"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;

    _windowWidget = new WindowWidget();
    contentLayout->addWidget(_windowWidget, 0, Qt::AlignHCenter);


    _resetLayout = new IButton(trs("ResetLayout"));
    _resetLayout->setEnableKeyAction(false);
    _resetLayout->setFixedSize(btnWidth, _itemH);
    _resetLayout->setFont(fontManager.textFont(fontSize));
    _resetLayout->setBorderEnabled(true);

    _saveLayout = new IButton(trs("SaveLayout"));
    _saveLayout->setEnableKeyAction(false);
    _saveLayout->setFixedSize(btnWidth, _itemH);
    _saveLayout->setFont(fontManager.textFont(fontSize));
    _saveLayout->setBorderEnabled(true);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_resetLayout);
    hlayout->setSpacing(1);
    hlayout->addWidget(_saveLayout);
    contentLayout->addStretch();
    contentLayout->addLayout(hlayout);
    contentLayout->setSpacing(6);
    contentLayout->addStretch();

    connect(_resetLayout, SIGNAL(realReleased()), this, SLOT(_resetReleased()));
    connect(_saveLayout, SIGNAL(realReleased()), this, SLOT(_saveReleased()));
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void WindowLayout::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void WindowLayout::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            _windowWidget->focusOrder(false);
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            _windowWidget->focusOrder(true);
            focusNextChild();
            break;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 解除病人按键槽函数。
 *************************************************************************************************/
void WindowLayout::_resetReleased()
{

}

/**************************************************************************************************
 * 保存/新病人按键槽函数。
 *************************************************************************************************/
void WindowLayout::_saveReleased()
{

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WindowLayout::WindowLayout() : PopupWidget()
{
    layoutExec();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WindowLayout::~WindowLayout()
{

}
