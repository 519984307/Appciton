#include "TitrateTableWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "DoseCalculationManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollBar>
#include "IButton.h"
#include "ITableWidget.h"
#include "LanguageManager.h"
#include "TitrateTableManager.h"
#include "TitrateTableSetWidget.h"

#define MAX_DOSE                10000
#define MAX_INFUSIONRATE        1000
#define MAX_DRIPRATE            1000
#define MAX_ROWCOUNT            9
#define TITRATETABLE_HEIGHT     250

TitrateTableWidget *TitrateTableWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TitrateTableWidget::~TitrateTableWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void TitrateTableWidget::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight());

    int submenuW = windowManager.getPopMenuWidth();

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);
    int btnWidth = itemW / 4;
//    int labelWidth = btnWidth/2;

    QGridLayout *glayout = new QGridLayout();
    glayout->setSpacing(0);
    for(int i = 0; i < TITRATE_TABLE_NR; i ++)
    {
        _paramName[i] = new QLabel(trs(DoseCalculationSymbol::convert(DoseCalculationSymbol::convert(static_cast<TitrateTableParam>(i)))));
        _paramValue[i] = new QLabel();
        _paramUnit[i] = new QLabel();
        _paramName[i]->setFont(font);
        _paramName[i]->setAlignment(Qt::AlignCenter);
        _paramValue[i]->setFont(font);
        _paramValue[i]->setAlignment(Qt::AlignCenter);
        _paramUnit[i]->setFont(font);
        _paramUnit[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        _paramName[i]->setFixedHeight(_itemH);
        _paramValue[i]->setFixedHeight(_itemH);
        _paramUnit[i]->setFixedHeight(_itemH);
        if(i < TITRATE_TABLE_DOSE)
        {
            glayout->addWidget(_paramName[i], i, 0, 1, 1);
            glayout->addWidget(_paramValue[i], i, 1, 1, 1);
            glayout->addWidget(_paramUnit[i], i, 2, 1, 1);
        }
        else
        {
            glayout->addWidget(_paramName[i], i - 3, 3, 1, 1);
            glayout->addWidget(_paramValue[i], i - 3, 4, 1, 1);
            glayout->addWidget(_paramUnit[i], i - 3, 5, 1, 1);
        }
    }

    _titrateTable = new ITableWidget();
    _titrateTable->setFixedWidth(itemW);
    _titrateTable->setSelectionMode(QAbstractItemView::NoSelection);            // 不能选择
    _titrateTable->setFocusPolicy(Qt::NoFocus);
//    _titrateTable->setSelectionBehavior(QAbstractItemView::SelectRows);
//    _titrateTable->setRowCount(9);
    _titrateTable->setColumnCount(6);
    _titrateTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _titrateTable->horizontalHeader()->setClickable(false);
    _titrateTable->verticalHeader()->setHidden(true);
    _titrateTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    _titrateTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    _titrateTable->setFixedHeight(TITRATETABLE_HEIGHT);

    _tableSet = new IButton(trs("TitrateTableSet"));
    _tableSet->setFixedSize(btnWidth, _itemH);
    _tableSet->setFont(font);
    _tableSet->setBorderEnabled(true);
    connect(_tableSet, SIGNAL(realReleased()), this, SLOT(_tableSetReleased()));

    _record = new IButton(trs("Record"));
    _record->setFixedSize(btnWidth, _itemH);
    _record->setFont(font);
    _record->setBorderEnabled(true);
    connect(_record, SIGNAL(realReleased()), this, SLOT(_recordReleased()));

    _up = new IButton();
    _up->setFixedSize(_itemH - 2, _itemH - 2);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(_itemH - 2, _itemH - 2);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_tableSet);
    hlayout->addWidget(_record);
    hlayout->addWidget(_up);
    hlayout->addWidget(_down);

    contentLayout->addLayout(glayout);
    contentLayout->addWidget(_titrateTable);
    contentLayout->addStretch();
    contentLayout->addLayout(hlayout);
    contentLayout->addStretch();
}

/**************************************************************************************************
 * 功能： 根据设置项参数，更新滴定表内数据。
 *************************************************************************************************/
void TitrateTableWidget::updateTitrateTableData()
{
    // 清空表格内容
    _titrateTable->clearContents();

    // 刷新窗口名称
    setTitleBarText(trs("TitrationTable") + "-" + trs(DoseCalculationSymbol::convert(doseCalculationManager.getDrugName())));

    // 刷新刷新参数栏
    float paramValue;
    for(int i = 0; i < TITRATE_TABLE_NR; i ++)
    {
        if (i == TITRATE_TABLE_DOSE)
        {
            paramValue = doseCalculationManager.getDrugParam(
                        DoseCalculationSymbol::convert(
                            static_cast<TitrateTableParam>(i)) +
                        (int)titrateTableManager.getSetTableParam().doseType).value;
            _paramUnit[i]->setText(doseCalculationManager.getDrugParam(
                                       DoseCalculationSymbol::convert(
                                           static_cast<TitrateTableParam>(i)) +
                                       (int)titrateTableManager.getSetTableParam().doseType).unit);
        }
        else
        {
            paramValue = doseCalculationManager.getDrugParam(
                        DoseCalculationSymbol::convert(
                            static_cast<TitrateTableParam>(i))).value;
            _paramUnit[i]->setText(doseCalculationManager.getDrugParam(
                                        DoseCalculationSymbol::convert(
                                            static_cast<TitrateTableParam>(i))).unit);
        }


        if (paramValue < 0)
        {
            _paramValue[i]->setText("---");
        }
        else
        {
            _paramValue[i]->setText(QString::number(paramValue, 'f', 2));
        }

    }

    // 刷新表格
    QStringList headlist;
    if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_DRIPRATE)
    {
        headlist << trs("Dosemin") << trs("DripRate")
                    << trs("Dosemin") << trs("DripRate")
                       << trs("Dosemin") << trs("DripRate");
        datumTermDripRate();

    }
    else
    {
        headlist << trs("Dosemin") << trs("InfusionRate")
                    << trs("Dosemin") << trs("InfusionRate")
                       << trs("Dosemin") << trs("InfusionRate");
        if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_DOSE)
        {
            datumTermDose();
        }
        else if (titrateTableManager.getSetTableParam().datumTerm == DATUM_TERM_INFUSIONRATE)
        {
            datumTermInfusionRate();
        }
    }
    _titrateTable->setHorizontalHeaderLabels(headlist);
}

/**************************************************************************************************
 * 功能： 计算基准项为剂量时。
 *************************************************************************************************/
void TitrateTableWidget::datumTermDose()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    concentration.unit = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
    concentration.value = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                DoseCalculationSymbol::convert(
                    static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                (int)titrateTableManager.getSetTableParam().doseType).unit;
    switch ((int)titrateTableManager.getSetTableParam().doseType)
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

    if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxDose = maxDose / 60;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxDose = maxDose / 60 / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxDose = maxDose / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

        // 判断基准值是否超过最大值
    if (maxDose > MAX_DOSE)
    {
        maxDose = MAX_DOSE;
    }

    float itemCount = maxDose/TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    if (itemCount - (int)itemCount > 0)
    {
        itemCount = (int)itemCount + 1;
    }
    else
    {
        itemCount = (int)itemCount;
    }

    int rowCount = (int)itemCount/(MAX_ROWCOUNT * 3) * MAX_ROWCOUNT;
    if (((int)itemCount % (MAX_ROWCOUNT * 3)) > 0)
    {
        rowCount = rowCount + MAX_ROWCOUNT;
    }

    _titrateTable->setRowCount(rowCount);

    // 填入数据项
    float doseValue = 0;
    float infusionRateValue;
    int sumFlag = 0;
    int row = 0;
    int rowFlag = 0;
    int twoColumn = 0;
    int threeColumn = 0;

    QTableWidgetItem *item;

    for (int i = 0; i < itemCount; i ++)
    {
        if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
        {
            infusionRateValue = doseValue * 60/ concentration.value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_H)
        {
            infusionRateValue = doseValue / concentration.value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
        {
            infusionRateValue = doseValue * 60 *  doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value/ concentration.value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
        {
            infusionRateValue = doseValue * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value / concentration.value;
        }
        if(sumFlag < MAX_ROWCOUNT)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 2)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 2,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 3,item);

            twoColumn ++;
            sumFlag ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 3)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 4,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 5,item);

            threeColumn ++;
            sumFlag ++;
        }
        else
        {
            rowFlag = row;
            twoColumn = 0;
            threeColumn = 0;
            sumFlag = 0;
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        doseValue = doseValue + TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    }
}

/**************************************************************************************************
 * 功能： 计算基准项为输液速度时。
 *************************************************************************************************/
void TitrateTableWidget::datumTermInfusionRate()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    concentration.unit = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
    concentration.value = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                DoseCalculationSymbol::convert(
                    static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                (int)titrateTableManager.getSetTableParam().doseType).unit;
    switch ((int)titrateTableManager.getSetTableParam().doseType)
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
    if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxInfusionRate = maxInfusionRate * 60;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxInfusionRate = maxInfusionRate * 60 * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxInfusionRate = maxInfusionRate * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

    if (maxInfusionRate > MAX_INFUSIONRATE)
    {
        maxInfusionRate = MAX_INFUSIONRATE;
    }

    float itemCount = maxInfusionRate/TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    if (itemCount - (int)itemCount > 0)
    {
        itemCount = (int)itemCount + 1;
    }
    else
    {
        itemCount = (int)itemCount;
    }

    int rowCount = (int)itemCount/(MAX_ROWCOUNT * 3) * MAX_ROWCOUNT;
    if (((int)itemCount % (MAX_ROWCOUNT * 3)) > 0)
    {
        rowCount = rowCount + MAX_ROWCOUNT;
    }

    _titrateTable->setRowCount(rowCount);

    // 填入数据项
    float doseValue = 0;
    float infusionRateValue;
    int sumFlag = 0;
    int row = 0;
    int rowFlag = 0;
    int twoColumn = 0;
    int threeColumn = 0;
    QTableWidgetItem *item;

    for (int i = 0; i < itemCount; i ++)
    {
        if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
        {
            doseValue = infusionRateValue * concentration.value / 60;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_H)
        {
            doseValue = infusionRateValue * concentration.value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
        {
            doseValue = infusionRateValue * concentration.value / 60 / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
        {
            doseValue = infusionRateValue * concentration.value / doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
        }
        if(sumFlag < MAX_ROWCOUNT)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 2)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 2,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 3,item);

            twoColumn ++;
            sumFlag ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 3)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 4,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 5,item);

            threeColumn ++;
            sumFlag ++;
        }
        else
        {
            rowFlag = row;
            twoColumn = 0;
            threeColumn = 0;
            sumFlag = 0;
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(infusionRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        infusionRateValue = infusionRateValue + TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    }

}

/**************************************************************************************************
 * 功能： 计算基准项为滴速时。
 *************************************************************************************************/
void TitrateTableWidget::datumTermDripRate()
{
    // 换算出浓度对应剂量的单位和值
    DrugInfo concentration;
    concentration.unit = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
    concentration.value = doseCalculationManager.getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
    QString convertUnit = doseCalculationManager.getDrugParam(
                DoseCalculationSymbol::convert(
                    static_cast<TitrateTableParam>(TITRATE_TABLE_DOSE)) +
                (int)titrateTableManager.getSetTableParam().doseType).unit;
    switch ((int)titrateTableManager.getSetTableParam().doseType)
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
    if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
    {
        maxDripRate = maxDripRate * 60;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
    {
        maxDripRate = maxDripRate * 60 * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }
    else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
    {
        maxDripRate = maxDripRate * doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
    }

    if (maxDripRate > MAX_DRIPRATE)
    {
        maxDripRate = MAX_DRIPRATE;
    }

    float itemCount = maxDripRate/TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    if (itemCount - (int)itemCount > 0)
    {
        itemCount = (int)itemCount + 1;
    }
    else
    {
        itemCount = (int)itemCount;
    }

    int rowCount = (int)itemCount/(MAX_ROWCOUNT * 3) * MAX_ROWCOUNT;
    if (((int)itemCount % (MAX_ROWCOUNT * 3)) > 0)
    {
        rowCount = rowCount + MAX_ROWCOUNT;
    }

    _titrateTable->setRowCount(rowCount);

    // 填入数据项
    float doseValue = 0;
    float dripRateValue;
    int sumFlag = 0;
    int row = 0;
    int rowFlag = 0;
    int twoColumn = 0;
    int threeColumn = 0;
    QTableWidgetItem *item;

    for (int i = 0; i < itemCount; i ++)
    {
        if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_MIN)
        {
            doseValue = dripRateValue * 60 /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                    concentration.value / 60;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_H)
        {
            doseValue = dripRateValue * 60 /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                    concentration.value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGMIN)
        {
            doseValue = dripRateValue * 60 /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                    concentration.value / 60 /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
        }
        else if(titrateTableManager.getSetTableParam().doseType == DOSE_TYPE_KGH)
        {
            doseValue = dripRateValue * 60 /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_DROPSIZE).value *
                    concentration.value /
                    doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value;
        }
        if(sumFlag < MAX_ROWCOUNT)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(dripRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 2)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 2,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(dripRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + twoColumn, 3,item);

            twoColumn ++;
            sumFlag ++;
        }
        else if (sumFlag < MAX_ROWCOUNT * 3)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 4,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(dripRateValue, 'f', 2));
            _titrateTable->setItem(rowFlag + threeColumn, 5,item);

            threeColumn ++;
            sumFlag ++;
        }
        else
        {
            rowFlag = row;
            twoColumn = 0;
            threeColumn = 0;
            sumFlag = 0;
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(doseValue, 'f', 2));
            _titrateTable->setItem(row, 0,item);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString::number(dripRateValue, 'f', 2));
            _titrateTable->setItem(row, 1,item);

            sumFlag ++;
            row ++;
        }
        dripRateValue = dripRateValue + TitrateTableDefine::convert(titrateTableManager.getSetTableParam().step);
    }
}

/**************************************************************************************************
 * 功能： 显示事件。
 *************************************************************************************************/
void TitrateTableWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

/**************************************************************************************************
 * 功能： 向上翻页滴定表格。
 *************************************************************************************************/
static int curScroller = 0;
void TitrateTableWidget::_upReleased()
{
    int maxValue = _titrateTable->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    curScroller = _titrateTable->verticalScrollBar()->value();

    if(curScroller>0)
    {
        _titrateTable->verticalScrollBar()->setSliderPosition(curScroller-MAX_ROWCOUNT);
    }
    else
    {
        _titrateTable->verticalScrollBar()->setSliderPosition(maxValue);
    }
}

/**************************************************************************************************
 * 功能： 向下翻页滴定表格。
 *************************************************************************************************/
void TitrateTableWidget::_downReleased()
{
    int maxValue = _titrateTable->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    curScroller = _titrateTable->verticalScrollBar()->value(); //获得当前scroller值

    if(curScroller<maxValue)
    {
        _titrateTable->verticalScrollBar()->setSliderPosition(MAX_ROWCOUNT+curScroller);
    }
    else
    {
        _titrateTable->verticalScrollBar()->setSliderPosition(0);
    }
}

/**************************************************************************************************
 * 功能： 弹出滴定表设置参数窗口。
 *************************************************************************************************/
void TitrateTableWidget::_tableSetReleased()
{
    titrateTableSetWidget.autoShow();
}

/**************************************************************************************************
 * 功能： 记录按键槽函数。
 *************************************************************************************************/
void TitrateTableWidget::_recordReleased()
{

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TitrateTableWidget::TitrateTableWidget() : PopupWidget()
{
    layoutExec();
}
