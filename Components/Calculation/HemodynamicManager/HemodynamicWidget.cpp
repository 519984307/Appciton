#include "HemodynamicWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include <QHeaderView>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBrush>
#include "NumberInput.h"
#include "IMessageBox.h"
#include "HemodynamicManager.h"
#include "HemodynamicReviewWidget.h"
#include "LayoutManager.h"

HemodynamicWidget *HemodynamicWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
HemodynamicWidget::~HemodynamicWidget()
{

}

/**************************************************************************************************
 * 功能： 界面布局。
 *************************************************************************************************/
void HemodynamicWidget::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight()/8*7);

    int submenuW = windowManager.getPopMenuWidth();

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth/2;

    _inout = new QLabel(trs("InputValue"));
    _inout->setFixedSize(btnWidth, _itemH);
    _inout->setFont(font);

    _calcReviewA = new IButton(trs("CalculationReview"));
    _calcReviewA->setFixedSize(btnWidth, _itemH);
    _calcReviewA->setFont(font);
    _calcReviewA->setBorderEnabled(true);
    _calcReviewA->setEnabled(false);
    connect(_calcReviewA, SIGNAL(realReleased()), this, SLOT(_calcReviewReleased()));

    _calcReviewB = new IButton(trs("CalculationReview"));
    _calcReviewB->setFixedSize(btnWidth, _itemH);
    _calcReviewB->setFont(font);
    _calcReviewB->setBorderEnabled(true);
    _calcReviewB->setEnabled(false);
    connect(_calcReviewB, SIGNAL(realReleased()), this, SLOT(_calcReviewReleased()));

    _checkOutput = new IButton(trs("CheckOutput"));
    _checkOutput->setFixedSize(btnWidth, _itemH);
    _checkOutput->setFont(font);
    _checkOutput->setBorderEnabled(true);
    connect(_checkOutput, SIGNAL(realReleased()), this, SLOT(_checkOutputReleased()));

    _calculation = new IButton(trs("Calculation"));
    _calculation->setFixedSize(btnWidth, _itemH);
    _calculation->setFont(font);
    _calculation->setBorderEnabled(true);
    connect(_calculation, SIGNAL(realReleased()), this, SLOT(_calcReleased()));

    _reference = new IButton(trs("ReferenceRange"));
    _reference->setFixedSize(btnWidth, _itemH);
    _reference->setFont(font);
    _reference->setBorderEnabled(true);
    connect(_reference, SIGNAL(realReleased()), this, SLOT(_refRangeReleased()));

    _checkInput = new IButton(trs("CheckInput"));
    _checkInput->setFixedSize(btnWidth, _itemH);
    _checkInput->setFont(font);
    _checkInput->setBorderEnabled(true);
    connect(_checkInput, SIGNAL(realReleased()), this, SLOT(_checkInputReleased()));

    _record = new IButton(trs("Record"));
    _record->setFixedSize(btnWidth, _itemH);
    _record->setFont(font);
    _record->setBorderEnabled(true);

    // 输入窗口
    _signalMapper = new QSignalMapper(this);
    QHBoxLayout *hlayout[HEMODYNAMIC_PARAM_NR];
    QVBoxLayout *fristColumnlayout = new QVBoxLayout();
    QVBoxLayout *secondColumnLayout = new QVBoxLayout();
    for(int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
    {
        _calcParam[i] = new LabelButton(trs(HemodynamicSymbol::convert(static_cast<HemodynamicParam>(i))));
        _calcParam[i]->setFont(font);
        _calcParam[i]->label->setFixedSize(labelWidth, _itemH);
        _calcParam[i]->button->setFixedSize(btnWidth/2, _itemH);
        _calcParam[i]->label->setAlignment(Qt::AlignLeft);
        //        _calcParam[i]->setValue();
        _calcParamUnit[i] = new QLabel(HemodynamicSymbol::convertUnit(static_cast<HemodynamicParam>(i)));
        _calcParamUnit[i]->setFont(font);
        _calcParamUnit[i]->setFixedSize(btnWidth/2, _itemH);
        connect(_calcParam[i]->button, SIGNAL(realReleased()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(_calcParam[i]->button, i);

        hlayout[i] = new QHBoxLayout();
        hlayout[i]->addWidget(_calcParam[i]);
        hlayout[i]->addWidget(_calcParamUnit[i]);
        hlayout[i]->addStretch();

        if(i <= HEMODYNAMIC_PARAM_PAMEAN)
        {
            fristColumnlayout->addLayout(hlayout[i]);
        }
        else
        {
            secondColumnLayout->addLayout(hlayout[i]);
        }
    }
    fristColumnlayout->addStretch();
    secondColumnLayout->addStretch();
    connect(_signalMapper, SIGNAL(mapped(int)), this, SLOT(_btnParamReleased(int)));
    QHBoxLayout *OneHLayout = new QHBoxLayout();
    OneHLayout->addLayout(fristColumnlayout);
    OneHLayout->addStretch();
    OneHLayout->addLayout(secondColumnLayout);
    OneHLayout->addStretch();
    OneHLayout->setContentsMargins(20,0,0,0);
    QHBoxLayout *TwoHLayout = new QHBoxLayout();
    TwoHLayout->addWidget(_calcReviewA);
    TwoHLayout->addWidget(_checkOutput);
    TwoHLayout->addWidget(_calculation);
    QVBoxLayout *OneVLayout = new QVBoxLayout();
    OneVLayout->addStretch();
    OneVLayout->addLayout(OneHLayout);
    OneVLayout->addStretch();
    OneVLayout->addLayout(TwoHLayout);
    _inputWidget = new QWidget();
    _inputWidget->setLayout(OneVLayout);

    // 输出窗口
    _outputTable = new ITableWidget();
    _outputTable->setFixedWidth(itemW);
    _outputTable->setSelectionMode(QAbstractItemView::NoSelection);
    _outputTable->setFocusPolicy(Qt::NoFocus);
    _outputTable->setRowCount(9);
    _outputTable->setColumnCount(6);
    _outputTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _outputTable->horizontalHeader()->setClickable(false);
    _outputTable->horizontalHeader()->setHidden(true);
    _outputTable->verticalHeader()->setHidden(true);
    _outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    _outputTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    _outputTable->setFixedHeight(250);
    QTableWidgetItem *item;
    for(int j = 0; j < HEMODYNAMIC_OUTPUT_NR; j ++)
    {
        if(j <= HEMODYNAMIC_OUTPUT_EF)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convert((HemodynamicOutput)j));
            _outputTable->setItem(j, 0, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText("");
            _outputTable->setItem(j, 1, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)j));
            _outputTable->setItem(j, 2, item);
        }
        else
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convert((HemodynamicOutput)j));
            _outputTable->setItem(j - (int)HEMODYNAMIC_OUTPUT_BSA, 3, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText("");
            _outputTable->setItem(j - (int)HEMODYNAMIC_OUTPUT_BSA, 4, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)j));
            _outputTable->setItem(j - (int)HEMODYNAMIC_OUTPUT_BSA, 5, item);
        }
    }

    QHBoxLayout *ThreeHLayout = new QHBoxLayout();
    ThreeHLayout->addWidget(_reference);
    ThreeHLayout->addWidget(_calcReviewB);
    ThreeHLayout->addWidget(_checkInput);
    ThreeHLayout->addWidget(_record);
    QVBoxLayout *TwoVLayout = new QVBoxLayout();
    TwoVLayout->addWidget(_outputTable);
    TwoVLayout->addStretch();
    TwoVLayout->addLayout(ThreeHLayout);
    TwoVLayout->setContentsMargins(5,0,10,10);
    TwoVLayout->setSpacing(10);
    _outputWidget = new QWidget();
    _outputWidget->setLayout(TwoVLayout);

    _stackLayout = new QStackedLayout();
    _stackLayout->addWidget(_inputWidget);
    _stackLayout->addWidget(_outputWidget);

    contentLayout->addStretch();
    contentLayout->addWidget(_inout);
    contentLayout->addStretch();
    contentLayout->addLayout(_stackLayout);

    connect(this, SIGNAL(checkOutputSignal(int)), _stackLayout, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(checkInputSignal(int)), _stackLayout, SLOT(setCurrentIndex(int)));

    defaultInput();
}

/**************************************************************************************************
 * 功能： 初始化输入参数（将控件上的数据存入manager）。
 *************************************************************************************************/
void HemodynamicWidget::initCalcInput()
{
    QString text;
    float value;
    for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
    {
        text = _calcParam[i]->button->text();
        if (text == "")
        {
            value = InvData();
        }
        else
        {
            value = text.toFloat();
        }
        hemodynamicManager.setCalcInput(i, value);
    }
}

/**************************************************************************************************
 * 功能： 更新数据（将manager存入的数据显示在控件上）。
 *************************************************************************************************/
void HemodynamicWidget::updateData()
{
    QString text;
    float value;
    for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
    {
        value = hemodynamicManager.getCalcValue().calcInput[i];
        if (value == InvData())
        {
            text = EmptyStr();
        }
        else
        {
            text = QString::number(value, 'f', hemodynamicManager.inputAccuracy[i]);
        }
        _calcParam[i]->button->setText(text);
    }

    for (int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
    {
        value = hemodynamicManager.getCalcValue().calcOutput[i];
        if (value == InvData())
        {
            text = InvStr();
        }
        else
        {
            text = QString::number(value, 'f', hemodynamicManager.outputAccuracy[i]);
        }
        if (i <= HEMODYNAMIC_OUTPUT_EF)
        {
            if (text != InvStr() && i < HEMODYNAMIC_OUTPUT_LVSW)
            {
                if (value < hemodynamicManager.outputLimit[i].low)
                {
                    _outputTable->item(i, 1)->setBackground(YELLOW_BACKGROUND);
                    text = text + DOWN_ARROW;
                }
                else if (value > hemodynamicManager.outputLimit[i].high)
                {
                    _outputTable->item(i, 1)->setBackground(YELLOW_BACKGROUND);
                    text = text + UP_ARROW;
                }
                else
                {
                    _outputTable->item(i, 1)->setBackground(WHITE_BACKGROUND);
                }
            }
            _outputTable->item(i, 1)->setText(text);
        }
        else
        {
            if (text != InvStr() && i > HEMODYNAMIC_OUTPUT_BSA)
            {
                if (value < hemodynamicManager.outputLimit[i].low)
                {
                    _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 4)->setBackground(YELLOW_BACKGROUND);
                    text = text + DOWN_ARROW;
                }
                else if (value > hemodynamicManager.outputLimit[i].high)
                {
                    _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 4)->setBackground(YELLOW_BACKGROUND);
                    text = text + UP_ARROW;
                }
                else
                {
                    _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 4)->setBackground(WHITE_BACKGROUND);
                }
            }
            _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 4)->setText(text);
        }
    }
}

/**************************************************************************************************
 * 功能： 默认输入参数（测试）。
 *************************************************************************************************/
void HemodynamicWidget::defaultInput()
{
    _calcParam[0]->button->setText("12");
    _calcParam[1]->button->setText("65");
    _calcParam[2]->button->setText("8");
    _calcParam[3]->button->setText("36");
    _calcParam[4]->button->setText("46");
    _calcParam[5]->button->setText("26");
    _calcParam[6]->button->setText("256");
    _calcParam[7]->button->setText("177");
    _calcParam[8]->button->setText("65");

}

/**************************************************************************************************
 * 功能： 显示事件。
 *************************************************************************************************/
void HemodynamicWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = layoutManager.getMenuArea();
    //    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

/**************************************************************************************************
 * 功能： 输入参数槽函数（弹出数字输入框）。。
 *************************************************************************************************/
void HemodynamicWidget::_btnParamReleased(int index)
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs(HemodynamicSymbol::convert(static_cast<HemodynamicParam>(index))));
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(_calcParam[index]->button->text());
    numberPad.setUnitDisplay(_calcParamUnit[index]->text());
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        float value = text.toFloat(&ok);
        if (ok)
        {
            if((value < hemodynamicManager.inputLimit[index].low) || (value > hemodynamicManager.inputLimit[index].high))
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + QString::number(hemodynamicManager.inputLimit[index].low) +
                                       "-" + QString::number(hemodynamicManager.inputLimit[index].high), QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else
            {
                _calcParam[index]->button->setText(text);
                hemodynamicManager.setCalcInput(index, text.toFloat());
            }
        }
    }

}

/**************************************************************************************************
 * 功能： 查看输出。
 *************************************************************************************************/
void HemodynamicWidget::_checkOutputReleased()
{

    _inout->setText(trs("OutputValue"));
    _reference->setFocus();
    updateData();
    emit checkInputSignal(1);
}

/**************************************************************************************************
 * 功能： 查看输入。
 *************************************************************************************************/
void HemodynamicWidget::_checkInputReleased()
{
    _inout->setText(trs("InputValue"));
    emit checkOutputSignal(0);
}

/**************************************************************************************************
 * 功能： 参考范围与单位切换。
 *************************************************************************************************/
void HemodynamicWidget::_refRangeReleased()
{
    if(!_refUnitFlag)
    {
        _reference->setText(trs("TEMPUnit"));
        _refUnitFlag = 1;
        for(int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {
            if(i <= HEMODYNAMIC_OUTPUT_EF)
            {
                _outputTable->item(i, 2)->setText(HemodynamicSymbol::convertRange((HemodynamicOutput)i));
            }
            else
            {
                _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 5)->setText(HemodynamicSymbol::convertRange((HemodynamicOutput)i));
            }
        }
    }
    else
    {
        _reference->setText(trs("ReferenceRange"));
        _refUnitFlag = 0;
        for(int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {
            if(i <= HEMODYNAMIC_OUTPUT_EF)
            {
                _outputTable->item(i, 2)->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)i));
            }
            else
            {
                _outputTable->item(i - (int)HEMODYNAMIC_OUTPUT_BSA, 5)->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)i));
            }
        }
    }

}

/**************************************************************************************************
 * 功能： 血液动力学计算槽函数。
 *************************************************************************************************/
void HemodynamicWidget::_calcReleased()
{
    hemodynamicManager.calcAction();
    _checkOutputReleased();
    hemodynamicManager.insertReviewData(hemodynamicManager.getCalcValue());
    if (hemodynamicManager.getReviewData().length() != 0)
    {
        _calcReviewA->setEnabled(true);
        _calcReviewB->setEnabled(true);
    }
}

/**************************************************************************************************
 * 功能： 打开计算回顾窗口。
 *************************************************************************************************/
void HemodynamicWidget::_calcReviewReleased()
{

    hemodynamicReviewWidget.updateReviewTable();
    hemodynamicReviewWidget.defaultSelectColumn();
    hemodynamicReviewWidget.autoShow();
}

/**************************************************************************************************
 * 功能： 构造函数。
 *************************************************************************************************/
HemodynamicWidget::HemodynamicWidget() : _refUnitFlag(0)
{
    layoutExec();
    initCalcInput();
}

