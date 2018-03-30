#include "TrendGraphSetWidget.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "ParamManager.h"
#include "TrendDataDefine.h"
#include "TrendDataSymbol.h"
#include "TrendGraphWidget.h"
#include <QScrollBar>

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      80

TrendGraphSetWidget *TrendGraphSetWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendGraphSetWidget::~TrendGraphSetWidget()
{

}

TrendGroup TrendGraphSetWidget::getTrendGroup()
{
    return _trendGroup;
}

void TrendGraphSetWidget::setTrendGroup(TrendGroup g)
{
    _trendGroup = g;
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

    QHBoxLayout *hBottomLayout = new QHBoxLayout();

    _allAuto = new IButton(trs("AllAuto"));
    _allAuto->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _allAuto->setFont(fontManager.textFont(fontSize));
    hBottomLayout->addWidget(_allAuto);

    _trendGroupList = new IDropList(trs("TrendGroup"));
    _trendGroupList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _trendGroupList->setFont(fontManager.textFont(fontSize));
    _trendGroupList->addItem("Resp");
    _trendGroupList->addItem("IBP");
    _trendGroupList->addItem("AG");
    hBottomLayout->addWidget(_trendGroupList);
    connect(_trendGroupList, SIGNAL(currentIndexChange(int)), this,
            SLOT(_trendGroupReleased(int)));

    _timeIntervalList = new IDropList(trs("TimeInterval"));
    _timeIntervalList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _timeIntervalList->setFont(fontManager.textFont(fontSize));
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        _timeIntervalList->addItem(TrendDataSymbol::convert((ResolutionRatio)i));
    }
    _timeIntervalList->setCurrentIndex(1);
    hBottomLayout->addWidget(_timeIntervalList);
    connect(_timeIntervalList, SIGNAL(currentIndexChange(int)), this,
            SLOT(_timeIntervalReleased(int)));

    _waveNumberList = new IDropList(trs("WaveNumber"));
    _waveNumberList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _waveNumberList->setFont(fontManager.textFont(fontSize));
    _waveNumberList->addItem("1");
    _waveNumberList->addItem("2");
    _waveNumberList->addItem("3 ");
    _waveNumberList->setCurrentIndex(2);
    hBottomLayout->addWidget(_waveNumberList);
    connect(_waveNumberList, SIGNAL(currentIndexChange(int)), this,
            SLOT(_waveNumberReleased(int)));

    _rulerSetWidget = new IWidget();
    _rulerSetWidget->setFocusPolicy(Qt::NoFocus);
    _mScrollArea = new QScrollArea();
    _mScrollArea->setWidgetResizable(true);
    _mScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mScrollArea->setFocusPolicy(Qt::NoFocus);
    _mScrollArea->setFrameShape(QFrame::NoFrame);
    _mScrollArea->setFixedHeight(ITEM_HEIGHT * 6);

    QString name;
    SetItem *item = NULL;
    int count = _itemList.count();
    _rulerLayout = new QVBoxLayout();
    _rulerLayout->setMargin(0);
    _rulerSetWidget->setLayout(_rulerLayout);
    for (int i = 0; i < count; i ++)
    {
        item = _itemList.at(i);
        name = paramInfo.getSubParamName(item->sid);

        item->combo->setFixedWidth(ITEM_WIDTH * 2 + 10);
        item->combo->setSpacing(10);
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
    }
    _mScrollArea->setWidget(_rulerSetWidget);

    contentLayout->addLayout(hTitleLayout);
    contentLayout->addStretch();
    contentLayout->addWidget(_mScrollArea);
    contentLayout->addStretch();
    contentLayout->addLayout(hBottomLayout);

    _clearRulerLayout();
    _trendRulerLayout();

}

/**************************************************************************************************
 * 主窗口绘图事件
 *************************************************************************************************/
void TrendGraphSetWidget::paintEvent(QPaintEvent *event)
{
    PopupWidget::paintEvent(event);

    QPainter barPainter(this);
    barPainter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));

    barPainter.drawLine(rect().topLeft().x(), ITEM_HEIGHT * 2, rect().topRight().x(), ITEM_HEIGHT * 2);
    barPainter.drawLine(rect().topLeft().x(), height() - (ITEM_HEIGHT + 10), rect().topRight().x(), height() - (ITEM_HEIGHT + 10));
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

void TrendGraphSetWidget::_trendGroupReleased(int)
{

}

void TrendGraphSetWidget::_timeIntervalReleased(int timeInterval)
{
    trendGraphWidget.timeIntervalChange(timeInterval);
}

void TrendGraphSetWidget::_waveNumberReleased(int num)
{
    trendGraphWidget.waveNumberChange(num + 1);
}

/**************************************************************************************************
 * 添加标尺控件
 *************************************************************************************************/
void TrendGraphSetWidget::_trendRulerLayout()
{
    SetItem *item = NULL;
    int count = _itemList.count();
    for (int i = 0; i < count; i ++)
    {
        item = _itemList.at(i);
        if (_trendGroup == TREND_GROUP_RESP)
        {
            switch(item->sid)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_RR_BR:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:
                break;
            default:
                continue;
            }
        }
        _rulerLayout->addWidget(item);
    }
}

/**************************************************************************************************
 * 清除标尺控件
 *************************************************************************************************/
void TrendGraphSetWidget::_clearRulerLayout()
{
    int count = _rulerLayout->count();
    for (int i = 0; i < count; i ++)
    {
        QLayoutItem *item = _rulerLayout->takeAt(0);
        SetItem *widget = (SetItem *)item->widget();
        if (widget != NULL)
        {
            widget->setVisible(false);
            widget->setParent(NULL);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendGraphSetWidget::TrendGraphSetWidget() :  _trendGroup(TREND_GROUP_RESP)
{
    SetItem *item = NULL;

    QList<ParamID> pids;
    paramManager.getParams(pids);
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        switch (i)
        {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_SPO2:
        case SUB_PARAM_RR_BR:
        case SUB_PARAM_NIBP_SYS:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_CVP_MEAN:
        case SUB_PARAM_LAP_MEAN:
        case SUB_PARAM_RAP_MEAN:
        case SUB_PARAM_ICP_MEAN:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
        case SUB_PARAM_CO_CO:
        case SUB_PARAM_CO_CI:
        case SUB_PARAM_CO_TB:
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
        case SUB_PARAM_T1:
            break;
        default:
            continue;
        }

        ParamID id = paramInfo.getParamID((SubParamID)i);
        if (-1 != pids.indexOf(id))
        {
            item = new SetItem(id, (SubParamID)i);
            _itemList.append(item);
        }
    }

    layoutExec();
}
