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
#include "Button.h"
#include "LanguageManager.h"
#include "QGridLayout"
#include "PatientInfoWindow.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include "DataStorageDirManager.h"
#include <QApplication>

class PatientManagementMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_PATIENT_INFO,
        ITEM_BTN_NEW_PATIENT
    };

    PatientManagementMenuContentPrivate()
                : patientInfo(NULL),
                  newPatient(NULL)
    {
    }

    Button *patientInfo;
    Button *newPatient;
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
    btn = new Button(trs("NewPatient"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setProperty("Item", qVariantFromValue(index));
    glayout->addWidget(btn, index, 1);
    index++;
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));

    glayout->setRowStretch(index, 1);
}

void PatientManagementMenuContent::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    int index = btn->property("Item").toInt();
    switch (index)
    {
        case PatientManagementMenuContentPrivate::ITEM_BTN_PATIENT_INFO:
        {
            patientInfoWindow.newPatientStatus(false);
            patientInfoWindow.widgetChange();
            windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseOthers
                                     | WindowManager::ShowBehaviorCloseIfVisiable);
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
            MessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
            if (messageBox.exec() == 1)
            {
                dataStorageDirManager.createDir(true);
                patientInfoWindow.newPatientStatus(true);
                patientInfoWindow.widgetChange();
                windowManager.showWindow(&patientInfoWindow , WindowManager::ShowBehaviorCloseOthers);
            }
        }
        break;
    }
}
