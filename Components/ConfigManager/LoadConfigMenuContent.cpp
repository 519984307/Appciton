/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13
 **/

#include "LoadConfigMenuContent.h"
#include "Button.h"
#include <QLabel>
#include <QMap>
#include "LanguageManager.h"
#include "ConfigManager.h"
#include <QList>
#include "IConfig.h"
#include <QFile>
#include "PatientManager.h"
#include "MessageBox.h"
#include "MenuManager.h"
#include <QHBoxLayout>
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "ConfigEditMenuWindow.h"
#include "WindowManager.h"
#include "SystemManager.h"
#include "LayoutManager.h"
#include "SoundManager.h"
#include "AlarmConfig.h"
#include "ColorManager.h"
#include "ECGParam.h"
#include "ECGDupParam.h"
#include "RESPParam.h"
#include "RESPDupParam.h"
#include "CO2Param.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_MAX_NUM 3
#define LISTVIEW_MAX_VISIABLE_TIME 6
class LoadConfigMenuContentPrivate
{
public:
    LoadConfigMenuContentPrivate();

    void loadConfigs();
    void updateConfigList();

    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;
    Button *loadBtn;
    Button *ViewBtn;
    ListView *configListView;
    ListDataModel *configDataModel;
};

LoadConfigMenuContentPrivate::LoadConfigMenuContentPrivate():
    lastSelectItem(NULL),
    curConfig(NULL),
    curEditIndex(0),
    loadBtn(NULL),
    ViewBtn(NULL),
    configListView(NULL),
    configDataModel(NULL)
{
    configs.clear();
}

void LoadConfigMenuContentPrivate::loadConfigs()
{
    configs.clear();
    QString defaultStr = trs("DefaultConfig");
    ConfigManager::UserDefineConfigInfo defaultConfig[] =
    {
        {QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))),
            "AdultConfig.Original.xml", PatientSymbol::convert(PATIENT_TYPE_ADULT)},
        {QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_PED))),
            "PedConfig.Original.xml", PatientSymbol::convert(PATIENT_TYPE_PED)},
        {QString("%1(%2)").arg(defaultStr).arg(trs(PatientSymbol::convert(PATIENT_TYPE_NEO))),
            "NeoConfig.Original.xml", PatientSymbol::convert(PATIENT_TYPE_NEO)}
    };
    for (int i = 0; i < CONFIG_MAX_NUM; i++)
    {
        configs.append(defaultConfig[i]);
    }
    configs = configs + configManager.getUserDefineConfigInfos();
}
/**
 * @brief LoadConfigMenuContentPrivate::updateConfigList
 *        update the config list widget
 */
void LoadConfigMenuContentPrivate::updateConfigList()
{
    QStringList configNameList;
    for (int i = 0; i < CONFIG_MAX_NUM; i++)
    {
        configNameList.append(configs.at(i).name);
    }
    QList<ConfigManager::UserDefineConfigInfo> userConfig
            = configManager.getUserDefineConfigInfos();
    for (int i = 0; i < userConfig.count(); i++)
    {
        // 重组用户配置字符串
        configNameList.append(QString("%1(%2)")
                              .arg(userConfig[i].name)
                              .arg(trs(userConfig[i].patType)));
    }
    configDataModel->setStringList(configNameList);

    int curSelectedRow = configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    loadBtn->setEnabled(isEnable);
    ViewBtn->setEnabled(isEnable);
}

LoadConfigMenuContent::LoadConfigMenuContent():
    MenuContent(trs("LoadConfigMenuContent"),
                trs("LoadConfigMenuContentDesc")),
    d_ptr(new LoadConfigMenuContentPrivate)
{
}

LoadConfigMenuContent::~LoadConfigMenuContent()
{
    delete d_ptr;
}

void LoadConfigMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void LoadConfigMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    // load config
    ListView *listView = new ListView();
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    layout->addWidget(listView);
    ListDataModel *model = new ListDataModel(this);
    d_ptr->configDataModel = model;
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    listView->setModel(model);
    listView->setRowsSelectMode();
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->configListView = listView;

    Button *button;
    QHBoxLayout *hl = new QHBoxLayout;

    // load buton
    button = new Button();
    button->setText(trs("Load"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->loadBtn = button;

    // view buton
    button = new Button();
    button->setText(trs("View"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->ViewBtn = button;

    layout->addLayout(hl);
    layout->addStretch();
}


void LoadConfigMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (!btn)
    {
        return;
    }

    // load the config
    if (btn == d_ptr->loadBtn)
    {
        MessageBox msg(trs("Prompt"), trs("ChangeConfig"), true, true);
        if (!msg.exec())
        {
            return;
        }
        // add new config setting
        int index = d_ptr->configListView->curCheckedRow();
        d_ptr->curEditIndex = index;

        // 更新病人类型
        int patitentTypeInt = 255;
        // 如果从原始文件加载，通过文件名称获取病人类型
        QString fileNameStr(d_ptr->configs.at(index).fileName);
        if (fileNameStr.indexOf("Adult") >= 0)
        {
            patitentTypeInt = PATIENT_TYPE_ADULT;
        }
        else if (fileNameStr.indexOf("Ped") >= 0)
        {
            patitentTypeInt = PATIENT_TYPE_PED;
        }
        else if (fileNameStr.indexOf("Neo") >= 0)
        {
            patitentTypeInt = PATIENT_TYPE_NEO;
        }
        else
        {
            // 如果是从用户自定义文件加载，通过病人类型标记获取病人类型
            QString fileName(d_ptr->configs.at(index).patType);

            if (fileName == PatientSymbol::convert(PATIENT_TYPE_ADULT))
            {
                patitentTypeInt = PATIENT_TYPE_ADULT;
            }
            else if (fileName == PatientSymbol::convert(PATIENT_TYPE_PED))
            {
                patitentTypeInt = PATIENT_TYPE_PED;
            }
            else if (fileName == PatientSymbol::convert(PATIENT_TYPE_NEO))
            {
                patitentTypeInt = PATIENT_TYPE_NEO;
            }
            else
            {
                patitentTypeInt = PATIENT_TYPE_ADULT;
            }
        }
        // 更新病人类型 、文件名称
        systemConfig.setNumValue("General|PatientType", patitentTypeInt);
        patientManager.setType(static_cast<PatientType>(patitentTypeInt));
        systemConfig.updateCurConfigName();

        // 更新当前选择的文件
        QString curConfigName = systemConfig.getCurConfigName();
        currentConfig.allowToSave(false);
        QFile::remove(curConfigName);
        QString loadPath = QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName);
        QFile::copy(loadPath, curConfigName);
        currentConfig.setCurrentFilePath(curConfigName);
        currentConfig.allowToSave(true);
        currentConfig.load(loadPath);
        alarmConfig.clearLimitAlarmInfo();
        colorManager.clearColorMap();
        configManager.setOriginalConfig(static_cast<PatientType>(patitentTypeInt), d_ptr->configs.at(index).fileName);

        soundManager.volumeInit();
        layoutManager.updateLayoutWidgetsConfig();
    }
    // view config
    else if (btn == d_ptr->ViewBtn)
    {
        int index = d_ptr->configListView->curCheckedRow();
        if (index != -1)
        {
            QString fileName = d_ptr->configs.at(index).fileName;
            QString name = d_ptr->configs.at(index).name;
            d_ptr->curEditIndex = index;
            d_ptr->curConfig = new Config(QString("%1/%2")
                                          .arg(CONFIG_DIR)
                                          .arg(fileName));
            ConfigEditMenuWindow *cmWnd = new ConfigEditMenuWindow;
            cmWnd->setCurrentEditConfigName(name);

            QString strType = d_ptr->configs.at(index).patType;
            PatientType type = PATIENT_TYPE_ADULT;
            if (strType == PatientSymbol::convert(PATIENT_TYPE_NEO))
            {
                type = PATIENT_TYPE_NEO;
            }
            else if (strType == PatientSymbol::convert(PATIENT_TYPE_PED))
            {
                type = PATIENT_TYPE_PED;
            }
            cmWnd->setCurrentEditConfigInfo(d_ptr->curConfig, type);
            cmWnd->initializeSubMenu();

            QString pathName;
            if (fileName.indexOf("User") >= 0)
            {
                pathName = "View-";
                pathName += name;
            }
            else
            {
                pathName = "View-DefaultSetting";
            }
            cmWnd->setWindowTitlePrefix(pathName);

            // force updating the first window title
            QMetaObject::invokeMethod(cmWnd, "onVisiableItemChanged", Q_ARG(int, 0));

            configManager.setWidgetIfOnlyShown(true);
            windowManager.showWindow(cmWnd, WindowManager::ShowBehaviorModal);
            configManager.setWidgetIfOnlyShown(false);
            if (cmWnd)
            {
                delete cmWnd;
                cmWnd = NULL;
            }
            if (d_ptr->curConfig)
            {
                delete d_ptr->curConfig;
                d_ptr->curConfig = NULL;
            }
        }
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}

void LoadConfigMenuContent::updateBtnStatus()
{
    int row = d_ptr->configListView->curCheckedRow();
    bool isEnabled;
    if (row == -1)
    {
        isEnabled = false;
    }
    else
    {
        isEnabled = true;
    }

    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        // can't load config in the demo mode
        d_ptr->loadBtn->setEnabled(false);
    }
    else
    {
        d_ptr->loadBtn->setEnabled(isEnabled);
    }
    d_ptr->ViewBtn->setEnabled(isEnabled);
}
