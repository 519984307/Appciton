/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/

#include "FactoryImportExportMenuContent.h"
#include "IListWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "Button.h"
#include <QList>
#include <QListWidgetItem>
#include <QDomDocument>
#include <QTextStream>
#include "IMessageBox.h"
#include "ConfigManagerMenuWindow.h"
#include <QProcess>
#include "ImportFileSubWidget.h"
#include <QDir>

#define CONFIG_MAX_NUM 6
#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200

#define USB0_DEVICE_NODE      ("/dev/sda2")
#define USB0_PATH_NAME        ("/mnt/usb0")
#define CONFIG_DIR            ("/usr/local/nPM/etc/")
#define CONFIG_FILE_ADULT     ("AdultConfig.Original.xml")
#define CONFIG_FILE_NEO       ("NeoConfig.Original.xml")
#define CONFIG_FILE_PED       ("PedConfig.Original.xml")
#define CONFIG_FILE_SYSTEM    ("System.Original.xml")
#define CONFIG_FILE_MACHINE   ("Machine.xml")

class FactoryImportExportMenuContentPrivate
{
public:
    FactoryImportExportMenuContentPrivate();

    enum PortState
    {
        FAILED = -1,
        SUCCEED,
        NOT_FOUND_UNODE,
        NOT_FOUND_UDISK,
        NOT_SELECT_FILES,
        CLOSE,
        IMPORTFILES_EMPTY,
    };

    struct FactoryConfigInfo
    {
        QString name;         // name of the this config
        QString fileName;     // xml filename
    };

    enum Import_XML_Type
    {
        SYSTEM_CONFIG_XML = 0,
        MACHINE_CONFIG_XML,
        USER_CONFIG_XML,
        XML_NR
    };

    /**
     * @brief loadOptions
     */
    void loadOptions();
    /**
     * @brief updateConfigList
     */
    void updateConfigList();

    /**
     * @brief exportFileToUSB 导出文件到U盘方法
     * @return 导出成功返回true 导出失败返回false
     */
    bool exportFileToUSB();
    /**
     * @brief insertFileFromUSB 导入文件到U盘方法
     * @return 导入成功返回true 导入失败返回false
     */
    bool insertFileFromUSB();
    /**
     * @brief tagFindElement  寻找标签方法
     * @param list     指定标签的结点路径
     * @return         成功返回寻找到的标签 失败返回0
     */
    QDomElement tagFindElement(const QStringList &list);  // 寻找本地的标签
    /**
     * @brief checkXMLContent  检查导入的xml文件有效性方法
     * @param importTagList    导入标签链表
     * @param importTag        导入标签
     * @return 返回：ture-有效;false-无效；
     */
    bool checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag);

    /**
     * @brief compareTagAttribute  检查每个元素节点下属性名称的匹配和属性值的有效性方法
     * @param importtag   导入标签
     * @param localtag    本地检查标签
     * @return   返回：ture-有效;false-无效；
     */
    bool compareTagAttribute(QDomElement importtag, QDomElement localtag);


    QList<QString>  exportFileName;  // 导出的XML文件名称的链表

    QList<QDomDocument>  insertxml;  // 导入的XML文件的链表

    QTextStream       textStream;  // 日志接口

    QDomDocument      localXml;  // 本地检查文件

    QDomDocument      importXml;  // 导入检查文件

    QString           failedImportXmlName;  // 首个失败的导入文件名称

    QString           failedExportXmlName;  // 首个失败的导出文件名称

    QString           sameImportXmlName;  // 首个同名的导入文件名称

    QString           sameExportXmlName;  // 首个同名的导出文件名称

    int               checkExportFileFlag;  // 检查导出文件标志位

    int               checkImportFileFlag;  // 检查导入文件标志位

    int               usbFd;  // 挂载U盘的设备节点

    int               repeatFileChooseFlag;  // 重复文件选择状态位

    IListWidget   *configList;

    IListWidget   *configListImport;

    Button     *exportBtn;

    Button     *importBtn;

    int         remainderImportItems;

    int         remainderExportItems;

    QList<QListWidgetItem *> selectItems;

    QList<QListWidgetItem *> selectItemsImport;

    QList<FactoryConfigInfo> configs;

    bool attrCheckFlag;
};

FactoryImportExportMenuContentPrivate::FactoryImportExportMenuContentPrivate()
    : failedImportXmlName(""),
      failedExportXmlName(""),
      sameImportXmlName(""),
      sameExportXmlName(""),
      checkExportFileFlag(0),
      checkImportFileFlag(0),
      usbFd(0),
      repeatFileChooseFlag(0),
      configList(NULL),
      configListImport(NULL),
      exportBtn(NULL),
      importBtn(NULL),
      remainderImportItems(0),
      remainderExportItems(0),
      attrCheckFlag(true)
{
    textStream.reset();
    localXml.clear();
    importXml.clear();
}

FactoryImportExportMenuContent::FactoryImportExportMenuContent()
    : MenuContent(trs("ExportImport"),
                  trs("ExportImportDesc")),
      d_ptr(new FactoryImportExportMenuContentPrivate)
{
}

FactoryImportExportMenuContent::~FactoryImportExportMenuContent()
{
    delete d_ptr;
}

void FactoryImportExportMenuContent::readyShow()
{
    d_ptr->loadOptions();
    d_ptr->updateConfigList();

    if (d_ptr->configList->count() >= CONFIG_MAX_NUM)
    {
        d_ptr->importBtn->setEnabled(false);
    }
    else
    {
        d_ptr->importBtn->setEnabled(false);
    }

    /*更新导出按钮使能状态*/
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }
}

void FactoryImportExportMenuContent::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        d_ptr->configList->setFont(fontManager.textFont(
                                       font().pixelSize()));
    }
}

void FactoryImportExportMenuContent::layoutExec()
{
    if (layout())
    {
        //  already install layout
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label = new QLabel(trs("ExportImportConfig"));
    layout->addWidget(label, 0, 0);

    IListWidget *listWidget = new IListWidget;
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setFrameStyle(QFrame::Plain);
    listWidget->setSpacing(2);
    listWidget->setUniformItemSizes(true);
    listWidget->setIconSize(QSize(16, 16));
    QString configListStyleSheet =
        QString("QListWidget { margin-left: 1px; border:0px solid #808080; border-radius: 1px; background-color: transparent; outline: none; }\n "
                "QListWidget::item {padding: 1px; border-radius: 1px; border: none; background-color: %1;}\n"
                "QListWidget::item:focus {background-color: %2;}").arg("gray").arg("blue");
    listWidget->setStyleSheet(configListStyleSheet);
    layout->addWidget(listWidget, 1, 0);
    connect(listWidget, SIGNAL(realRelease()), this, SLOT(onConfigUpdated()));
    listWidget->installEventFilter(this);
    d_ptr->configList = listWidget;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button;

    button = new Button(trs("Export"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    d_ptr->exportBtn = button;
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));

    button = new Button(trs("Import"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    d_ptr->importBtn = button;
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));

    layout->addLayout(hl, 2, 0);

    layout->setRowStretch(3, 1);

    if (d_ptr->exportBtn == NULL)
    {
        return;
    }
    //  update export status
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }
}

bool FactoryImportExportMenuContent::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == d_ptr->configList)
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if (e->reason() == Qt::TabFocusReason)
            {
                d_ptr->configList->setCurrentRow(0);
            }
            else if (e->reason() == Qt::BacktabFocusReason)
            {
                d_ptr->configList->setCurrentRow(d_ptr->configList->count() - 1);
            }
        }

        if (ev->type() == QEvent::Hide)
        {
            if (!d_ptr->selectItems.isEmpty())
            {
                d_ptr->importBtn->setEnabled(true);
                d_ptr->exportBtn->setEnabled(false);
                d_ptr->selectItems.clear();
            }
            while (!d_ptr->configs.isEmpty())
            {
                d_ptr->configs.removeLast();
            }
        }
    }
    return false;
}

void FactoryImportExportMenuContentPrivate::loadOptions()
{
    QStringList factoryConfigInfoList;
    factoryConfigInfoList.append(CONFIG_FILE_ADULT);
    factoryConfigInfoList.append(CONFIG_FILE_NEO);
    factoryConfigInfoList.append(CONFIG_FILE_PED);
    factoryConfigInfoList.append(CONFIG_FILE_SYSTEM);
    factoryConfigInfoList.append(CONFIG_FILE_MACHINE);

    FactoryConfigInfo factoryConfigInfo;
    for (int i = 0; i < factoryConfigInfoList.count(); i++)
    {
        if (QFile::exists(QString("%1%2").arg(CONFIG_DIR).arg(factoryConfigInfoList.at(i))))
        {
            factoryConfigInfo.fileName = factoryConfigInfoList.at(i);
            factoryConfigInfo.name = factoryConfigInfoList.at(i);
            factoryConfigInfo.name.replace(".Original", "");
            factoryConfigInfo.name.replace(".xml", "OfFactory");
            configs.append(factoryConfigInfo);
        }
    }
}

void FactoryImportExportMenuContentPrivate::updateConfigList()
{
    // remove old item
    while (configList->count())
    {
        QListWidgetItem *item = configList->takeItem(0);
        delete item;
    }

    for (int i = 0; i < configs.count(); i++)
    {
        QListWidgetItem *item  = new QListWidgetItem(configs.at(i).name, configList);
        item->setSizeHint(QSize(CONFIG_LIST_ITME_W, CONFIG_LIST_ITEM_H));
    }

    int count = configs.count();
    if (count)
    {
        configList->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        configList->setFocusPolicy(Qt::NoFocus);
    }
}

bool FactoryImportExportMenuContentPrivate::compareTagAttribute(QDomElement importTag, QDomElement localTag)
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
                // 信息加入日志
                textStream << QString("Inserted xml files NodeName(%1)`s number value parser failed !!!\r\n").arg(localTag.nodeName());
                float floatMin = localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat();
                float floatMax = localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat();
                textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(floatMin).arg(floatMax);
                return false;
            }
        }
        else  /*字符串属性 直接比较是否相等*/
        {
            if (importTag.nodeValue() != localTag.nodeValue())
            {
                // 信息加入日志
                textStream << QString("Inserted xml files NodeName(%1)`s string value parser failed !!!\r\n").arg(localTag.nodeName());
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
                    // 信息加入日志
                    textStream << QString("Inserted xml files NodeName(%1)`s attribute1 parser failed !!!\r\n").arg(localTag.nodeName());
                    return false;
                }
            }
            else if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX1) /*Min、Max属性*/
            {
                if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("Min").nodeValue().toFloat() || \
                        importTag.text().toFloat() > localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                    // 信息加入日志
                    textStream << QString("Inserted xml files NodeName(%1)`s attribute2 parser failed !!!\r\n").arg(localTag.nodeName());
                    float fl = localDomNameNodeMap.namedItem("Min").nodeValue().toFloat();
                    float floa = localDomNameNodeMap.namedItem("Max").nodeValue().toFloat();
                    textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(fl).arg(floa);
                    return false;
                }
            }
        }
    }
    return true;
}


bool FactoryImportExportMenuContentPrivate::checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag)
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
                attrCheckFlag = false;
                return attrCheckFlag;
            }
        }

        if (importTagList.empty())
        {
            attrCheckFlag = true;
            return attrCheckFlag;
        }
        importTag = importTagList.last().nextSiblingElement();  // 下一个同类子节点
        importTagList.removeLast();  // 移除最后一个子节点
        if (importTagList.empty())
        {
            attrCheckFlag = true;
            return attrCheckFlag;
        }

        if (importTag.isNull())
        {
            importTag = importTagList.last();
            importTag = importTag.nextSiblingElement();  // 下一个同类子节点
            importTagList.removeLast();  // 移除最后一个子节点
        }
        checkXMLContent(importTagList, importTag);
    }
    else
    {
        importTagList.append(importTag);
        importTag = importTag.firstChildElement();
        checkXMLContent(importTagList, importTag);
    }

    return attrCheckFlag;
}

void FactoryImportExportMenuContent::onBtnReleasedChanged()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->exportBtn)
    {
        d_ptr->exportFileToUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (d_ptr->checkExportFileFlag)
        {
        case FactoryImportExportMenuContentPrivate::FAILED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = QString("%1:%2").arg(trs("ExportFilefailed")).arg(d_ptr->failedExportXmlName);
            break;
        case FactoryImportExportMenuContentPrivate::SUCCEED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("ExportFileCompleted");
            break;
        case FactoryImportExportMenuContentPrivate::NOT_FOUND_UNODE:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundDeviceNode");
            break;
        case FactoryImportExportMenuContentPrivate::NOT_FOUND_UDISK:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundUDisk");
            break;
        case FactoryImportExportMenuContentPrivate::CLOSE:
            isClose = true;
            break;
        case FactoryImportExportMenuContentPrivate::NOT_SELECT_FILES:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseSelectFiles");
            break;
        case FactoryImportExportMenuContentPrivate::IMPORTFILES_EMPTY:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseAddExportFiles");
            break;
        }
        d_ptr->checkExportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.exec();
        }
        // d_ptr->selectItems.clear();
    }
    else if (btn == d_ptr->importBtn)
    {
        d_ptr->insertFileFromUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (d_ptr->checkImportFileFlag)
        {
        case FactoryImportExportMenuContentPrivate::FAILED:
            paramTitle = trs("Import");
            paraText = QString("%1:\n%2").arg("ImportFilefailed").arg(d_ptr->failedImportXmlName);
            isClose = false;
            break;
        case FactoryImportExportMenuContentPrivate::SUCCEED:
            paramTitle = trs("Import");
            paraText = trs("ImportFileCompleted");
            isClose = false;
            break;
        case FactoryImportExportMenuContentPrivate::NOT_FOUND_UNODE:
            paramTitle = trs("Import");
            paraText = trs("NotFoundDeviceNode");
            isClose = false;
            break;
        case FactoryImportExportMenuContentPrivate::NOT_FOUND_UDISK:
            paramTitle = trs("Import");
            paraText = trs("NotFoundUDisk");
            isClose = false;
            break;
        case FactoryImportExportMenuContentPrivate::CLOSE:
            isClose = true;
            break;
        case FactoryImportExportMenuContentPrivate::NOT_SELECT_FILES:
            paramTitle = trs("Import");
            paraText = trs("PleaseSelectFiles");
            isClose = false;
            break;
        case FactoryImportExportMenuContentPrivate::IMPORTFILES_EMPTY:
            paramTitle = trs("Import");
            paraText = trs("PleaseAddImportFiles");
            isClose = false;
            break;
        }
        d_ptr->checkImportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.exec();
        }
        d_ptr->selectItemsImport.clear();
    }
}

void FactoryImportExportMenuContent::onConfigUpdated()
{
    QListWidgetItem *item = d_ptr->configList->currentItem();
    /*加入链表*/

    if (!d_ptr->selectItems.isEmpty()) /*链表不为空，进入比较*/
    {
        int indexFlag = 0;
        for (int index = 0; index < d_ptr->selectItems.count(); index ++) /*轮询比较是否再次选中对应选择项*/
        {
            if (d_ptr->selectItems.at(index) == item) /*只能进行指针比较，不能变量比较*/
            {
                item->setIcon(QIcon());
                d_ptr->selectItems.removeAt(index);
                indexFlag = 1;
                break;
            }
        }
        if (indexFlag == 0)
        {
            d_ptr->selectItems.append(item);  /*将选择项指针压入链表中*/
            item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        }
        indexFlag = 0;  /*复位标志位*/
    }
    else
    {
        d_ptr->selectItems.append(item);  /*将选择项指针压入链表中*/
        if (item != NULL)
        {
            item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        }
    }

    /*更新导出按钮使能状态*/
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }
}


QDomElement FactoryImportExportMenuContentPrivate::tagFindElement(const QStringList &list)
{
    QDomElement tag;

    if (localXml.isNull())
    {
        return tag;
    }

    if (list.size() < 1)
    {
        return tag;
    }

    tag = localXml.firstChildElement();

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

bool FactoryImportExportMenuContentPrivate::insertFileFromUSB()
{
    // 建立日志文件
    QFile filelogg(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filelogg.open(QIODevice::WriteOnly | QIODevice::Append);
    textStream.setDevice(&filelogg);  /*打开日志IO文件*/

    // 检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        checkImportFileFlag = NOT_FOUND_UNODE;  // 设备节点不存在
        return false;
    }

    // 挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit){} // 挂载失败
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        checkImportFileFlag = NOT_FOUND_UDISK;  // 挂载U盘失败
        return false;
    }

    // 遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters.append(CONFIG_FILE_ADULT);
    nameFilters.append(CONFIG_FILE_NEO);
    nameFilters.append(CONFIG_FILE_PED);
    nameFilters.append(CONFIG_FILE_SYSTEM);
    nameFilters.append(CONFIG_FILE_MACHINE);
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        checkImportFileFlag = IMPORTFILES_EMPTY;  // 文件为空
        return false;
    }

    /*选择导入哪些文件*/
    configListImport = new IListWidget;
    configListImport->addItems(files);
    ImportSubWidget myImportSubWidget(files);
    bool status = myImportSubWidget.exec();
    if (status == true) // OK
    {
        selectItemsImport = myImportSubWidget.readSelectItemsImport();
        remainderImportItems = selectItemsImport.count();
        if (selectItemsImport.isEmpty())
        {
            checkImportFileFlag = NOT_SELECT_FILES;  // 未选择条目
            return false;
        }
    }
    else  // close
    {
        selectItemsImport.clear();
        checkImportFileFlag = CLOSE;  // 关闭选择项
        return false;
    }

    // 加载本地xml数据
    QFile fileLocal(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if (!fileLocal.open(QFile::ReadOnly | QFile::Text))
    {
        textStream << QString("%1 open failed!!!\r\n").arg(fileLocal.fileName());
        checkImportFileFlag = FAILED;  // 失败
        return false;
    }

    if (!localXml.setContent(&fileLocal))
    {
        textStream << QString("%1 parser failed!!!\r\n").arg(fileLocal.fileName());
        checkImportFileFlag = FAILED;  // 失败
        return false;
    }
    fileLocal.close();



    QList<QDomElement> importTagList;
    QDomElement  importTag = importXml.documentElement();
    repeatFileChooseFlag = -1;
    for (int i = 0; i < selectItemsImport.count(); i++)
    {
        for (int j = 0; j < files.count(); j++)
        {
            if (files.at(j) == selectItemsImport.at(i)->text())
            {
                //  加载导入xml文件数据
                QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(j)));
                if (!file_import.open(QFile::ReadOnly | QFile::Text))
                {
                    textStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
                    checkImportFileFlag = FAILED;  // 失败
                    return false;
                }
                if (!importXml.setContent(&file_import))
                {
                    textStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
                    checkImportFileFlag = FAILED;//失败
                    return false;
                }
                file_import.close();
                break;
            }
        }

        importTagList.clear();
        importTag = importXml.documentElement();
        bool checkXmlFlag = checkXMLContent(importTagList, importTag);


        // 导入文件内容不合数据处理
        if (checkXmlFlag == false)
        {

            textStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(selectItemsImport.at(i)->text());

            if (checkXmlFlag == false)
            {
                checkImportFileFlag = FAILED;
                failedImportXmlName = selectItemsImport.at(i)->text();
            }
            return false;
        }
        else
        {
            bool isExist = false;
            int ChooseFlag = 1;

            if (QFile::exists(QString("%1%2").arg(CONFIG_DIR).arg(selectItemsImport.at(i)->text())))
            {
                isExist = true;
            }

            if (repeatFileChooseFlag == 3) // 全选覆盖
            {
                isExist = false;
            }
            else if (repeatFileChooseFlag == 2 && isExist == true) // 全选取消
            {
                isExist = false;
                ChooseFlag = 0;
            }

            if (isExist == true) // 发现同名文件
            {
                QStringList stringList;
                stringList.append(trs("NotRepeated"));  // 0
                stringList.append(trs("Repeated"));  // 1
                stringList.append(trs("AllNotRepeated"));  // 2
                stringList.append(trs("AllRepeated"));  // 3
                IMessageBox messageBox(trs("Import"),
                                       trs(QString("%1\r\n%2?").arg(selectItemsImport.at(i)->text()).arg(trs("IfSelectTheSameNameFile"))),
                                       stringList);
                ChooseFlag = messageBox.exec();
                repeatFileChooseFlag = ChooseFlag;
            }

            if (ChooseFlag == 1 || ChooseFlag == 3) // yes
            {
                QFile::copy(QString("%1/%2").arg(USB0_PATH_NAME).arg(selectItemsImport.at(i)->text()),
                            QString("%1%2").arg(CONFIG_DIR).arg(selectItemsImport.at(i)->text()));
                // 信息加入日志
                textStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(selectItemsImport.at(i)->text());
                // 装载导入文件  导入配置未完成
                FactoryConfigInfo  userDefine;
                userDefine.fileName = selectItemsImport.at(i)->text();
                userDefine.name     = selectItemsImport.at(i)->text().replace(".xml", "");
                bool configFlag = false;
                for (int configIndex = 0; configIndex < configList->count(); configIndex++)
                {
                    if (configs.at(configIndex).fileName == userDefine.fileName)
                    {
                        configFlag = true;
                        break;
                    }
                }
                if (configFlag == false)
                {
                    configs.append(userDefine);
                }
                if (configs.count() >= CONFIG_MAX_NUM)
                {
                    importBtn->setEnabled(false);
                    break;
                }
            }
            else if (ChooseFlag == 0 || ChooseFlag == 2) // no
            {
                remainderImportItems--;
                if (remainderImportItems == 0)
                {
                    checkImportFileFlag = NOT_SELECT_FILES;  // 未选择条目
                    return false;
                }
            }
        }
        filelogg.flush();
    }
    updateConfigList();
    //  configManager.saveUserConfigInfo(d_ptr->configs);

    // 解挂U盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit) // 挂载失败
    {
        // _checkImportFileFlag = FactoryImportExportMenuPrivate::NOT_FOUND_UDISK;//解挂U盘失败
        // return false;
    }

    filelogg.close();

    return true;
}


bool FactoryImportExportMenuContentPrivate::exportFileToUSB()
{
    // 检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        checkExportFileFlag = NOT_FOUND_UNODE;  // 设备节点不存在
        return false;
    }

    // 挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit){} // 挂载失败
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        checkExportFileFlag = NOT_FOUND_UDISK;  // 挂载U盘失败
        return false;
    }

    // 获得即将导出字符串的文件路径名称
    exportFileName.clear();
    for (int i = 0; i < selectItems.count(); i ++)
    {
        for (int j = 0; j < configs.count(); j ++)
        {
            if (selectItems.at(i)->text() == configs.at(j).name)
            {
                exportFileName.append(configs.at(j).fileName);
            }
        }
    }

    int count = exportFileName.count();
    remainderExportItems = count;
    // 开始导出数据
    if (count == 0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
        // 导出数据(.xml格式)
        repeatFileChooseFlag = -1;

        for (int i = 0; i < count; i++)
        {
            bool isExist = false;
            int ChooseFlag = 1;

            if (QFile::exists(QString("%1/%2").arg(USB0_PATH_NAME).arg(exportFileName.at(i))))
            {
                isExist = true;
            }

            if (repeatFileChooseFlag == 3) // 全选覆盖
            {
                isExist = false;
            }
            else if (repeatFileChooseFlag == 2 && isExist == true) // 全选取消
            {
                isExist = false;
                ChooseFlag = 0;
            }

            if (isExist == true) // 发现同名文件
            {
                QStringList stringList;
                stringList.append(trs("NotRepeated"));  // 0
                stringList.append(trs("Repeated"));  // 1
                stringList.append(trs("AllNotRepeated"));  // 2
                stringList.append(trs("AllRepeated"));  // 3
                IMessageBox messageBox(trs("Export"),
                                       trs(QString("%1\r\n%2?").arg(selectItems.at(i)->text()).arg(trs("IfSelectTheSameNameFile"))),
                                       stringList);
                ChooseFlag = messageBox.exec();
                repeatFileChooseFlag = ChooseFlag;
            }

            if (ChooseFlag == 1 || ChooseFlag == 3) // yes
            {
                QFile::copy(QString("%1/%2").arg(CONFIG_DIR).arg(exportFileName.at(i)),
                            QString("%1%2").arg(USB0_PATH_NAME).arg(exportFileName.at(i)));
            }
            else if (ChooseFlag == 0 || ChooseFlag == 2) // no
            {
                remainderExportItems--;
                if (remainderExportItems == 0)
                {
                    checkExportFileFlag = NOT_SELECT_FILES;  // 未选择条目
                    return false;
                }
            }
        }
    }

    // 解挂载u盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit){} // 解挂载失败
    exportFileName.clear();
    return true;
}
























