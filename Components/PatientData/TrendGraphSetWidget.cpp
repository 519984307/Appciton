#include "TrendGraphSetWidget.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "ParamManager.h"
#include "TrendDataDefine.h"
#include "TrendDataSymbol.h"
#include "TrendGraphWidget.h"
#include "IBPParam.h"
#include "AlarmConfig.h"
#include "IConfig.h"
#include <QScrollBar>
#include "LayoutManager.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      80

TrendGraphSetWidget *TrendGraphSetWidget::_selfObj = NULL;

TrendGraphSetWidget::~TrendGraphSetWidget()
{

}

TrendGroup TrendGraphSetWidget::getTrendGroup()
{
    return _trendGroup;
}

void TrendGraphSetWidget::layoutExec()
{
    int subW = 400;
    int subH = 300;
    setFixedSize(subW, subH);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);
    setFont(font);

    QHBoxLayout *hTitleLayout = new QHBoxLayout();

    QLabel *l = new QLabel(trs("Parameter"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(font);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("AutoRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(font);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("DownRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(font);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("UpRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(font);
    hTitleLayout->addWidget(l);
    hTitleLayout->setSpacing(10);

    QHBoxLayout *hBottomLayout = new QHBoxLayout();

    _allAuto = new IButton(trs("AllAuto"));
    _allAuto->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _allAuto->setFont(font);
    hBottomLayout->addWidget(_allAuto);
    connect(_allAuto, SIGNAL(realReleased()), this, SLOT(_allAutoReleased()));

    QString prefix = "TrendGraph|";
    int index = 0;

    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    _trendGroupList = new IDropList(trs("TrendGroup"));
    _trendGroupList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _trendGroupList->setFont(font);
    _trendGroupList->addItem("Resp");
    _trendGroupList->addItem("IBP");
    _trendGroupList->addItem("AG");
    _trendGroupList->setCurrentIndex(index);
    _trendGroup = (TrendGroup)index;
    hBottomLayout->addWidget(_trendGroupList);
    connect(_trendGroupList, SIGNAL(currentIndexChange(int)), this,
            SLOT(_trendGroupReleased(int)));

    index = 0;
    QString intervalPrefix = prefix + "TimeInterval";
    systemConfig.getNumValue(intervalPrefix, index);
    _timeIntervalList = new IDropList(trs("TimeInterval"));
    _timeIntervalList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _timeIntervalList->setFont(font);
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        _timeIntervalList->addItem(TrendDataSymbol::convert((ResolutionRatio)i));
    }
    _timeIntervalList->setCurrentIndex(index);
    hBottomLayout->addWidget(_timeIntervalList);
    connect(_timeIntervalList, SIGNAL(currentIndexChange(int)), this,
            SLOT(_timeIntervalReleased(int)));

    index = 0;
    QString numPrefix = prefix + "WaveNumber";
    systemConfig.getNumValue(numPrefix, index);
    _waveNumberList = new IDropList(trs("WaveNumber"));
    _waveNumberList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _waveNumberList->setFont(font);
    _waveNumberList->addItem("1");
    _waveNumberList->addItem("2");
    _waveNumberList->addItem("3");
    _waveNumberList->setCurrentIndex(index);
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
    SetRulerItem *item = NULL;
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
        item->combo->setFont(font);
        item->combo->label->setFont(font);
        connect(item->combo, SIGNAL(currentIndexChanged(int, int)),
                this, SLOT(_comboListIndexChanged(int, int)));

        item->downRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->downRuler->setFont(font);
        item->downRuler->enableCycle(false);
        item->downRuler->setID(i * 2);
        connect(item->downRuler, SIGNAL(valueChange(QString,int)),
                this, SLOT(_upDownRulerChange(QString,int)));

        item->upRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->upRuler->setFont(font);
        item->upRuler->enableCycle(false);
        item->upRuler->setID(i * 2 + 1);
        connect(item->upRuler, SIGNAL(valueChange(QString,int)),
                this, SLOT(_upDownRulerChange(QString,int)));
    }
    _loadOptions();
    _mScrollArea->setWidget(_rulerSetWidget);

    contentLayout->addLayout(hTitleLayout);
    contentLayout->addStretch();
    contentLayout->addWidget(_mScrollArea);
    contentLayout->addStretch();
    contentLayout->addLayout(hBottomLayout);

    upDateTrendGroup();

}

void TrendGraphSetWidget::paintEvent(QPaintEvent *event)
{
    PopupWidget::paintEvent(event);

    QPainter barPainter(this);
    barPainter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));

    barPainter.drawLine(rect().topLeft().x(), ITEM_HEIGHT * 2, rect().topRight().x(), ITEM_HEIGHT * 2);
    barPainter.drawLine(rect().topLeft().x(), height() - (ITEM_HEIGHT + 10), rect().topRight().x(), height() - (ITEM_HEIGHT + 10));
}

void TrendGraphSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = layoutManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

void TrendGraphSetWidget::_allAutoReleased()
{
    SetRulerItem *item = NULL;
    int count = _itemList.count();
    for (int i = 0; i < count; i ++)
    {
        item = _itemList.at(i);
        item->combo->setCurrentIndex(1);
        item->downRuler->setEnabled(false);
        item->downRuler->disable(true);
        item->upRuler->setEnabled(false);
        item->upRuler->disable(true);
        SubParamID subID = item->sid;
        ParamID id = item->pid;
        UnitType type = paramManager.getSubParamUnit(id, subID);
        ParamRulerConfig config = alarmConfig.getParamRulerConfig(subID, type);
        item->downRuler->setEnabled(false);
        item->downRuler->disable(true);
        item->upRuler->setEnabled(false);
        item->upRuler->disable(true);
        if (config.scale == 1)
        {
            item->downRuler->setValue(config.downRuler);
            item->upRuler->setValue(config.upRuler);
            trendGraphWidget.setSubWidgetRulerLimit(subID, item->downRuler->getText().toInt(),
                                                    item->upRuler->getText().toInt());
        }
        else
        {
            item->downRuler->setValue((double)config.downRuler / config.scale);
            item->upRuler->setValue((double)config.upRuler / config.scale);
            trendGraphWidget.setSubWidgetRulerLimit(subID, (int)item->downRuler->getText().toDouble(),
                                                    (int)item->upRuler->getText().toDouble());
        }
    }
}

void TrendGraphSetWidget::_trendGroupReleased(int g)
{
    QString prefix = "TrendGraph|TrendGroup";
    systemConfig.setNumValue(prefix, g);
    _trendGroup = (TrendGroup)g;
    upDateTrendGroup();
    trendGraphWidget.updateTrendGraph();
}

void TrendGraphSetWidget::_timeIntervalReleased(int timeInterval)
{
    QString prefix = "TrendGraph|TimeInterval";
    systemConfig.setNumValue(prefix, timeInterval);
    trendGraphWidget.timeIntervalChange(timeInterval);
}

void TrendGraphSetWidget::_waveNumberReleased(int num)
{
    QString prefix = "TrendGraph|WaveNumber";
    systemConfig.setNumValue(prefix, num);
    trendGraphWidget.waveNumberChange(num + 1);
}

void TrendGraphSetWidget::_comboListIndexChanged(int id, int index)
{
    if (id >= _itemList.count())
    {
        return;
    }
    SetRulerItem *item = _itemList.at(id);

    if (index == 0)
    {
        item->downRuler->setEnabled(true);
        item->downRuler->disable(false);
        item->upRuler->setEnabled(true);
        item->upRuler->disable(false);
    }
    else
    {
        SubParamID subID = item->sid;
        ParamID id = item->pid;
        UnitType type = paramManager.getSubParamUnit(id, subID);
        ParamRulerConfig config = alarmConfig.getParamRulerConfig(subID, type);
        item->downRuler->setEnabled(false);
        item->downRuler->disable(true);
        item->upRuler->setEnabled(false);
        item->upRuler->disable(true);
        if (config.scale == 1)
        {
            item->downRuler->setValue(config.downRuler);
            item->upRuler->setValue(config.upRuler);
            trendGraphWidget.setSubWidgetRulerLimit(subID, item->downRuler->getText().toInt(),
                                                    item->upRuler->getText().toInt());
        }
        else
        {
            item->downRuler->setValue((double)config.downRuler / config.scale);
            item->upRuler->setValue((double)config.upRuler / config.scale);
            trendGraphWidget.setSubWidgetRulerLimit(subID, (int)item->downRuler->getText().toDouble(),
                                                    (int)item->upRuler->getText().toDouble());
        }
    }
}

void TrendGraphSetWidget::_upDownRulerChange(QString, int id)
{
    int curID = id / 2;
    if (curID >= _itemList.count())
    {
        return;
    }

    SetRulerItem *item = _itemList.at(curID);
    trendGraphWidget.setSubWidgetRulerLimit(item->sid, item->downRuler->getText().toInt(),
                                            item->upRuler->getText().toInt());
}

void TrendGraphSetWidget::upDateTrendGroup()
{
    _clearRulerLayout();
    _trendRulerLayout();
}

void TrendGraphSetWidget::_trendRulerLayout()
{
    SetRulerItem *item = NULL;
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
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:
                break;
            default:
                continue;
            }
        }
        else if (_trendGroup == TREND_GROUP_IBP)
        {
            SubParamID ibp1;
            SubParamID ibp2;
            ibpParam.getSubParamID(ibp1, ibp2);
            if (item->sid != ibp1 && item->sid != ibp2 &&
                    item->sid != SUB_PARAM_HR_PR && item->sid != SUB_PARAM_SPO2 &&
                    item->sid != SUB_PARAM_NIBP_MAP && item->sid != SUB_PARAM_T1)
            {
                continue;
            }
        }
        else if (_trendGroup == TREND_GROUP_AG)
        {
            switch(item->sid)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_T1:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
                break;
            default:
                continue;
            }
        }
        _rulerLayout->addWidget(item);
        item->setVisible(true);
    }
}

void TrendGraphSetWidget::_clearRulerLayout()
{
    int count = _rulerLayout->count();
    for (int i = 0; i < count; i ++)
    {
        QLayoutItem *item = _rulerLayout->takeAt(0);
        SetRulerItem *widget = qobject_cast<SetRulerItem *>(item->widget());
        if (widget != NULL)
        {
            widget->setVisible(false);
        }
    }
}

void TrendGraphSetWidget::_loadOptions()
{
    SetRulerItem *item;
    int count = _itemList.count();
    for (int i = 0; i < count; i++)
    {
        item = _itemList.at(i);
        if (NULL == item)
        {
            continue;
        }

        SubParamID subID = item->sid;
        ParamID id = item->pid;
        UnitType type = paramManager.getSubParamUnit(id, subID);
        ParamRulerConfig config = alarmConfig.getParamRulerConfig(subID, type);

        item->combo->setCurrentIndex(1);
        item->downRuler->setEnabled(false);
        item->downRuler->disable(true);
        item->upRuler->setEnabled(false);
        item->upRuler->disable(true);

        if(config.scale == 1)
        {
            item->downRuler->setMode(ISPIN_MODE_INT);
            item->downRuler->setRange(config.minDownRuler, config.upRuler - 1);
            item->downRuler->setValue(config.downRuler);
            item->downRuler->setStep(1);

            item->upRuler->setMode(ISPIN_MODE_INT);
            item->upRuler->setRange(config.downRuler + 1, config.maxUpRuler);
            item->upRuler->setValue(config.upRuler);
            item->upRuler->setStep(1);

        }
        else
        {
            int fStepValue, fLowMinValue, fLowMaxValue, fHighMinValue, fHighMaxValue;
            fStepValue = 1;
            fLowMinValue = config.minDownRuler / config.scale;
            fLowMaxValue = (config.upRuler - 1) / config.scale;
            fHighMinValue = (config.downRuler + 1) / config.scale;
            fHighMaxValue = config.maxUpRuler / config.scale;

            item->downRuler->setMode(ISPIN_MODE_INT);
            item->downRuler->setRange(fLowMinValue, fLowMaxValue);
            item->downRuler->setValue(config.downRuler / config.scale);
            item->downRuler->setStep(fStepValue);

            item->upRuler->setMode(ISPIN_MODE_INT);
            item->upRuler->setRange(fHighMinValue, fHighMaxValue);
            item->upRuler->setValue(config.upRuler / config.scale);
            item->upRuler->setStep(fStepValue);
        }
    }
}

TrendGraphSetWidget::TrendGraphSetWidget() : _mScrollArea(NULL), _rulerSetWidget(NULL),
    _allAuto(NULL), _trendGroupList(NULL), _timeIntervalList(NULL),
    _waveNumberList(NULL), _trendGroup(TREND_GROUP_RESP), _rulerLayout(NULL)
{
    SetRulerItem *item = NULL;

    QList<ParamID> pids;
    paramManager.getParams(pids);
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        switch (i)
        {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_SPO2:
        case SUB_PARAM_RR_BR:
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP2_MAP:
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
            item = new SetRulerItem(id, (SubParamID)i);
            _itemList.append(item);
        }
    }

    layoutExec();
}
