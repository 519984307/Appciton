/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/30
 **/
#include "TitrateTableWindow.h"
#include "TitrateTableModel.h"
#include "DoseCalculationDefine.h"
#include "DoseCalculationManager.h"
#include "TitrateTableManager.h"
#include "TableView.h"
#include <QLabel>
#include <QEvent>
#include "Button.h"
#include <QAbstractSlider>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include "WindowManager.h"
#include <QHeaderView>
#include "TableHeaderView.h"
#include "TableViewItemDelegate.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include "Framework/Language/LanguageManager.h"
#include "TitrateTableSetWindow.h"
#include "LayoutManager.h"

#define MAX_DOSE                10000
#define MAX_INFUSIONRATE        1000
#define MAX_DRIPRATE            1000
#define TABLE_ROW_NUM           6

class TitrateTableWindowPrivate
{
public:
    TitrateTableWindowPrivate();
    QList<TitrateTableInfo> titrateTableInfos;
    TableView *titrateTable;
    QLabel *paramName[TITRATE_TABLE_NR];
    QLabel *paramValue[TITRATE_TABLE_NR];
    QLabel *paramUnit[TITRATE_TABLE_NR];
    Button *btnUp;
    Button *btnDown;
    Button *btnTableSet;
    Button *btnRecord;
    int curScroller;
    TitrateTableModel *model;
};

TitrateTableWindowPrivate::TitrateTableWindowPrivate()
    : titrateTable(NULL),
      btnUp(NULL),
      btnDown(NULL),
      btnTableSet(NULL),
      btnRecord(NULL),
      curScroller(0),
      model(NULL)
{
    titrateTableInfos.clear();
    for (int i = 0; i < TITRATE_TABLE_NR; i++)
    {
        paramName[i] = NULL;
        paramValue[i] = NULL;
        paramUnit[i] = NULL;
    }
}

TitrateTableWindow::TitrateTableWindow()
    : Dialog(),
      d_ptr(new TitrateTableWindowPrivate)
{
    layoutExecIt();
    updateTitrateTableData();
}

TitrateTableWindow::~TitrateTableWindow()
{
    delete d_ptr;
}

TitrateTableWindow *TitrateTableWindow::getInstance()
{
    static TitrateTableWindow *instance = NULL;
    if (!instance)
    {
        instance = new TitrateTableWindow;
    }
    return instance;
}

void TitrateTableWindow::layoutExecIt()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    QGridLayout *gl = new QGridLayout;
    for (int i = 0; i < TITRATE_TABLE_DOSE; i++)
    {
        label = new QLabel(
            trs(DoseCalculationSymbol::convert(DoseCalculationSymbol::convert(static_cast<TitrateTableParam>(i)))));
        gl->addWidget(label, i, 0);
        d_ptr->paramName[i] = label;
        label = new QLabel();
        gl->addWidget(label, i, 1);
        d_ptr->paramValue[i] = label;
        label = new QLabel();
        gl->addWidget(label, i, 2);
        d_ptr->paramUnit[i] = label;
    }
    gl->setColumnMinimumWidth(3, 100);
    for (int i = TITRATE_TABLE_DOSE; i < TITRATE_TABLE_NR; i++)
    {
        label = new QLabel(
            trs(DoseCalculationSymbol::convert(DoseCalculationSymbol::convert(static_cast<TitrateTableParam>(i)))));
        gl->addWidget(label, (i - TITRATE_TABLE_DOSE), 4);
        d_ptr->paramName[i] = label;
        label = new QLabel();
        gl->addWidget(label, (i - TITRATE_TABLE_DOSE), 5);
        d_ptr->paramValue[i] = label;
        label = new QLabel();
        gl->addWidget(label, (i - TITRATE_TABLE_DOSE), 6);
        d_ptr->paramUnit[i] = label;
    }
    layout->addLayout(gl, 0, 0);

    d_ptr->titrateTable = new TableView();
    d_ptr->titrateTable->setSelectionMode(QAbstractItemView::NoSelection);
    d_ptr->titrateTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->titrateTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    TableHeaderView *hor = new TableHeaderView(Qt::Horizontal);
    d_ptr->titrateTable->setHorizontalHeader(hor);
    d_ptr->titrateTable->horizontalHeader()->setClickable(false);
    d_ptr->titrateTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->titrateTable->verticalHeader()->setHidden(true);
    d_ptr->titrateTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->titrateTable->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                        + d_ptr->model->getRowHeightHint() * TABLE_ROW_NUM);
    d_ptr->titrateTable->setFixedWidth(770);
    d_ptr->model = new TitrateTableModel;
    d_ptr->titrateTable->setModel(d_ptr->model);
    d_ptr->titrateTable->setItemDelegate(new TableViewItemDelegate(this));
    d_ptr->titrateTable->viewport()->installEventFilter(d_ptr->model);
    layout->addWidget(d_ptr->titrateTable, 1, 0);

    Button *button;
    QHBoxLayout *hl = new QHBoxLayout;
    button = new Button(trs("TitrateTableSet"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnTableSetReleased()));
    d_ptr->btnTableSet = button;
    hl->addWidget(button);

    button = new Button(trs("Record"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnRecordReleased()));
    d_ptr->btnRecord = button;
    hl->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowUp.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnUpReleased()));
    d_ptr->btnUp = button;
    hl->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowDown.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnDownReleased()));
    d_ptr->btnDown = button;
    hl->addWidget(button);

    layout->addLayout(hl, 2, 0);
    layout->setRowStretch(3, 1);
    setWindowLayout(layout);
}

void TitrateTableWindow::updateTitrateTableData()
{
    // 清空表格内容
    d_ptr->titrateTable->clearSpans();
    d_ptr->titrateTableInfos.clear();
    // 刷新窗口名称
    setWindowTitle(trs("TitrationTable")
                   + "-" + trs(DoseCalculationSymbol::convert(doseCalculationManager.getDrugName())));

    // 刷新刷新参数栏
    float paramValue;
    for (int i = 0; i < TITRATE_TABLE_NR; i ++)
    {
        if (i == TITRATE_TABLE_DOSE)
        {
            paramValue = doseCalculationManager.getDrugParam(
                             DoseCalculationSymbol::convert(
                                 static_cast<TitrateTableParam>(i)) +
                             static_cast<int>(titrateTableManager.getSetTableParam().doseType)).value;
            d_ptr->paramUnit[i]->setText(doseCalculationManager.getDrugParam(
                                             DoseCalculationSymbol::convert(
                                                 static_cast<TitrateTableParam>(i)) +
                                             static_cast<int>(titrateTableManager.getSetTableParam().doseType)).unit);
        }
        else
        {
            paramValue = doseCalculationManager.getDrugParam(
                             DoseCalculationSymbol::convert(
                                 static_cast<TitrateTableParam>(i))).value;
            d_ptr->paramUnit[i]->setText(doseCalculationManager.getDrugParam(
                                             DoseCalculationSymbol::convert(
                                                 static_cast<TitrateTableParam>(i))).unit);
        }

        if (paramValue < 0)
        {
            d_ptr->paramValue[i]->setText("---");
        }
        else
        {
            d_ptr->paramValue[i]->setText(QString::number(paramValue, 'f', 2));
        }
    }

    if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_DRIPRATE)
    {
        datumTermDripRate();
    }
    else if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_DOSE)
    {
        datumTermDose();
    }
    else if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_INFUSIONRATE)
    {
        datumTermInfusionRate();
    }
    d_ptr->model->setupTitrateTableInfos(d_ptr->titrateTableInfos);
}

void TitrateTableWindow::datumTermDose()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    DrugInfo tmp = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION);
    concentration.unit = tmp.unit.left(tmp.unit.length() - 3);
    concentration.value = tmp.value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                              DoseCalculationSymbol::convert(
                                  static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                              static_cast<int>(titrateTableManager.getSetTableParam().doseType)).unit;
    switch (static_cast<int>(titrateTableManager.getSetTableParam().doseType))
    {
    case DOSE_TYPE_MIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 4);
        break;
    }
    case DOSE_TYPE_H:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 2);
        break;
    }
    case DOSE_TYPE_KGMIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 7);
        break;
    }
    case DOSE_TYPE_KGH:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 5);
        break;
    }
    }
    doseCalculationManager.specifyUnit(concentration, convertUnit);

    // 计算出行数，项数
    // 最大结果值算出基准值的最大值
    float maxDose = concentration.value * MAX_INFUSIONRATE;

    if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxDose = maxDose / 60;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxDose = maxDose / 60 / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxDose = maxDose / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

    // 判断基准值是否超过最大值
    if (maxDose > MAX_DOSE)
    {
        maxDose = MAX_DOSE;
    }

    float itemCount = maxDose / (titrateTableManager.getSetTableParam().step + 1);
    if (itemCount - static_cast<int>(itemCount) > 0)
    {
        itemCount = static_cast<int>(itemCount) + 1;
    }
    else
    {
        itemCount = static_cast<int>(itemCount);
    }

    // 填入数据项
    float doseValue = 0;
    float infusionRateValue = 0;

    TitrateTableInfo dataInfos;
    DoseType doseType = titrateTableManager.getSetTableParam().doseType;
    for (int i = 0; i < itemCount; i ++)
    {
        switch (doseType)
        {
        case DOSE_TYPE_MIN:
            infusionRateValue = doseValue * 60 / concentration.value;
            break;

        case DOSE_TYPE_H:
            infusionRateValue = doseValue / concentration.value;
            break;

        case DOSE_TYPE_KGMIN:
            infusionRateValue = doseValue * 60 *  doseCalculationManager.
                                getDrugParam(CALCULATION_PARAM_WEIGHT).value / concentration.value;
            break;

        case DOSE_TYPE_KGH:
            infusionRateValue = doseValue * doseCalculationManager.
                                getDrugParam(CALCULATION_PARAM_WEIGHT).value / concentration.value;
            break;

        case DOSE_TYPE_NR:
            break;
        };

        dataInfos.doseValue = QString::number(doseValue, 'f', 2);
        dataInfos.rateVlue = QString::number(infusionRateValue, 'f', 2);
        d_ptr->titrateTableInfos.append(dataInfos);
        doseValue = doseValue + (titrateTableManager.getSetTableParam().step + 1);
    }
}

void TitrateTableWindow::datumTermInfusionRate()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    concentration.unit = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(
                             doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
    concentration.value = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                              DoseCalculationSymbol::convert(
                                  static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                              static_cast<int>(titrateTableManager.getSetTableParam().doseType)).unit;
    switch (static_cast<int>(titrateTableManager.getSetTableParam().doseType))
    {
    case DOSE_TYPE_MIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 4);
        break;
    }
    case DOSE_TYPE_H:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 2);
        break;
    }
    case DOSE_TYPE_KGMIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 7);
        break;
    }
    case DOSE_TYPE_KGH:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 5);
        break;
    }
    }
    doseCalculationManager.specifyUnit(concentration, convertUnit);

    // 计算出行数，项数
    float maxInfusionRate = MAX_DOSE / concentration.value;
    if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxInfusionRate = maxInfusionRate * 60;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxInfusionRate = maxInfusionRate * 60 * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxInfusionRate = maxInfusionRate * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

    if (maxInfusionRate > MAX_INFUSIONRATE)
    {
        maxInfusionRate = MAX_INFUSIONRATE;
    }

    float itemCount = maxInfusionRate / (titrateTableManager.getSetTableParam().step + 1);
    if (itemCount - static_cast<int>(itemCount) > 0)
    {
        itemCount = static_cast<int>(itemCount) + 1;
    }
    else
    {
        itemCount = static_cast<int>(itemCount);
    }


    // 填入数据项
    float doseValue = 0;
    float infusionRateValue = 0;

    TitrateTableInfo dataInfos;
    DoseType doseType = titrateTableManager.getSetTableParam().doseType;
    for (int i = 0; i < itemCount; i ++)
    {
        switch (doseType)
        {
        case DOSE_TYPE_MIN:
            doseValue = infusionRateValue * concentration.value / 60;
            break;

        case DOSE_TYPE_H:
            doseValue = infusionRateValue * concentration.value;
            break;

        case DOSE_TYPE_KGMIN:
            doseValue = infusionRateValue * concentration.value /
                        60 / doseCalculationManager.getDrugParam(
                            CALCULATION_PARAM_WEIGHT).value;
            break;

        case DOSE_TYPE_KGH:
            doseValue = infusionRateValue * concentration.value /
                        doseCalculationManager.getDrugParam(
                            CALCULATION_PARAM_WEIGHT).value;
            break;

        case DOSE_TYPE_NR:
            break;
        };

        dataInfos.doseValue = QString::number(doseValue, 'f', 2);
        dataInfos.rateVlue = QString::number(infusionRateValue, 'f', 2);
        d_ptr->titrateTableInfos.append(dataInfos);
        infusionRateValue = infusionRateValue + (titrateTableManager.getSetTableParam().step + 1);
    }
}

void TitrateTableWindow::datumTermDripRate()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    concentration.unit = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(
                             doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
    concentration.value = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                              DoseCalculationSymbol::convert(
                                  static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                              static_cast<int>(titrateTableManager.getSetTableParam().doseType)).unit;
    switch (static_cast<int>(titrateTableManager.getSetTableParam().doseType))
    {
    case DOSE_TYPE_MIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 4);
        break;
    }
    case DOSE_TYPE_H:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 2);
        break;
    }
    case DOSE_TYPE_KGMIN:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 7);
        break;
    }
    case DOSE_TYPE_KGH:
    {
        convertUnit = convertUnit.left(convertUnit.length() - 5);
        break;
    }
    }
    doseCalculationManager.specifyUnit(concentration, convertUnit);

    // 计算出行数，项数
    float maxDripRate = MAX_DOSE / concentration.value *
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value / 60;
    if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxDripRate = maxDripRate * 60;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxDripRate = maxDripRate * 60 * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if (titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxDripRate = maxDripRate * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

    if (maxDripRate > MAX_DRIPRATE)
    {
        maxDripRate = MAX_DRIPRATE;
    }

    float itemCount = maxDripRate / (titrateTableManager.getSetTableParam().step + 1);
    if (itemCount - static_cast<int>(itemCount) > 0)
    {
        itemCount = static_cast<int>(itemCount) + 1;
    }
    else
    {
        itemCount = static_cast<int>(itemCount);
    }

    // 填入数据项
    float doseValue = 0;
    float dripRateValue = 0;

    TitrateTableInfo dataInfos;
    DoseType doseType = titrateTableManager.getSetTableParam().doseType;
    for (int i = 0; i < itemCount; i ++)
    {
        switch (doseType)
        {
        case DOSE_TYPE_MIN:
            doseValue = dripRateValue * 60 /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                        concentration.value / 60;
            break;

        case DOSE_TYPE_H:
            doseValue = dripRateValue * 60 /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                        concentration.value;
            break;

        case DOSE_TYPE_KGMIN:
            doseValue = dripRateValue * 60 /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                        concentration.value / 60 /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            break;

        case DOSE_TYPE_KGH:
            doseValue = dripRateValue * 60 /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                        concentration.value /
                        doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            break;

        case DOSE_TYPE_NR:
            break;
        };

        dataInfos.doseValue = QString::number(doseValue, 'f', 2);
        dataInfos.rateVlue = QString::number(dripRateValue, 'f', 2);
        d_ptr->titrateTableInfos.append(dataInfos);
        dripRateValue = dripRateValue + (titrateTableManager.getSetTableParam().step + 1);
    }
}


void TitrateTableWindow::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);

    // 居中显示。
    QRect r = layoutManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}


void TitrateTableWindow::onBtnUpReleased()
{
    int maxValue = d_ptr->titrateTable->verticalScrollBar()->maximum();
    d_ptr->curScroller = d_ptr->titrateTable->verticalScrollBar()->value();

    if (d_ptr->curScroller > 0)
    {
        d_ptr->titrateTable->verticalScrollBar()->setSliderPosition(
            d_ptr->curScroller - MAX_ROWCOUNT);
    }
    else
    {
        d_ptr->titrateTable->verticalScrollBar()->setSliderPosition(
            maxValue);
    }
}

void TitrateTableWindow::onBtnDownReleased()
{
    int maxValue = d_ptr->titrateTable->verticalScrollBar()->maximum();
    d_ptr->curScroller = d_ptr->titrateTable->verticalScrollBar()->value();

    if (d_ptr->curScroller < maxValue)
    {
        d_ptr->titrateTable->verticalScrollBar()->setSliderPosition(
            d_ptr->curScroller + MAX_ROWCOUNT);
    }
    else
    {
        d_ptr->titrateTable->verticalScrollBar()->setSliderPosition(
            0);
    }
}

void TitrateTableWindow::onBtnTableSetReleased()
{
    TitrateTableSetWindow::getInstance()->exec();
}

void TitrateTableWindow::onBtnRecordReleased()
{
}
