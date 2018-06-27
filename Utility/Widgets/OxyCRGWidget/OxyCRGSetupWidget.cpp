#include "OxyCRGSetupWidget.h"
#include "IComboList.h"
#include "ISpinBox.h"
#include "SubMenu.h"
#include <QFont>
#include "ConfigManager.h"
#include "ConfigEditMenuGrp.h"
#include <QDesktopWidget>
#include <QApplication>

class OxyCRGSetupWidgetPrivate{
public:
    enum ComboType{
        COMBOTYPE_TREND_1=0,
        COMBOTYPE_TREND_2,
        COMBOTYPE_WAVE,
        COMBOTYPE_NR
    };
    enum RulerAdjType{
        RULERADJTYPE_HR_LOW=0,
        RULERADJTYPE_HR_HIGH,
        RULERADJTYPE_SPO2_LOW,
        RULERADJTYPE_SPO2_HIGH,
        RULERADJTYPE_CO2_LOW,
        RULERADJTYPE_CO2_HIGH,
        RULERADJTYPE_RESP_LOW,
        RULERADJTYPE_RESP_HIGH,
        RULERADJTYPE_NR
    };
    IComboList  *comboList[COMBOTYPE_NR];
    ISpinBox    *spinBox[RULERADJTYPE_NR];
    QVBoxLayout *conLayout;
    int _waveTypeIndex;
    union RulerValue{
        int data[8];
        struct RulerValueStruct{
            int hrLow;
            int hrHigh;
            int spo2Low;
            int spo2High;
            int co2Low;
            int co2High;
            int respLow;
            int respHigh;
        }s;
    }rulerValue;

    struct RulerUpdated{
        bool hrUpdatedStatus;
        bool spo2UpdatedStatus;
        bool co2UpdatedStatus;
        bool respUpdatedStatus;
    }rulerUpdated;
};

#define NUMLENS(arrName) (sizeof(arrName)/sizeof(arrName[0]))


OxyCRGSetupWidget::OxyCRGSetupWidget():PopupWidget(),
                                       d_ptr(new OxyCRGSetupWidgetPrivate())
{
    d_ptr->_waveTypeIndex = 0;
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    QString stringComboTypeName[]={
        "Trend1", "Trend2", "Wave"
    };
    QString stringRulerAdjTypeName[]={
        "HRLow", "HRHigh", "SPO2Low", "SPO2High",
        "CO2Low", "CO2High", "RESPLow", "RESPHigh"
    };
    QStringList comboItem[d_ptr->COMBOTYPE_NR];
    comboItem[d_ptr->COMBOTYPE_TREND_1].append(trs("HR_PR"));
    comboItem[d_ptr->COMBOTYPE_TREND_1].append(trs("RR"));

    comboItem[d_ptr->COMBOTYPE_TREND_2].append(trs("SPO2"));

    comboItem[d_ptr->COMBOTYPE_WAVE].append(trs("RESP"));
    comboItem[d_ptr->COMBOTYPE_WAVE].append(trs("CO2"));


    d_ptr->conLayout = new QVBoxLayout();
    d_ptr->conLayout->setContentsMargins(0, 0, 0, 0);

    for(unsigned i=0; i<NUMLENS(stringComboTypeName); i++)
    {
        d_ptr->comboList[i] = new IComboList(trs(stringComboTypeName[i]));
        d_ptr->comboList[i]->setFont(defaultFont());
        d_ptr->comboList[i]->addItems(comboItem[i]);
        d_ptr->comboList[i]->label->setFixedSize(labelWidth,ITEM_H);
        d_ptr->comboList[i]->combolist->setFixedSize(btnWidth,ITEM_H);
        d_ptr->comboList[i]->label->setAlignment(Qt::AlignHCenter);
        d_ptr->conLayout->addWidget(d_ptr->comboList[i], 0, Qt::AlignRight);

        connect(d_ptr->comboList[i], SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListUpdated(int)));
        d_ptr->comboList[i]->setProperty("comboid", qVariantFromValue(i));

        int index=0;
        currentConfig.getNumValue(QString("OxyCRG|%1").arg(stringComboTypeName[i]), index);
        d_ptr->comboList[i]->combolist->setCurrentIndex(index);
        index = 0;
    }

    int rulerRange[8][2]={{0,300}, {0,300}, {0,100}, {0,100},
                        {0,100}, {0,100}, {InvData(),InvData()}, {InvData(),InvData()}};
    for(unsigned i=0; i<NUMLENS(stringRulerAdjTypeName); i++)
    {
        d_ptr->spinBox[i] = new ISpinBox(trs(stringRulerAdjTypeName[i]));
        d_ptr->spinBox[i]->setFont(defaultFont());
        d_ptr->conLayout->addWidget(d_ptr->spinBox[i], 0, Qt::AlignRight);
        d_ptr->spinBox[i]->setLabelWidth(labelWidth);
        d_ptr->spinBox[i]->setValueWidth(btnWidth);
        d_ptr->spinBox[i]->setRange(rulerRange[i][0],rulerRange[i][1]);
        d_ptr->spinBox[i]->setLabelHeight(ITEM_H);
        d_ptr->spinBox[i]->setValueHeight(ITEM_H);
        d_ptr->spinBox[i]->setLabelAlignment(Qt::AlignHCenter);

        connect(d_ptr->spinBox[i], SIGNAL(valueChange(QString,int)), this, SLOT(onSpinBoxUpdated(QString, int)));
        d_ptr->spinBox[i]->setID(i);
        d_ptr->spinBox[i]->setProperty("spinboxid", qVariantFromValue(i));

        int index=0;
        currentConfig.getNumValue(QString("OxyCRG|Ruler|%1").arg(stringRulerAdjTypeName[i]), index);
        d_ptr->spinBox[i]->setValue(index);
        d_ptr->rulerValue.data[i] = index;
        index = 0;
    }
    d_ptr->conLayout->addStretch();
    d_ptr->spinBox[d_ptr->RULERADJTYPE_RESP_LOW]->setEnabled(false);
    d_ptr->spinBox[d_ptr->RULERADJTYPE_RESP_HIGH]->setEnabled(false);

    setTitleBarText(trs("OxyCRGWidgetSetup"));
    contentLayout->addLayout(d_ptr->conLayout);
}
OxyCRGSetupWidget::~OxyCRGSetupWidget()
{

}
void OxyCRGSetupWidget::onComboListUpdated(int index)
{
    IComboList *como = qobject_cast<IComboList*>(sender());
    int num = como->property("comboid").toInt();
    switch(num)
    {
        case OxyCRGSetupWidgetPrivate::COMBOTYPE_TREND_1:
            currentConfig.setNumValue("OxyCRG|Trend1", index);
        break;

        case OxyCRGSetupWidgetPrivate::COMBOTYPE_TREND_2:
            currentConfig.setNumValue("OxyCRG|Trend2", index);
        break;

        case OxyCRGSetupWidgetPrivate::COMBOTYPE_WAVE:
            setWaveTypeIndex(index);
            currentConfig.setNumValue("OxyCRG|Wave", index);
        break;
    }
}

void OxyCRGSetupWidget::setWaveTypeIndex(int index)
{
    if(index>=0 && index <2)
    {
        d_ptr->_waveTypeIndex = index;
    }
}

int OxyCRGSetupWidget::getWaveTypeIndex()const
{
    return d_ptr->_waveTypeIndex;
}

int OxyCRGSetupWidget::getHRLow(bool &status)const
{
    status = d_ptr->rulerUpdated.hrUpdatedStatus;
    return d_ptr->rulerValue.s.hrLow;
}
int OxyCRGSetupWidget::getHRHigh(bool &status)const
{
    status = d_ptr->rulerUpdated.hrUpdatedStatus;
    return d_ptr->rulerValue.s.hrHigh;
}
int OxyCRGSetupWidget::getSPO2Low(bool &status)const
{
    status = d_ptr->rulerUpdated.spo2UpdatedStatus;
    return d_ptr->rulerValue.s.spo2Low;
}
int OxyCRGSetupWidget::getSPO2High(bool &status)const
{
    status = d_ptr->rulerUpdated.spo2UpdatedStatus;
    return d_ptr->rulerValue.s.spo2High;
}
int OxyCRGSetupWidget::getCO2Low(bool &status)const
{
    status = d_ptr->rulerUpdated.co2UpdatedStatus;
    return d_ptr->rulerValue.s.co2Low;
}
int OxyCRGSetupWidget::getCO2High(bool &status)const
{
    status = d_ptr->rulerUpdated.co2UpdatedStatus;
    return d_ptr->rulerValue.s.co2High;
}
int OxyCRGSetupWidget::getRESPLow(bool &status)const
{
    status = d_ptr->rulerUpdated.respUpdatedStatus;
    return d_ptr->rulerValue.s.respLow;
}
int OxyCRGSetupWidget::getRESPHigh(bool &status)const
{
    status = d_ptr->rulerUpdated.respUpdatedStatus;
    return d_ptr->rulerValue.s.respHigh;
}
void OxyCRGSetupWidget::onSpinBoxUpdated(QString valueStr, int index)
{
    QString type;
    switch (index) {
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_HR_LOW:
            type = "HRLow";
            d_ptr->rulerValue.s.hrLow = valueStr.toInt();
            d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_HR_HIGH:
            type = "HRHigh";
            d_ptr->rulerValue.s.hrHigh = valueStr.toInt();
            d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_SPO2_LOW:
            type = "SPO2Low";
            d_ptr->rulerValue.s.spo2Low = valueStr.toInt();
            d_ptr->rulerUpdated.spo2UpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_SPO2_HIGH:
            type = "SPO2High";
            d_ptr->rulerValue.s.spo2High = valueStr.toInt();
            d_ptr->rulerUpdated.spo2UpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_CO2_LOW:
            type = "CO2Low";
            d_ptr->rulerValue.s.co2Low = valueStr.toInt();
            d_ptr->rulerUpdated.co2UpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_CO2_HIGH:
            type = "CO2High";
            d_ptr->rulerValue.s.co2High = valueStr.toInt();
            d_ptr->rulerUpdated.co2UpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_RESP_LOW:
            type = "RESPLow";
            d_ptr->rulerValue.s.respLow = valueStr.toInt();
            d_ptr->rulerUpdated.respUpdatedStatus = true;
        break;
        case OxyCRGSetupWidgetPrivate::RULERADJTYPE_RESP_HIGH:
            type = "RESPHigh";
            d_ptr->rulerValue.s.respHigh = valueStr.toInt();
            d_ptr->rulerUpdated.respUpdatedStatus = true;
        break;
    }
    currentConfig.setNumValue(QString("OxyCRG|Ruler|%1").arg(type), valueStr.toInt());
}




