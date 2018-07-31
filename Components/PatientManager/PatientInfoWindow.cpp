/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#include "PatientInfoWindow.h"
#include <QMap>
#include <QLabel>
#include "Button.h"
#include "ComboBox.h"
#include "LanguageManager.h"
#include "PatientDefine.h"
#include <QEvent>
#include <QGridLayout>
#include "WindowManager.h"
#include "EnglishInputPanel.h"
#include "KeyInputPanel.h"
#include "PatientManager.h"
#include "RelievePatientWindow.h"
#include "DataStorageDirManager.h"
#include "MessageBox.h"

class PatientInfoWindowPrivate
{
public:
    static const int itemH = 30;
    enum MenuItem
    {
        ITEM_CBO_TYPE = 0,
        ITEM_CBO_SEX,
        ITEM_CBO_BLOOD,
        ITEM_CBO_PACER,
        ITEM__CBO_MAX,

        ITEM_BTN_ID = 0,
        ITEM_BTN_NAME,
        ITEM_BTN_AGE,
        ITEM_BTN_HEIGHT,
        ITEM_BTN_WEIGHT,
        ITEM_BTN_RELIEVE,
        ITEM_BTN_SAVEINFO,
        ITEM_BTN_MAX,
    };

    PatientInfoWindowPrivate();

    bool isInfochange;
    bool isPatientNew;
    bool isRelieveFlag;

    QMap <MenuItem, ComboBox *> combos;
    QMap <MenuItem, Button *> btns;
};

PatientInfoWindowPrivate::PatientInfoWindowPrivate()
    : isInfochange(false),
      isPatientNew(false),
      isRelieveFlag(false)
{
    combos.clear();
    btns.clear();
}

PatientInfoWindow *PatientInfoWindow::getInstance()
{
    static PatientInfoWindow *instance = NULL;
    if (!instance)
    {
        instance = new PatientInfoWindow;
    }
    return instance;
}
/**
 * @brief checkAgeValue
 * @param value
 * @return
 */
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
PatientInfoWindow::PatientInfoWindow()
    : Window(),
      d_ptr(new PatientInfoWindowPrivate)
{
    setWindowTitle(trs("PatientInformation"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(100, 10, 100, 20);

    QLabel *label;
    Button *button;
    ComboBox *combo;
    int itemId;

    label = new QLabel(trs("PatientID"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_ID, button);
    itemId = d_ptr->ITEM_BTN_ID;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("PatientType"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))
                    << trs(PatientSymbol::convert(PATIENT_TYPE_PED))
                    << trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));

    layout->addWidget(combo, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(d_ptr->ITEM_CBO_TYPE, combo);
    itemId = d_ptr->ITEM_CBO_TYPE;
    button->setProperty("comboItem", qVariantFromValue(itemId));
    connect(combo, SIGNAL(released()), this, SLOT(onComboIndexChanged()));

    label = new QLabel(trs("PatientSex"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(PatientSymbol::convert(PATIENT_SEX_MALE))
                    << trs(PatientSymbol::convert(PATIENT_SEX_FEMALE)));

    layout->addWidget(combo, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(d_ptr->ITEM_CBO_SEX, combo);
    itemId = d_ptr->ITEM_CBO_SEX;
    button->setProperty("comboItem", qVariantFromValue(itemId));
    connect(combo, SIGNAL(released()), this, SLOT(onComboIndexChanged()));


    label = new QLabel(trs("PatientBloodType"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_A))
                    << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_B))
                    << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_AB))
                    << trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_O)));

    layout->addWidget(combo, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(d_ptr->ITEM_CBO_BLOOD, combo);
    itemId = d_ptr->ITEM_CBO_BLOOD;
    button->setProperty("comboItem", qVariantFromValue(itemId));
    connect(combo, SIGNAL(released()), this, SLOT(onComboIndexChanged()));

    label = new QLabel(trs("PatientName"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_NAME, button);
    itemId = d_ptr->ITEM_BTN_NAME;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("PatientPaceMarker"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(PatientSymbol::convert(PATIENT_PACER_OFF))
                    << trs(PatientSymbol::convert(PATIENT_PACER_ON)));

    layout->addWidget(combo, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(d_ptr->ITEM_CBO_PACER, combo);
    itemId = d_ptr->ITEM_CBO_PACER;
    button->setProperty("comboItem", qVariantFromValue(itemId));
    connect(combo, SIGNAL(released()), this, SLOT(onComboIndexChanged()));

    label = new QLabel(trs("PatientAge"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_AGE, button);
    itemId = d_ptr->ITEM_BTN_AGE;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("PatientHeight"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_HEIGHT, button);
    itemId = d_ptr->ITEM_BTN_HEIGHT;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("PatientWeight"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_WEIGHT, button);
    itemId = d_ptr->ITEM_BTN_WEIGHT;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("RelievePatient"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_RELIEVE, button);
    itemId = d_ptr->ITEM_BTN_RELIEVE;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    label = new QLabel(trs("SavePatient"));
    layout->addWidget(label, d_ptr->btns.count() + d_ptr->combos.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->btns.count() + d_ptr->combos.count(), 1);
    d_ptr->btns.insert(d_ptr->ITEM_BTN_SAVEINFO, button);
    itemId = d_ptr->ITEM_BTN_SAVEINFO;
    button->setProperty("btnItem", qVariantFromValue(itemId));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    layout->setRowStretch(d_ptr->btns.count() + d_ptr->combos.count(), 1);

    setWindowLayout(layout);
}


void PatientInfoWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    int indexType = btn->property("btnItem").toInt();

    EnglishInputPanel englishPanel;
    englishPanel.setMaxInputLength(patientManager.getIDLength());
    englishPanel.setInitString(btn->text());

    KeyInputPanel keyPanel(KeyInputPanel::KEY_TYPE_NUMBER);
    keyPanel.setMaxInputLength(3);
    keyPanel.setInitString(btn->text());
    keyPanel.setSpaceEnable(false);
    keyPanel.setSymbolEnable(false);
    keyPanel.setKeytypeSwitchEnable(false);

    switch (indexType)
    {
    case PatientInfoWindowPrivate::ITEM_BTN_ID:
        englishPanel.setWindowTitle(trs("PatientID"));
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_NAME:
        englishPanel.setWindowTitle(trs("PatientName"));
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_AGE:
        keyPanel.setWindowTitle(trs("PatientAge"));
        keyPanel.setCheckValueHook(checkAgeValue);
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_HEIGHT:
        keyPanel.setWindowTitle(trs("PatientHeight"));
        keyPanel.setCheckValueHook(checkHeightValue);
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_WEIGHT:
        keyPanel.setWindowTitle(trs("PatientWeight"));
        keyPanel.setCheckValueHook(checkWeightValue);
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_RELIEVE:
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_SAVEINFO:
        break;
    case PatientInfoWindowPrivate::ITEM_BTN_MAX:
        break;
    }

    if (indexType >=  PatientInfoWindowPrivate::ITEM_BTN_MAX)
    {
        return;
    }

    if (indexType <=  PatientInfoWindowPrivate::ITEM_BTN_NAME)
    {
        if (englishPanel.exec())
        {
            QString str(englishPanel.getStrValue());
            btn->setText(str.toUtf8().constData());
        }
        return;
    }

    if (indexType == PatientInfoWindowPrivate::ITEM_BTN_RELIEVE)
    {
        hide();
        QString str;
        if (d_ptr->isRelieveFlag)
        {
            str = "RelievePatient";
        }
        else
        {
            str = "CleanPatientData";
        }
        RelievePatientWindow::getInstance()->setWindowTitle(trs(str));
        RelievePatientWindow::getInstance()->exec();
        return;
    }

    if (indexType == PatientInfoWindowPrivate::ITEM_BTN_SAVEINFO)
    {
        if (d_ptr->isPatientNew)
        {
            hide();
            d_ptr->isRelieveFlag = true;
            return;
        }
        if (d_ptr->isRelieveFlag)
        {
            hide();
            QStringList sList;
            sList << trs("EnglishYESChineseSURE");
            sList << trs("No");
            MessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), sList);
            if (messageBox.exec())
            {
                return;
            }
            dataStorageDirManager.createDir(true);
            return;
        }
        hide();
        QStringList slist;
        slist << trs("EnglishYESChineseSURE") << trs("No") << trs("Cancel");
        MessageBox messageBox(trs("Warn"), trs("ApplicationDataToReceivingPatients"), slist);
        int warnStatus = messageBox.exec();
        dataStorageDirManager.createDir(!!warnStatus);
        d_ptr->isRelieveFlag = true;
        return;
    }

    if (!keyPanel.exec())
    {
        return;
    }

    QString str = keyPanel.getStrValue();
    bool isOk = false;
    int value = str.toInt(&isOk);
    if (isOk)
    {
        switch (indexType)
        {
        case PatientInfoWindowPrivate::ITEM_BTN_AGE:
            patientManager.setAge(value);
            break;
        case PatientInfoWindowPrivate::ITEM_BTN_HEIGHT:
            patientManager.setHeight(value);
            break;
        case PatientInfoWindowPrivate::ITEM_BTN_WEIGHT:
            patientManager.setWeight(value);
            break;
        }
        btn->setText(QString::number(value));
    }
    else if (str.isEmpty())
    {
        switch (indexType)
        {
        case PatientInfoWindowPrivate::ITEM_BTN_AGE:
            patientManager.setAge(-1);
            break;
        case PatientInfoWindowPrivate::ITEM_BTN_HEIGHT:
            patientManager.setHeight(-1);
            break;
        case PatientInfoWindowPrivate::ITEM_BTN_WEIGHT:
            patientManager.setWeight(-1);
            break;
        }
        btn->setText("");
    }
    d_ptr->isInfochange = true;
}

void PatientInfoWindow::onComboIndexChanged()
{
}

void PatientInfoWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);

    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void PatientInfoWindow::keyPressEvent(QKeyEvent *ev)
{
    Window::keyPressEvent(ev);
}

void PatientInfoWindow::widgetChange()
{
    Button *btnSaveInfo = d_ptr->btns[d_ptr->ITEM_BTN_SAVEINFO];
    Button *btnRelieve = d_ptr->btns[d_ptr->ITEM_BTN_RELIEVE];
    if (d_ptr->isPatientNew == false && d_ptr->isRelieveFlag == false)
    {
        btnSaveInfo->setText(trs("PatientCreate"));
        btnRelieve->setText(trs("CleanPatientData"));
        btnRelieve->setVisible(true);
    }
    else if (d_ptr->isPatientNew == true)
    {
        btnSaveInfo->setText(trs("EnglishYESChineseSURE"));
        btnRelieve->hide();
    }
    else if (d_ptr->isPatientNew == false && d_ptr->isRelieveFlag == true)
    {
        btnSaveInfo->setText(trs("PatientCreate"));
        btnRelieve->setText(trs("RelievePatient"));
        btnRelieve->setVisible(true);
    }
}

void PatientInfoWindow::relieveStatus(bool flag)
{
    d_ptr->isRelieveFlag = flag;
}

void PatientInfoWindow::newPatientStatus(bool flag)
{
    d_ptr->isPatientNew = flag;
}


PatientInfoWindow::~PatientInfoWindow()
{
    delete d_ptr;
}









