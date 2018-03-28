#include "HemodynamicReviewWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include <QHeaderView>
#include <QScrollBar>
#include "HemodynamicManager.h"
#include "HemodynamicWidget.h"

#define MAX_ROW_COUNT           28
#define MAX_COLUMN_COUNT        12

HemodynamicReviewWidget *HemodynamicReviewWidget::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 析构函数。
 *************************************************************************************************/
HemodynamicReviewWidget::~HemodynamicReviewWidget()
{

}

/**************************************************************************************************
 * 功能： 界面布局。
 *************************************************************************************************/
void HemodynamicReviewWidget::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight());

    int submenuW = windowManager.getPopMenuWidth();

    int itemW = submenuW - 20;
    int tableW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);
    int btnWidth = itemW / 4;
//    int labelWidth = btnWidth/2;

    _model = new QStandardItemModel();
    _reviewTable = new FreezeTableWidget(_model);
    _reviewTable->setFreezeColumn(2);
    _reviewTable->verticalHeader()->setHidden(true);
    _reviewTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _reviewTable->setFocusPolicy(Qt::NoFocus);
    _reviewTable->setSelectionBehavior(QAbstractItemView::SelectColumns);
    _reviewTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _reviewTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _reviewTable->horizontalHeader()->setClickable(false);
    _reviewTable->setFixedSize(tableW + 10 , _itemH * 11 + 10);                     // 分别加10是滚动条的宽度
    _reviewTable->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    _reviewTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    _headList << trs("InOutValue") << trs("TEMPUnit");
    for (int itemHead = 0; itemHead < MAX_COLUMN_COUNT - 2; itemHead ++)            // 减去冻结的两列
    {
        _headList << "";
    }
//    _model->setHorizontalHeaderLabels(_headList);
    QStandardItem *item;
    for (int i = 0; i < MAX_ROW_COUNT; i ++)
    {
        for (int j = 0; j < MAX_COLUMN_COUNT; j ++)
        {
            _reviewTable->setColumnWidth(j, tableW/6);
            item = new QStandardItem();
            item->setTextAlignment(Qt::AlignCenter);
            _model->setItem(i, j,item);
            if ( j == 0 && i == 0)
            {
                item->setText(trs("InputValue"));
            }
            else if (j == 0 && i == 10)
            {
                item->setText(trs("OutputValue"));
            }
            else if(j == 0 && i < 10)
            {
                item->setText(HemodynamicSymbol::convert((HemodynamicParam)(i - 1)));
            }
            else if(j == 0 && i > 10)
            {
                item->setText(HemodynamicSymbol::convert((HemodynamicOutput)(i - 11)));
            }
            else if(j == 1 && i < 10)
            {
                item->setText(HemodynamicSymbol::convertUnit((HemodynamicParam)(i - 1)));
            }
            else if(j == 1 && i > 10)
            {
                item->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)(i - 11)));
            }
        }
        _reviewTable->setRowHeight(i, _itemH);
    }

    _reviewTable->selectColumn(_selectColumn);                          // 选中第二列

    _return = new IButton(trs("PrimitiveCalculation"));
    _return->setFixedSize(btnWidth, _itemH);
    _return->setFont(font);
    _return->setBorderEnabled(true);
    connect(_return, SIGNAL(realReleased()), this, SLOT(_returnReleased()));

    _reference = new IButton(trs("ReferenceRange"));
    _reference->setFixedSize(btnWidth, _itemH);
    _reference->setFont(font);
    _reference->setBorderEnabled(true);
    connect(_reference, SIGNAL(realReleased()), this, SLOT(_referenceReleased()));

    _up = new IButton();
    _up->setFixedSize(_itemH - 2, _itemH - 2);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(_itemH - 2, _itemH - 2);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    _left = new IButton();
    _left->setFixedSize(_itemH - 2, _itemH - 2);
    _left->setPicture(QImage("/usr/local/nPM/icons/ArrowLeft.png"));
    connect(_left, SIGNAL(realReleased()), this, SLOT(_leftReleased()));

    _right = new IButton();
    _right->setFixedSize(_itemH - 2, _itemH - 2);
    _right->setPicture(QImage("/usr/local/nPM/icons/ArrowRight.png"));
    connect(_right, SIGNAL(realReleased()), this, SLOT(_rightReleased()));

    QHBoxLayout *_hlayout = new QHBoxLayout();
    _hlayout->addWidget(_return);
    _hlayout->addWidget(_reference);
    _hlayout->addWidget(_up);
    _hlayout->addWidget(_down);
    _hlayout->addWidget(_left);
    _hlayout->addWidget(_right);

    contentLayout->addStretch();
    contentLayout->addWidget(_reviewTable);
    contentLayout->addStretch();
    contentLayout->addLayout(_hlayout);
    contentLayout->addStretch();

}

/**************************************************************************************************
 * 功能： 更新计算回顾表格数据。
 *************************************************************************************************/
void HemodynamicReviewWidget::updateReviewTable()
{
    QList<HemodynamicInfo> reviewData = hemodynamicManager.getReviewData();
    HemodynamicInfo singleData;
    QString text;
    float value;

    for (int i = 0; i < reviewData.length(); i ++)
    {
        singleData = reviewData.at(reviewData.length() - i - 1);
        _headList.replace(i + 2, QDateTime::fromTime_t(singleData.calcTime).toString("MM.dd hh:mm"));   // 将时间戳转换成QDateTime,使用静态使用方法;
        for (int m = 0; m  < HEMODYNAMIC_PARAM_NR; m ++)
        {
            value = singleData.calcInput[m];
            if (value == InvData())
            {
                text = InvStr();
            }
            else
            {
                text = QString::number(singleData.calcInput[m], 'f', hemodynamicManager.inputAccuracy[m]);
            }
            _model->item(1 + m, i + 2)->setText(text);
        }
        for (int n = 0; n < HEMODYNAMIC_OUTPUT_NR; n ++)
        {
            value = singleData.calcOutput[n];
            if (value == InvData())
            {
                text = InvStr();
            }
            else
            {
                text = QString::number(singleData.calcOutput[n], 'f', hemodynamicManager.outputAccuracy[n]);
            }
            if (text != InvStr() && (n < HEMODYNAMIC_OUTPUT_LVSW || n > HEMODYNAMIC_OUTPUT_BSA))
            {
                if (value < hemodynamicManager.outputLimit[n].low)
                {
                    _model->item(1 + n + 10, i + 2)->setBackground(YELLOW_BACKGROUND);
                    text = text + DOWN_ARROW;
                }
                else if (value > hemodynamicManager.outputLimit[n].high)
                {
                    _model->item(1 + n + 10, i + 2)->setBackground(YELLOW_BACKGROUND);
                    text = text + UP_ARROW;
                }
                else
                {
                    _model->item(1 + n + 10, i + 2)->setBackground(WHITE_BACKGROUND);
                }
            }           
            _model->item(1 + n + 10, i + 2)->setText(text);
        }
    }
    _model->setHorizontalHeaderLabels(_headList);
}

/**************************************************************************************************
 * 功能： 设置默认选中列。
 *************************************************************************************************/
void HemodynamicReviewWidget::defaultSelectColumn(void)
{
    _selectColumn = 2;
    _reviewTable->selectColumn(_selectColumn);
}

/**************************************************************************************************
 * 功能： 显示事件。
 *************************************************************************************************/
void HemodynamicReviewWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

/**************************************************************************************************
 * 功能： 上箭头槽函数(向上移动滚动条）。
 *************************************************************************************************/
void HemodynamicReviewWidget::_upReleased()
{
    _curVScroller = _reviewTable->verticalScrollBar()->value();

    if(_curVScroller>0)
    {
        _reviewTable->verticalScrollBar()->setSliderPosition(_curVScroller-MAX_ROW_COUNT);
    }
}

/**************************************************************************************************
 * 功能： 下箭头槽函数（向下移动滚动条）。
 *************************************************************************************************/
void HemodynamicReviewWidget::_downReleased()
{
    int maxValue = _reviewTable->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    _curVScroller = _reviewTable->verticalScrollBar()->value(); //获得当前scroller值

    if(_curVScroller<maxValue)
    {
        _reviewTable->verticalScrollBar()->setSliderPosition(MAX_ROW_COUNT+_curVScroller);
    }
}

/**************************************************************************************************
 * 功能： 左箭头槽函数（向左移动滚动条）。
 *************************************************************************************************/
void HemodynamicReviewWidget::_leftReleased()
{
    if (_selectColumn != 2)
    {
        _selectColumn --;
        _reviewTable->selectColumn(_selectColumn);
    }
}

/**************************************************************************************************
 * 功能： 右箭头槽函数（向右移动滚动条）。
 *************************************************************************************************/
void HemodynamicReviewWidget::_rightReleased()
{
    if (_selectColumn != hemodynamicManager.getReviewData().length() + 1)
    {
        _selectColumn ++;
        _reviewTable->selectColumn(_selectColumn);
    }
}

/**************************************************************************************************
 * 功能： 返回计算槽函数（设置当前选中计算列的输入输出参数）。
 *************************************************************************************************/
void HemodynamicReviewWidget::_returnReleased()
{
    hemodynamicManager.setCalcValue(hemodynamicManager.getReviewData().at(hemodynamicManager.getReviewData().length() - _selectColumn + 1));
    hemodynamicWidget.updateData();
    exit();
}

/**************************************************************************************************
 * 功能： 参考范围与单位的切换槽函数。
 *************************************************************************************************/
void HemodynamicReviewWidget::_referenceReleased()
{
    if(!_refUnitFlag)
    {
        _reference->setText(trs("TEMPUnit"));
        _refUnitFlag = 1;
        for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
        {

            _model->item(1 + i, 1)->setText("");
        }
        for(int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {

            _model->item(11 + i, 1)->setText(HemodynamicSymbol::convertRange((HemodynamicOutput)(i)));
        }
    }
    else
    {
        _reference->setText(trs("ReferenceRange"));
        _refUnitFlag = 0;
        for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
        {
            _model->item(1 + i, 1)->setText(HemodynamicSymbol::convertUnit((HemodynamicParam)(i)));
        }
        for(int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {
            _model->item(11 + i, 1)->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)(i)));
        }
    }
}

/**************************************************************************************************
 * 功能： 构造函数。
 *************************************************************************************************/
HemodynamicReviewWidget::HemodynamicReviewWidget() : _refUnitFlag(0), _selectColumn(2), _curVScroller(0)
{
    layoutExec();
}
