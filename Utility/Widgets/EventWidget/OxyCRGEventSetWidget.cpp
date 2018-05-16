#include "OxyCRGEventSetWidget.h"
#include "IComboList.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "OxyCRGEventWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#define ITEM_HEIGHT             30

OxyCRGEventSetWidget *OxyCRGEventSetWidget::_selfObj = NULL;

OxyCRGEventSetWidget::~OxyCRGEventSetWidget()
{

}

void OxyCRGEventSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

void OxyCRGEventSetWidget::_trend1Slot(int index)
{
    bool isRR;
    if (index == 0)
    {
        isRR = false;
    }
    else
    {
        isRR = true;
    }
    oxyCRGEventWidget.waveWidgetTrend1(isRR);
}

void OxyCRGEventSetWidget::_compressedSlot(int index)
{
    bool isCO2;
    if (index == 0)
    {
        isCO2 = false;
    }
    else
    {
        isCO2 = true;
    }
    oxyCRGEventWidget.waveWidgetCompressed(isCO2);
}

OxyCRGEventSetWidget::OxyCRGEventSetWidget()
{
    int submenuW = windowManager.getPopMenuWidth();
    int itemW = submenuW - 20;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _trend1 = new IComboList("Trend - 1");
    _trend1->setFont(fontManager.textFont(fontSize));
    _trend1->label->setAlignment(Qt::AlignHCenter);
    _trend1->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _trend1->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    _trend1->addItem("HR");
    _trend1->addItem("HR+RR");
    connect(_trend1, SIGNAL(currentIndexChanged(int)), this, SLOT(_trend1Slot(int)));

    _compressed = new IComboList("Compressed");
    _compressed->setFont(fontManager.textFont(fontSize));
    _compressed->label->setAlignment(Qt::AlignHCenter);
    _compressed->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _compressed->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    _compressed->addItem("Resp Waveform");
    _compressed->addItem("CO2 Waveform");
    connect(_compressed, SIGNAL(currentIndexChanged(int)), this, SLOT(_compressedSlot(int)));

    contentLayout->addWidget(_trend1);
    contentLayout->addWidget(_compressed);
    contentLayout->setSpacing(5);
    contentLayout->addStretch();
}
