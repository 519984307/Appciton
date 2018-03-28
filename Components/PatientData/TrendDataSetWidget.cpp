#include "TrendDataSetWidget.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "TrendDataSymbol.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#define ITEM_HEIGHT         30

TrendDataSetWidget *TrendDataSetWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendDataSetWidget::~TrendDataSetWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void TrendDataSetWidget::layoutExec()
{
    int submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("TrendTableSet"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _resolutionRatio = new IComboList(trs("ResolutionRatio"));
    _resolutionRatio->setFont(fontManager.textFont(fontSize));
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        _resolutionRatio->addItem(trs(TrendDataSymbol::convert((ResolutionRatio)i)));
    }
    _resolutionRatio->label->setAlignment(Qt::AlignHCenter);
    _resolutionRatio->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _resolutionRatio->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);

    _trendGroup = new IComboList(trs("TrendGroup"));
    _trendGroup->setFont(fontManager.textFont(fontSize));
    _trendGroup->label->setAlignment(Qt::AlignHCenter);
    _trendGroup->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _trendGroup->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);

    _yes = new IButton(trs("EnglishYESChineseSURE"));
    _yes->setFixedSize(btnWidth/2, ITEM_HEIGHT);
    _yes->setFont(fontManager.textFont(fontSize));
    _yes->setBorderEnabled(true);

    _cancel = new IButton(trs("Cancel"));
    _cancel->setFixedSize(btnWidth/2, ITEM_HEIGHT);
    _cancel->setFont(fontManager.textFont(fontSize));
    _cancel->setBorderEnabled(true);
    connect(_cancel, SIGNAL(realReleased()), this, SLOT(_cancelReleased()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_yes);
    hlayout->addWidget(_cancel);

    contentLayout->addWidget(_resolutionRatio);
    contentLayout->addWidget(_trendGroup);
    contentLayout->addLayout(hlayout);
    contentLayout->setSpacing(3);

}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void TrendDataSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

/***************************************************************************************************
 * 取消槽函数
 **************************************************************************************************/
void TrendDataSetWidget::_cancelReleased()
{
    this->hide();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendDataSetWidget::TrendDataSetWidget()
{
    layoutExec();
}
