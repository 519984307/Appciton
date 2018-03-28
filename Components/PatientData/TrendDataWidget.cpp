#include "TrendDataWidget.h"
#include "IButton.h"
#include "IDropList.h"
#include "ITableWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamManager.h"
#include "TimeDate.h"
#include "TrendDataSetWidget.h"
#include "TimeManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QStylePainter>
#include <QApplication>
#include <QScrollBar>

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100
#define TABLE_ROW_NR            7
#define TABLE_COL_NR            7
#define TABLE_ITEM_WIDTH        65
#define TABLE_ITEM_HEIGHT       31

TrendDataWidget *TrendDataWidget::_selfObj = NULL;

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
TrendDataWidget::~TrendDataWidget()
{

}

/**********************************************************************************************************************
 * 设置IBP对应标名参数是否可见
 **********************************************************************************************************************/
void TrendDataWidget::isIBPSubParamVisible(IBPPressureName name, bool flag)
{
    _ibpNameMap.find(name).value() = flag;
    _loadTableTitle();
}

/**********************************************************************************************************************
 * 加载demo数据
 **********************************************************************************************************************/
void TrendDataWidget::loadDemoData()
{
    QString timePoint[7] = {"15:12:00", "15:11:30", "15:11:00", "15:10:30",
                           "15:10:00", "15:09:30", "15:09:00"};
    QStringList timeTitle;
    QTableWidgetItem *item;
    QBrush *middleAlarmColor = new QBrush(Qt::yellow);
    QBrush *highAlarmColor = new QBrush(Qt::red);
    for (int i = 0; i < 7; i ++)
    {
        timeTitle << timePoint[i];
    }
    table->setHorizontalHeaderLabels(timeTitle);
//    table->horizontalHeader()->setStyleSheet("QHeaderView::section::hover{background-color:red;}");
    table->horizontalHeaderItem(3)->setBackground(*middleAlarmColor);
    table->horizontalHeaderItem(5)->setBackground(*highAlarmColor);

    for (int i = 0; i < TABLE_COL_NR; i ++)
    {
        for (int j = 0; j < _curDisplayParamRow; j ++)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            if (_displayList.at(j) == SUB_PARAM_NIBP_SYS || _displayList.at(j) == SUB_PARAM_ART_MEAN ||
                    _displayList.at(j) == SUB_PARAM_PA_MEAN ||
                    _displayList.at(j) == SUB_PARAM_AUXP1_MEAN || _displayList.at(j) == SUB_PARAM_AUXP2_MEAN)
            {
                item->setText("120/80/90");
            }
            else
            {
                item->setText("90");
            }
            table->setItem(j, i, item);
        }
    }

}

/**********************************************************************************************************************
 * 趋势数据设置槽函数
 **********************************************************************************************************************/
void TrendDataWidget::_trendDataSetReleased()
{
    trendDataSetWidget.autoShow();
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_upReleased()
{
    int maxValue = table->verticalScrollBar()->maximum();
    _curVScroller = table->verticalScrollBar()->value();

    if(_curVScroller>0)
    {
        table->verticalScrollBar()->setSliderPosition(_curVScroller-(maxValue * TABLE_ROW_NR)/(_curDisplayParamRow - TABLE_ROW_NR));
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_downReleased()
{
    int maxValue = table->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    _curVScroller = table->verticalScrollBar()->value(); //获得当前scroller值

    if(_curVScroller<maxValue)
    {
        table->verticalScrollBar()->setSliderPosition(_curVScroller + (maxValue * TABLE_ROW_NR)/(_curDisplayParamRow - TABLE_ROW_NR));
    }
}

/**********************************************************************************************************************
 * 左翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_leftReleased()
{

}

/**********************************************************************************************************************
 * 右翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_rightReleased()
{

}

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
TrendDataWidget::TrendDataWidget()
{
    _trendParamInit();

    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    table = new ITableWidget();
    table->setColumnCount(TABLE_COL_NR);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFixedHeight(ITEM_HEIGHT * 7 * 2 + table->horizontalHeader()->height());
    table->setFixedWidth(_maxWidth - 10);
    table->horizontalHeader()->setVisible(true);
    table->verticalHeader()->setVisible(true);
    table->verticalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:46px;}");
//    table->horizontalHeader()->setStyleSheet("QHeaderView::section{border:0px;}");
    table->verticalHeader()->setFixedWidth(ITEM_WIDTH);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setSelectionBehavior(QAbstractItemView::SelectColumns);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // 左上角的按钮（表头交叉处）
    label = new QLabel(table);

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    _left = new IButton();
    _left->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _left->setPicture(QImage("/usr/local/nPM/icons/ArrowLeft.png"));
    connect(_left, SIGNAL(realReleased()), this, SLOT(_leftReleased()));

    _right = new IButton();
    _right->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _right->setPicture(QImage("/usr/local/nPM/icons/ArrowRight.png"));
    connect(_right, SIGNAL(realReleased()), this, SLOT(_rightReleased()));

    _incidentMove = new IButton(trs("IncidentMove"));
    _incidentMove->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _incidentMove->setFont(fontManager.textFont(fontSize));

    _incident = new IDropList(trs("Incident"));
    _incident->setFont(fontManager.textFont(fontSize));
    _incident->setFixedHeight(ITEM_HEIGHT);
    _incident->setMinimumWidth(ITEM_WIDTH);
    _incident->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    _printParam = new IDropList(trs("Print"));
    _printParam->setFont(fontManager.textFont(fontSize));
    _printParam->setFixedHeight(ITEM_HEIGHT);
    _printParam->setMinimumWidth(ITEM_WIDTH);
    _printParam->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    _set = new IButton(trs("Set"));
    _set->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _set->setFont(fontManager.textFont(fontSize));
    connect(_set, SIGNAL(realReleased()), this, SLOT(_trendDataSetReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addStretch();
    lineLayout->addWidget(_up);
    lineLayout->addWidget(_down);
    lineLayout->addWidget(_left);
    lineLayout->addWidget(_right);
    lineLayout->addWidget(_incidentMove);
    lineLayout->addWidget(_incident);
    lineLayout->addWidget(_printParam);
    lineLayout->addWidget(_set);
    lineLayout->addStretch();

//    QVBoxLayout *layout = new QVBoxLayout();
//    layout->setMargin(1);
//    layout->setSpacing(1);
//    layout->addWidget(table);
//    layout->addLayout(lineLayout);
    contentLayout->addStretch();
    contentLayout->addWidget(table);
    contentLayout->addStretch();
    contentLayout->addLayout(lineLayout);
//    contentLayout->addLayout(layout);

    setFixedSize(_maxWidth, _maxHeight);

    _curDate.clear();
    _loadCurParam(0);
    _updateHeaderDate();
    _loadTableTitle();

}

/**************************************************************************************************
 * 载入表格表头信息。
 *************************************************************************************************/
void TrendDataWidget::_loadTableTitle()
{
    QStringList hheader;
    QString str("");

    table->clear();
    _displayList.clear();

    label->setText(_curDate);
    str.clear();

    _curDisplayParamRow = 0;
    for (int i = 0; i < _curList.length(); i ++)
    {
        SubParamID id = _curList.at(i);

        if (id  == SUB_PARAM_NIBP_SYS)
        {
            str = trs(paramInfo.getParamName(PARAM_NIBP));
        }
        else if(id == SUB_PARAM_ART_MEAN || id == SUB_PARAM_PA_MEAN ||
                id == SUB_PARAM_CVP_MEAN || id == SUB_PARAM_LAP_MEAN ||
                id == SUB_PARAM_RAP_MEAN || id == SUB_PARAM_ICP_MEAN ||
                id == SUB_PARAM_AUXP1_MEAN || id == SUB_PARAM_AUXP2_MEAN)
        {
            if (_ibpNameMap.find(_ibpSubParamMap.find(id).value()).value())
            {
                str = paramInfo.getSubParamName(id);
                str = str.left(str.length()-5);
            }
            else
            {
                continue;
            }
        }
        else
        {
            str = trs(paramInfo.getSubParamName(id));
        }
        str += '\n';
        str += "(";
        str += Unit::localeSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(id), id));
        str += ")";
        _curDisplayParamRow ++;
        hheader << str;
        _displayList.append(id);

    }

    table->setRowCount(_curDisplayParamRow);
    for (int i = 0; i < _curDisplayParamRow; i++)
    {
        table->setRowHeight(i, TABLE_ITEM_HEIGHT * 2);
    }
    table->setVerticalHeaderLabels(hheader);

    // 加载demo数据
    loadDemoData();
}

/**************************************************************************************************
 * 载入当前显示参数。
 *************************************************************************************************/
void TrendDataWidget::_loadCurParam(int index)
{
    _curList.clear();

    TrendParamList list;
    if (_orderMap.end() != _orderMap.find(PARAM_ECG))
    {
        list = _orderMap.values(PARAM_ECG);
        qSort(list);
        _curList.append(list);
    }

    if (_orderMap.end() != _orderMap.find(PARAM_SPO2))
    {
        list = _orderMap.values(PARAM_SPO2);
        qSort(list);
        _curList.append(list);
    }

    if (_orderMap.end() != _orderMap.find(PARAM_NIBP))
    {
        list = _orderMap.values(PARAM_NIBP);
        qSort(list);
        _curList.append(list);
    }

    if (index == 0)
    {
        if (_orderMap.end() != _orderMap.find(PARAM_DUP_RESP))
        {
            list = _orderMap.values(PARAM_DUP_RESP);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.end() != _orderMap.find(PARAM_CO2))
        {
            list = _orderMap.values(PARAM_CO2);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.end() != _orderMap.find(PARAM_IBP))
        {
            list = _orderMap.values(PARAM_IBP);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.end() != _orderMap.find(PARAM_AG))
        {
            list = _orderMap.values(PARAM_AG);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.end() != _orderMap.find(PARAM_CO))
        {
            list = _orderMap.values(PARAM_CO);
            qSort(list);
            _curList.append(list);
        }
    }
    else
    {
        if (_orderMap.end() != _orderMap.find(PARAM_TEMP))
        {
            list = _orderMap.values(PARAM_TEMP);
            qSort(list);
            _curList.append(list);
        }
    }

    if (_curList.count() >= (TABLE_ROW_NR - 1))
    {
        return;
    }
}

/**********************************************************************************************************************
 * 趋势参数初始化。
 **********************************************************************************************************************/
void TrendDataWidget::_trendParamInit()
{
    _ibpNameMap.clear();

    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _ibpNameMap.insert((IBPPressureName)i, false);
    }

    _ibpSubParamMap.clear();

    _ibpSubParamMap.insert(SUB_PARAM_ART_MEAN, IBP_PRESSURE_ART);
    _ibpSubParamMap.insert(SUB_PARAM_PA_MEAN, IBP_PRESSURE_PA);
    _ibpSubParamMap.insert(SUB_PARAM_CVP_MEAN, IBP_PRESSURE_CVP);
    _ibpSubParamMap.insert(SUB_PARAM_LAP_MEAN, IBP_PRESSURE_LAP);
    _ibpSubParamMap.insert(SUB_PARAM_RAP_MEAN, IBP_PRESSURE_RAP);
    _ibpSubParamMap.insert(SUB_PARAM_ICP_MEAN, IBP_PRESSURE_ICP);
    _ibpSubParamMap.insert(SUB_PARAM_AUXP1_MEAN, IBP_PRESSURE_AUXP1);
    _ibpSubParamMap.insert(SUB_PARAM_AUXP2_MEAN, IBP_PRESSURE_AUXP2);

    _orderMap.clear();

    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);
    qSort(paramIDList);

    int count = 0;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        ParamID paramID = paramInfo.getParamID((SubParamID)i);
        if (-1 == paramIDList.indexOf(paramID))
        {
            continue;
        }

        switch (i)
        {
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_ART_PR:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_PA_PR:
        case SUB_PARAM_CVP_PR:
        case SUB_PARAM_LAP_PR:
        case SUB_PARAM_RAP_PR:
        case SUB_PARAM_ICP_PR:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP1_PR:
        case SUB_PARAM_AUXP2_DIA:
        case SUB_PARAM_AUXP2_SYS:
        case SUB_PARAM_AUXP2_PR:
            break;
        default:
        {
            ++ count;
            _orderMap.insert(paramID, (SubParamID)i);
            break;
        }
        }
    }
}

/**********************************************************************************************************************
 * 当前日期。
 **********************************************************************************************************************/
void TrendDataWidget::_updateHeaderDate(unsigned t)
{
    if (0 == t)
    {
//        int index = getIncidentIndex();
//        if(index <= 0)
//        {
            t = timeManager.getCurTime();
//        }
//        else
//        {
//            QStringList datatimeList;
//            loadRescueTime(datatimeList);
//            if(index < datatimeList.size())
//            {
//                QString str = datatimeList.at(index);
//                QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
//                t = dt.toTime_t();
//            }
//            else
//            {
//                t = timeManager.getCurTime();
//            }
//        }
    }

    QString updateDate;
    timeDate.getDate(t, updateDate, true);

    if (_curDate != updateDate)
    {
        QTableWidgetItem *item = table->verticalHeaderItem(0);
        if (NULL != item)
        {
            item->setText(updateDate);
        }

        _curDate = updateDate;
    }
}
