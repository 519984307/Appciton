/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/7/31
 **/

#include "PatientInfoWindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QStringList>
#include "MessageBox.h"
#include <QHBoxLayout>
#include "EnglishInputPanel.h"
#include "KeyInputPanel.h"
#include "PatientManager.h"
#include "DischargePatientWindow.h"
#include <QMap>
#include "IConfig.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/SpinBox.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Utility/Unit.h"
#include "Framework/TimeDate/TimeDate.h"
#include "Framework/TimeDate/TimeSymbol.h"
#include "Framework/Language/LanguageManager.h"
#include <QDate>
#include "FloatHandle.h"
#include "RunningStatusBar.h"
#include "SystemManager.h"

#define PATIENT_BORN_DATE_RANAGE 1900
class PatientInfoWindowPrivate
{
public:
    PatientInfoWindowPrivate() :
        id(NULL) , type(NULL) , sex(NULL)
        , blood(NULL) , name(NULL) , pacer(NULL)
        , height(NULL) , weight(NULL)
        , cancel(NULL) , savePatientInfo(NULL)
        , heightLbl(NULL)
        , weightLbl(NULL)
        , bornDateLbl(NULL)
        , bedNum(NULL)
        , heightType(UNIT_NONE)
        , weightType(UNIT_NONE)
    {}

    enum MenuItem
    {
        ITEM_CBO_PATIENT_TYPE = 1,
        ITEM_BTN_PATIENT_BED,
        ITEM_CBO_PACER_MARKER,
        ITEM_BTN_PATIENT_ID,
        ITEM_CBO_PATIENT_SEX,
        ITEM_CBO_BLOOD_TYPE,
        ITEM_SPIN_BORN_FIRST,
        ITEM_SPIN_BORN_SECOND,
        ITEM_SPIN_BORN_THIRD,
        ITEM_BTN_PATIENT_NAME,
        ITEM_BTN_PATIENT_HEIGHT,
        ITEM_BTN_PATIENT_WEIGHT,
        ITEM_BTN_CREATE_PATIENT,
        ITEM_BTN_CANCEL
    };

    enum BornDate
    {
        Born_Date_Year,
        Born_Date_Month,
        Born_Date_Day
    };

    /**
     * @brief loadOptions 加载选项信息
     */
    void loadOptions();
    Button *id;                      // ID。
    ComboBox *type;                  // 病人类型。
    ComboBox *sex;                   // 性别。
    ComboBox *blood;                 // 血型。
    Button *name;                    // 姓名。
    ComboBox *pacer;                 // 起搏分析。
    Button *height;                  // 身高。
    Button *weight;                  // 体重。
    Button *cancel;
    Button *savePatientInfo;         // 保存病人信息
    QLabel *heightLbl;
    QLabel *weightLbl;
    QLabel *bornDateLbl;
    Button *bedNum;                  // 保存病床号

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem , Button *> buttons;
    QMap<MenuItem, SpinBox *> spinBoxs;

    UnitType heightType;
    UnitType weightType;

    QMap<BornDate, SpinBox *> dateItem;       // 记录年月日排列顺序

    /**
     * @brief savePatientInfoToManager 保存病人信息到病人配置
     */
    void savePatientInfoToManager(void);

    /**
     * @brief refreshDayRange 刷新日期范围
     */
    void refreshDayRange();
    /**
     * @brief refreshDayRange 刷新月份范围
     */
    void refreshMonthRange();
};


/**
 * @brief checkHeightValue 身高输入合法性判断
 * @param value
 * @return
 */
static bool checkHeightValue(const QString &value)
{
    if (value.isEmpty())
    {
        return true;
    }
    UnitType unitHeightIndex = patientManager.getHeightUnit();
    bool ok = false;
    float heightValue = value.toFloat(&ok);
    if (!ok)
    {
        return false;
    }
    Range range;
    switch (unitHeightIndex)
    {
    case UNIT_CM:         // cm: 20.0-300.0(cm)
        range = Unit::getRange(UNIT_CM);
        break;
    case UNIT_INCH:         // inch: 8.0-118.0(inch)
        range = Unit::getRange(UNIT_INCH);
        break;
    default:
        return false;
    }

    if (isUpper(heightValue, range.upLimit) ||
            isUpper(range.downLimit, heightValue))
    {
        return false;
    }
    return true;
}
/**
 * @brief checkWeightValue　体重输入合法性判断
 * @param value
 * @return
 */
static bool checkWeightValue(const QString &value)
{
    if (value.isEmpty())
    {
        return true;
    }

    UnitType unitWeightValue = patientManager.getWeightUnit();
    bool ok = false;
    float weightValue = value.toFloat(&ok);
    if (!ok)
    {
        return false;
    }
    Range range;
    switch (unitWeightValue)
    {
    case UNIT_KG:           // 0.1-200.0
        range = Unit::getRange(UNIT_KG);
        break;
    case UNIT_LB:           // 0.2-440.9
        range = Unit::getRange(UNIT_LB);
        break;
    default:
        return false;
    }

    if (isUpper(range.downLimit, weightValue) ||
            isUpper(weightValue, range.upLimit))
    {
        return false;
    }

    return true;
}
void PatientInfoWindowPrivate::loadOptions()
{
    combos[ITEM_CBO_PATIENT_TYPE]->setCurrentIndex(patientManager.getType());
    int isNeoMachine = 0;
    machineConfig.getNumValue("NeonateMachine", isNeoMachine);
    if (isNeoMachine)
    {
        combos[ITEM_CBO_PATIENT_TYPE]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_PATIENT_TYPE]->setEnabled(true);
    }

    combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(patientManager.getPacermaker());
    patientManager.setPacermaker(patientManager.getPacermaker());
    combos[ITEM_CBO_PATIENT_SEX]->setCurrentIndex(patientManager.getSex());
    combos[ITEM_CBO_BLOOD_TYPE]->setCurrentIndex(patientManager.getBlood());
    buttons[ITEM_BTN_PATIENT_NAME]->setText(patientManager.getName());

    // height
    heightType = patientManager.getHeightUnit();

    heightLbl->setText(QString("%1(%2)").arg(trs("PatientHeight"))
                       .arg(Unit::getSymbol(heightType)));

    QString heightStr = Unit::convert(heightType, UNIT_CM, patientManager.getHeight());
    if (patientManager.getHeight() > 0.000001)
    {
        buttons[ITEM_BTN_PATIENT_HEIGHT]->setText(heightStr);
    }
    else
    {
        buttons[ITEM_BTN_PATIENT_HEIGHT]->setText("");
    }

    // weight
    weightType = patientManager.getWeightUnit();

    weightLbl->setText(QString("%1(%2)").arg(trs("PatientWeight"))
                       .arg(Unit::getSymbol(weightType)));

    QString weightStr = Unit::convert(weightType, UNIT_KG, patientManager.getWeight());
    if (patientManager.getWeight() > 0.000001)
    {
        buttons[ITEM_BTN_PATIENT_WEIGHT]->setText(weightStr);
    }
    else
    {
        buttons[ITEM_BTN_PATIENT_WEIGHT]->setText("");
    }
    buttons[ITEM_BTN_PATIENT_ID]->setText(patientManager.getPatID());
    bedNum->setText(patientManager.getBedNum());

    int index = 0;
    systemConfig.getNumValue("General|ChangeBedNumberRight", index);
    buttons[ITEM_BTN_PATIENT_BED]->setEnabled(index);

    // born date item
    DateFormat dateFormat = systemManager.getSystemDateFormat();
    bornDateLbl->setText(QString("%1(%2)")
                         .arg(trs("BornDate"))
                         .arg(trs(TimeSymbol::convert(dateFormat))));

    unsigned int year = 0, month = 0, day = 0;
    QDate date = patientManager.getBornDate();
    if (date.isValid())
    {
        year = date.year();
        month = date.month();
        day = date.day();
    }
    switch (dateFormat)
    {
    case DATE_FORMAT_Y_M_D:
        dateItem[Born_Date_Year] = spinBoxs[ITEM_SPIN_BORN_FIRST];
        dateItem[Born_Date_Month] = spinBoxs[ITEM_SPIN_BORN_SECOND];
        dateItem[Born_Date_Day] = spinBoxs[ITEM_SPIN_BORN_THIRD];
        break;
    case DATE_FORMAT_M_D_Y:
        dateItem[Born_Date_Year] = spinBoxs[ITEM_SPIN_BORN_THIRD];
        dateItem[Born_Date_Month] = spinBoxs[ITEM_SPIN_BORN_FIRST];
        dateItem[Born_Date_Day] = spinBoxs[ITEM_SPIN_BORN_SECOND];
        break;
    case DATE_FORMAT_D_M_Y:
        dateItem[Born_Date_Year] = spinBoxs[ITEM_SPIN_BORN_THIRD];
        dateItem[Born_Date_Month] = spinBoxs[ITEM_SPIN_BORN_SECOND];
        dateItem[Born_Date_Day] = spinBoxs[ITEM_SPIN_BORN_FIRST];
        break;
    default:
        break;
    }

    dateItem[Born_Date_Year]->setRange(PATIENT_BORN_DATE_RANAGE, timeDate->getDateYear());
    for (int i = Born_Date_Year; i <= Born_Date_Day; i++)
    {
        dateItem[static_cast<BornDate>(i)]->blockSignals(true);
    }
    dateItem[Born_Date_Year]->setValue(year);
    if (year == 0)
    {
        dateItem[Born_Date_Year]->setStartValue(timeDate->getDateYear());
    }
    refreshMonthRange();
    dateItem[Born_Date_Month]->setValue(month);
    refreshDayRange();
    dateItem[Born_Date_Day]->setValue(day);
    for (int i = Born_Date_Year; i <= Born_Date_Day; i++)
    {
        dateItem[static_cast<BornDate>(i)]->blockSignals(false);
    }
}

PatientInfoWindow::PatientInfoWindow()
    : PatientInfoWindowInterface()
    , d_ptr(new PatientInfoWindowPrivate)
{
    setWindowTitle(trs("PatientInformation"));
    setFixedSize(themeManager.defaultWindowSize());
    QGridLayout *layout = new QGridLayout();
    QVBoxLayout *backgroundLayout = new QVBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QHBoxLayout * bornDateLayout = new QHBoxLayout();
    QLabel *label;
    int itemId;
    int itemPos = 0;
    int itemWidth = 200;

    // patient type
    label = new QLabel(trs("PatientType"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->type = new ComboBox();
    d_ptr->type->setFixedWidth(itemWidth);
    d_ptr->type ->addItems(QStringList()
                           << trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))
                           << trs(PatientSymbol::convert(PATIENT_TYPE_PED))
                           << trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE);
    d_ptr->type ->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->type
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE
                         , d_ptr->type);

    // bed num
    label = new QLabel();
    label->setText(trs("BedNumber"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label,
                      (d_ptr->combos.count() + d_ptr->buttons.count()) / 2,
                      itemPos++ % 4);
    d_ptr->bedNum = new Button();
    d_ptr->bedNum->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->bedNum->setFixedWidth(itemWidth);
    layout->addWidget(d_ptr->bedNum,
                      (d_ptr->combos.count() + d_ptr->buttons.count()) / 2,
                      itemPos++ % 4);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_BED
                         , d_ptr->bedNum);
    connect(d_ptr->bedNum, SIGNAL(released()), this, SLOT(bedNumReleased()));

    // patient name
    label = new QLabel(trs("PatientName"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->name = new Button();
    d_ptr->name->setFixedWidth(itemWidth);
    d_ptr->name->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME);
    d_ptr->name->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->name
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME
                          , d_ptr->name);
    connect(d_ptr->name, SIGNAL(released()), this, SLOT(nameReleased()));

    // patient pace marker
    label = new QLabel(trs("Pacemaker"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->pacer = new ComboBox();
    d_ptr->pacer->setFixedWidth(itemWidth);
    d_ptr->pacer->addItems(QStringList() << trs("Off") << trs("On"));
//                           << trs(PatientSymbol::convert(PATIENT_PACER_OFF))
//                           << trs(PatientSymbol::convert(PATIENT_PACER_ON)));
    itemId = static_cast<int>
             (PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER);
    d_ptr->pacer->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->pacer
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER
                         , d_ptr->pacer);

    // patient id
    label = new QLabel(trs("PatientID"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->id = new Button();
    d_ptr->id->setFixedWidth(itemWidth);
    d_ptr->id->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID);
    d_ptr->id->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->id
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID
                          , d_ptr->id);
    connect(d_ptr->id, SIGNAL(released()), this, SLOT(idReleased()));

    // patient sex
    label = new QLabel(trs("PatientSex"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->sex = new ComboBox();
    d_ptr->sex->setFixedWidth(itemWidth);
    d_ptr->sex->addItems(QStringList()
                         << ""
                         << trs(PatientSymbol::convert(PATIENT_SEX_MALE))
                         << trs(PatientSymbol::convert(PATIENT_SEX_FEMALE)));

    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX);
    d_ptr->sex->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->sex
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX
                         , d_ptr->sex);

    // Patient Height
    label = new QLabel();
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->heightLbl = label;
    d_ptr->height = new Button();
    d_ptr->height->setFixedWidth(itemWidth);
    d_ptr->height->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT);
    d_ptr->height->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->height
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT
                          , d_ptr->height);
    connect(d_ptr->height, SIGNAL(released()), this, SLOT(heightReleased()));

    // blood type
    label = new QLabel(trs("PatientBloodType"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->blood = new ComboBox();
    d_ptr->blood->setFixedWidth(itemWidth);
    d_ptr->blood->addItems(QStringList()
                           << ""
                           << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_A))
                           << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_B))
                           << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_AB))
                           << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_O)));

    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_BLOOD_TYPE);
    d_ptr->blood->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->blood
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_BLOOD_TYPE
                         , d_ptr->blood);

    // Patient Weight
    label = new QLabel();
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->weightLbl = label;
    d_ptr->weight = new Button();
    d_ptr->weight->setFixedWidth(itemWidth);
    d_ptr->weight->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT);
    d_ptr->weight->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->weight
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT
                          , d_ptr->weight);
    connect(d_ptr->weight, SIGNAL(released()), this, SLOT(weightReleased()));

    // patient born date
    label = new QLabel();
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bornDateLayout->addWidget(label);
    d_ptr->bornDateLbl = label;
    for (int itemId = PatientInfoWindowPrivate::ITEM_SPIN_BORN_FIRST;
         itemId <= PatientInfoWindowPrivate::ITEM_SPIN_BORN_THIRD; ++itemId)
    {
        SpinBox *spin = new SpinBox();
        spin->setScale(1);
        spin->setStep(1);
        spin->setArrow(false);
        spin->setProperty("Item", qVariantFromValue(itemId));
        connect(spin, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
        bornDateLayout->addWidget(spin);
        d_ptr->spinBoxs.insert(static_cast<PatientInfoWindowPrivate::MenuItem>(itemId), spin);
    }

    // create patient button
    d_ptr->savePatientInfo = new Button(trs("EnglishYESChineseSURE"));
    d_ptr->savePatientInfo->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CREATE_PATIENT);
    d_ptr->savePatientInfo->setProperty("Item" , qVariantFromValue(itemId));
    d_ptr->savePatientInfo->setFixedWidth(this->width() / 4);
    buttonLayout->addWidget(d_ptr->savePatientInfo);
    connect(d_ptr->savePatientInfo, SIGNAL(released()), this, SLOT(onBtnReleased()));

    // cancel Button
    d_ptr->cancel = new Button(trs("Cancel"));
    d_ptr->cancel->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CANCEL);
    d_ptr->cancel->setProperty("Item" , qVariantFromValue(itemId));
    d_ptr->cancel->setFixedWidth(this->width() / 4);
    buttonLayout->addWidget(d_ptr->cancel);
    connect(d_ptr->cancel, SIGNAL(released()), this, SLOT(onBtnReleased()));

    layout->setSpacing(10);
    bornDateLayout->setSpacing(10);
    backgroundLayout->setSpacing(10);
    backgroundLayout->addLayout(layout);
    backgroundLayout->addLayout(bornDateLayout);
    backgroundLayout->addStretch();
    backgroundLayout->addLayout(buttonLayout);
    setWindowLayout(backgroundLayout);
}

void PatientInfoWindowPrivate::savePatientInfoToManager()
{
    QDate date(dateItem[Born_Date_Year]->getValue(),
               dateItem[Born_Date_Month]->getValue(),
               dateItem[Born_Date_Day]->getValue());
    patientManager.setBornDate(date);
    patientManager.setBlood(static_cast<PatientBloodType>(blood->currentIndex()));
    float heightFloat = height->text().toFloat();
    if (heightType == UNIT_INCH)
    {
        // 病人信息保存的身高默认是cm单位
        heightFloat = height->text().toFloat() * 2.54;
    }
    patientManager.setHeight(heightFloat);
    patientManager.setName(name->text());
    patientManager.setPatID(id->text());
    patientManager.setSex(static_cast<PatientSex>(sex->currentIndex()));
    patientManager.setType(static_cast<PatientType>(type->currentIndex()));
    float weightFloat = weight->text().toFloat();
    if (weightType == UNIT_LB)
    {
        // 病人信息保存的体重默认是kg单位
        weightFloat = weight->text().toFloat() / 2.20462;
    }
    patientManager.setWeight(weightFloat);
    patientManager.setPacermaker(static_cast<PatientPacer>(pacer->currentIndex()));
    patientManager.setBedNum(bedNum->text());
    patientManager.updatePatientInfo();
}

void PatientInfoWindowPrivate::refreshDayRange()
{
    unsigned int year = dateItem[PatientInfoWindowPrivate::Born_Date_Year]->getValue();
    unsigned int month = dateItem[PatientInfoWindowPrivate::Born_Date_Month]->getValue();
    unsigned int day = dateItem[PatientInfoWindowPrivate::Born_Date_Day]->getValue();
    unsigned int upRange = 1;
    if (year == timeDate->getDateYear() && month == timeDate->getDateMonth())
    {
        upRange = timeDate->getDateDay();
    }
    else
    {
        QDate date(year, month, 1);
        upRange = date.daysInMonth();
    }

    dateItem[PatientInfoWindowPrivate::Born_Date_Day]->setRange(1, upRange);
    if (day > upRange)
    {
        // 如果超出日份范围，则设置为最大值。
        dateItem[PatientInfoWindowPrivate::Born_Date_Day]->setValue(upRange);
    }
}

void PatientInfoWindowPrivate::refreshMonthRange()
{
    unsigned int year = dateItem[PatientInfoWindowPrivate::Born_Date_Year]->getValue();
    unsigned int month = dateItem[PatientInfoWindowPrivate::Born_Date_Month]->getValue();
    unsigned int upRange = 12;
    if (year == timeDate->getDateYear())
    {
        upRange = timeDate->getDateMonth();
    }
    dateItem[PatientInfoWindowPrivate::Born_Date_Month]->setRange(1, upRange);
    if (month > upRange)
    {
        dateItem[PatientInfoWindowPrivate::Born_Date_Month]->setValue(upRange);
    }
}

void PatientInfoWindow::idReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientID"));
    englishPanel.setMaxInputLength(120);
    englishPanel.setInitString(d_ptr->id->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->id->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::nameReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientName"));
    englishPanel.setMaxInputLength(120);
    englishPanel.setInitString(d_ptr->name->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->name->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::heightReleased()
{
    KeyInputPanel inputPanel(KeyInputPanel::KEY_TYPE_NUMBER, true);
    inputPanel.setWindowTitle(trs("PatientHeight"));
    inputPanel.setMaxInputLength(5);
    inputPanel.setInitString(d_ptr->height->text());
    inputPanel.setSymbolEnable(false);
    inputPanel.setKeytypeSwitchEnable(false);
    inputPanel.setCheckValueHook(checkHeightValue);
    Range range = Unit::getRange(d_ptr->heightType);
    QString rangeStr = QString("%1 ~ %2").arg(QString::number(range.downLimit, 'f', 1))
                       .arg(QString::number(range.upLimit, 'f', 1));
    inputPanel.setInvalidHint(QString("%1(%2)").arg(trs("InvalidInput"))
                             .arg(rangeStr));

    if (inputPanel.exec())
    {
        QString text = inputPanel.getStrValue();
        bool ok = false;
        float height = text.toFloat(&ok);
        if (ok)
        {
            d_ptr->height->setText(QString::number(height, 'f', 1));
        }
        else if (text.isEmpty())
        {
            d_ptr->height->setText("");
        }
    }
}

void PatientInfoWindow::weightReleased()
{
    KeyInputPanel inputPanel(KeyInputPanel::KEY_TYPE_NUMBER, true);
    inputPanel.setWindowTitle(trs("PatientWeight"));
    inputPanel.setMaxInputLength(5);
    inputPanel.setInitString(d_ptr->weight->text());
    inputPanel.setSymbolEnable(false);
    inputPanel.setKeytypeSwitchEnable(false);
    inputPanel.setCheckValueHook(checkWeightValue);
    Range range = Unit::getRange(d_ptr->weightType);
    QString rangeStr = QString("%1 ~ %2").arg(QString::number(range.downLimit, 'f', 1))
                       .arg(QString::number(range.upLimit, 'f', 1));
    inputPanel.setInvalidHint(QString("%1(%2)").arg(trs("InvalidInput"))
                             .arg(rangeStr));

    if (inputPanel.exec())
    {
        QString text = inputPanel.getStrValue();
        bool ok = false;
        float weight = text.toFloat(&ok);
        if (ok)
        {
            d_ptr->weight->setText(QString::number(weight, 'f', 1));
        }
        else if (text.isEmpty())
        {
            d_ptr->weight->setText("");
        }
    }
}

void PatientInfoWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->savePatientInfo)
    {
        if (d_ptr->type->currentIndex() != static_cast<int>(patientManager.getType()))
        {
            MessageBox msg(trs("Prompt"), trs("ChangePatientType"), true, true);
            if (msg.exec() == QDialog::Rejected)
            {
                this->hide();
                return;
            }
        }
        d_ptr->savePatientInfoToManager();
    }
    this->hide();
}

void PatientInfoWindow::bedNumReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    KeyInputPanel idPanel;
    idPanel.setWindowTitle(trs("BedNumber"));
    idPanel.setInitString(btn->text());
    idPanel.setMaxInputLength(11);
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = btn->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            btn->setText(text);
        }
    }
}

void PatientInfoWindow::onSpinBoxValueChanged(int, int)
{
    d_ptr->refreshMonthRange();
    d_ptr->refreshDayRange();
}

void PatientInfoWindow::showEvent(QShowEvent *ev)
{
    d_ptr->loadOptions();
    if (patientManager.isNewPatient())
    {
        setWindowTitle(trs("AdmitPatient"));
    }
    else
    {
        setWindowTitle(trs("PatientInfo"));
    }
    Dialog::showEvent(ev);
}

void PatientInfoWindow::hideEvent(QHideEvent *ev)
{
    patientManager.finishPatientInfo();
    Dialog::hideEvent(ev);
}

PatientInfoWindow &PatientInfoWindow::getInstance()
{
    static PatientInfoWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new PatientInfoWindow;
        PatientInfoWindowInterface *old = PatientInfoWindow::registerPatientInfoWindow(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

PatientInfoWindow::~PatientInfoWindow()
{
    delete d_ptr;
}
