/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/10
 **/

#include "PatientManagementMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include "QGridLayout"
#include "PatientManager.h"
#include "PatientInfoWindow.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include <QApplication>
#include "RescueDataDeleteWindow.h"
#include "DischargePatientWindow.h"
#include <QVariant>

class PatientManagementMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_PATIENT_INFO,
        ITEM_BTN_NEW_PATIENT,
        ITEM_BTN_DISCHARGE_PATIENT,
        ITEM_BTN_DATA_DELETE_CASE
    };

    PatientManagementMenuContentPrivate()
                : dischargePatient(NULL)
    {
    }

    Button *dischargePatient;

    void loadOption();
};



PatientManagementMenuContent::PatientManagementMenuContent()
                            : MenuContent(trs("PatientManager"),
                                          trs("PatientManagerDesc")),
                              d_ptr(new PatientManagementMenuContentPrivate)
{
}

PatientManagementMenuContent::~PatientManagementMenuContent()
{
    delete d_ptr;
}

void PatientManagementMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 1);
    Button *btn;
    int index = 0;

    // patientInfo
    btn = new Button(trs("PatientInfo"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(index));
    glayout->addWidget(btn, index, 1);
    index++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));

    // newPatient
    btn = new Button(trs("AdmitPatient"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(index));
    glayout->addWidget(btn, index, 1);
    index++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));

    // discharge patient
    btn = new Button(trs("RelievePatient"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(index));
    glayout->addWidget(btn, index, 1);
    index++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->dischargePatient = btn;

    // recues data delete case
    btn = new Button(trs("DeletePatientData"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(index));
    glayout->addWidget(btn, index, 1);
    index++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));

    glayout->setRowStretch(index, 1);
}

void PatientManagementMenuContent::showEvent(QShowEvent *ev)
{
    d_ptr->loadOption();
    MenuContent::showEvent(ev);
}

void PatientManagementMenuContent::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    int index = btn->property("Item").toInt();
    switch (index)
    {
        case PatientManagementMenuContentPrivate::ITEM_BTN_PATIENT_INFO:
        {
            if (!patientManager.isMonitoring())
            {
                patientManager.newPatient();
            }
            windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseIfVisiable
                                                        | WindowManager::ShowBehaviorCloseOthers);
        }
        break;
        case PatientManagementMenuContentPrivate::ITEM_BTN_NEW_PATIENT:
        {
            bool isVisible = patientInfoWindow.isVisible();
            if (isVisible)
            {
                break;
            }

            // 创建新病人
            QStringList slist;
            slist << trs("No") << trs("EnglishYESChineseSURE");
            MessageBox messageBox(trs("Warn"), trs("DischargeAndRecePatient"), slist, true);
            if (messageBox.exec() == 1)
            {
                patientManager.newPatient();
                windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseOthers);
            }
        }
        break;
        case PatientManagementMenuContentPrivate::ITEM_BTN_DISCHARGE_PATIENT:
        {
            DischargePatientWindow dischargeWin;
            if (dischargeWin.exec() == QDialog::Accepted)
            {
                patientManager.dischargePatient();
            }
        }
        break;
        case PatientManagementMenuContentPrivate::ITEM_BTN_DATA_DELETE_CASE:
        {
            bool isVisible = rescueDataDeleteWindow.isVisible();
            while (NULL != QApplication::activeModalWidget())
            {
                QApplication::activeModalWidget()->hide();
            }

            if (isVisible)
            {
                return;
            }
            windowManager.showWindow(&rescueDataDeleteWindow , WindowManager::ShowBehaviorCloseIfVisiable);
        }
        break;
        default:
            break;
    }
}

void PatientManagementMenuContentPrivate::loadOption()
{
    if (patientManager.isMonitoring())
    {
        dischargePatient->setText(trs("RelievePatient"));
    }
    else
    {
        dischargePatient->setText(trs("ClearPatientData"));
    }
}
