/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#include "ConfigManager.h"
#include <unistd.h>
#include <QProcess>
#include "TimeDate.h"
#include <dirent.h>
#include <QDir>
#include "ExportDataWidget.h"
#include "MessageBox.h"
#include "ImportFileSubWidget.h"
#include "ConfigExportImportMenuContent.h"
#include <QGridLayout>
#include <QMap>
#include "Button.h"
#include <QDomComment>
#include <QHBoxLayout>
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include <QLabel>
#include <QString>
#include "USBManager.h"
#include "ThemeManager.h"
#include <QTimer>

#define CONFIG_MAX_NUM 3

#define FILE_PATH          ("/media/usbdisk/etc")

#define CONFIG_DIR         ("/usr/local/nPM/etc/")

#define USER_DEFINE_CONFIG_PREFIX ("UserDefine")

#define LISTVIEW_MAX_VISIABLE_TIME (5)

#define HEIGHT_HINT (themeManger.getAcceptableControlHeight())

class ConfigExportImportMenuContentPrivate
{
public:
    /**
     * @brief ConfigExportImportMenuContentPrivate  构造方法
     */
    ConfigExportImportMenuContentPrivate():
        attrCheckFlag(true),
        configListView(NULL),
        configDataModel(NULL),
        exportBtn(NULL),
        importBtn(NULL),
        importFlag(255),
        infoLab(NULL),
        timer(NULL)
    {
        configs.clear();
        exportFileName.clear();
        importFileName.clear();
        localXml.clear();
        importXml.clear();
    }

    /**
     * @brief loadConfigs  装载数据方法
     */
    void loadConfigs();
    /**
     * @brief updateConfigList  更新配置列表
     */
    void updateConfigList();

    QList<ConfigManager::UserDefineConfigInfo> configs;

    QStringList      exportFileName;  //  导出的XML文件名称的链表
    QStringList      importFileName;  //  导入的XML文件名称的链表

    QTextStream       textStream;  //  日志接口

    QDomDocument      localXml;  //  本地检查文件

    QDomDocument      importXml;  //  导入检查文件

    bool attrCheckFlag;

    ListView   *configListView;
    ListDataModel *configDataModel;
    Button *exportBtn;
    Button *importBtn;
    int importFlag;
    QLabel *infoLab;  // 记录usb是否存在信息
    QTimer *timer;
};


void ConfigExportImportMenuContentPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuContentPrivate::updateConfigList()
{
    QStringList configNameList;
    for (int i = 0; i < configs.count(); i++)
    {
        configNameList.append(configs.at(i).name);
    }
    configDataModel->setStringList(configNameList);

    if (configNameList.isEmpty())
    {
        configListView->setEnabled(false);
    }
    else
    {
        configListView->setEnabled(true);
    }

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
    exportBtn->setEnabled(isEnable);

    if (configDataModel->getRowCount() >= CONFIG_MAX_NUM)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    importBtn->setEnabled(isEnable);
}

void ConfigExportImportMenuContent::onTimeOut()
{
    bool isEnable;
    if (usbManager.isUSBExist())
    {
        isEnable = true;
        d_ptr->infoLab->hide();
    }
    else
    {
        isEnable = false;
        d_ptr->infoLab->show();
    }
    updateBtnStatus();
    d_ptr->importBtn->setEnabled(isEnable);
}

ConfigExportImportMenuContent::ConfigExportImportMenuContent()
    : MenuContent(trs("ExportImport"), trs("ExportImportDesc")),
      d_ptr(new ConfigExportImportMenuContentPrivate)

{
}

ConfigExportImportMenuContent::~ConfigExportImportMenuContent()
{
    delete d_ptr;
}

void ConfigExportImportMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("ExportImportConfig"));
    layout->addWidget(label);

    ListView *listView = new ListView();
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    layout->addWidget(listView);
    ListDataModel *model = new ListDataModel(this);
    d_ptr->configDataModel = model;
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    listView->setModel(model);
    listView->setRowsSelectMode(true);
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->configListView = listView;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button;

    button = new Button(trs("Export"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->exportBtn = button;

    button = new Button(trs("Import"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->importBtn = button;

    layout->addLayout(hl);
    layout->addStretch(1);

    label = new QLabel(trs("WarningNoUSB"));
    label->setFixedHeight(HEIGHT_HINT);
    d_ptr->infoLab = label;
    layout->addWidget(label, 1, Qt::AlignRight);

    layout->addStretch(1);

    // 需要加入this，绑定父子关系，以便自动释放空间,避免造成内存泄露
    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start();
    d_ptr->timer = timer;
}

void ConfigExportImportMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void ConfigExportImportMenuContent::showEvent(QShowEvent *ev)
{
    MenuContent::showEvent(ev);

    d_ptr->timer->start();
    onTimeOut();
}

void ConfigExportImportMenuContent::hideEvent(QHideEvent *ev)
{
    MenuContent::hideEvent(ev);

    d_ptr->timer->stop();
}


/**************************************************************************************************
 * 导出xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenuContent::exportFileToUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return false;
    }

    // 如果没有对应文件夹，则主动创建此文件夹
    QDir dir;
    dir.mkdir(FILE_PATH);

    // get selected row file path names
    d_ptr->exportFileName.clear();
    int rowCount = d_ptr->configDataModel->getRowCount();
    QMap<int, bool> selectedMap = d_ptr->configListView->getRowsSelectMap();
    for (int i = 0; i < rowCount; i++)
    {
        if (selectedMap[i])
        {
            d_ptr->exportFileName.append(d_ptr->configs.at(i).fileName);
        }
    }

    // export data
    for (int i = 0; i < d_ptr->exportFileName.count(); i++)
    {
        bool isExist;
        if (QFile::exists(QString("%1/%2").
                          arg(FILE_PATH).
                          arg(d_ptr->exportFileName.at(i))))
        {
            isExist = true;
        }
        else
        {
            isExist = false;
        }

        // find the same name
        int exportFlag = 1;
        if (isExist)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2?").
                                   arg(d_ptr->exportFileName.at(i)).
                                   arg(trs("IfSelectTheSameNameFile"))),
                               QStringList() << trs("NotRepeated")
                               << trs("Repeated"));
            exportFlag = message.exec();
        }

        // cover
        bool isCopyOk = true;
        if (exportFlag)
        {
            QString fileName = QString("%1%2").
                               arg(CONFIG_DIR).
                               arg(d_ptr->exportFileName.at(i));
            QString newFileName = QString("%1/%2").
                                  arg(FILE_PATH).
                                  arg(d_ptr->exportFileName.at(i));
            QFile::remove(newFileName);
            isCopyOk = QFile::copy(fileName, newFileName);
        }
        if (!isCopyOk)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2").
                                   arg(d_ptr->exportFileName.at(i)).
                                   arg(trs("ExportFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return false;
        }
    }

    return true;
}

/**************************************************************************************************
 * 导入xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenuContent::insertFileFromUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return false;
    }

    // find the xml files
    QDir dir(QString("%1%2").arg(FILE_PATH).arg("/"));
    QStringList nameFilters;
    nameFilters.append("*.xml");
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        MessageBox messageBox(trs("Import"), trs("PleaseAddImportFiles"), false);
        messageBox.exec();
        return false;
    }


    ImportSubWidget myImportSubWidget(files, FILE_PATH);
    bool status = myImportSubWidget.exec();
    d_ptr->importFileName.clear();
    if (status == true) //  OK
    {
        QMap<int, bool> rowsMap = myImportSubWidget.readRowsMap();
        for (int i = 0; i < files.count(); i++)
        {
            if (rowsMap[i])
            {
                d_ptr->importFileName.append(files.at(i));
            }
        }
        d_ptr->importFlag = 255;
    }
    else
    {
        d_ptr->importFlag = 0;
    }

    // load local data
    QFile fileLocal(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if (!fileLocal.open(QFile::ReadOnly | QFile::Text))
    {
        MessageBox message(trs("Import"), trs("ImportFileFailed"), false);
        message.exec();
        return false;
    }
    if (!d_ptr->localXml.setContent(&fileLocal))
    {
        MessageBox message(trs("Import"), trs("ImportFileFailed"), false);
        message.exec();
        return false;
    }
    fileLocal.close();

    // load configs again
    d_ptr->loadConfigs();

    for (int i = 0; i < d_ptr->importFileName.count(); i++)
    {
        QFile fileImport(QString("%1/%2").
                         arg(FILE_PATH).
                         arg(d_ptr->importFileName.at(i)));
        if (!fileImport.open(QFile::ReadOnly | QFile::Text))
        {
            MessageBox message(trs("Import"), trs("ImportFileFailed"), false);
            message.exec();
            return false;
        }
        if (!d_ptr->importXml.setContent(&fileImport))
        {
            MessageBox message(trs("Import"), trs("ImportFileFailed"), false);
            message.exec();
            return false;
        }
        fileImport.close();

        QList<QDomElement> importTagList;
        QDomElement  importTag = d_ptr->importXml.documentElement();

        // check the import file
        bool checkXmlFlag = checkXMLContent(importTagList, importTag);

        if (!checkXmlFlag)
        {
            MessageBox message(trs("Import"), trs("ImportFileFailed"), false);
            message.exec();
            return false;
        }

        // find the same file
        bool isExist;
        if (QFile::exists(QString("%1%2").
                          arg(CONFIG_DIR).
                          arg(d_ptr->importFileName.at(i))))
        {
            isExist = true;
        }
        else
        {
            isExist = false;
        }
        int importFlag = 1;
        if (isExist)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2?").
                                   arg(d_ptr->importFileName.at(i)).
                                   arg(trs("IfSelectTheSameNameFile"))),
                               QStringList() << trs("NotRepeated")
                               << trs("Repeated"));
            importFlag = message.exec();
        }

        // cover
        bool isCopyOk = true;
        if (importFlag)
        {
            QString fileName = QString("%1/%2").
                               arg(FILE_PATH).
                               arg(d_ptr->importFileName.at(i));
            QString newFileName = QString("%1%2").
                                  arg(CONFIG_DIR).
                                  arg(d_ptr->importFileName.at(i));
            QFile::remove(newFileName);
            isCopyOk = QFile::copy(fileName, newFileName);
        }
        if (!isCopyOk)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2").
                                   arg(d_ptr->exportFileName.at(i)).
                                   arg(trs("ImportFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return false;
        }

        // update import configs
        ConfigManager::UserDefineConfigInfo newUserDfine;
        newUserDfine.fileName = d_ptr->importFileName.at(i);

        // 更新导入命名，防止命名相同
        int addIndex = 0;
        int count = d_ptr->configs.count();
        while (true)
        {
            bool sameConfigName = false;
            newUserDfine.name = QString("UserConfig%1").arg(count + (++addIndex));
            foreach(ConfigManager::UserDefineConfigInfo info, d_ptr->configs)
            {
                if (newUserDfine.name == info.name)
                {
                    sameConfigName = true;
                    break;
                }
            }
            if (sameConfigName == false)
            {
                break;
            }
            // 超出命名范围
            if (addIndex > CONFIG_MAX_NUM)
            {
                MessageBox message(trs("Import"),
                                   trs(QString("%1\r\n%2").
                                       arg(newUserDfine.fileName).
                                       arg(trs("ImportFailed"))),
                                   QStringList() << trs("Yes"));
                return false;
            }
        }

        // 查找是否有同名文件，不再更新同名文件名称
        bool isAppend = true;
        foreach(ConfigManager::UserDefineConfigInfo fileInfo, d_ptr->configs)
        {
            if (fileInfo.fileName == newUserDfine.fileName)
            {
                isAppend = false;
                break;
            }
        }
        if (isAppend)
        {
            d_ptr->configs.append(newUserDfine);
        }

        // 导入文件不超过规定最大值
        if (d_ptr->configs.count() >= CONFIG_MAX_NUM)
        {
            break;
        }
    }
    d_ptr->updateConfigList();
    configManager.saveUserConfigInfo(d_ptr->configs);

    return true;
}

QDomElement ConfigExportImportMenuContent::tagFindElement(const QStringList &list)
{
    QDomElement tag;

    if (d_ptr->localXml.isNull())
    {
        return tag;
    }

    if (list.size() < 1)
    {
        return tag;
    }

    tag = d_ptr->localXml.firstChildElement();

    for (int i = 0; i < list.size(); i++)
    {
        if (tag.isNull())
        {
            return tag;
        }

        tag = tag.firstChildElement(list.at(i));
    }
    return tag;
}


bool ConfigExportImportMenuContent::compareTagAttribute(QDomElement importTag, QDomElement localTag)
{
#define PARAMETER_PREFIX_NUM0   (4)  /*参数前缀的长度*/
#define PARAMETER_PREFIX_NUM1   (2)  /*参数前缀的长度*/
#define PARAMETER_PREFIX0       ("Enable")  /*参数前缀*/
#define PARAMETER_PREFIX1       ("Min")  /*参数前缀*/
    float numArrary[] = {1.0, 2.0};
    QString attrString[] = {"Enable", "Prio"};
    QDomNamedNodeMap localDomNameNodeMap = localTag.attributes();

    if (localDomNameNodeMap.count() == PARAMETER_PREFIX_NUM0) /*导入标签无属性值*/
    {
        if (localDomNameNodeMap.namedItem("cAttr0").nodeValue().toFloat() == 0) /*数字属性 需要比较上下限*/
        {
            if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat() || \
                    importTag.text().toFloat() > localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat())
            {
                //  信息加入日志
                d_ptr->textStream << QString("Inserted xml files NodeName(%1)`s number value parser failed !!!\r\n").arg(
                                      localTag.nodeName());
                float floatMin = localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat();
                float floatMax = localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat();
                d_ptr->textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(floatMin).arg(floatMax);
                return false;
            }
        }
        else/*字符串属性 直接比较是否相等*/
        {
            if (importTag.nodeValue() != localTag.nodeValue())
            {
                //  信息加入日志
                d_ptr->textStream << QString("Inserted xml files NodeName(%1)`s string value parser failed !!!\r\n").arg(
                                      localTag.nodeName());
                return false;
            }
        }
    }
    else if (localDomNameNodeMap.count() == PARAMETER_PREFIX_NUM1) /*导入标签有属性值的*/
    {
        int localAttrbuiteSize = localDomNameNodeMap.count();
        for (int i = 0; i < localAttrbuiteSize; i++)
        {
            if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX0) /*Enable、Prio属性*/
            {
                if (importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat() < 0 || \
                        importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat() > numArrary[i])
                {
                    //  信息加入日志
                    d_ptr->textStream << QString("Inserted xml files NodeName(%1)`s attribute1 parser failed !!!\r\n").arg(
                                          localTag.nodeName());
                    return false;
                }
            }
            else if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX1) /*Min、Max属性*/
            {
                if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("Min").nodeValue().toFloat() || \
                        importTag.text().toFloat() > localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                    //  信息加入日志
                    d_ptr->textStream << QString("Inserted xml files NodeName(%1)`s attribute2 parser failed !!!\r\n").arg(
                                          localTag.nodeName());
                    float fl = localDomNameNodeMap.namedItem("Min").nodeValue().toFloat();
                    float floa = localDomNameNodeMap.namedItem("Max").nodeValue().toFloat();
                    d_ptr->textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(fl).arg(floa);
                    return false;
                }
            }
        }
    }
    return true;
}
//  检查内容是否有效。 有效返回true；无效返回false；
//  递归方式查询，注意递归结束条件的有效性
bool ConfigExportImportMenuContent::checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag)
{
    if (importTag.isNull())
    {
        QStringList nameList;

        for (int i = 0; i < importTagList.count(); i++)
        {
            nameList.append(importTagList.at(i).nodeName());
        }

        QDomElement localTag = tagFindElement(nameList);
        if (!localTag.isNull())
        {
            bool attrFlag = compareTagAttribute(importTagList.at(importTagList.count() - 1), localTag);

            if (attrFlag == false)
            {
                d_ptr->attrCheckFlag = false;
                return d_ptr->attrCheckFlag;
            }
        }

        if (importTagList.empty())
        {
            d_ptr->attrCheckFlag = true;
            return d_ptr->attrCheckFlag;
        }
        importTag = importTagList.last().nextSiblingElement();//下一个同类子节点
        importTagList.removeLast();//移除最后一个子节点
        if (importTagList.empty())
        {
            d_ptr->attrCheckFlag = true;
            return d_ptr->attrCheckFlag;
        }

        if (importTag.isNull())
        {
            importTag = importTagList.last();
            importTag = importTag.nextSiblingElement();//下一个同类子节点
            importTagList.removeLast();//移除最后一个子节点
        }
        checkXMLContent(importTagList, importTag);
    }
    else
    {
        importTagList.append(importTag);
        importTag = importTag.firstChildElement();
        checkXMLContent(importTagList, importTag);
    }

    return d_ptr->attrCheckFlag;
}

void ConfigExportImportMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->exportBtn)
    {
        bool isOk =  exportFileToUSB();
        QString paramTitle(trs("Export"));
        QString paraText;
        if (isOk)
        {
            paraText = trs("ExportFileCompleted");
            MessageBox messageBox(paramTitle, paraText, false);
            messageBox.exec();
        }
    }
    else if (btn == d_ptr->importBtn)
    {
        bool isOk = insertFileFromUSB();
        QString paramTitle(trs("Import"));
        QString paraText;
        if (isOk && d_ptr->importFlag != 0)
        {
            paraText = trs("ImportFileCompleted");
            MessageBox messageBox(paramTitle, paraText, false);
            messageBox.exec();
        }
        d_ptr->importFlag = 0;
    }
}


void ConfigExportImportMenuContent::updateBtnStatus()
{
    int curSelectedRow = d_ptr->configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1
            || !usbManager.isUSBExist())
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    d_ptr->exportBtn->setEnabled(isEnable);
}
