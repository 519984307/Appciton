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
#include "PatientDefine.h"
#include "MessageBox.h"
#include <QHBoxLayout>
#include "EnglishInputPanel.h"
#include "KeyInputPanel.h"
#include "PatientManager.h"
#include "DischargePatientWindow.h"
#include "DataStorageDirManager.h"
#include <QMap>
#include "Button.h"
#include "WindowManager.h"
#include "IConfig.h"
#include "UnitManager.h"
#include "ECGParam.h"

PatientInfoWindow *PatientInfoWindow::_selfObj = NULL;
class PatientInfoWindowPrivate
{
public:
    PatientInfoWindowPrivate() :
        id(NULL) , type(NULL) , sex(NULL)
        , blood(NULL) , name(NULL) , pacer(NULL)
        , age(NULL) , height(NULL) , weight(NULL)
        , relievePatient(NULL) , savePatientInfo(NULL)
        , infoChange(false)
        , patientNew(false)
        , relieveFlag(true)
    {}

    enum MenuItem
    {
        ITEM_CBO_PATIENT_TYPE = 1,
        ITEM_CBO_PACER_MARKER,
        ITEM_BTN_PATIENT_ID,
        ITEM_CBO_PATIENT_SEX,
        ITEM_CBO_BLOOD_TYPE,
        ITEM_BTN_PATIENT_NAME,
        ITEM_BTN_PATIENT_AGE,
        ITEM_BTN_PATIENT_HEIGHT,
        ITEM_BTN_PATIENT_WEIGHT,
        ITEM_BTN_CREATE_PATIENT,
        ITEM_BTN_CLEAR_PATIENT
    };

    void loadOptions();
    Button *id;                      // ID。
    ComboBox *type;                  // 病人类型。
    ComboBox *sex;                   // 性别。
    ComboBox *blood;                 // 血型。
    Button *name;                    // 姓名。
    ComboBox *pacer;                 // 起搏分析。
    Button *age;                     // 年龄。
    Button *height;                  // 身高。
    Button *weight;                  // 体重。
    Button *relievePatient;          // 病人信息获取
    Button *savePatientInfo;         // 保存病人信息

    bool infoChange;                 // settting has been change
    bool patientNew;                 // 新建病人标志
    bool relieveFlag;                // 解除病人标志

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem , Button *> buttons;
};

static bool checkAgeValue(const QString &value)
{
    if (value.isEmpty())
    {
        return true;
    }

    bool ok = false;
    int age = value.toInt(&ok);
    if (!ok)
    {
        return false;
    }

    if (age > 120 || age < 0)
    {
        return false;
    }

    return true;
}

/**
 * @brief checkHeightValue
 * @param value
 * @return
 */
static bool checkHeightValue(const QString &value)
{
    if (value.isEmpty())
    {
        return true;
    }
    int unitHeightIndex = -1;
    systemConfig.getNumValue("Unit|HightUnit", unitHeightIndex);
    bool ok = false;
    float heightValue = value.toFloat(&ok);
    if (!ok)
    {
        return false;
    }
    switch (unitHeightIndex)
    {
    case UNIT_CM:         // cm: 20.0-300.0(cm)
        if (heightValue - 300.0 > 0.000001 ||
                heightValue - 20.0 < 0.000001)
        {
            return false;
        }
        break;
    case UNIT_INCH:         // inch: 8.0-118.0(inch)
        if (heightValue - 118.0 > 0.000001 ||
                heightValue - 8.0 < 0.000001)
        {
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}
/**
 * @brief checkWeightValue
 * @param value
 * @return
 */
static bool checkWeightValue(const QString &value)
{
    if (value.isEmpty())
    {
        return true;
    }

    int unitWeightValue = -1;
    systemConfig.getNumValue("Unit|WeightUnit", unitWeightValue);
    bool ok = false;
    float weightValue = value.toFloat(&ok);
    if (!ok)
    {
        return false;
    }
    switch (unitWeightValue)
    {
    case UNIT_KG:           // 0.1-200.0
        if (weightValue - 0.1 < 0.000001 ||
                weightValue - 200.0 > 0.000001)
        {
            return false;
        }
        break;
    case UNIT_LB:           // 0.2-440.9
        if (weightValue - 0.2 < 0.000001 ||
                weightValue - 440.9 > 0.000001)
        {
            return false;
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}
void PatientInfoWindowPrivate::loadOptions()
{
    combos[ITEM_CBO_PATIENT_TYPE]->setCurrentIndex(patientManager.getType());
    if (patientNew)         // 新建病人时默认打开起博
    {
        combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(ECG_PACE_ON);
    }
    else
    {
        combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(ecgParam.getPacermaker());
    }
    buttons[ITEM_BTN_PATIENT_ID]->setEnabled(true);
    combos[ITEM_CBO_PATIENT_SEX]->setCurrentIndex(patientManager.getSex());
    combos[ITEM_CBO_BLOOD_TYPE]->setCurrentIndex(patientManager.getBlood());
    buttons[ITEM_BTN_PATIENT_NAME]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_AGE]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_HEIGHT]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_WEIGHT]->setEnabled(true);
}

PatientInfoWindow::PatientInfoWindow()
    : Window()
    , d_ptr(new PatientInfoWindowPrivate)
{
    setWindowTitle(trs("PatientInformation"));
    resize(800, 580);
    QGridLayout *layout = new QGridLayout();
    QVBoxLayout *backgroundLayout = new QVBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QLabel *label;
    int itemId;

    // patient type
    label = new QLabel(trs("PatientType"));
    label->setFixedWidth(120);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 0);
    d_ptr->type = new ComboBox();
    d_ptr->type ->addItems(QStringList()
                            << trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))
                            << trs(PatientSymbol::convert(PATIENT_TYPE_PED))
                            << trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE);
    d_ptr->type ->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->type
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE
                         , d_ptr->type);

    // patient pace marker
    label = new QLabel(trs("PatientPacemarker"));
    label->setFixedWidth(120);
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 2);
    d_ptr->pacer = new ComboBox();
    d_ptr->pacer->addItems(QStringList()
                            << trs(PatientSymbol::convert(PATIENT_PACER_OFF))
                            << trs(PatientSymbol::convert(PATIENT_PACER_ON))
                           );
    itemId = static_cast<int>
             (PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER);
    d_ptr->pacer->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->pacer
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 3);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER
                         , d_ptr->pacer);

    connect(d_ptr->pacer, SIGNAL(currentIndexChanged(int)), this , SLOT(_pacerMakerReleased(int)));

    // patient id
    label = new QLabel(trs("PatientID"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 0);
    d_ptr->id = new Button();
    d_ptr->id->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID);
    d_ptr->id->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->id
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID
                          , d_ptr->id);
    connect(d_ptr->id, SIGNAL(released()), this, SLOT(_idReleased()));

    // patient sex
    label = new QLabel(trs("PatientSex"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 2);
    d_ptr->sex = new ComboBox();
    d_ptr->sex->addItems(QStringList()
                          << ""
                          << trs(PatientSymbol::convert(PATIENT_SEX_MALE))
                          << trs(PatientSymbol::convert(PATIENT_SEX_FEMALE))
                         );
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX);
    d_ptr->sex->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->sex
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 3);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX
                         , d_ptr->sex);

    // blood type
    label = new QLabel(trs("PatientBloodType"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 0);
    d_ptr->blood = new ComboBox();
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
                      , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_BLOOD_TYPE
                         , d_ptr->blood);

    // patient name
    label = new QLabel(trs("PatientName"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 2);
    d_ptr->name = new Button();
    d_ptr->name->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME);
    d_ptr->name->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->name
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 3);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME
                          , d_ptr->name);
    connect(d_ptr->name, SIGNAL(released()), this, SLOT(_nameReleased()));

    // patient age
    label = new QLabel(trs("PatientAge"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 0);
    d_ptr->age = new Button();
    d_ptr->age->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_AGE);
    d_ptr->age->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->age
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_AGE
                          , d_ptr->age);

    connect(d_ptr->age, SIGNAL(released()), this, SLOT(_ageReleased()));

    // Patient Height
    label = new QLabel(trs("PatientHeight"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 2);
    d_ptr->height = new Button();
    d_ptr->height->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT);
    d_ptr->height->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->height
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 3);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT
                          , d_ptr->height);
    connect(d_ptr->height, SIGNAL(released()), this, SLOT(_heightReleased()));

    // Patient Weight
    label = new QLabel(trs("PatientWeight"));
    layout->addWidget(label
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 0);
    d_ptr->weight = new Button();
    d_ptr->weight->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT);
    d_ptr->weight->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->weight
                      , (d_ptr->combos.count() + d_ptr->buttons.count()) / 2
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT
                          , d_ptr->weight);
    connect(d_ptr->weight, SIGNAL(released()), this, SLOT(_weightReleased()));

    // create patient button
    d_ptr->savePatientInfo = new Button(trs("PatientCreate"));
    d_ptr->savePatientInfo->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CREATE_PATIENT);
    d_ptr->savePatientInfo->setProperty("Item" , qVariantFromValue(itemId));
    d_ptr->savePatientInfo->setFixedWidth(180);
    buttonLayout->addWidget(d_ptr->savePatientInfo);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_CREATE_PATIENT
                          , d_ptr->savePatientInfo);
    connect(d_ptr->savePatientInfo, SIGNAL(released()), this, SLOT(_saveInfoReleased()));

    // clean patient data
    d_ptr->relievePatient = new Button(trs("CleanPatientData"));
    d_ptr->relievePatient->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CLEAR_PATIENT);
    d_ptr->relievePatient->setProperty("Item" , qVariantFromValue(itemId));
    d_ptr->relievePatient->setFixedWidth(180);
    buttonLayout->addWidget(d_ptr->relievePatient);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_CLEAR_PATIENT
                          , d_ptr->relievePatient);
    connect(d_ptr->relievePatient, SIGNAL(released()), this, SLOT(_relieveReleased()));
    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count() , 1);
    layout->setSpacing(15);
    layout->setMargin(5);
    backgroundLayout->addLayout(layout);
    backgroundLayout->addLayout(buttonLayout);
    setWindowLayout(backgroundLayout);
}

void PatientInfoWindow::_setPatientInfo()
{
    patientManager.setAge(d_ptr->age->text().toInt());
    patientManager.setBlood(static_cast<PatientBloodType>(d_ptr->blood->currentIndex()));
    patientManager.setHeight(d_ptr->height->text().toShort());
    patientManager.setName(d_ptr->name->text());
    patientManager.setPatID(d_ptr->id->text());
    patientManager.setSex(static_cast<PatientSex>(d_ptr->sex->currentIndex()));
    patientManager.setType(static_cast<PatientType>(d_ptr->type->currentIndex()));
    patientManager.setWeight(d_ptr->weight->text().toInt());
    patientManager.setPacermaker(static_cast<PatientPacer>(d_ptr->pacer->currentIndex()));
}

void PatientInfoWindow::widgetChange()
{
    if (d_ptr->patientNew == false && d_ptr->relieveFlag == false)
    {
        d_ptr->savePatientInfo->setText(trs("PatientCreate"));
        d_ptr->relievePatient->setText(trs("CleanPatientData"));
        d_ptr->relievePatient->setVisible(true);
    }
    else if (d_ptr->patientNew == true)
    {
        d_ptr->savePatientInfo->setText(trs("EnglishYESChineseSURE"));
        d_ptr->relievePatient->hide();
    }
    else if (d_ptr->patientNew == false && d_ptr->relieveFlag == true)
    {
        d_ptr->savePatientInfo->setText(trs("PatientCreate"));
        d_ptr->relievePatient->setText(trs("RelievePatient"));
        d_ptr->relievePatient->setVisible(true);
    }
}

void PatientInfoWindow::relieveStatus(bool relStauts)
{
    d_ptr->relieveFlag = relStauts;
}

void PatientInfoWindow::newPatientStatus(bool pStatus)
{
    d_ptr->patientNew = pStatus;
}

void PatientInfoWindow::_idReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientID"));
    englishPanel.setMaxInputLength(100);
    englishPanel.setInitString(d_ptr->id->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->id->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::_nameReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientName"));
    englishPanel.setMaxInputLength(100);
    englishPanel.setInitString(d_ptr->name->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->name->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::_ageReleased()
{
    KeyInputPanel inputPanel;
    inputPanel.setWindowTitle(trs("PatientAge"));
    inputPanel.setMaxInputLength(3);
    inputPanel.setInitString(d_ptr->age->text());
    inputPanel.setSpaceEnable(false);
    inputPanel.setSymbolEnable(false);
    inputPanel.setKeytypeSwitchEnable(false);
    inputPanel.setCheckValueHook(checkAgeValue);

    if (inputPanel.exec())
    {
        QString text = inputPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            d_ptr->age->setText(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            d_ptr->age->setText("");
        }
        d_ptr->infoChange = true;
    }
}

void PatientInfoWindow::_heightReleased()
{
    KeyInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientHeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(d_ptr->height->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkHeightValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        float height = text.toFloat(&ok);
        if (ok)
        {
            patientManager.setHeight(height);
            d_ptr->height->setText(QString::number(height));
        }
        else if (text.isEmpty())
        {
            patientManager.setHeight(-1);
            d_ptr->height->setText("");
        }
        d_ptr->infoChange = true;
    }
}

void PatientInfoWindow::_weightReleased()
{
    KeyInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientWeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(d_ptr->weight->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkWeightValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        float weight = text.toFloat(&ok);
        if (ok)
        {
            patientManager.setWeight(weight);
            d_ptr->weight->setText(QString::number(weight));
        }
        else if (text.isEmpty())
        {
            patientManager.setWeight(-1);
            d_ptr->weight->setText("");
        }
        d_ptr->infoChange = true;
    }
}

void PatientInfoWindow::_relieveReleased()
{
    this->hide();
    if (d_ptr->relieveFlag == true)
    {
        DischargePatientWindow::getInstance()->setWindowTitle(trs("RelievePatient"));
        int ret = DischargePatientWindow::getInstance()->exec();
        if (ret)
            relieveStatus(false);
    }
    else
    {
        DischargePatientWindow::getInstance()->setWindowTitle(trs("CleanPatientData"));
        DischargePatientWindow::getInstance()->exec();
    }
}

void PatientInfoWindow::_saveInfoReleased()
{
    int warnStatus;

    if (d_ptr->patientNew == true)
    {
        this->hide();
        relieveStatus(true);
    }
    else if (d_ptr->patientNew == false && d_ptr->relieveFlag == false)
    {
        QStringList slist;
        slist << trs("Cancel") << trs("EnglishYESChineseSURE");
        MessageBox messageBox(trs("Warn"), trs("ApplicationDataToReceivingPatients"), slist);
        this->hide();
        warnStatus = messageBox.exec();
        if (warnStatus == 0)
        {
            relieveStatus(false);
        }
        else if (warnStatus == 1)
        {
            relieveStatus(true);
            dataStorageDirManager.createDir(true);
            _setPatientInfo();
        }
    }
    else if (d_ptr->patientNew == false && d_ptr->relieveFlag == true)
    {
        this->hide();
        QStringList slist;
        slist << trs("No") << trs("EnglishYESChineseSURE");
        MessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
        if (messageBox.exec() == 1)
        {
            dataStorageDirManager.createDir(true);
            _setPatientInfo();
        }
    }
}

void PatientInfoWindow::_pacerMakerReleased(int index)
{
    ecgParam.setPacermaker(static_cast<ECGPaceMode>(index));
}

void PatientInfoWindow::showEvent(QShowEvent *ev)
{
    d_ptr->loadOptions();
    Window::showEvent(ev);
}

PatientInfoWindow::~PatientInfoWindow()
{
    delete d_ptr;
}









