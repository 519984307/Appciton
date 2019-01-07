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
#include "ComboBox.h"
#include "LanguageManager.h"
#include <QStringList>
#include "MessageBox.h"
#include <QHBoxLayout>
#include "EnglishInputPanel.h"
#include "KeyInputPanel.h"
#include "PatientManager.h"
#include "DischargePatientWindow.h"
#include <QMap>
#include "Button.h"
#include "UnitManager.h"
#include "IConfig.h"
#include "WindowManager.h"
#include "SpinBox.h"
#include "TimeDate.h"
#include "TimeSymbol.h"
#include <QDate>

PatientInfoWindow *PatientInfoWindow::_selfObj = NULL;
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

    if (heightValue - range.upLimit > 0.000001 ||
            heightValue - range.downLimit < 0.000001)
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

    if (weightValue - range.downLimit < 0.000001 ||
            weightValue - range.upLimit > 0.000001)
    {
        return false;
    }

    return true;
}
void PatientInfoWindowPrivate::loadOptions()
{
    combos[ITEM_CBO_PATIENT_TYPE]->setCurrentIndex(patientManager.getType());
    bool patientNew = patientManager.isNewPatient();
    if (patientNew)         // 新建病人时默认打开起博
    {
        combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(PATIENT_PACER_ON);
    }
    else
    {
        combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(patientManager.getPacermaker());
    }
    combos[ITEM_CBO_PATIENT_SEX]->setCurrentIndex(patientManager.getSex());
    combos[ITEM_CBO_BLOOD_TYPE]->setCurrentIndex(patientManager.getBlood());
    buttons[ITEM_BTN_PATIENT_NAME]->setText(patientManager.getName());
    if (patientManager.getHeight() > 0.000001)
    {
        buttons[ITEM_BTN_PATIENT_HEIGHT]->setText(QString::number(patientManager.getHeight()));
    }
    else
    {
        buttons[ITEM_BTN_PATIENT_HEIGHT]->setText("");
    }
    if (patientManager.getWeight() > 0.000001)
    {
        buttons[ITEM_BTN_PATIENT_WEIGHT]->setText(QString::number(patientManager.getWeight()));
    }
    else
    {
        buttons[ITEM_BTN_PATIENT_WEIGHT]->setText("");
    }
    buttons[ITEM_BTN_PATIENT_ID]->setText(patientManager.getPatID());
    bedNum->setText(patientManager.getBedNum());

    UnitType oldHeightType = heightType;
    heightType = patientManager.getHeightUnit();

    heightLbl->setText(QString("%1(%2)").arg(trs("PatientHeight"))
                       .arg(Unit::getSymbol(heightType)));

    UnitType oldWeightType = weightType;
    weightType = patientManager.getWeightUnit();

    weightLbl->setText(QString("%1(%2)").arg(trs("PatientWeight"))
                       .arg(Unit::getSymbol(weightType)));

    bool ok;
    float heightValue = height->text().toFloat(&ok);
    if (ok)
    {
        QString ret = Unit::convert(heightType, oldHeightType, heightValue);
        height->setText(ret);
    }

    float weightValue = weight->text().toFloat(&ok);
    if (ok)
    {
        QString ret = Unit::convert(weightType, oldWeightType, weightValue);
        weight->setText(ret);
    }

    int index = 0;
    systemConfig.getNumValue("General|ChangeBedNumberRight", index);
    buttons[ITEM_BTN_PATIENT_BED]->setEnabled(index);

    // born date item
    systemConfig.getNumValue("DateTime|DateFormat", index);
    DateFormat dateFormat = static_cast<DateFormat>(index);
    bornDateLbl->setText(QString("%1(%2)")
                         .arg(trs("BornDate"))
                         .arg(trs(TimeSymbol::convert(dateFormat)))
                         );

    unsigned int year = 0, month = 0, day = 0;
    patientManager.getBornDate(year, month, day);
    if (year > timeDate.getDateYear())
    {
        year = timeDate.getDateYear();
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

    dateItem[Born_Date_Year]->setRange(1970, timeDate.getDateYear());
    dateItem[Born_Date_Year]->setValue(year);
    dateItem[Born_Date_Month]->setValue(month);
    dateItem[Born_Date_Day]->setValue(day);
}

PatientInfoWindow::PatientInfoWindow()
    : Window()
    , d_ptr(new PatientInfoWindowPrivate)
{
    setWindowTitle(trs("PatientInformation"));
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
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
    label = new QLabel(trs("PatientPacemarker"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->pacer = new ComboBox();
    d_ptr->pacer->setFixedWidth(itemWidth);
    d_ptr->pacer->addItems(QStringList()
                           << trs(PatientSymbol::convert(PATIENT_PACER_OFF))
                           << trs(PatientSymbol::convert(PATIENT_PACER_ON))
                          );
    itemId = static_cast<int>
             (PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER);
    d_ptr->pacer->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->pacer
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , itemPos++ % 4);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER
                         , d_ptr->pacer);

    connect(d_ptr->pacer, SIGNAL(currentIndexChanged(int)), this , SLOT(pacerMakerReleased(int)));

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
                         << trs(PatientSymbol::convert(PATIENT_SEX_FEMALE))
                        );
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
                           << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_O))
                          );
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
        connect(this, SIGNAL(spinValueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
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
    QString format;
    timeDate.getDateFormat(format, true);
    QDate date(dateItem[Born_Date_Year]->getValue(),
               dateItem[Born_Date_Month]->getValue(),
               dateItem[Born_Date_Day]->getValue());
    QString bornDate = date.toString(format);
    patientManager.setBornDate(bornDate);
    patientManager.setBlood(static_cast<PatientBloodType>(blood->currentIndex()));
    patientManager.setHeight(height->text().toFloat());
    patientManager.setName(name->text());
    patientManager.setPatID(id->text());
    patientManager.setSex(static_cast<PatientSex>(sex->currentIndex()));
    patientManager.setType(static_cast<PatientType>(type->currentIndex()));
    patientManager.setWeight(weight->text().toFloat());
    patientManager.setPacermaker(static_cast<PatientPacer>(pacer->currentIndex()));
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
        d_ptr->savePatientInfoToManager();
    }
    this->hide();
}

void PatientInfoWindow::pacerMakerReleased(int index)
{
    patientManager.setPacermaker(static_cast<PatientPacer>(index));
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
            patientManager.setBedNum(text);
        }
    }
}

void PatientInfoWindow::onSpinBoxValueChanged(int, int)
{
    SpinBox *spinBox = qobject_cast<SpinBox *>(sender());
    if (spinBox == d_ptr->dateItem.value(PatientInfoWindowPrivate::Born_Date_Year))
    {
        // 设置月份范围和月份的值
        unsigned int year = spinBox->getValue();
        unsigned int month = d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Month]->getValue();
        if (year == timeDate.getDateYear())
        {
            d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Month]->setRange(1, timeDate.getDateMonth());
        }
        else
        {
            d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Month]->setRange(1, 12);
        }
        d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Month]->setValue(month);
    }
    else if (spinBox == d_ptr->dateItem.value(PatientInfoWindowPrivate::Born_Date_Month))
    {
        // 设置日份范围和值
        unsigned int year = d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Year]->getValue();
        unsigned int month = spinBox->getValue();
        unsigned int day = d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Day]->getValue();
        if (year == timeDate.getDateYear() && month == timeDate.getDateMonth())
        {
            d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Day]->setRange(1, timeDate.getDateDay());
        }
        else
        {
            d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Day]->setRange(1, timeDate.getMaxDay(year, month));
        }
        d_ptr->dateItem[PatientInfoWindowPrivate::Born_Date_Day]->setValue(day);
    }
}

void PatientInfoWindow::showEvent(QShowEvent *ev)
{
    d_ptr->loadOptions();
    if (patientManager.isNewPatient())
    {
        setWindowTitle(trs("NewPatient"));
    }
    else
    {
        setWindowTitle(trs("PatientInfo"));
    }
    Window::showEvent(ev);
}

void PatientInfoWindow::hideEvent(QHideEvent *ev)
{
    patientManager.finishPatientInfo();
    Window::hideEvent(ev);
}

PatientInfoWindow::~PatientInfoWindow()
{
    delete d_ptr;
}
