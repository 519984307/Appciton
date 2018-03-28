#include "PatientTypeSelectWidget.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include <QBoxLayout>
#include "Debug.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 处理新病人类型。
 *************************************************************************************************/
void PatientSelectWidget::_typeChanged(PatientType t)
{
    patientManager.setType(t);
}

/**************************************************************************************************
 * 鼠标点击IListWidget的item槽。
 *************************************************************************************************/
void PatientSelectWidget::_listItemClicked(QListWidgetItem *)
{
    _typeChanged((PatientType)_list->currentRow());
    hide();
}

/**************************************************************************************************
 * enter按键释放IListWidget的item槽。
 *************************************************************************************************/
void PatientSelectWidget::_listReleased()
{
    _typeChanged((PatientType)_list->currentRow());
    hide();
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void PatientSelectWidget::showEvent(QShowEvent *event)
{
    PopupWidget::showEvent(event);
    _list->setCurrentRow(patientManager.getType());
    QListWidgetItem *item = _list->item(PATIENT_TYPE_NEO);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientSelectWidget::PatientSelectWidget() : PopupWidget(true)
{
    setBorderWidth(8);

    _list = new IListWidget();
    _list->setSpacing(2);
    _list->setFrameStyle(QFrame::NoFrame);
    _list->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    connect(_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(_listItemClicked(QListWidgetItem *)));
    connect(_list, SIGNAL(realRelease()), this, SLOT(_listReleased()));

    QListWidgetItem *item = new QListWidgetItem(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(PatientSymbol::convert(PATIENT_TYPE_PED)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    item = new QListWidgetItem(trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    item->setTextAlignment(Qt::AlignCenter);
    _list->addItem(item);

    // 添加到布局起。
    contentLayout->setContentsMargins(4, 4, 4, 4);
    contentLayout->addWidget(_list);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientSelectWidget::~PatientSelectWidget()
{

}
