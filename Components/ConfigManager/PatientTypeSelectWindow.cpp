/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/26
 **/

#include "PatientTypeSelectWindow.h"
#include "LanguageManager.h"
#include "ListView.h"
#include <QGridLayout>
#include "ListDataModel.h"
#include "Button.h"
#include "ConfigManager.h"
#include <QMap>
#include "SystemManagerInterface.h"

#define CONFIG_DIR  ("/usr/local/nPM/etc/")
#define CONFIG_MAX_NUM (3)
#define LISTVIEW_MAX_VISIABLE_COUNT (2 * CONFIG_MAX_NUM)
class PatientTypeSelectWindowPrivate
{
public:
    PatientTypeSelectWindowPrivate()
                : listView(NULL)
                , model(NULL)
                , btn(NULL)
                , configPath("")
                , patientType(PATIENT_TYPE_NULL)
    {}

    ListView *listView;
    ListDataModel *model;
    Button *btn;
    QString configPath;
    QStringList nameList;
    PatientType patientType;
    QMap<QString, PatientType> patientTypeMap;
};

PatientTypeSelectWindow::PatientTypeSelectWindow()
                       : Dialog(),
                         d_ptr(new PatientTypeSelectWindowPrivate)
{
    setWindowTitle(trs("SelectOriginalConfig"));
    ListView * listView = new ListView();
    ListDataModel *dataModel = new ListDataModel(listView);
    listView->setModel(dataModel);
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_COUNT * dataModel->getRowHeightHint()
                             + (LISTVIEW_MAX_VISIABLE_COUNT + 2) * listView->spacing());
    listView->setRowsSelectMode();
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->listView = listView;
    d_ptr->model = dataModel;

    // 加载数据
    QString defaultStr = trs("DefaultConfig");
    QStringList nameList;
    SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();
    if (systemManager && !systemManager->isNeonateMachine())
    {
        nameList.append(QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))));
        nameList.append(QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_PED))));
    }
    nameList.append(QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_NEO))));
    QList<ConfigManager::UserDefineConfigInfo> userConfig
            = configManager.getUserDefineConfigInfos();
    int count = userConfig.count();
    for (int i = 0; i < count; i++)
    {
        nameList.append(QString("%1(%2)")
                              .arg(userConfig[i].name)
                              .arg(trs(userConfig[i].patType)));
    }
    dataModel->setStringList(nameList);

    // 保存对应id的配置文件名称
    nameList.clear();
    if (systemManager && !systemManager->isNeonateMachine())
    {
        nameList.append("AdultConfig.Original.xml");
        nameList.append("PedConfig.Original.xml");
    }
    nameList.append("NeoConfig.Original.xml");

    if (systemManager && !systemManager->isNeonateMachine())
    {
        d_ptr->patientTypeMap.insert(nameList.at(0), PATIENT_TYPE_ADULT);
        d_ptr->patientTypeMap.insert(nameList.at(1), PATIENT_TYPE_PED);
        d_ptr->patientTypeMap.insert(nameList.at(2), PATIENT_TYPE_NEO);
    }
    else
    {
        d_ptr->patientTypeMap.insert(nameList.at(0), PATIENT_TYPE_NEO);
    }

    for (int i = 0; i < count; i++)
    {
        QString patType = userConfig[i].patType;
        nameList.append(userConfig[i].fileName);
        PatientType type = PATIENT_TYPE_NULL;
        if (PatientSymbol::convert(PATIENT_TYPE_ADULT) == patType)
        {
            type = PATIENT_TYPE_ADULT;
        }
        else if (PatientSymbol::convert(PATIENT_TYPE_PED) == patType)
        {
            type = PATIENT_TYPE_PED;
        }
        else if (PatientSymbol::convert(PATIENT_TYPE_NEO) == patType)
        {
            type = PATIENT_TYPE_NEO;
        }
        d_ptr->patientTypeMap.insert(userConfig[i].fileName, type);
    }
    d_ptr->nameList = nameList;


    Button *btn = new Button(trs("Ok"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setEnabled(false);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setFixedWidth(100);
    d_ptr->btn = btn;

    QGridLayout *glayout = new QGridLayout;
    glayout->setMargin(10);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 14);
    glayout->setColumnStretch(2, 1);
    glayout->setVerticalSpacing(4);

    glayout->addWidget(listView, 0, 1);
    glayout->addWidget(btn, 1, 1, Qt::AlignRight);
    setWindowLayout(glayout);
    setFixedSize(450, 460);
}

PatientTypeSelectWindow::~PatientTypeSelectWindow()
{
    delete d_ptr;
}

void PatientTypeSelectWindow::getConfigInfo(PatientType &patientType, QString &configPath) const
{
    patientType = d_ptr->patientType;
    configPath = d_ptr->configPath;
}

void PatientTypeSelectWindow::updateBtnStatus()
{
    bool isEnable;
    if (d_ptr->listView->curCheckedRow() >= 0)
    {
        isEnable = true;
    }
    else
    {
        isEnable = false;
    }
    d_ptr->btn->setEnabled(isEnable);
}

void PatientTypeSelectWindow::onBtnReleased()
{
    int row = d_ptr->listView->curCheckedRow();
    if (row < 0)
    {
        return;
    }
    if (row >= d_ptr->nameList.count())
    {
        return;
    }
    QString name = d_ptr->nameList.at(row);
    d_ptr->patientType = d_ptr->patientTypeMap[name];
    // 更新配置文件完整路径
    d_ptr->configPath = QString("%1%2").arg(CONFIG_DIR).arg(name);

    // 选择有效item后退出
    this->accept();
}
