#include "TrendDataSetWidget.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "TrendDataSymbol.h"
#include "TrendDataWidget.h"
#include "IConfig.h"
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

    QString prefix = "ConfigManager|TrendTable|";
    int index = 0;

    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    _resolutionRatio = new IComboList(trs("ResolutionRatio"));
    _resolutionRatio->setFont(fontManager.textFont(fontSize));
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        _resolutionRatio->addItem(trs(TrendDataSymbol::convert((ResolutionRatio)i)));
    }
    _resolutionRatio->label->setAlignment(Qt::AlignHCenter);
    _resolutionRatio->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _resolutionRatio->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    _resolutionRatio->combolist->setCurrentIndex(index);
    connect(_resolutionRatio->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_timeIntervalReleased(int)));

    index = 0;
    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    _trendGroupList = new IComboList(trs("TrendGroup"));
    _trendGroupList->setFont(fontManager.textFont(fontSize));
    _trendGroupList->label->setAlignment(Qt::AlignHCenter);
    _trendGroupList->label->setFixedSize(labelWidth, ITEM_HEIGHT);
    _trendGroupList->combolist->setFixedSize(btnWidth, ITEM_HEIGHT);
    _trendGroupList->combolist->addItem("Resp");
    _trendGroupList->combolist->addItem("IBP");
    _trendGroupList->combolist->addItem("AG");
    _trendGroupList->combolist->setCurrentIndex(index);
    connect(_trendGroupList->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_trendGroupReleased(int)));

    _yes = new IButton(trs("EnglishYESChineseSURE"));
    _yes->setFixedSize(btnWidth/2, ITEM_HEIGHT);
    _yes->setFont(fontManager.textFont(fontSize));
    _yes->setBorderEnabled(true);
    connect(_yes, SIGNAL(realReleased()), this, SLOT(_yesReleased()));

    _cancel = new IButton(trs("Cancel"));
    _cancel->setFixedSize(btnWidth/2, ITEM_HEIGHT);
    _cancel->setFont(fontManager.textFont(fontSize));
    _cancel->setBorderEnabled(true);
    connect(_cancel, SIGNAL(realReleased()), this, SLOT(_cancelReleased()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_yes);
    hlayout->addWidget(_cancel);

    contentLayout->addWidget(_resolutionRatio);
    contentLayout->addWidget(_trendGroupList);
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
 * 确定槽函数
 **************************************************************************************************/
void TrendDataSetWidget::_yesReleased()
{
    hide();
    trendDataWidget.autoShow();
}

/***************************************************************************************************
 * 取消槽函数
 **************************************************************************************************/
void TrendDataSetWidget::_cancelReleased()
{
    this->hide();
}

/***************************************************************************************************
 * 选择分辨率槽函数
 **************************************************************************************************/
void TrendDataSetWidget::_timeIntervalReleased(int t)
{
    QString prefix = "ConfigManager|TrendTable|ResolutionRatio";
    systemConfig.setNumValue(prefix, t);
    trendDataWidget.setTimeInterval((ResolutionRatio)t);
}

/***************************************************************************************************
 * 选择趋势组槽函数
 **************************************************************************************************/
void TrendDataSetWidget::_trendGroupReleased(int g)
{
    QString prefix = "ConfigManager|TrendTable|TrendGroup";
    systemConfig.setNumValue(prefix, g);
    trendDataWidget.loadCurParam(g);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendDataSetWidget::TrendDataSetWidget()
{
    layoutExec();
}
