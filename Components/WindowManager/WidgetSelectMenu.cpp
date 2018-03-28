#include <QBoxLayout>
#include "WidgetSelectMenu.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "SystemManager.h"
#include "Debug.h"
#include "IConfig.h"

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void WidgetSelectMenu::showEvent(QShowEvent *event)
{
    if (isTopWaveform)
    {
        _removeButton->setEnabled(false);
    }
    else
    {
        _removeButton->setEnabled(true);
    }

    _loadWidget();
    PopupWidget::showEvent(event);
    setCloseBtnFocus();

    QRect r = rect();
    QRect wr = windowManager.geometry();
    if (_y + r.height() > wr.y() + wr.height())
    {
        _y = wr.y() + wr.height() - r.height();
    }

    move(_x, _y);
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void WidgetSelectMenu::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;

        default:
            break;
    }

    PopupWidget::keyPressEvent(event);
}

/**************************************************************************************************
 * 设置被操作的波形控件。
 *************************************************************************************************/
void WidgetSelectMenu::setWidgetName(const QString &name)
{
    widgetName = name;
}

/**************************************************************************************************
 * 设置是否为操作Top Waveform。
 *************************************************************************************************/
void WidgetSelectMenu::setTopWaveform(bool topWaveform)
{
    isTopWaveform = topWaveform;
}

/**************************************************************************************************
 * 是否关联了TopWaveform。
 *************************************************************************************************/
bool WidgetSelectMenu::getTopWaveform() const
{
    return isTopWaveform;
}

/**************************************************************************************************
 * 重新刷新。
 *************************************************************************************************/
void WidgetSelectMenu::refresh(void)
{
    if (!isVisible())
    {
        return;
    }

    _loadWidget();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WidgetSelectMenu::WidgetSelectMenu(const QString &widgetSelectMenuName, bool replaceOnly) : PopupWidget()
{
    isTopWaveform = false;

    setTitleBarText(widgetSelectMenuName);

    int fontSize = fontManager.getFontSize(1);
    setFixedWidth(300);

    _replaceList = new IComboList(trs("Replace"));
    _replaceList->setFont(fontManager.textFont(fontSize));
    _replaceList->setStretch(1, 2);
    connect(_replaceList, SIGNAL(currentIndexChanged(int)), this, SLOT(_replaceListSlot(int)));

    _insertList = new IComboList(trs("Insert"));
    _insertList->setFont(fontManager.textFont(fontSize));
    _insertList->setStretch(1, 2);
    connect(_insertList, SIGNAL(currentIndexChanged(int)), this, SLOT(_insertListSlot(int)));

    QHBoxLayout *hlayout = new QHBoxLayout();
    _removeButton = new IButton(trs("Remove"));
    _removeButton->setBorderEnabled(true);
    _removeButton->setFont(fontManager.textFont(fontSize));
    connect(_removeButton, SIGNAL(realReleased()), this, SLOT(_removeButtonSlot()));
    hlayout->addStretch(100);
    hlayout->addWidget(_removeButton, 190);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 1, 2, 1);
    layout->setSpacing(2);
    layout->addWidget(_replaceList);

    if (!replaceOnly)
    {
        layout->addWidget(_insertList);
        layout->addLayout(hlayout);
    }

    contentLayout->addLayout(layout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WidgetSelectMenu::~WidgetSelectMenu()
{

}
