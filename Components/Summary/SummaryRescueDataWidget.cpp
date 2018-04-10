#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include "SummaryRescueDataWidget.h"
#include "SummaryStorageManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "TimeDate.h"
#include "SummaryRescueRangePrint.h"
#include "ITableWidget.h"
#include "DataStorageDirManager.h"
#include "PrintManager.h"
#include "WindowManager.h"
#include "MenuManager.h"
#include "Debug.h"
#include "ParamInfo.h"
#include "IMessageBox.h"

SummaryRescueDataWidget *SummaryRescueDataWidget::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
SummaryRescueDataWidget::SummaryRescueDataWidget() : RescueDataReview(SUMMARY_RESCUE_DATA)
  ,_totalData(0)
  ,_totalPage(0)
  ,_currentPage(0)
  ,_incidentIndex(0)
  , _file(NULL)
{
    table->setRowCount(_rowNR);
    table->setColumnCount(_colNR);
    table->setFixedHeight(getMaxHeight() - 32);// 固定尺寸。
    table->horizontalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:30px;}");
    table->verticalHeader()->setVisible(false);                        //列首隐藏
    connect(table, SIGNAL(itemEnter(int)), this, SLOT(_updateCheckFlag(int)));

    QStringList hheader;
    QTableWidgetItem *item;
    int itemTimeWidth = (getMaxWidth()) / 7;
    int itemSummaryWidth = itemTimeWidth * 3;
    // 创建Item。
    for (int i = 0; i < _rowNR; i++)
    {
        for (int j = 0; j < _colNR; j++)
        {
            table->setColumnWidth(j, (j < (_colNR - 1)) ? itemSummaryWidth : itemTimeWidth);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, j, item);
        }

        table->setRowHeight(i, _tableItemHeight);
    }
    hheader << trs("Time") << trs("Summary") << QString("");
    table->setHorizontalHeaderLabels(hheader);

    addPrintType(trs("Selected"));
    addPrintType(trs("AllSnapshots"));
    addPrintType(trs("IncidentLog"));
    addPrintType(trs("More"));

    setIncidentVisiable(false);
    setPrintVisiable(false);
    setPrintBtnVisiable(true);
    setBackBtnVisiable(true);
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
SummaryRescueDataWidget::~SummaryRescueDataWidget()
{
    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }

    _checkFlag.clear();
}

/**********************************************************************************************************************
 * 刷新页数。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::_refreshTitlePage(void)
{
    QString str;
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("SelectSnapshotsToPrint"))
                .arg(_currentPage + 1)
                .arg(((_totalPage == 0) ? 1 : _totalPage));
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("SelectSnapshotsToPrint"))
                .arg(_currentPage + 1)
                .arg(((_totalPage == 0) ? 1 : _totalPage));
    }

    setTitleBarText(str);
}

/**************************************************************************************************
 * 更新选择标志。
 *************************************************************************************************/
void SummaryRescueDataWidget::_updateCheckFlag(int index)
{
    index = _totalData - _currentPage * _rowNR - index - 1;
    if (index < 0 || index >= _checkFlag.count())
    {
        return;
    }

    _checkFlag[index] = !_checkFlag[index];
}

/**************************************************************************************************
 * 计算相关的控制成员。
 *************************************************************************************************/
void SummaryRescueDataWidget::_calcInfo(void)
{
    if (NULL == _file)
    {
        _totalData = summaryStorageManager.backend()->getBlockNR();
    }
    else
    {
        _totalData = _file->getBlockNR();
    }

    while (_checkFlag.count() < _totalData)
    {
        _checkFlag.append(false);
    }

    _totalPage = (int) (_totalData / (_rowNR));
    _totalPage += (_totalData % (_rowNR)) ? 1 : 0;    // 计算出总共需要的页数。

    // 计算当前的页数。
    _currentPage = (_currentPage > _totalPage) ? _totalPage : _currentPage;

    _refreshTitlePage();
}

/**************************************************************************************************
 * 载入当前页的数据。
 *************************************************************************************************/
void SummaryRescueDataWidget::_loadData(void)
{
    // 先清除掉当前页的所有数据。
    for (int i = 0; i < _rowNR; i++)
    {
        for (int j = 0; j < _colNR; j++)
        {
            QTableWidgetItem *item = table->item(i, j);
            if (j < (_colNR - 1))
            {
                item->setText(" ");
            }
            else
            {
                table->clearCheck(item);
            }
        }
    }

    // 无数据。
    if(_totalData <= 0)
    {
        return;
    }

    int startIndex = _totalData - _currentPage * _rowNR - 1;   // 起始索引，从最新数据开始。

    SummaryItemDesc item;
    QString timeStr;

    // 读取数据并进行显示。
    for (int i = 0; i < _rowNR; i++)
    {
        bool isOk = false;
        if (NULL == _file)
        {
            isOk = summaryStorageManager.backend()->readBlockData(startIndex, (char *)&item, sizeof(item));
        }
        else
        {
            isOk = _file->readBlockData(startIndex, (char *)&item, sizeof(item));
        }

        if (!isOk)
        {
            break;
        }

        if (item.type < SUMMARY_NR)
        {
            timeDate.getDateTime(item.time, timeStr, true, true);
            table->item(i, 0)->setText(timeStr);
            if(item.type == SUMMARY_PHY_ALARM)
            {
                SummaryPhyAlarm phyAlm;
                if (NULL == _file)
                {
                    summaryStorageManager.backend()->readBlockData(startIndex, (char*) &phyAlm, sizeof(SummaryPhyAlarm));
                }
                else
                {
                    _file->readBlockData(startIndex, (char*) &phyAlm, sizeof(SummaryPhyAlarm));
                }

                QString text = trs(summaryStorageManager.getPhyAlarmMessage((ParamID)phyAlm.curAlarmInfo.paramid,
                                                      phyAlm.curAlarmInfo.alarmType,
                                                      phyAlm.curAlarmInfo.isOneshot));

                table->item(i, 1)->setText(text);
            }
            else if(item.type == SUMMARY_CODE_MAKER)
            {
                SummaryCodeMarker CodeMarker;
                if (NULL == _file)
                {
                    summaryStorageManager.backend()->readBlockData(startIndex, (char*) &CodeMarker, sizeof(SummaryCodeMarker));
                }
                else
                {
                    _file->readBlockData(startIndex, (char*) &CodeMarker, sizeof(SummaryCodeMarker));
                }

                QString text = trs(CodeMarker.selectCodeName);
                table->item(i, 1)->setText(text);
            }
#if 0
            else if(item.type == SUMMARY_NIBP)
            {
                SummaryNIBP nibpSnapshot;
                if(NULL == _file)
                {
                    summaryStorageManager.backend()->readBlockData(startIndex, (char*) &nibpSnapshot, sizeof(SummaryNIBP));
                }
                else
                {
                    _file->readBlockData(startIndex, (char *) &nibpSnapshot, sizeof(SummaryNIBP));
                }
                QString text = QString("%1/%2 (%3)");
                if(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value == InvData())
                {
                    text = text.arg(InvStr()).arg(InvStr()).arg(InvStr());
                }
                else
                {
                    UnitType    unit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
                    UnitType curUnit = nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].type;
                    int mul = nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].mul;
                    if (unit == curUnit)
                    {
                        text = text.arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value))
                                .arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_DIA].value))
                                .arg(QString::number(nibpSnapshot.paramValue[SUB_PARAM_NIBP_MAP].value));
                    }
                    else
                    {
                        text = text.arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].value * 1.0 / mul))
                                .arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_DIA].value * 1.0 / mul))
                                .arg(Unit::convert(curUnit, unit, nibpSnapshot.paramValue[SUB_PARAM_NIBP_MAP].value * 1.0 / mul));
                    }

                }
                text +=  " " + Unit::localeSymbol(nibpSnapshot.paramValue[SUB_PARAM_NIBP_SYS].type);
                table->item(i, 1)->setText(text);
            }
#endif
            else
            {
                table->item(i, 1)->setText(trs(getSummaryName((SummaryType)item.type)));
            }

            if (startIndex < _checkFlag.count())
            {
                if (_checkFlag[startIndex])
                {
                    table->setCheck(table->item(i, 2));
                }
            }
        }

        startIndex -= 1;
        if (startIndex < 0)
        {
            break;
        }
    }
}

/**********************************************************************************************************************
 * 获取打印文件路径。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::getPrintPath(QStringList &list)
{
    SummaryDataPrintType type = getPrintType();

    list.clear();
    switch (type)
    {
        case PRINT_RESCUE_HEAD_AND_SNAPSHOTS:
        case PRINT_SELECTED_SNAPSHOT:
        case PRINT_RESCUE_LOG:
        {
            if(isIncidentVisiable())
            {
                int index = getIncidentIndex();
                list << dataStorageDirManager.getRescueDataDir(index);
            }
            else
            {
                list << dataStorageDirManager.getRescueDataDir(_incidentIndex);
            }
            break;
        }
        default:
            //summaryRescueRangePrint.getPrintPath(list);
            break;
    }
}

/**********************************************************************************************************************
 * 获取打印类型。
 **********************************************************************************************************************/
SummaryDataPrintType SummaryRescueDataWidget::getPrintType()
{
    if (isPrintVisiable())
    {
        SummaryDataPrintType type = (SummaryDataPrintType)getPrintIndex();

        if (type >= PRINT_RESCUE_MORE)
        {
            type = summaryRescueRangePrint.getPrintType();
        }

        return type;
    }
    else
    {
        return PRINT_SELECTED_SNAPSHOT;
    }

}

/***************************************************************************************************
 * setIncidentIndex : set the incident index, the widget will load data according to the index
 **************************************************************************************************/
void SummaryRescueDataWidget::setIncidentIndex(int index)
{
    _incidentIndex = index;
    this->incidentChange(index);
}

/**********************************************************************************************************************
 * 判断是否选择。
 **********************************************************************************************************************/
bool SummaryRescueDataWidget::isChecked(int index)
{
    SummaryDataPrintType type = getPrintType();
    if (PRINT_SELECTED_SNAPSHOT != type)
    {
        return false;
    }

    if (index >= _checkFlag.count())
    {
        return false;
    }

    return _checkFlag[index];
}

/***************************************************************************************************
 * get the check flags of all items
 **************************************************************************************************/
QVector<bool> SummaryRescueDataWidget::getCheckFlags() const
{
    return _checkFlag;
}

/**********************************************************************************************************************
 * 显示事件。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::showEvent(QShowEvent *e)
{
    RescueDataReview::showEvent(e);

    _currentPage = 0;
    _calcInfo();
    _loadData();
}

/**********************************************************************************************************************
 * 隐藏事件。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::hideEvent(QHideEvent *e)
{
    RescueDataReview::hideEvent(e);

    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }

    _checkFlag.clear();
}

/**********************************************************************************************************************
 * 动态更新。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::timeOut()
{
    if (NULL != _file)
    {
        return;
    }

    int total  = summaryStorageManager.backend()->getBlockNR();
    if (total <= 0)
    {
        return;
    }

    if((quint32)_totalData == summaryStorageManager.backend()->getBlockNR())
    {
        return;
    }

    _calcInfo();
    _loadData();
}

void SummaryRescueDataWidget::onPrintBtnRelease()
{
    int i;
    for (i = 0; i < _checkFlag.count(); ++i)
    {
        if (_checkFlag[i])
        {
            printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
            break;
        }
    }

    //no check item
    if(i == _checkFlag.count())
    {
        QString title = trs("PrintSnapshot");
        QString msg = trs("SelectSnapshotsToPrint");
        IMessageBox(title, msg,  QStringList(trs("EnglishYESChineseSURE"))).exec();
    }
}

void SummaryRescueDataWidget::onBackBtnRelease()
{
    this->close();
}

/**********************************************************************************************************************
 * 上下翻页加载数据。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::loadData(PageLoadType pageType)
{
    if (0 == _totalPage)
    {
        return;
    }

    switch (pageType)
    {
        case PAGE_UP:
        {
            if (_totalData <= _rowNR)
            {
                return;
            }

            if (0 < _currentPage)
            {
                --_currentPage;
            }
            else
            {
                return;
            }
            break;
        }
        case PAGE_DOWN:
        {
            if (_totalData <= _rowNR)
            {
                return;
            }

            if (_currentPage < (_totalPage - 1))
            {
                ++_currentPage;
            }
            else
            {
                return;
            }
            break;
        }
        default:
            return;
    }

    _refreshTitlePage();
    _loadData();
}

/**********************************************************************************************************************
 * 加载营救事件列表数据。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::loadRescueTime(QStringList &list)
{
    dataStorageDirManager.getRescueEvent(list);
}

/**********************************************************************************************************************
 * 营救时间改变。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::incidentChange(int index)
{
    if (0 == index)
    {
        if (NULL != _file)
        {
            delete _file;
            _file = NULL;
        }
    }
    else
    {
        QStringList list;
        dataStorageDirManager.getRescueEvent(list);

        if (index >= list.count())
        {
            debug("index over range");
            list.clear();
            return;
        }

        QString path = dataStorageDirManager.getRescueDataDir(index,
                                                              list.at(index));
        list.clear();
        if (path.isEmpty())
        {
            return;
        }

        if (NULL == _file)
        {
            _file = StorageManager::open(DATA_STORE_PATH + path + SUMMARY_DATA_FILE_NAME, QIODevice::ReadOnly);
        }
        else
        {
            _file->reload(DATA_STORE_PATH + path + SUMMARY_DATA_FILE_NAME, QIODevice::ReadOnly);
        }

    }

    _checkFlag.clear();
    if(this->isVisible()) {
        _currentPage = 0;
        _calcInfo();
        _loadData();
    }
}

/**********************************************************************************************************************
 * 打印。
 **********************************************************************************************************************/
void SummaryRescueDataWidget::print(int type)
{
    switch ((RescueDataPrintType)type)
    {
        case PRINT_SELECTED_SNAPSHOT:
        {
            int count = _checkFlag.count();
            int i = 0;
            for (; i < count; ++i)
            {
                if (_checkFlag[i])
                {
                    break;
                }
            }

            if (i == count)
            {
                break;
            }
        }
        case PRINT_RESCUE_HEAD_AND_SNAPSHOTS:
        case PRINT_RESCUE_LOG:
            printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
            break;
        default:
        {
            QRect r = windowManager.getMenuArea();
            int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
            int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
            summaryRescueRangePrint.autoShow(x, y);
            break;
        }
    }
}
