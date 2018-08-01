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
#include "RelievePatientWindow.h"
#include "DataStorageDirManager.h"
#include <QMap>
#include "Button.h"
#include "WindowManager.h"

PatientInfoWindow *PatientInfoWindow::_selfObj = NULL;
class PatientInfoWindowPrivate
{
public:
    PatientInfoWindowPrivate() :
        _id(NULL) , _type(NULL) , _sex(NULL)
        , _blood(NULL) , _name(NULL) , _pacer(NULL)
        , _age(NULL) , _height(NULL) , _weight(NULL)
        , _relievePatient(NULL) , _savePatientInfo(NULL)
        , _infoChange(false)
        , _patientNew(false)
        , _relieveFlag(false)
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
    Button *_id;                      // ID。
    ComboBox *_type;                  // 病人类型。
    ComboBox *_sex;                   // 性别。
    ComboBox *_blood;                 // 血型。
    Button *_name;                    // 姓名。
    ComboBox *_pacer;                 // 起搏分析。
    Button *_age;                     // 年龄。
    Button *_height;                  // 身高。
    Button *_weight;                  // 体重。
    Button *_relievePatient;          // 病人信息获取
    Button *_savePatientInfo;         // 保存病人信息

    bool _infoChange;                 // settting has been change
    bool _patientNew;                 // 新建病人标志
    bool _relieveFlag;                // 解除病人标志

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

    bool ok = false;
    int age = value.toInt(&ok);
    if (!ok)
    {
        return false;
    }

    if (age > 480 || age < 0)
    {
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

    bool ok = false;
    int age = value.toInt(&ok);
    if (!ok)
    {
        return false;
    }

    if (age > 480 || age < 0)
    {
        return false;
    }

    return true;
}
void PatientInfoWindowPrivate::loadOptions()
{
    combos[ITEM_CBO_PATIENT_TYPE]->setCurrentIndex(0);
    combos[ITEM_CBO_PACER_MARKER]->setCurrentIndex(0);
    buttons[ITEM_BTN_PATIENT_ID]->setEnabled(true);
    combos[ITEM_CBO_PATIENT_SEX]->setCurrentIndex(0);
    combos[ITEM_CBO_BLOOD_TYPE]->setCurrentIndex(0);
    buttons[ITEM_BTN_PATIENT_NAME]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_AGE]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_HEIGHT]->setEnabled(true);
    buttons[ITEM_BTN_PATIENT_WEIGHT]->setEnabled(true);
}

PatientInfoWindow::PatientInfoWindow()
    : Window()
    , d_ptr(new PatientInfoWindowPrivate)
{
    setWindowTitle("Patient Infomation");
    layoutExec();
}
void PatientInfoWindow::readyShow()
{
    d_ptr->loadOptions();
}

void PatientInfoWindow::layoutExec()
{
    QGridLayout *backgroundLayout = new QGridLayout(this);
    QGridLayout *layout = new QGridLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);

    QLabel *label;
    int itemId;

    // patient type
    label = new QLabel(trs("PatientType"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_type = new ComboBox();
    d_ptr->_type ->addItems(QStringList()
                            << trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))
                            << trs(PatientSymbol::convert(PATIENT_TYPE_PED))
                            << trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE);
    d_ptr->_type ->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_type
                      , d_ptr->combos.count() + d_ptr->buttons.count() , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_TYPE
                         , d_ptr->_type);

    // patient pace marker
    label = new QLabel(trs("PatientPacemarker"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_pacer = new ComboBox();
    d_ptr->_pacer->addItems(QStringList()
                            << trs(PatientSymbol::convert(PATIENT_PACER_OFF))
                            << trs(PatientSymbol::convert(PATIENT_PACER_ON))
                           );
    itemId = static_cast<int>
             (PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER);
    d_ptr->_pacer->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_pacer
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PACER_MARKER
                         , d_ptr->_pacer);

    // patient id
    label = new QLabel(trs("PatientID"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_id = new Button();
    d_ptr->_id->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID);
    d_ptr->_id->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_id
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_ID
                          , d_ptr->_id);
    connect(d_ptr->_id, SIGNAL(released()), this, SLOT(_idReleased()));

    // patient sex
    label = new QLabel(trs("PatientSex"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_sex = new ComboBox();
    d_ptr->_sex->addItems(QStringList()
                          << ""
                          << trs(PatientSymbol::convert(PATIENT_SEX_MALE))
                          << trs(PatientSymbol::convert(PATIENT_SEX_FEMALE))
                         );
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX);
    d_ptr->_sex->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_sex
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_PATIENT_SEX
                         , d_ptr->_sex);

    // blood type
    label = new QLabel(trs("PatientBloodType"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_blood = new ComboBox();
    d_ptr->_blood->addItems(QStringList()
                            << ""
                            << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_A))
                            << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_B))
                            << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_AB))
                            << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_O))
                           );
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_CBO_BLOOD_TYPE);
    d_ptr->_blood->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_blood
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->combos.insert(PatientInfoWindowPrivate::ITEM_CBO_BLOOD_TYPE
                         , d_ptr->_blood);

    // patient name
    label = new QLabel(trs("PatientName"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_name = new Button();
    d_ptr->_name->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME);
    d_ptr->_name->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_name
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_NAME
                          , d_ptr->_name);
    connect(d_ptr->_name, SIGNAL(released()), this, SLOT(_nameReleased()));

    // patient age
    label = new QLabel(trs("PatientAge"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_age = new Button();
    d_ptr->_age->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_AGE);
    d_ptr->_age->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_age
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_AGE
                          , d_ptr->_age);

    connect(d_ptr->_age, SIGNAL(released()), this, SLOT(_ageReleased()));

    // Patient Height
    label = new QLabel(trs("PatientHeight"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_height = new Button();
    d_ptr->_height->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT);
    d_ptr->_height->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_height
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_HEIGHT
                          , d_ptr->_height);
    connect(d_ptr->_height, SIGNAL(released()), this, SLOT(_heightReleased()));

    // Patient Weight
    label = new QLabel(trs("PatientWeight"));
    layout->addWidget(label
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 0);
    d_ptr->_weight = new Button();
    d_ptr->_weight->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT);
    d_ptr->_weight->setProperty("Item" , qVariantFromValue(itemId));
    layout->addWidget(d_ptr->_weight
                      , d_ptr->combos.count() + d_ptr->buttons.count()
                      , 1);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_PATIENT_WEIGHT
                          , d_ptr->_weight);
    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count() , 1);
    backgroundLayout->addLayout(layout , 1 , 0);
    connect(d_ptr->_weight, SIGNAL(released()), this, SLOT(_weightReleased()));

    // create patient button
    d_ptr->_savePatientInfo = new Button(trs("PatientCreate"));
    d_ptr->_savePatientInfo->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CREATE_PATIENT);
    d_ptr->_savePatientInfo->setProperty("Item" , qVariantFromValue(itemId));
    buttonLayout->addStretch(50);
    buttonLayout->addWidget(d_ptr->_savePatientInfo, 150);
    buttonLayout->addStretch(50);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_CREATE_PATIENT
                          , d_ptr->_savePatientInfo);

    connect(d_ptr->_savePatientInfo, SIGNAL(released()), this, SLOT(_saveInfoReleased()));

    // clean patient data
    d_ptr->_relievePatient = new Button(trs("CleanPatientData"));
    d_ptr->_relievePatient->setButtonStyle(Button::ButtonTextOnly);
    itemId = static_cast<int>(PatientInfoWindowPrivate::ITEM_BTN_CLEAR_PATIENT);
    d_ptr->_relievePatient->setProperty("Item" , qVariantFromValue(itemId));
    buttonLayout->addStretch(50);
    buttonLayout->addWidget(d_ptr->_relievePatient, 150);
    buttonLayout->addStretch(50);
    d_ptr->buttons.insert(PatientInfoWindowPrivate::ITEM_BTN_CLEAR_PATIENT
                          , d_ptr->_relievePatient);
    connect(d_ptr->_relievePatient, SIGNAL(released()), this, SLOT(_relieveReleased()));

    backgroundLayout->addLayout(buttonLayout , 2 , 0);
    setWindowLayout(backgroundLayout);
}

void PatientInfoWindow::widgetChange()
{
    if (d_ptr->_patientNew == false && d_ptr->_relieveFlag == false)
    {
        d_ptr->_savePatientInfo->setText(trs("PatientCreate"));
        d_ptr->_relievePatient->setText(trs("CleanPatientData"));
        d_ptr->_relievePatient->setVisible(true);
    }
    else if (d_ptr->_patientNew == true)
    {
        d_ptr->_savePatientInfo->setText(trs("EnglishYESChineseSURE"));
        d_ptr->_relievePatient->hide();
    }
    else if (d_ptr->_patientNew == false && d_ptr->_relieveFlag == true)
    {
        d_ptr->_savePatientInfo->setText(trs("PatientCreate"));
        d_ptr->_relievePatient->setText(trs("RelievePatient"));
        d_ptr->_relievePatient->setVisible(true);
    }
}

void PatientInfoWindow::relieveStatus(bool relStauts)
{
    d_ptr->_relieveFlag = relStauts;
}

void PatientInfoWindow::newPatientStatus(bool pStatus)
{
    d_ptr->_patientNew = pStatus;
}

void PatientInfoWindow::autoShow()
{
    if (isVisible())
    {
        hide();
    }
    else
    {
        setFixedWidth(500);
        setFixedHeight(630);
        exec();
    }
}

void PatientInfoWindow::_idReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientID"));
    englishPanel.setMaxInputLength(100);
    englishPanel.setInitString(d_ptr->_id->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->_id->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::_nameReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientName"));
    englishPanel.setMaxInputLength(100);
    englishPanel.setInitString(d_ptr->_name->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        d_ptr->_name->setText(text.toUtf8().constData());
    }
}

void PatientInfoWindow::_ageReleased()
{
    KeyInputPanel inputPanel;
    inputPanel.setWindowTitle(trs("PatientAge"));
    inputPanel.setMaxInputLength(3);
    inputPanel.setInitString(d_ptr->_age->text());
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
            d_ptr->_age->setText(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            d_ptr->_age->setText("");
        }
        d_ptr->_infoChange = true;
    }
}

void PatientInfoWindow::_heightReleased()
{
    KeyInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientHeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(d_ptr->_height->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkHeightValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            d_ptr->_height->setText(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            d_ptr->_height->setText("");
        }
        d_ptr->_infoChange = true;
    }
}

void PatientInfoWindow::_weightReleased()
{
    KeyInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientWeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(d_ptr->_weight->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkWeightValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            d_ptr->_weight->setText(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            d_ptr->_weight->setText("");
        }
        d_ptr->_infoChange = true;
    }
}

void PatientInfoWindow::_relieveReleased()
{
    this->hide();
    if (d_ptr->_relieveFlag == true)
    {
        RelievePatientWindow::getInstance()->setWindowTitle(trs("RelievePatient"));
        RelievePatientWindow::getInstance()->exec();
    }
    else
    {
        RelievePatientWindow::getInstance()->setWindowTitle(trs("CleanPatientData"));
        RelievePatientWindow::getInstance()->exec();
    }
}

void PatientInfoWindow::_saveInfoReleased()
{
    int warnStatus;

    if (d_ptr->_patientNew == true)
    {
        this->hide();
        d_ptr->_relieveFlag = true;
    }
    else if (d_ptr->_patientNew == false && d_ptr->_relieveFlag == false)
    {
        QStringList slist;
        slist << trs("EnglishYESChineseSURE") << trs("No") << trs("Cancel");
        MessageBox messageBox(trs("Warn"), trs("ApplicationDataToReceivingPatients"), slist);
        this->hide();
        warnStatus = messageBox.exec();
        if (warnStatus == 0)
        {
            d_ptr->_relieveFlag = true;
        }
        else if (warnStatus == 1)
        {
            d_ptr->_relieveFlag = true;
            dataStorageDirManager.createDir(true);
        }
    }
    else if (d_ptr->_patientNew == false && d_ptr->_relieveFlag == true)
    {
        this->hide();
        QStringList slist;
        slist << trs("EnglishYESChineseSURE") << trs("No");
        MessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
        if (messageBox.exec() == 0)
        {
            dataStorageDirManager.createDir(true);
        }
    }
}

PatientInfoWindow::~PatientInfoWindow()
{
    delete d_ptr;
}









