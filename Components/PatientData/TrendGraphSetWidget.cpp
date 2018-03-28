#include "TrendGraphSetWidget.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "ParamManager.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      80

TrendGraphSetWidget *TrendGraphSetWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendGraphSetWidget::~TrendGraphSetWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void TrendGraphSetWidget::layoutExec()
{
    int subW = 400;
    int subH = 300;
    setFixedSize(subW, subH);

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    QHBoxLayout *hTitleLayout = new QHBoxLayout();

    QLabel *l = new QLabel(trs("Parameter"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("AutoRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("DownRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("UpRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    hTitleLayout->addWidget(l);
    hTitleLayout->setSpacing(10);
    contentLayout->addLayout(hTitleLayout);


    _rulerSetWidget = new IWidget();
    _mScrollArea = new QScrollArea();
    _mScrollArea->setWidgetResizable(true);
    QString name;
    SetItem *item = NULL;
    int count = _itemList.count();
    QVBoxLayout *rulerLayout = new QVBoxLayout();
    _rulerSetWidget->setLayout(rulerLayout);
    for (int i = 0; i < count; i ++)
    {
        item = _itemList.at(i);
        name = paramInfo.getSubParamName(item->sid);

        item->combo->label->setText(name);
        item->combo->label->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->combo->label->setAlignment(Qt::AlignCenter);
        item->combo->combolist->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->combo->addItem(trs("Off"));
        item->combo->addItem(trs("On"));
        item->combo->SetID(i);
        item->combo->setFont(fontManager.textFont(fontSize));
        item->combo->label->setFont(fontManager.textFont(fontSize));

        item->downRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->downRuler->setFont(fontManager.textFont(fontSize));
        item->downRuler->enableCycle(false);
        item->downRuler->setID(i * 2);

        item->upRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->upRuler->setFont(fontManager.textFont(fontSize));
        item->upRuler->enableCycle(false);
        item->upRuler->setID(i * 2);

        rulerLayout->addWidget(item);
    }


    _mScrollArea->setWidget(_rulerSetWidget);
    contentLayout->addWidget(_mScrollArea);
    contentLayout->addStretch();

}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void TrendGraphSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendGraphSetWidget::TrendGraphSetWidget()
{
    SetItem *item = NULL;

    QList<ParamID> pids;
    paramManager.getParams(pids);
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        ParamID id = paramInfo.getParamID((SubParamID)i);
        if (-1 != pids.indexOf(id))
        {
            item = new SetItem(id, (SubParamID)i);
            _itemList.append(item);
        }
    }

    layoutExec();
}
