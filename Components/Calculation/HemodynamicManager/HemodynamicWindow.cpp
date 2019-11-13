/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/
#include "HemodynamicWindow.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QStackedLayout>
#include "HemodynamicDefine.h"
#include <QTableWidget>
#include "HemodynamicManager.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include <QHeaderView>
#include "HemodynamicReviewWindow.h"
#include "WindowManager.h"
#include "LayoutManager.h"

#define TABLE_ROW_NUM           9
#define ROW_HEIGHT   40
class HemodynamicWindowPrivate
{
public:
    HemodynamicWindowPrivate()
            : refUnitFlag(0),
              inLabel(NULL),
              outLabel(NULL),
              inputWindow(NULL),
              outWindow(NULL),
              stackLayout(NULL),
              calcReviewA(NULL),
              calcReviewB(NULL),
              checkOutput(NULL),
              calculation(NULL),
              checkInput(NULL),
              reference(NULL),
              record(NULL),
              outputTable(NULL)
    {
        for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i++)
        {
            calcParam[i] = NULL;
            calcParamUnit[i] = NULL;
        }
    }

    int refUnitFlag;                   // （参考范围）和（单位）之间的切换
    QLabel *inLabel;                     // 输入值标题
    QLabel *outLabel;                     // 输出值标题

    QWidget *inputWindow;
    QWidget *outWindow;

    QStackedLayout *stackLayout;

    Button *calcParam[HEMODYNAMIC_PARAM_NR];
    QLabel *calcParamUnit[HEMODYNAMIC_PARAM_NR];

    Button *calcReviewA;
    Button *calcReviewB;

    Button *checkOutput;
    Button *calculation;

    Button *checkInput;
    Button *reference;
    Button *record;

    QTableWidget *outputTable;
};

HemodynamicWindow *HemodynamicWindow::getInstance()
{
    static HemodynamicWindow *instance = NULL;
    if (!instance)
    {
        instance = new HemodynamicWindow;
    }
    return instance;
}

HemodynamicWindow::~HemodynamicWindow()
{
    delete d_ptr;
}

void HemodynamicWindow::layoutExec()
{
    // input layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QGridLayout *gl = new QGridLayout(this);

    QLabel *label;
    Button *button;

    // input value
    label = new QLabel(trs("InputValue"));
    gl->addWidget(label, 0, 0);
    d_ptr->inLabel = label;

    // parameters
    for (int i = 0; i < HEMODYNAMIC_PARAM_CVP; i++)
    {
        label = new QLabel(trs(HemodynamicSymbol::
                               convert(static_cast<HemodynamicParam>(i))));
        gl->addWidget(label, (i + 1), 0);
        button = new Button;
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setMinimumWidth(100);
        gl->addWidget(button, (i + 1), 1);
        button->setProperty("btnItem", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnParamReleased()));
        d_ptr->calcParam[i] = button;
        label = new QLabel(HemodynamicSymbol::
                           convertUnit(static_cast<HemodynamicParam>(i)));
        d_ptr->calcParamUnit[i] = label;
        gl->addWidget(label, (i + 1), 2);
    }

    for (int i = HEMODYNAMIC_PARAM_CVP; i < HEMODYNAMIC_PARAM_NR; i++)
    {
        label = new QLabel(trs(HemodynamicSymbol::
                               convert(static_cast<HemodynamicParam>(i))));
        gl->addWidget(label, (i - HEMODYNAMIC_PARAM_CVP + 1), 3);
        button = new Button;
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setMinimumWidth(100);
        button->setProperty("btnItem", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnParamReleased()));
        gl->addWidget(button, (i - HEMODYNAMIC_PARAM_CVP + 1), 4);
        d_ptr->calcParam[i] = button;
        label = new QLabel(HemodynamicSymbol::
                           convertUnit(static_cast<HemodynamicParam>(i)));
        d_ptr->calcParamUnit[i] = label;
        gl->addWidget(label, (i - HEMODYNAMIC_PARAM_CVP + 1), 5);
    }
    layout->addLayout(gl);

    QHBoxLayout *hl = new QHBoxLayout(this);

    // calReviewA
    button = new Button(trs("CalculationReview"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onCalcReviewReleased()));
    hl->addWidget(button);
    d_ptr->calcReviewA = button;

    // checkout
    button = new Button(trs("CheckOutput"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onCheckOutputReleased()));
    hl->addWidget(button);
    d_ptr->checkOutput = button;

    // Calculation
    button = new Button(trs("Calculation"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onCalcReleased()));
    hl->addWidget(button);
    d_ptr->calculation = button;

    layout->addStretch(1);
    layout->addLayout(hl);

    d_ptr->inputWindow = new QWidget();
    d_ptr->inputWindow->setWindowFlags(Qt::CustomizeWindowHint);
    d_ptr->inputWindow->setLayout(layout);

    // output layout
    QVBoxLayout *vlOutLayout = new QVBoxLayout(this);
    vlOutLayout->setMargin(10);
    vlOutLayout->setAlignment(Qt::AlignTop);

    label = new QLabel(trs("OutputValue"));
    d_ptr->outLabel = label;
    // output value
    vlOutLayout->addWidget(d_ptr->outLabel);

    // outputtable
    d_ptr->outputTable = new QTableWidget();
    d_ptr->outputTable->setSelectionMode(QAbstractItemView::NoSelection);
    d_ptr->outputTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->outputTable->setRowCount(9);
    d_ptr->outputTable->setColumnCount(6);
    d_ptr->outputTable->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    d_ptr->outputTable->setFrameStyle(QFrame::NoFrame);
    d_ptr->outputTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->outputTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d_ptr->outputTable->horizontalHeader()->setVisible(false);
    d_ptr->outputTable->verticalHeader()->setVisible(false);
    d_ptr->outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->outputTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->outputTable->setFixedHeight(ROW_HEIGHT * TABLE_ROW_NUM);
    QTableWidgetItem *item;
    for (int j = 0; j < HEMODYNAMIC_OUTPUT_NR; j ++)
    {
        if (j <= HEMODYNAMIC_OUTPUT_EF)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convert((HemodynamicOutput)j));
            d_ptr->outputTable->setItem(j, 0, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText("");
            d_ptr->outputTable->setItem(j, 1, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)j));
            d_ptr->outputTable->setItem(j, 2, item);
        }
        else
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convert((HemodynamicOutput)j));
            d_ptr->outputTable->setItem(j - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA), 3, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText("");
            d_ptr->outputTable->setItem(j - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA), 4, item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)j));
            d_ptr->outputTable->setItem(j - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA), 5, item);
        }
    }

    vlOutLayout->addWidget(d_ptr->outputTable);

    hl = new QHBoxLayout;

    // calReviewB
    button = new Button(trs("CalculationReview"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onCalcReviewReleased()));
    hl->addWidget(button);
    d_ptr->calcReviewB = button;

    // ReferenceRange
    button = new Button(trs("ReferenceRange"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onRefRangeReleased()));
    hl->addWidget(button);
    d_ptr->reference = button;

    // CheckInput
    button = new Button(trs("CheckInput"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onCheckInputReleased()));
    hl->addWidget(button);
    d_ptr->checkInput = button;

    // Record
    button = new Button(trs("Record"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    d_ptr->record = button;

    vlOutLayout->addStretch(1);
    vlOutLayout->addLayout(hl);

    d_ptr->outWindow = new QWidget();
    d_ptr->outWindow->setWindowFlags(Qt::CustomizeWindowHint);
    d_ptr->outWindow->setLayout(vlOutLayout);


    d_ptr->stackLayout = new QStackedLayout(this);
    d_ptr->stackLayout->addWidget(d_ptr->inputWindow);
    d_ptr->stackLayout->addWidget(d_ptr->outWindow);

    setWindowLayout(d_ptr->stackLayout);

    connect(this, SIGNAL(checkOutputSignal(int)), d_ptr->stackLayout, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(checkInputSignal(int)),  d_ptr->stackLayout, SLOT(setCurrentIndex(int)));
    defaultInput();
    setFixedSize(600, 404);
    setWindowTitle(trs("Input"));
    d_ptr->calcReviewA->setEnabled(false);
    d_ptr->calcReviewB->setEnabled(false);
}

void HemodynamicWindow::initCalInput()
{
    QString text;
    float value;
    for (int i = 0; i < HEMODYNAMIC_PARAM_NR; i ++)
    {
        text = d_ptr->calcParam[i]->text();
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

void HemodynamicWindow::updateData()
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
        d_ptr->calcParam[i]->setText(text);
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
                    d_ptr->outputTable->item(i, 1)->setBackground(YELLOW_BACKGROUND);
                    text = text + DOWN_ARROW;
                }
                else if (value > hemodynamicManager.outputLimit[i].high)
                {
                    d_ptr->outputTable->item(i, 1)->setBackground(YELLOW_BACKGROUND);
                    text = text + UP_ARROW;
                }
                else
                {
                    d_ptr->outputTable->item(i, 1)->setBackground(WHITE_BACKGROUND);
                }
            }
            d_ptr->outputTable->item(i, 1)->setText(text);
        }
        else
        {
            if (text != InvStr() && i > HEMODYNAMIC_OUTPUT_BSA)
            {
                QTableWidgetItem *item = d_ptr->outputTable->item(i - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA), 4);
                if (value < hemodynamicManager.outputLimit[i].low)
                {
                    item->setBackground(YELLOW_BACKGROUND);
                    text = text + DOWN_ARROW;
                }
                else if (value > hemodynamicManager.outputLimit[i].high)
                {
                    item->setBackground(YELLOW_BACKGROUND);
                    text = text + UP_ARROW;
                }
                else
                {
                    item->setBackground(WHITE_BACKGROUND);
                }
            }
            d_ptr->outputTable->item(i - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA), 4)->setText(text);
        }
    }
}

void HemodynamicWindow::defaultInput()
{
    d_ptr->calcParam[0]->setText("12");
    d_ptr->calcParam[1]->setText("65");
    d_ptr->calcParam[2]->setText("8");
    d_ptr->calcParam[3]->setText("36");
    d_ptr->calcParam[4]->setText("46");
    d_ptr->calcParam[5]->setText("26");
    d_ptr->calcParam[6]->setText("256");
    d_ptr->calcParam[7]->setText("177");
    d_ptr->calcParam[8]->setText("65");
}

void HemodynamicWindow::resizeEvent(QResizeEvent *ev)
{
    QRect r = layoutManager.getMenuArea();
    QPoint globalTopLeft;
    if (layoutManager.getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        globalTopLeft = r.center() - this->rect().center();
    }
    else
    {
        r.adjust(r.width() - this->width(), 0, 0, 0);  // 菜单将靠右上显示
        globalTopLeft = r.topLeft();
    }
    move(globalTopLeft);
    Dialog::resizeEvent(ev);
}


void HemodynamicWindow::onBtnParamReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("btnItem").toInt();

    KeyInputPanel numberPad;
    QString strTitle(trs(HemodynamicSymbol::
                         convert(static_cast<HemodynamicParam>(index))));
    strTitle += " (";
    strTitle += trs("Unit");
    strTitle += ": ";
    strTitle += d_ptr->calcParamUnit[index]->text();
    strTitle += ")";

    numberPad.setWindowTitle(strTitle);
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(d_ptr->calcParam[index]->text());
    numberPad.setBtnEnable("[0-9]|[.]");

    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        float value = text.toFloat(&ok);
        if (ok)
        {
            if ((value < hemodynamicManager.inputLimit[index].low)
                    || (value > hemodynamicManager.inputLimit[index].high))
            {
                MessageBox messageBox(trs("Prompt"),
                                      trs("InvalidInput") + QString::number(hemodynamicManager.inputLimit[index].low)
                                      + "-" + QString::number(hemodynamicManager.inputLimit[index].high),
                                      QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else
            {
                d_ptr->calcParam[index]->setText(text);
                hemodynamicManager.setCalcInput(index, text.toFloat());
            }
        }
    }
}

void HemodynamicWindow::onCheckOutputReleased()
{
    setWindowTitle(trs("Output"));
    d_ptr->reference->setFocus();
    updateData();
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
    emit checkInputSignal(1);
}

void HemodynamicWindow::onCheckInputReleased()
{
    setWindowTitle(trs("Input"));
    setFixedSize(600, 404);
    emit checkOutputSignal(0);
}

void HemodynamicWindow::onRefRangeReleased()
{
    if (!d_ptr->refUnitFlag)
    {
        d_ptr->reference->setText(trs("TEMPUnit"));
        d_ptr->refUnitFlag = 1;
        for (int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {
            if (i <= HEMODYNAMIC_OUTPUT_EF)
            {
                d_ptr->outputTable->item(i, 2)->setText(HemodynamicSymbol::
                                                        convertRange((HemodynamicOutput)i));
            }
            else
            {
                d_ptr->outputTable->item(i - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA),
                                         5)->setText(HemodynamicSymbol::convertRange((HemodynamicOutput)i));
            }
        }
    }
    else
    {
        d_ptr->reference->setText(trs("ReferenceRange"));
        d_ptr->refUnitFlag = 0;
        for (int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
        {
            if (i <= HEMODYNAMIC_OUTPUT_EF)
            {
                d_ptr->outputTable->item(i, 2)->setText(HemodynamicSymbol::
                                                        convertUnit((HemodynamicOutput)i));
            }
            else
            {
                d_ptr->outputTable->item(i - static_cast<int>(HEMODYNAMIC_OUTPUT_BSA),
                                         5)->setText(HemodynamicSymbol::convertUnit((HemodynamicOutput)i));
            }
        }
    }
}

void HemodynamicWindow::onCalcReleased()
{
    hemodynamicManager.calcAction();
    onCheckOutputReleased();
    hemodynamicManager.insertReviewData(hemodynamicManager.getCalcValue());
    if (hemodynamicManager.getReviewData().length() != 0)
    {
        d_ptr->calcReviewA->setEnabled(true);
        d_ptr->calcReviewB->setEnabled(true);
    }
}

void HemodynamicWindow::onCalcReviewReleased()
{
    windowManager.showWindow(HemodynaimcReviewWindow::getInstance(),
                             WindowManager::ShowBehaviorModal);
}

HemodynamicWindow::HemodynamicWindow()
    : Dialog(),
      d_ptr(new HemodynamicWindowPrivate)
{
    layoutExec();
    initCalInput();
}
