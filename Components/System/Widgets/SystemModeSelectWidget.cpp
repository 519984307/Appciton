#include "SystemModeSelectWidget.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include <QBoxLayout>
#include "Debug.h"
#include "LayoutManager.h"

/**************************************************************************************************
 * 处理新病人类型。
 *************************************************************************************************/
void SystemModeSelectWidget::_typeChanged(UserFaceType t)
{
    layoutManager.setUFaceType(t);
}

/**************************************************************************************************
 * 鼠标点击IListWidget的item槽。
 *************************************************************************************************/
void SystemModeSelectWidget::_listItemClicked(QListWidgetItem *)
{
    _typeChanged((UserFaceType)_list->currentRow());
    hide();
}

/**************************************************************************************************
 * enter按键释放IListWidget的item槽。
 *************************************************************************************************/
void SystemModeSelectWidget::_listReleased()
{
    _typeChanged((UserFaceType)_list->currentRow());
    hide();
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void SystemModeSelectWidget::showEvent(QShowEvent *event)
{
    PopupWidget::showEvent(event);
    _list->setCurrentRow(layoutManager.getUFaceType());
    QListWidgetItem *item = _list->item(UFACE_MONITOR_STANDARD);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemModeSelectWidget::SystemModeSelectWidget() : PopupWidget(true)
{
    setBorderWidth(8);

    _list = new IListWidget();
    _list->setSpacing(2);
    _list->setFrameStyle(QFrame::NoFrame);
    _list->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    connect(_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(_listItemClicked(QListWidgetItem *)));
    connect(_list, SIGNAL(realRelease()), this, SLOT(_listReleased()));

    QListWidgetItem *item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_STANDARD)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_12LEAD)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_OXYCRG)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_TREND)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_BIGFONT)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(SystemSymbol::convert(UFACE_MONITOR_CUSTOM)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    // 添加到布局起。
    contentLayout->setContentsMargins(4, 4, 4, 4);
    contentLayout->addWidget(_list);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemModeSelectWidget::~SystemModeSelectWidget()
{

}
