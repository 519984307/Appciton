#include <QTableWidget>
#include <QHBoxLayout>
#include <QTimer>
#include "RescueDataReview.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "WindowManager.h"
#include "ITableWidget.h"
#include "IDropList.h"
#include "PrintManager.h"
#include "TimeDate.h"
#include <QDateTime>

//--------usb传送------------for_test----
#include "ExportDataWidget.h"
#include "IMessageBox.h"
#include "DataStorageDefine.h"
//--------usb传送------------for_test----

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
RescueDataReview::RescueDataReview(RescueDataType type) : PopupWidget()
{
    _type = type;

    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));
    table = new ITableWidget();

    _incident = new IComboList(trs("Incident"));
    _incident->setFont(fontManager.textFont(fontSize));
    _incident->combolist->setFixedHeight(RESCUE_HELP_ITEM_H);
    _incident->setMinimumWidth(230);
    _incident->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(_incident->combolist, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_rescueTimeReleased(int)));

    _solution = new IDropList(trs("Interval"));
    _solution->setFixedHeight(RESCUE_HELP_ITEM_H);
    _solution->setMaximumWidth(70);
    _solution->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _solution->setFont(fontManager.textFont(fontSize));
    _solution->setVisible(false);
    _solution->addItem(trs("_30sec"));
    _solution->addItem(trs("_1min"));
    _solution->addItem(trs("_2min"));
    _solution->addItem(trs("_5min"));
    _solution->addItem(trs("_10min"));
    _solution->addItem(trs("_30min"));
    connect(_solution, SIGNAL(currentIndexChange(int)), this,
            SLOT(_solutionTimeReleased(int)));

    _print = new IDropList(trs("PrintOption"));
    _print->setFont(fontManager.textFont(fontSize));
    //_print->setFixedSize(130, RESCUE_HELP_ITEM_H);
    _print->setFixedHeight(RESCUE_HELP_ITEM_H);
    _print->setMinimumWidth(120);
    _print->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(_print, SIGNAL(currentIndexChange(int)), this, SLOT(_printReleased(int)));

    _printParam = new IDropList(trs("Column"));
    _printParam->setVisible(false);
    _printParam->setFixedHeight(RESCUE_HELP_ITEM_H);
    _printParam->setMaximumWidth(70);
    _printParam->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _printParam->setFont(fontManager.textFont(fontSize));
    connect(_printParam, SIGNAL(currentIndexChange(int)), this, SLOT(_printParamReleased(int)));

    _printBtn = new LButtonEx();
    _printBtn->setText(trs("Print"));
    _printBtn->setFixedSize(100, RESCUE_HELP_ITEM_H);
    _printBtn->setFont(fontManager.textFont(fontSize));
    _printBtn->setVisible(false);
    connect(_printBtn, SIGNAL(realReleased()), this, SLOT(onPrintBtnRelease()));

    _backBtn = new LButtonEx();
    _backBtn->setText(trs("Back"));
    _backBtn->setFixedSize(100, RESCUE_HELP_ITEM_H);
    _backBtn->setFont(fontManager.textFont(fontSize));
    _backBtn->setVisible(false);
    connect(_backBtn, SIGNAL(realReleased()), this, SLOT(onBackBtnRelease()));


    _transferType = new IDropList(trs("Transfer"));
    _transferType->setVisible(false);
    _transferType->setFixedSize(150, RESCUE_HELP_ITEM_H);
    _transferType->setFont(fontManager.textFont(fontSize));
    connect(_transferType, SIGNAL(currentIndexChange(int)), this,
            SLOT(_transferTypeReleased(int)));


    _up = new IButton();
    _up->setFixedSize(RESCUE_HELP_ITEM_H - 2, RESCUE_HELP_ITEM_H - 2);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(RESCUE_HELP_ITEM_H - 2, RESCUE_HELP_ITEM_H - 2);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addStretch();
    lineLayout->addWidget(_incident);
    lineLayout->addWidget(_solution);
    lineLayout->addWidget(_print);
    lineLayout->addWidget(_printParam);
    lineLayout->addWidget(_transferType);
    lineLayout->addWidget(_printBtn);
    lineLayout->addWidget(_backBtn);
    lineLayout->addStretch();
    lineLayout->addWidget(_up);
    lineLayout->addWidget(_down);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(1);
    layout->setSpacing(1);
    layout->addWidget(table);
    layout->addLayout(lineLayout);
    contentLayout->addLayout(layout);

    setFixedSize(_maxWidth, _maxHeight);

    _timer = new QTimer();
    _timer->setInterval(1000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
RescueDataReview::~RescueDataReview()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/**********************************************************************************************************************
 * 设置分辨率按钮可用。
 **********************************************************************************************************************/
void RescueDataReview::setSolutionVisiable(bool isVisible)
{
    _solution->setVisible(isVisible);
}

/***************************************************************************************************
 * setPrintVisiable : set the visibility of _print
 **************************************************************************************************/
void RescueDataReview::setPrintVisiable(bool isVisible)
{
    _print->setVisible(isVisible);
}

/**********************************************************************************************************************
 * 设置事件按钮可用。
 **********************************************************************************************************************/
void RescueDataReview::setIncidentVisiable(bool isVisible)
{
    _incident->setVisible(isVisible);
}

/**********************************************************************************************************************
 * 设置事件按钮可用。
 **********************************************************************************************************************/
void RescueDataReview::setPrintParamVisiable(bool isVisible)
{
    _printParam->setVisible(isVisible);
}

/***************************************************************************************************
 * check _printParam is visible or not
 **************************************************************************************************/
bool RescueDataReview::isPrintParamVisiable() const
{
    return _printParam->isVisible();
}

/**********************************************************************************************************************
 * 设置传输类型按钮可用。
 **********************************************************************************************************************/
void RescueDataReview::settransferTypeVisiable(bool isVisible)
{
    _transferType->setVisible(isVisible);
}

/***************************************************************************************************
 * setPrintBtnVisiable : set print button visiable
 **************************************************************************************************/
void RescueDataReview::setPrintBtnVisiable(bool isVisible)
{
    _printBtn->setVisible(isVisible);
}

/***************************************************************************************************
 * setBackBtnVisiable : set back button visiable
 **************************************************************************************************/
void RescueDataReview::setBackBtnVisiable(bool isVisible)
{
    _backBtn->setVisible(isVisible);
}

/***************************************************************************************************
 * isIncidentVisiable : check whether _incident is visiable
 **************************************************************************************************/
bool RescueDataReview::isIncidentVisiable() const
{
    return _incident->isVisible();
}

/***************************************************************************************************
 * isPrintVisiable : check whether _print is visiable
 **************************************************************************************************/
bool RescueDataReview::isPrintVisiable() const
{
    return _print->isVisible();
}

/**********************************************************************************************************************
 * 添加打印类型。
 **********************************************************************************************************************/
void RescueDataReview::addPrintType(const QString typeStr)
{
//    _range->addItem(typeStr);
    _print->addItem(typeStr);
}

/**********************************************************************************************************************
 * 添加打印参数。
 **********************************************************************************************************************/
void RescueDataReview::addPrintParam(const QString &paramStr)
{
    _printParam->addItem(paramStr);
}

/**********************************************************************************************************************
 * 获取打印索引。
 **********************************************************************************************************************/
int RescueDataReview::getPrintIndex()
{
    return _print->currentIndex();
}
/**********************************************************************************************************************
 * 获取传输索引。
 **********************************************************************************************************************/
int RescueDataReview::getTransferIndex()
{
    return _transferType->currentIndex();
}
/**********************************************************************************************************************
 * 获取营救索引。
 **********************************************************************************************************************/
int RescueDataReview::getIncidentIndex()
{
    return _incident->currentIndex();
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void RescueDataReview::_upReleased(void)
{
    loadData(PAGE_UP);
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void RescueDataReview::_downReleased(void)
{
    loadData(PAGE_DOWN);
}

/**********************************************************************************************************************
 * 打印。
 **********************************************************************************************************************/
void RescueDataReview::_printReleased(int index)
{
    print(index);
}

/**********************************************************************************************************************
 * 打印参数改变。
 **********************************************************************************************************************/
void RescueDataReview::_printParamReleased(int index)
{
    printParamChange(index);
}

/**********************************************************************************************************************
 * 传输类型回调。
 **********************************************************************************************************************/
void RescueDataReview::_transferTypeReleased(int index)
{
    index = index;
}
/***************************************************************************************************
 * wifi和usb传送的实现
 **************************************************************************************************/
 bool RescueDataReview::TransferProcess(ECG12LEADTransferType type)
 {
     type = type;
     return true;
 }

 void RescueDataReview::onPrintBtnRelease()
 {

 }

 void RescueDataReview::onBackBtnRelease()
 {

 }

/***************************************************************************************************
 * 选择范围改变。
 **************************************************************************************************/
void RescueDataReview::_rangeReleased(int index)
{
    _selectType = (RescueDataPrintType)index;
}

/**********************************************************************************************************************
 * 营救时间改变。
 **********************************************************************************************************************/
void RescueDataReview::_rescueTimeReleased(int index)
{
    incidentChange(index);
}

/**********************************************************************************************************************
 * 分辨率改变。
 **********************************************************************************************************************/
void RescueDataReview::_solutionTimeReleased(int index)
{
    solutionChange(index);
}

/**********************************************************************************************************************
 * timeout。
 **********************************************************************************************************************/
void RescueDataReview::_timeOutSlot()
{
    timeOut();
}

/**********************************************************************************************************************
 * 按键事件。
 **********************************************************************************************************************/
void RescueDataReview::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            focusNextPrevChild(false);
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            focusNextChild();
            break;
        default:
            break;
    }
}

/**********************************************************************************************************************
 * 显示事件。
 **********************************************************************************************************************/
void RescueDataReview::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 强制设置关闭按钮为焦点，防止焦点在列表上面导致绘画出错
    setCloseBtnFocus();

    _readShow();

    _timer->start();
}

/**********************************************************************************************************************
 * 隐藏事件。
 **********************************************************************************************************************/
void RescueDataReview::hideEvent(QHideEvent *e)
{
    PopupWidget::hideEvent(e);

    _timer->stop();
}

/**********************************************************************************************************************
 * 上下左右翻页加载数据。
 * 0,上翻页；1,下翻页；
 **********************************************************************************************************************/
void RescueDataReview::loadData(PageLoadType /*type*/)
{

}

/**********************************************************************************************************************
 * 分辨率改变。
 **********************************************************************************************************************/
void RescueDataReview::solutionChange(int /*index*/)
{

}

/**********************************************************************************************************************
 * 营救时间改变。
 **********************************************************************************************************************/
void RescueDataReview::incidentChange(int /*index*/)
{

}

/**********************************************************************************************************************
 * 打印参数改变。
 **********************************************************************************************************************/
void RescueDataReview::printParamChange(int /*index*/)
{

}

/**********************************************************************************************************************
 * 加载营救事件列表数据。
 **********************************************************************************************************************/
void RescueDataReview::loadRescueTime(QStringList &/*list*/)
{

}

/**********************************************************************************************************************
 * 打印。
 **********************************************************************************************************************/
void RescueDataReview::print(int /*type*/)
{

}

/**********************************************************************************************************************
 * 定时时间到。
 **********************************************************************************************************************/
void RescueDataReview::timeOut()
{

}

/***************************************************************************************************
 * 修改打印标题。
 **************************************************************************************************/
void RescueDataReview::printTitleSet(const QString &paramStr)
{
    _print->setText(paramStr);
}

/***************************************************************************************************
 * 添加传送类型子项目。
 **************************************************************************************************/
void RescueDataReview::addTransfer(const QString &paramStr)
{
    _transferType->addItem(paramStr);
}
/***************************************************************************************************
 * 准备显示数据。
 **************************************************************************************************/
void RescueDataReview::_readShow()
{
    QStringList strlist;
    loadRescueTime(strlist);
    _incident->clear();

    int count = strlist.count();
    if (0 == count)
    {
        _incident->addItem(" ");
    }
    else
    {
        foreach(QString str, strlist)
        {
            QString timeStr;
            QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
            timeDate.getDateTime(dt.toTime_t(), timeStr, true, true);
            _incident->addItem(timeStr);
        }
    }

    _incident->setCurrentIndex(0);
}
