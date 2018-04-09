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
#include "TrendDataStorageManager.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QStylePainter>
#include <QApplication>
#include <QScrollBar>

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100
#define TABLE_ROW_NR            6
#define TABLE_COL_NR            7
#define TABLE_ITEM_WIDTH        65
#define TABLE_ITEM_HEIGHT       35

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
 * 加载趋势数据
 **********************************************************************************************************************/
void TrendDataWidget::loadTrendData()
{
    QString time;
    QStringList timeTitle;
//    QBrush *middleAlarmColor = new QBrush(Qt::yellow);
//    QBrush *highAlarmColor = new QBrush(Qt::red);
    QTableWidgetItem *item;
    int intervalNum = TrendDataSymbol::covertValue(_timeInterval)/TrendDataSymbol::covertValue(RESOLUTION_RATIO_5_SECOND);
    int col = 0;
    for (int i = _trendDataPack.length() - 1 - intervalNum * _currentMoveCount; i >= 0;
         i = i - intervalNum)
    {
        timeDate.getTime(_trendDataPack.at(i)->time, time, true);
        timeTitle << time;
        for (int j = 0; j < _curDisplayParamRow; j ++)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            if (_displayList.at(j) == SUB_PARAM_NIBP_SYS)
            {
                short nibpSys = _trendDataPack.at(i)->paramData.find(_displayList.at(j)).value();
                short nibpDia = _trendDataPack.at(i)->paramData.find((SubParamID)(_displayList.at(j) + 1)).value();
                short nibpMap = _trendDataPack.at(i)->paramData.find((SubParamID)(_displayList.at(j) + 2)).value();
                QString sysStr = nibpSys == InvData() ?"---":QString::number(nibpSys);
                QString diaStr = nibpDia == InvData() ?"---":QString::number(nibpDia);
                QString mapStr = nibpMap == InvData() ?"---":QString::number(nibpMap);
                QString nibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
                item->setText(nibpStr);
            }
            else if (_displayList.at(j) == SUB_PARAM_ART_MEAN || _displayList.at(j) == SUB_PARAM_PA_MEAN ||
                     _displayList.at(j) == SUB_PARAM_AUXP1_MEAN || _displayList.at(j) == SUB_PARAM_AUXP2_MEAN)
            {
                short ibpSys = _trendDataPack.at(i)->paramData.find((SubParamID)(_displayList.at(j) - 2)).value();
                short ibpDia = _trendDataPack.at(i)->paramData.find((SubParamID)(_displayList.at(j) - 1)).value();
                short ibpMap = _trendDataPack.at(i)->paramData.find(_displayList.at(j)).value();
                QString sysStr = ibpSys == InvData() ?"---":QString::number(ibpSys);
                QString diaStr = ibpDia == InvData() ?"---":QString::number(ibpDia);
                QString mapStr = ibpMap == InvData() ?"---":QString::number(ibpMap);
                QString ibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
                item->setText(ibpStr);
            }
            else if (_displayList.at(j) == SUB_PARAM_ETCO2 || _displayList.at(j) == SUB_PARAM_ETN2O ||
                     _displayList.at(j) == SUB_PARAM_ETAA1 || _displayList.at(j) == SUB_PARAM_ETAA2 ||
                     _displayList.at(j) == SUB_PARAM_ETO2 || _displayList.at(j) == SUB_PARAM_T1)
            {
                short data1 = _trendDataPack.at(i)->paramData.find(_displayList.at(j)).value();
                short data2 = _trendDataPack.at(i)->paramData.find((SubParamID)(_displayList.at(j) + 1)).value();
                QString dataStr1 = data1 == InvData() ?"---":QString::number(data1);
                QString dataStr2 = data2 == InvData() ?"---":QString::number(data2);
                QString dataStr = dataStr1 + "/" + dataStr2;
                item->setText(dataStr);
            }
            else
            {
                short data = _trendDataPack.at(i)->paramData.find(_displayList.at(j)).value();
                QString dataStr = data == InvData() ?"---":QString::number(data);
                item->setText(dataStr);
            }
            table->setItem(j, col, item);
        }
        col ++;
    }

    if (timeTitle.length() < TABLE_COL_NR)
    {
        for (timeTitle.length(); timeTitle.length() < TABLE_COL_NR;)
        {
            timeTitle << "";
        }
    }
    if ((_trendDataPack.length() - TABLE_COL_NR) < 0)
    {
        _hideColumn = 0;
    }
    else
    {
        _hideColumn = _trendDataPack.length() - TABLE_COL_NR;
    }
    table->setHorizontalHeaderLabels(timeTitle);
}

void TrendDataWidget::showEvent(QShowEvent *event)
{
    PopupWidget::showEvent(event);
    _loadTableTitle();
}

/**********************************************************************************************************************
 * 趋势数据设置槽函数
 **********************************************************************************************************************/
void TrendDataWidget::_trendDataSetReleased()
{
    trendDataSetWidget.autoShow();
    hide();
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
    if (_currentMoveCount > 0)
    {
        _currentMoveCount --;
    }
    loadTrendData();
}

/**********************************************************************************************************************
 * 右翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_rightReleased()
{
    if (_currentMoveCount < _hideColumn)
    {
        _currentMoveCount ++;
    }
    loadTrendData();
}

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
TrendDataWidget::TrendDataWidget() : _timeInterval(RESOLUTION_RATIO_5_SECOND), _currentMoveCount(0), _hideColumn(0)
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
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // 左上角的按钮（表头交叉处）
    label = new QLabel(table);
    label->setFixedWidth(ITEM_WIDTH);

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
    loadCurParam();
    _updateHeaderDate();
//    _loadTableTitle();

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
//    loadDemoData();
    _getTrendData();
    loadTrendData();
}

/**************************************************************************************************
 * 载入当前显示参数。
 *************************************************************************************************/
void TrendDataWidget::loadCurParam(int index)
{
    _curList.clear();

    TrendParamList list;

    if (_orderMap.end() != _orderMap.find(PARAM_DUP_ECG))
    {
        list = _orderMap.values(PARAM_DUP_ECG);
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

    if (_orderMap.end() != _orderMap.find(PARAM_TEMP))
    {
        list = _orderMap.values(PARAM_TEMP);
        qSort(list);
        _curList.append(list);
    }

    if ((TrendGroup)index == TREND_GROUP_RESP)
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
    }
    else if ((TrendGroup)index == TREND_GROUP_IBP)
    {
        if (_orderMap.end() != _orderMap.find(PARAM_IBP))
        {
            list = _orderMap.values(PARAM_IBP);
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
    else if ((TrendGroup)index == TREND_GROUP_AG)
    {
        if (_orderMap.end() != _orderMap.find(PARAM_AG))
        {
            list = _orderMap.values(PARAM_AG);
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
 * 设置时间间隔
 **********************************************************************************************************************/
void TrendDataWidget::setTimeInterval(ResolutionRatio flag)
{
    _timeInterval = flag;
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
        case SUB_PARAM_ECG_PVCS:
        case SUB_PARAM_ST_I:
        case SUB_PARAM_ST_II:
        case SUB_PARAM_ST_III:
        case SUB_PARAM_ST_aVR:
        case SUB_PARAM_ST_aVL:
        case SUB_PARAM_ST_aVF:
        case SUB_PARAM_ST_V1:
        case SUB_PARAM_ST_V2:
        case SUB_PARAM_ST_V3:
        case SUB_PARAM_ST_V4:
        case SUB_PARAM_ST_V5:
        case SUB_PARAM_ST_V6:
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
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
        case SUB_PARAM_T2:
        case SUB_PARAM_TD:
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

/**********************************************************************************************************************
 * 获取趋势数据
 **********************************************************************************************************************/
void TrendDataWidget::_getTrendData()
{
    IStorageBackend *backend;
    backend = trendDataStorageManager.backend();
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    TrendDataPackage *pack;
    _trendDataPack.clear();
    for (int i = 0; i < blockNum; i ++)
    {
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = (TrendDataSegment*)data.data();
        pack->time = dataSeg->timestamp;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->paramData.find((SubParamID)dataSeg->values[j].subParamId).value() =
                    dataSeg->values[j].value;
            if (!pack->alarmFlag && dataSeg->values[j].alarmFlag)
            {
                pack->alarmFlag = dataSeg->values[j].alarmFlag;
            }
        }
        _trendDataPack.append(pack);
    }
}
