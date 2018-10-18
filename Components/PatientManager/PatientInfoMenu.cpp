/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/26
 **/



#include <QHBoxLayout>
#include <QVBoxLayout>
#include "PatientInfoMenu.h"
#include "WindowManager.h"
#include "PatientManager.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "IButton.h"
#include "KeyBoardPanel.h"
#include "RelievePatientWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "EnglishInputPanel.h"
#include "LayoutManager.h"

PatientInfoMenu *PatientInfoMenu::_selfObj = NULL;

/**************************************************************************************************
 * 检查age的合法性。
 *************************************************************************************************/
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

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void PatientInfoMenu::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight());

    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("PatientInformation"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _id = new LabelButton(trs("PatientID"));
    _id->setFont(fontManager.textFont(fontSize));
    _id->label->setFixedSize(labelWidth, _itemH);
    _id->button->setFixedSize(btnWidth, _itemH);

    _type = new IComboList(trs("PatientType"));
    _type->setFont(fontManager.textFont(fontSize));
    _type->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT)));
    _type->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_PED)));
    _type->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    _type->label->setFixedSize(labelWidth, _itemH);
    _type->combolist->setFixedSize(btnWidth, _itemH);

    _sex = new IComboList(trs("PatientSex"));
    _sex->setFont(fontManager.textFont(fontSize));
    _sex->addItem("");
    _sex->addItem(trs(PatientSymbol::convert(PATIENT_SEX_MALE)));
    _sex->addItem(trs(PatientSymbol::convert(PATIENT_SEX_FEMALE)));
    _sex->label->setFixedSize(labelWidth, _itemH);
    _sex->combolist->setFixedSize(btnWidth, _itemH);

    _blood = new IComboList(trs("PatientBloodType"));
    _blood->setFont(fontManager.textFont(fontSize));
    _blood->addItem("");
    _blood->addItem(trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_A)));
    _blood->addItem(trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_B)));
    _blood->addItem(trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_AB)));
    _blood->addItem(trs(PatientSymbol::convert(PATIENT_BLOOD_TYPE_O)));
    _blood->label->setFixedSize(labelWidth, _itemH);
    _blood->combolist->setFixedSize(btnWidth, _itemH);

    _name = new LabelButton(trs("PatientName"));
    _name->setFont(fontManager.textFont(fontSize));
    _name->label->setFixedSize(labelWidth, _itemH);
    _name->button->setFixedSize(btnWidth, _itemH);

    _pacer = new IComboList(trs("PatientPacemarker"));
    _pacer->setFont(fontManager.textFont(fontSize));
    _pacer->addItem(trs(PatientSymbol::convert(PATIENT_PACER_OFF)));
    _pacer->addItem(trs(PatientSymbol::convert(PATIENT_PACER_ON)));
    _pacer->label->setFixedSize(labelWidth, _itemH);
    _pacer->combolist->setFixedSize(btnWidth, _itemH);

    _age = new LabelButton(trs("PatientAge"));
    _age->setFont(fontManager.textFont(fontSize));
    _age->label->setFixedSize(labelWidth, _itemH);
    _age->button->setFixedSize(btnWidth, _itemH);

    _height = new LabelButton(trs("PatientHeight"));
    _height->setFont(fontManager.textFont(fontSize));
    _height->label->setFixedSize(labelWidth, _itemH);
    _height->button->setFixedSize(btnWidth, _itemH);

    _weight = new LabelButton(trs("PatientWeight"));
    _weight->setFont(fontManager.textFont(fontSize));
    _weight->label->setFixedSize(labelWidth, _itemH);
    _weight->button->setFixedSize(btnWidth, _itemH);

    _relievePatient = new IButton(trs("RelievePatient"));
    _relievePatient->setFixedSize(btnWidth, _itemH);
    _relievePatient->setFont(fontManager.textFont(fontSize));
    _relievePatient->setBorderEnabled(true);

    _savePatientInfo = new IButton();
    _savePatientInfo->setFixedSize(btnWidth, _itemH);
    _savePatientInfo->setFont(fontManager.textFont(fontSize));
    _savePatientInfo->setBorderEnabled(true);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_relievePatient);
    hlayout->setSpacing(1);
    hlayout->addWidget(_savePatientInfo);
    contentLayout->addWidget(_type);
    contentLayout->addWidget(_pacer);
    contentLayout->addWidget(_id);
    contentLayout->addWidget(_sex);
    contentLayout->addWidget(_blood);
    contentLayout->addWidget(_name);
    contentLayout->addWidget(_age);
    contentLayout->addWidget(_height);
    contentLayout->addWidget(_weight);
    contentLayout->addStretch();
    contentLayout->addLayout(hlayout);
    contentLayout->setSpacing(6);
    contentLayout->addStretch();

    connect(_id->button, SIGNAL(realReleased()), this, SLOT(_idReleased()));
    connect(_name->button, SIGNAL(realReleased()), this, SLOT(_nameReleased()));
    connect(_age->button, SIGNAL(realReleased()), this, SLOT(_ageReleased()));
    connect(_height->button, SIGNAL(realReleased()), this, SLOT(_heightReleased()));
    connect(_weight->button, SIGNAL(realReleased()), this, SLOT(_weightReleased()));
    connect(_relievePatient, SIGNAL(realReleased()), this, SLOT(_relieveReleased()));
    connect(_savePatientInfo, SIGNAL(realReleased()), this, SLOT(_saveInfoReleased()));
}


/***************************************************************************************************
 * 根据标志位设置对应窗口病人信息设置显示
 **************************************************************************************************/
void PatientInfoMenu::widgetChange(void)
{

//    _getPatientInfo->hide();
//    if (visible)
//    {
//        _savePatientInfo->setText(trs("PatientSave"));
//        _relievePatient->setVisible(true);
//    }
//    else
//    {
//        _savePatientInfo->setText(trs("PatientCreate"));
//        _relievePatient->hide();
//    }
    if (_patientNew == false && _relieveFlag == false)
    {
        _savePatientInfo->setText(trs("PatientCreate"));
        _relievePatient->setText(trs("CleanPatientData"));
        _relievePatient->setVisible(true);
    }
    else if (_patientNew == true)
    {
        _savePatientInfo->setText(trs("EnglishYESChineseSURE"));
        _relievePatient->hide();
    }
    else if (_patientNew == false && _relieveFlag == true)
    {
        _savePatientInfo->setText(trs("PatientCreate"));
        _relievePatient->setText(trs("RelievePatient"));
        _relievePatient->setVisible(true);
    }
}

/***************************************************************************************************
 * 解除病人标志位设置
 **************************************************************************************************/
void PatientInfoMenu::relieveStatus(bool flag)
{
    _relieveFlag = flag;
}

/***************************************************************************************************
 * 接收病人和病人信息标志位设置
 **************************************************************************************************/
void PatientInfoMenu::newPatientStatus(bool flag)
{
    _patientNew = flag;
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void PatientInfoMenu::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = layoutManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void PatientInfoMenu::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        focusPreviousChild();
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        focusNextChild();
        break;
    default:
        break;
    }

    QWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * ID按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_idReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientID"));
    englishPanel.setMaxInputLength(patientManager.getIDLength());
    englishPanel.setInitString(_id->button->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        _id->setValue(text.toUtf8().constData());
    }
}

/**************************************************************************************************
 * Name按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_nameReleased()
{
    EnglishInputPanel englishPanel;
    englishPanel.setWindowTitle(trs("PatientName"));
    englishPanel.setMaxInputLength(patientManager.getIDLength());
    englishPanel.setInitString(_name->button->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        _name->setValue(text.toUtf8().constData());
    }
}

/**************************************************************************************************
 * Age按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_ageReleased()
{
    KeyBoardPanel englishPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    englishPanel.setTitleBarText(trs("PatientAge"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(_age->button->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkAgeValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            _age->setValue(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            _age->setValue("");
        }
        _infoChange = true;
    }
}

/**************************************************************************************************
 * Height按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_heightReleased()
{
    KeyBoardPanel englishPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    englishPanel.setTitleBarText(trs("PatientHeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(_height->button->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkAgeValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            _height->setValue(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            _height->setValue("");
        }
        _infoChange = true;
    }
}

/**************************************************************************************************
 * Weight按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_weightReleased()
{
    KeyBoardPanel englishPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    englishPanel.setTitleBarText(trs("PatientWeight"));
    englishPanel.setMaxInputLength(3);
    englishPanel.setInitString(_weight->button->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setSymbolEnable(false);
    englishPanel.setKeytypeSwitchEnable(false);
    englishPanel.setCheckValueHook(checkAgeValue);

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        bool ok = false;
        int age = text.toInt(&ok);
        if (ok)
        {
            patientManager.setAge(age);
            _weight->setValue(QString::number(age));
        }
        else if (text.isEmpty())
        {
            patientManager.setAge(-1);
            _weight->setValue("");
        }
        _infoChange = true;
    }
}

/**************************************************************************************************
 * 解除病人按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_relieveReleased()
{
    this->hide();
    if (_relieveFlag == true)
    {
        relievePatientWidget.setTitleBarText(trs("RelievePatient"));
        relievePatientWidget.autoShow();
    }
    else
    {
        relievePatientWidget.setTitleBarText(trs("CleanPatientData"));
        relievePatientWidget.autoShow();
    }
}

/**************************************************************************************************
 * 保存/新病人按键槽函数。
 *************************************************************************************************/
void PatientInfoMenu::_saveInfoReleased()
{
    int warnStatus;

    if (_patientNew == true)
    {
        this->hide();
        _relieveFlag = true;
    }
    else if (_patientNew == false && _relieveFlag == false)
    {
        QStringList slist;
        slist << trs("EnglishYESChineseSURE") << trs("No") << trs("Cancel");
        IMessageBox messageBox(trs("Warn"), trs("ApplicationDataToReceivingPatients"), slist);
        this->hide();
        warnStatus = messageBox.exec();
        if (warnStatus == 0)
        {
            _relieveFlag = true;
        }
        else if (warnStatus == 1)
        {
            _relieveFlag = true;
            dataStorageDirManager.createDir(true);
        }
    }
    else if (_patientNew == false && _relieveFlag == true)
    {
        this->hide();
        QStringList slist;
        slist << trs("EnglishYESChineseSURE") << trs("No");
        IMessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
        if (messageBox.exec() == 0)
        {
            dataStorageDirManager.createDir(true);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoMenu::PatientInfoMenu() : PopupWidget(), _patientNew(false), _relieveFlag(false)
{
    layoutExec();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoMenu::~PatientInfoMenu()
{
}
