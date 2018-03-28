#include <QHeaderView>
#include "ECG12LSnapShotReview.h"
#include "ITableWidget.h"
#include "LanguageManager.h"
#include "SystemManager.h"
#include "NetworkManager.h"
#include "ECG12LDataStorage.h"
#include "TimeDate.h"
#include "DataStorageDirManager.h"
#include "Debug.h"
#include "ECG12LDataPrint.h"
#include "IMessageBox.h"
#include "PrintManager.h"

ECG12LSnapShotReview *ECG12LSnapShotReview::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
ECG12LSnapShotReview::ECG12LSnapShotReview() : RescueDataReview(ECG12L_RESCUE_DATA)
    ,_totalData(0)
    ,_totalPage(0)
    ,_currentPage(0)
    , _file(NULL)
{
    //传输类型可见
    settransferTypeVisiable(TRUE);

    table->setRowCount(_rowNR);
    table->setColumnCount(_colNR);
    table->setFixedHeight(getMaxHeight() - 32);
    table->horizontalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:30px;}");
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);        // 不能编辑。
    table->verticalHeader()->setVisible(false);                        //列首隐藏
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(table, SIGNAL(itemEnter(int)), this, SLOT(_updateCheckFlag(int)));


    QStringList hheader;
    QTableWidgetItem *item;
    int itemTimeWidth = getMaxWidth() / 7;
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
    hheader << trs("Time") << trs("PatientID") << QString(" ");
    table->setHorizontalHeaderLabels(hheader);

    addPrintType(trs("CurrentIncident"));
    addPrintType(trs("Selected"));
    addPrintType("");

    addTransfer(trs("UsbCurrentRescue"));
    addTransfer(trs("UsbCurrentReport"));
    /* 当且仅当系统支持WIFI切WIFI功能开启时，才支持E-mail传输 */
    if(systemManager.isSupport(CONFIG_WIFI) && NetworkManager::isWifiTurnOn())
    {
        addTransfer(trs("MailSelectReport"));
    }
    addTransfer("");

    _snapShotTimeInedx = 0;
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
ECG12LSnapShotReview::~ECG12LSnapShotReview()
{
    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }
    _snapShotTimeInedx = 0;
    _checkFlag.clear();
}

/**********************************************************************************************************************
 * 数据初始化
 **********************************************************************************************************************/
void ECG12LSnapShotReview::dataInit()
{
    _totalData = 0;
    _totalPage = 0;
    _currentPage = 0;
    _snapShotTimeInedx = 0;
    _checkFlag.clear();
    _file = NULL;
}

/**********************************************************************************************************************
 * 获取传输类型。
 **********************************************************************************************************************/
ECG12LEADTransferType ECG12LSnapShotReview::getTransferType()
{
    ECG12LEADTransferType type = (ECG12LEADTransferType)getTransferIndex();

    if (type >= EMAIL_SELECT)
    {
        type = EMAIL_SELECT;
    }

    return type;
}

/**********************************************************************************************************************
 * 获取打印类型。
 **********************************************************************************************************************/
ECG12LeadDataPrintType ECG12LSnapShotReview::getPrintType()
{
    ECG12LeadDataPrintType type = (ECG12LeadDataPrintType)getPrintIndex();

    if (type >= PRINT_12L_SELECT_NR)
    {
        type = PRINT_12L_SELECT_NR;
    }

    return type;
}

/**********************************************************************************************************************
 * 判断是否选择。
 **********************************************************************************************************************/
bool ECG12LSnapShotReview::isChecked(int index)
{
    ECG12LeadDataPrintType type = ecg12LSnapShotReview.getPrintType();
    if (PRINT_12L_SELECT_RESCUE == type)
    {
        return true;
    }

    if (index >= _checkFlag.count())
    {
        return false;
    }

    return _checkFlag[index];
}

/**************************************************************************************************
 * 获取快照选择标记
 *************************************************************************************************/
QVector<bool> ECG12LSnapShotReview::getSnapShotCheckFlag()
{
    return _checkFlag;
}

/**************************************************************************************************
 * 打印
 *************************************************************************************************/
void ECG12LSnapShotReview::print(int type)
{
    emit snapShotReviewPrint(type);

    return ;
}

/**************************************************************************************************
 * 计算相关的控制成员。
 *************************************************************************************************/
void ECG12LSnapShotReview::_calControlMemberInfo(void)
{
    if (NULL == _file)
    {
        _totalData = ecg12LDataStorage.backend()->getBlockNR();
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
void ECG12LSnapShotReview::_loadCurrentPageData(void)
{
    // 先清除掉当前页的所ead有数据。
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

    ECG12LDataStorage::ECG12LeadData *item = new ECG12LDataStorage::ECG12LeadData();

    if (NULL == item)
    {
        return;
    }

    int startIndex = _totalData - _currentPage * _rowNR - 1;   // 起始索引，从最新数据开始。

    // 读取数据并进行显示。
    for (int i = 0; i < _rowNR; i++)
    {

        bool isOk = false;

        memset(item, 0, sizeof(ECG12LDataStorage::ECG12LeadData));
        if (NULL == _file)
        {
            isOk = ecg12LDataStorage.backend()->readBlockData(startIndex, (char *)item, sizeof(ECG12LDataStorage::ECG12LeadData));
        }
        else
        {
            isOk = _file->readBlockData(startIndex, (char *)item, sizeof(ECG12LDataStorage::ECG12LeadData));
        }

        if (!isOk)
        {
            break;
        }

        QString timeStr;
        timeDate.getDateTime(item->commonInfo.timeStamp, timeStr, true, true);
        table->item(i, 0)->setText(timeStr);
        table->item(i, 1)->setText(QString::fromLocal8Bit((char*)item->commonInfo.patientID));

        if (startIndex < _checkFlag.count())
        {
            if (_checkFlag[startIndex])
            {
                table->setCheck(table->item(i, 2));
            }
        }

        startIndex -= 1;
        if (startIndex < 0)
        {
            break;
        }
    }


    if (NULL != item)
    {
        delete item;
        item = NULL;
    }
}


/**********************************************************************************************************************
 * 快照是否选择，或者没有选择 用来判断是否弹出提示框
 **********************************************************************************************************************/
bool ECG12LSnapShotReview::_isSelectSnapShot()
{
    ECG12LEADTransferType type = (ECG12LEADTransferType)getTransferIndex();

    if (type == USB_SELECT || type == EMAIL_SELECT)
    {
        for(int i = 0; i < _checkFlag.count();i++)
        {
            if(_checkFlag[i])
            {
                return true;
            }
        }
    }
    return false;
}

/**********************************************************************************************************************
 * 计算快照选择的数量
 **********************************************************************************************************************/
int ECG12LSnapShotReview::_getSelectSnapShotCount()
{
    int count = 0;
    ECG12LEADTransferType type = (ECG12LEADTransferType)getTransferIndex();

    if (type == USB_SELECT || type == EMAIL_SELECT)
    {
        for(int i = 0; i < _checkFlag.count();i++)
        {
            if(_checkFlag[i])
            {
                count++;
            }
        }
    }
    else
    {
        count = _checkFlag.count();
    }

    return count;
}

/**********************************************************************************************************************
 * 刷新页数。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::_refreshTitlePage(void)
{
    QString str;
    if(languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("ECG12LeadSnapshot"))
                .arg((_totalPage <= 1) ? 1 : (_currentPage + 1))
                .arg(_totalPage <= 1 ? 1 : _totalPage);
    }
    else
    {
        str = trs("ECG12LeadSnapshot");
        str += "(";
        str += QString::number((_totalPage <= 1) ? 1 : (_currentPage + 1));
        str += "/";
        str += QString::number(_totalPage <= 1 ? 1 : _totalPage);
        str += ")";
    }
    setTitleBarText(str);
}


/**********************************************************************************************************************
 * 显示事件。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::showEvent(QShowEvent *e)
{
    RescueDataReview::showEvent(e);

    _calControlMemberInfo();
    _loadCurrentPageData();
}

/**********************************************************************************************************************
 * 隐藏事件。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::hideEvent(QHideEvent *e)
{
    RescueDataReview::hideEvent(e);

    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }
}

/**********************************************************************************************************************
 * 上下翻页加载数据。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::loadData(PageLoadType pageType)
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
    _loadCurrentPageData();
}

/**********************************************************************************************************************
 * 加载营救事件列表数据。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::loadRescueTime(QStringList &list)
{
    dataStorageDirManager.getRescueEvent(list);
}

/**********************************************************************************************************************
 * 营救时间改变。
 **********************************************************************************************************************/
void ECG12LSnapShotReview::incidentChange(int index)
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
        _snapShotTimeInedx = index;
        list.clear();
        if (path.isEmpty())
        {
            return;
        }

        if (NULL == _file)
        {
            _file = StorageManager::open(DATA_STORE_PATH + path + ECG12L_FILE_NAME, QIODevice::ReadOnly);
        }
        else
        {
            _file->reload(DATA_STORE_PATH + path + ECG12L_FILE_NAME, QIODevice::ReadOnly);
        }
    }

    _checkFlag.clear();
    _currentPage = 0;

    _calControlMemberInfo();
    _loadCurrentPageData();
}

/**************************************************************************************************
 * 超时处理
 *************************************************************************************************/
void ECG12LSnapShotReview::timeOut()
{
    _calControlMemberInfo();
    _loadCurrentPageData();
}

/**************************************************************************************************
 * 更新选择标志。
 *************************************************************************************************/
void ECG12LSnapShotReview::_updateCheckFlag(int index)
{
    index = _totalData - _currentPage * _rowNR - index - 1;
    if (index < 0 || index >= _checkFlag.count())
    {
        return;
    }

    _checkFlag[index] = !_checkFlag[index];

}

/**************************************************************************************************
 * 趋势营救的槽函数  虚函数用子类实现
 *************************************************************************************************/
void ECG12LSnapShotReview::_transferTypeReleased(int index)
{
    emit snapShotReviewTransfer(index);

    return ;
}
