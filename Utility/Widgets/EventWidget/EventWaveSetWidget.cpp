#include "EventWaveSetWidget.h"
#include "IComboList.h"
#include "IButton.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "EventDataSymbol.h"
#include "EventReviewWindow.h"

#define ITEM_HEIGHT         30

EventWaveSetWidget *EventWaveSetWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
EventWaveSetWidget::~EventWaveSetWidget()
{

}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void EventWaveSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void EventWaveSetWidget::_waveGainReleased(int index)
{
    eventReviewWindow.setWaveGain(index);
}

void EventWaveSetWidget::_waveSpeedReleased(int index)
{
    eventReviewWindow.setWaveSpeed(index);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
EventWaveSetWidget::EventWaveSetWidget()
{
    int submenuW = windowManager.getPopMenuWidth();
    int itemW = submenuW - 20;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _gain = new IComboList(trs("Gain"));
    _gain->setFont(fontManager.textFont(fontSize));
    _gain->label->setAlignment(Qt::AlignHCenter);
    _gain->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _gain->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    for (int i = 0; i < ECG_EVENT_GAIN_NR; i ++)
    {
        _gain->combolist->addItem(EventDataSymbol::convert((ECGEventGain)i));
    }
    _gain->combolist->setCurrentIndex(3);
    connect(_gain->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_waveGainReleased(int)));

    _speed = new IComboList(trs("Speed"));
    _speed->setFont(fontManager.textFont(fontSize));
    _speed->label->setAlignment(Qt::AlignHCenter);
    _speed->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _speed->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    _speed->combolist->addItem("6.25mm/s");
    _speed->combolist->addItem("12.5mm/s");
    _speed->combolist->addItem("25mm/s");
    _speed->combolist->addItem("50mm/s");
    _speed->combolist->setCurrentIndex(1);
    connect(_speed->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_waveSpeedReleased(int)));

    contentLayout->addWidget(_gain);
    contentLayout->addWidget(_speed);
    contentLayout->setSpacing(5);
    contentLayout->addStretch();
}
