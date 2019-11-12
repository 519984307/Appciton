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
#define LISTVIEW_MAX_VISIABLE_TIME (5)
#define HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define PARAMETER_PREFIX_NUM0   (4)  // 参数前缀的长度
#define PARAMETER_PREFIX_NUM1   (2)  // 参数前缀的长度
#define PARAMETER_PREFIX0       ("Enable")  // 参数前缀
#define PARAMETER_PREFIX1       ("Min")  // 参数前缀

enum TransferResult
{
    TRANSFER_FAIL,
    TRANSFER_SUCCESS,
    TRANSFER_IGNORE
};

class ConfigExportImportMenuContentPrivate
{
public:
    ConfigExportImportMenuContentPrivate()
        : valid(false)
        , configListView(NULL)
        , configDataModel(NULL)
        , exportBtn(NULL)
        , importBtn(NULL)
        , infoLab(NULL)
        , timer(NULL)
        , message(NULL)
    {
    }

    /**
     * @brief exportFileToUSB 导出配置文件到usb
     * @return  返回传输结果
     */
    TransferResult exportFileToUSB();

    /**
     * @brief insertFileFromUSB    从usb设备导入配置文件
     * @return 返回传输结果
     */
    TransferResult insertFileFromUSB();

    /**
     * @brief checkXMLContent  check validity of the import XML
     * @param importTagList    import tag list
     * @param importTag        import tag
     * @return 返回：ture-valid;false-invalid；
     */
    bool checkXMLContent(QList<QDomElement> *importTagList, QDomElement *importTag);

    /**
     * @brief tagFindElement  find tag elements
     * @param list     the element path
     * @return         true-the element path  false--0
     */
    QDomElement tagFindElement(const QStringList &list);

    /**
     * @brief compareTagAttribute  compare  attribute and value of the both of all XML nodes
     * @param importtag   import tag
     * @param localtag    local check tag
     * @return ture-valid;false-invalid；
     */
    bool compareTagAttribute(QDomElement importtag, QDomElement localtag);

    /**
     * @brief loadConfigs  装载配置
     */
    void loadConfigs();

    /**
     * @brief updateConfigList  更新配置列表
     */
    void updateConfigList();


    QList<ConfigManager::UserDefineConfigInfo> configs;  // 用户自定义的配置文件信息
    QStringList  exportFileName;  // 导出的XML文件名称
    QStringList  importFileName;  // 导入的XML文件名称
    QDomDocument checkXml;  // 本地检查文件内容
    QDomDocument importXml;  // 导入文件内容
    bool valid;  // 导入文件的有效性
    ListView  *configListView;
    ListDataModel *configDataModel;
    Button *exportBtn;
    Button *importBtn;
    QLabel *infoLab;  // 记录usb是否存在信息
    QTimer *timer;
    MessageBox *message;
};


TransferResult ConfigExportImportMenuContentPrivate::exportFileToUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return TRANSFER_IGNORE;
    }

    // 如果没有对应文件夹，则主动创建此文件夹
    QDir dir;
    dir.mkdir(FILE_PATH);

    // 获取所有的导出文件名称
    exportFileName.clear();
    int rowCount = configDataModel->getRowCount();
    QMap<int, bool> selectedMap = configListView->getRowsSelectMap();
    for (int i = 0; i < rowCount; i++)
    {
        if (selectedMap[i])
        {
            exportFileName.append(configs.at(i).fileName);
        }
    }

    // 开始导出数据
    int count = exportFileName.count();
    bool transferSuccess = false;
    for (int i = 0; i < count; i++)
    {
        QString name = exportFileName.at(i);
        // 判断是否存在同名文件
        bool isExist = false;
        if (QFile::exists(QString("%1/%2").arg(FILE_PATH).arg(name)))
        {
            isExist = true;
        }

        // 弹出是否覆盖的提示框
        QDialog::DialogCode status = QDialog::Accepted;
        if (isExist)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2?").arg(name).arg(trs("FileExistsOverWrite"))),
                               QStringList() << trs("Cancel") << trs("Repeated"));
            this->message = &message;
            status = static_cast<QDialog::DialogCode>(message.exec());
            this->message = NULL;
        }

        // 复制文件到usb设备
        bool copyOk = true;
        if (status == QDialog::Accepted)
        {
            QString fileName = QString("%1%2").arg(CONFIG_DIR).arg(name);
            QString newFileName = QString("%1/%2").arg(FILE_PATH).arg(name);
            QFile::remove(newFileName);
            copyOk = QFile::copy(fileName, newFileName);
            if (copyOk == true)
            {
                transferSuccess = true;
            }
        }

        // 复制文件失败时弹出提示框
        if (copyOk == false)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2").arg(name).arg(trs("ExportFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return TRANSFER_FAIL;
        }
    }

    if (transferSuccess == true)
    {
        return TRANSFER_SUCCESS;
    }
    return TRANSFER_IGNORE;
}

TransferResult ConfigExportImportMenuContentPrivate::insertFileFromUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return TRANSFER_IGNORE;
    }

    // 判断导入文件是否为空
    QDir dir(QString("%1%2").arg(FILE_PATH).arg("/"));
    QStringList files = dir.entryList(QStringList() << "A-*.xml"
                                                    << "N-*.xml"
                                                    << "P-*.xml", QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        MessageBox messageBox(trs("Import"), trs("PleaseAddImportFiles"), false);
        this->message = &messageBox;
        messageBox.exec();
        this->message = NULL;
        return TRANSFER_IGNORE;
    }

    // 获取导入文件名称
    importFileName.clear();
    ImportSubWidget subW(files, FILE_PATH);
    QDialog::DialogCode status = static_cast<QDialog::DialogCode>(subW.exec());
    if (status == QDialog::Accepted)
    {
        QMap<int, bool> rowsMap = subW.readRowsMap();
        for (int i = 0; i < files.count(); i++)
        {
            if (rowsMap[i])
            {
                importFileName.append(files.at(i));
            }
        }
    }

    // 未选取任何文件，直接退出
    if (importFileName.isEmpty())
    {
        return TRANSFER_IGNORE;
    }

    // 加载检查文件
    QFile checkFile(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if (false == checkFile.open(QFile::ReadOnly | QFile::Text))
    {
        return TRANSFER_FAIL;
    }
    if (false == checkXml.setContent(&checkFile))
    {
        return TRANSFER_FAIL;
    }
    checkFile.close();

    // 获取当前用户定义的配置信息
    loadConfigs();

    // 开始导入文件
    int count = importFileName.count();
    bool importSuccess = false;
    for (int i = 0; i < count; i++)
    {
        // 加载导入文件
        QString name = importFileName.at(i);
        QFile file(QString("%1/%2").arg(FILE_PATH).arg(name));
        if (false == file.open(QFile::ReadOnly | QFile::Text))
        {
            return TRANSFER_FAIL;
        }
        if (false == importXml.setContent(&file))
        {
            return TRANSFER_FAIL;
        }
        file.close();

        // 检查导入文件是否合乎要求
        QList<QDomElement> tagList;
        QDomElement  tag = importXml.documentElement();
        bool flag = checkXMLContent(&tagList, &tag);
        if (false == flag)
        {
            return TRANSFER_FAIL;
        }

        // 判断是否存在同名文件,如果存在则弹出选择提示框
        bool isExist = false;
        if (true == QFile::exists(QString("%1%2").arg(CONFIG_DIR).arg(name)))
        {
            isExist = true;
        }
        if (true == isExist)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2?").arg(name).arg(trs("FileExistsOverWrite"))),
                               QStringList() << trs("Cancel") << trs("Repeated"));
            this->message = &message;
            flag = message.exec();
            this->message = NULL;
        }
        else
        {
            // 导入文件不超过规定最大值
            if (configs.count() >= CONFIG_MAX_NUM)
            {
                MessageBox message(trs("ImportFileExceedRange"),
                                   trs(QString("%1\r\n%2").arg(name).arg(trs("ImportFileFailed"))),
                                   QStringList() << trs("Yes"));
                message.exec();
                break;
            }
        }

        // 从usb设备复制文件到文件系统中
        bool copyOk = true;
        if (true == flag)
        {
            QString fileName = QString("%1/%2").arg(FILE_PATH).arg(name);
            QString newFileName = QString("%1%2").arg(CONFIG_DIR).arg(name);
            QFile::remove(newFileName);
            copyOk = QFile::copy(fileName, newFileName);
            if (copyOk == true)
            {
                importSuccess = true;
            }
        }

        // 如果文件复制不成功,弹出提示框
        if (false == copyOk)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2").arg(name).arg(trs("ImportFileFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return TRANSFER_IGNORE;
        }

        // 更新导入配置名称，防止命名相同
        int addIndex = 0;
        int num = configs.count();
        ConfigManager::UserDefineConfigInfo newUserDefine;
        while (true && !isExist)
        {
            bool sameConfigName = false;
            newUserDefine.name = QString("UserConfig%1").arg(num + (++addIndex));
            foreach(ConfigManager::UserDefineConfigInfo info, configs)
            {
                if (newUserDefine.name == info.name)
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
            if (addIndex + num > CONFIG_MAX_NUM)
            {
                MessageBox message(trs("Import"),
                                   trs(QString("%1\r\n%2").arg(name).arg(trs("OutOfNamingRange"))),
                                   QStringList() << trs("Yes"));
                message.exec();
                break;
            }
        }

        // 查找系统配置文件中是否存在同名文件，不再更新同名文件名称
        flag = true;
        newUserDefine.fileName = name;
        foreach(ConfigManager::UserDefineConfigInfo fileInfo, configs)
        {
            if (fileInfo.fileName == newUserDefine.fileName)
            {
                flag = false;
                break;
            }
        }
        if (true == flag)
        {
            // 更新用户自定义文件的病人类型
            const QChar name = newUserDefine.fileName.at(0);
            PatientType type = PATIENT_TYPE_ADULT;
            if (name == 'N')
            {
                type = PATIENT_TYPE_NEO;
            }
            else if (name == 'P')
            {
                type = PATIENT_TYPE_PED;
            }
            else
            {
                newUserDefine.fileName.replace(0, 1, "A");
            }
            newUserDefine.patType = PatientSymbol::convert(type);
            configs.append(newUserDefine);
        }
    }

    // 更新配置列表
    updateConfigList();
    configManager.saveUserConfigInfo(configs);

    if (importSuccess == true)
    {
        return TRANSFER_SUCCESS;
    }
    return TRANSFER_IGNORE;
}

void ConfigExportImportMenuContentPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuContentPrivate::updateConfigList()
{
    QStringList configNameList;
    for (int i = 0; i < configs.count(); i++)
    {
        QString name = QString("%1(%2)").arg(configs.at(i).name).arg(trs(configs.at(i).patType));
        configNameList.append(name);
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
        if (d_ptr->message != NULL)
        {
            QMetaObject::invokeMethod(d_ptr->message, "done", Q_ARG(int, 0));
            d_ptr->message = NULL;
        }
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

    QLabel *label = new QLabel(trs("WarningNoUSB"));
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


QDomElement ConfigExportImportMenuContentPrivate::tagFindElement(const QStringList &list)
{
    QDomElement tag;

    if (checkXml.isNull())
    {
        return tag;
    }

    if (list.size() < 1)
    {
        return tag;
    }

    tag = checkXml.firstChildElement();

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


bool ConfigExportImportMenuContentPrivate::compareTagAttribute(QDomElement importTag, QDomElement localTag)
{
    float numArrary[] = {1.0, 2.0};
    QString attrString[] = {"Enable", "Prio"};
    QDomNamedNodeMap localDomNameNodeMap = localTag.attributes();

    if (localDomNameNodeMap.count() == PARAMETER_PREFIX_NUM0)  // 导入标签无属性值
    {
        if (localDomNameNodeMap.namedItem("cAttr0").nodeValue().toFloat() == 0)  // 数字属性 需要比较上下限
        {
            if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat() || \
                    importTag.text().toFloat() > localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat())
            {
                return false;
            }
        }
        else  // 字符串属性 直接比较是否相等
        {
            if (importTag.nodeValue() != localTag.nodeValue())
            {
                return false;
            }
        }
    }
    else if (localDomNameNodeMap.count() == PARAMETER_PREFIX_NUM1)  // 导入标签有属性值的
    {
        int localAttrbuiteSize = localDomNameNodeMap.count();
        for (int i = 0; i < localAttrbuiteSize; i++)
        {
            if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX0)  // Enable、Prio属性
            {
                if (importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat() < 0 || \
                        importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat() > numArrary[i])
                {
                    return false;
                }
            }
            else if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX1)  // Min、Max属性
            {
                if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("Min").nodeValue().toFloat() || \
                        importTag.text().toFloat() > localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool ConfigExportImportMenuContentPrivate::checkXMLContent(QList<QDomElement> *importTagList, QDomElement *importTag)
{
    if (importTag->isNull())
    {
        QStringList nameList;

        for (int i = 0; i < importTagList->count(); i++)
        {
            nameList.append(importTagList->at(i).nodeName());
        }

        QDomElement localTag = tagFindElement(nameList);
        if (!localTag.isNull())
        {
            bool attrFlag = compareTagAttribute(importTagList->at(importTagList->count() - 1), localTag);

            if (attrFlag == false)
            {
                valid = false;
                return valid;
            }
        }

        if (importTagList->empty())
        {
            valid = true;
            return valid;
        }
        *importTag = importTagList->last().nextSiblingElement();  // 下一个同类子节点
        importTagList->removeLast();  // 移除最后一个子节点
        if (importTagList->empty())
        {
            valid = true;
            return valid;
        }

        if (importTag->isNull())
        {
            *importTag = importTagList->last();
            *importTag = importTag->nextSiblingElement();  // 下一个同类子节点
            importTagList->removeLast();  // 移除最后一个子节点
        }
        checkXMLContent(importTagList, importTag);
    }
    else
    {
        importTagList->append(*importTag);
        *importTag = importTag->firstChildElement();
        checkXMLContent(importTagList, importTag);
    }

    return valid;
}

void ConfigExportImportMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->exportBtn)
    {
        TransferResult  result =  d_ptr->exportFileToUSB();
        if (result == TRANSFER_SUCCESS)
        {
            MessageBox messageBox(trs("Export"), trs("ExportFileCompleted"), false);
            d_ptr->message = &messageBox;
            messageBox.exec();
            d_ptr->message = NULL;
        }
    }
    else if (btn == d_ptr->importBtn)
    {
        TransferResult result = d_ptr->insertFileFromUSB();
        switch (result)
        {
        case TRANSFER_FAIL:
        {
            MessageBox messageBox(trs("Import"), trs("ImportFileFailed"), false);
            messageBox.exec();
        }
        break;
        case TRANSFER_IGNORE:
            break;
        case TRANSFER_SUCCESS:
        {
            MessageBox messageBox(trs("Import"), trs("ImportFileCompleted"), false);
            d_ptr->message = &messageBox;
            messageBox.exec();
            d_ptr->message = NULL;
        }
        break;
        }
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
