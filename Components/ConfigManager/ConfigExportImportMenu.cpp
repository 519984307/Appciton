/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/6
 **/
#include "ConfigExportImportMenu.h"
#include "ConfigMaintainMenuGrp.h"
#include "ConfigEditMenuGrp.h"
#include "ConfigManager.h"
#include "IListWidget.h"
#include "LabelButton.h"
#include <fcntl.h>
#include "IConfig.h"
#include <unistd.h>
#include <QProcess>
#include "TimeDate.h"
#include <dirent.h>
#include <QDir>
#include "ExportDataWidget.h"
#include "IMessageBox.h"
#include "ImportFileSubWidget.h"
#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

#define USB0_DEVICE_NODE   ("/dev/sda2")
#define USB0_PATH_NAME     ("/mnt/usb0")
#define CONFIG_DIR         ("/usr/local/nPM/etc/")
#define USER_DEFINE_CONFIG_PREFIX "UserDefine"


class ConfigExportImportMenuPrivate
{
public:
    /**
     * @brief ConfigExportImportMenuPrivate  构造方法
     */
    ConfigExportImportMenuPrivate()
        : configList(NULL), configListImport(),
          exportBtn(NULL), importBtn(NULL),
          remainderImportItems(0),
          remainderExportItems(0)
    {
        selectItems.clear();
        selectItemsImport.clear();
        configs.clear();
        remainderImportItems = 0;
    }

    enum
    {
        FAILED = -1,
        SUCCEED,
        NOT_FOUND_UNODE,
        NOT_FOUND_UDISK,
        NOT_SELECT_FILES,
        CLOSE,
        IMPORTFILES_EMPTY,
    };
    /**
     * @brief loadConfigs  装载数据方法
     */
    void loadConfigs();
    /**
     * @brief updateConfigList  更新配置列表
     */
    void updateConfigList();


    IListWidget   *configList;
    IListWidget   *configListImport;
    LButtonEx     *exportBtn;
    LButtonEx     *importBtn;
    int           remainderImportItems;
    int           remainderExportItems;
    QList<QListWidgetItem *> selectItems;
    QList<QListWidgetItem *> selectItemsImport;
    QList<ConfigManager::UserDefineConfigInfo> configs;
};


void ConfigExportImportMenuPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuPrivate::updateConfigList()
{
      // remove old item
    while (configList->count())
    {
        QListWidgetItem *item = configList->takeItem(0);
        delete item;
    }

    for (int i = 0; i < configs.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(configs.at(i).name, configList);
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

ConfigExportImportMenu::ConfigExportImportMenu()
    : SubMenu(trs("ExportImport")),
      _checkExportFileFlag(0),
      _checkImportFileFlag(0),
      _usbFd(0),
      _repeatFileChooseFlag(0),
      d_ptr(new ConfigExportImportMenuPrivate)

{
    setDesc(trs("ExportImport"));
    startLayout();
}

ConfigExportImportMenu::~ConfigExportImportMenu()
{
}

bool ConfigExportImportMenu::eventFilter(QObject *obj, QEvent *ev)
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
        }
    }
    return false;
}

void ConfigExportImportMenu::layoutExec()
{
    int submenuW = configMaintainMenuGrp.getSubmenuWidth();
    int submenuH = configMaintainMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(1);

      // configure label
    QLabel *label = new QLabel();
    label->setFont(fontManager.textFont(fontSize));
    QMargins margin = label->contentsMargins();
    margin.setTop(10);
    margin.setLeft(15);
    margin.setBottom(10);
    label->setContentsMargins(margin);
    label->setText(trs("ExportImportConfig"));
    mainLayout->addWidget(label);

      // config list
    d_ptr->configList = new IListWidget();
    d_ptr->configList->setFont(fontManager.textFont(fontSize));
    d_ptr->configList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->configList->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->configList->setFrameStyle(QFrame::Plain);
    d_ptr->configList->setSpacing(2);
    d_ptr->configList->setUniformItemSizes(true);
    d_ptr->configList->setIconSize(QSize(16, 16));

    QString configListStyleSheet =
        QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
                "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
                "QListWidget::item:focus {background-color: %2;}").arg("white").arg(colorManager.getHighlight().name());

    d_ptr->configList->setStyleSheet(configListStyleSheet);
    connect(d_ptr->configList, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(d_ptr->configList, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    d_ptr->configList->installEventFilter(this);
    d_ptr->configList->setFixedHeight(174);   // size for 5 items
    mainLayout->addWidget(d_ptr->configList);

      // buttons
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(QMargins(15, 10, 0, 40));
    hlayout->setSpacing(10);
    d_ptr->exportBtn = new LButtonEx();
    d_ptr->exportBtn->setText(trs("Export"));
    d_ptr->exportBtn->setFont(fontManager.textFont(fontSize));

    /*更新导出按钮使能状态*/
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }

    hlayout->addWidget(d_ptr->exportBtn);
    connect(d_ptr->exportBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d_ptr->importBtn = new LButtonEx();
    d_ptr->importBtn->setText(trs("Import"));
    d_ptr->importBtn->setFont(fontManager.textFont(fontSize));
    hlayout->addWidget(d_ptr->importBtn);
    connect(d_ptr->importBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    mainLayout->addLayout(hlayout);
}

void ConfigExportImportMenu::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();

    if (d_ptr->configList->count() >= CONFIG_MAX_NUM)
    {
        d_ptr->importBtn->setEnabled(false);
    }
    else
    {
        d_ptr->importBtn->setEnabled(true);
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

void ConfigExportImportMenu::onExitList(bool backTab)
{
    if (backTab)
    {
        focusPreviousChild();
    }
    else
    {
        focusNextChild();
    }
}

void ConfigExportImportMenu::onConfigClick()
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
            d_ptr->selectItems.append(item);/*将选择项指针压入链表中*/
            item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        }
        indexFlag = 0;/*复位标志位*/
    }
    else
    {
        d_ptr->selectItems.append(item);/*将选择项指针压入链表中*/
        item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
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

/**************************************************************************************************
 * 导出xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenu::exportFileToUSB()
{
      // 检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        _checkExportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UNODE;  // 设备节点不存在
        return false;
    }

      // 挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit)   // 挂载失败
    {
          // _checkExportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 挂载U盘失败
          // return false;
    }
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        _checkExportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 挂载U盘失败
        return false;
    }

      // 获得即将导出字符串的文件路径名称
    _exportFileName.clear();
    for (int i = 0; i < d_ptr->selectItems.count(); i ++)
    {
        for (int j = 0; j < d_ptr->configs.count(); j ++)
        {
            if (d_ptr->selectItems.at(i)->text() == d_ptr->configs.at(j).name)
            {
                _exportFileName.append(d_ptr->configs.at(j).fileName);
            }
        }
    }

    int count = _exportFileName.count();
    d_ptr->remainderExportItems = count;
      // 开始导出数据
    if (count == 0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
          // 导出数据(.xml格式)
        QDomDocument xml;
        QString fileName;
        QString fileNameTemp;
        _repeatFileChooseFlag = -1;

        for (int i = 0; i < count; i++)
        {
            bool isExist = false;
            int repeatFileChooseFlag = 1;

            if (QFile::exists(QString("%1/%2").arg(USB0_PATH_NAME).arg(_exportFileName.at(i))))
            {
                isExist = true;
            }

            if (_repeatFileChooseFlag == 3)   // 全选覆盖
            {
                isExist = false;
            }
            else if (_repeatFileChooseFlag == 2 && isExist == true)   // 全选取消
            {
                isExist = false;
                repeatFileChooseFlag = 0;
            }
            else
            {
            }

            if (isExist == true)   // 发现同名文件
            {
                QStringList stringList;
                QString stringtemp = trs("IfSelectTheSameNameFile");
                stringList.append(trs("NotRepeated"));  // 0
                stringList.append(trs("Repeated"));  // 1
                stringList.append(trs("AllNotRepeated"));  // 2
                stringList.append(trs("AllRepeated"));  // 3
                IMessageBox messageBox(trs("Export"),
                                       trs(QString("%1\r\n%2?").arg(d_ptr->selectItems.at(i)->text()).arg(stringtemp)),
                                       stringList);
                messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
                repeatFileChooseFlag = messageBox.exec();
                _repeatFileChooseFlag = repeatFileChooseFlag;
            }

            if (repeatFileChooseFlag == 1 || repeatFileChooseFlag == 3)   // yes
            {
                fileName = QString("%1%2").arg(CONFIG_DIR).arg(_exportFileName.at(i));
                QFile file(fileName);
                if (xml.setContent(&file) == false)
                {
                    file.close();
                    _checkExportFileFlag = ConfigExportImportMenuPrivate::FAILED;  //
                    _failedExportXmlName = _exportFileName.at(i);
                    return false;
                }

                fileNameTemp = QString("%1/%2").arg(USB0_PATH_NAME).arg(_exportFileName.at(i));
                QFile fileTemp(fileNameTemp);
                if (!fileTemp.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    _checkExportFileFlag = ConfigExportImportMenuPrivate::FAILED;  //
                    _failedExportXmlName = _exportFileName.at(i);
                    return false;
                }

                QTextStream writer(&fileTemp);
                writer.setCodec("UTF-8");
                xml.save(writer, 4, QDomNode::EncodingFromTextStream);
                fileTemp.close();
            }
            else if (repeatFileChooseFlag == 0 || repeatFileChooseFlag == 2)   // no
            {
                d_ptr->remainderExportItems--;
                if (d_ptr->remainderExportItems == 0)
                {
                    _checkExportFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;  // 未选择条目
                    return false;
                }
            }
        }
    }

      // 解挂载u盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit)   // 解挂载失败
    {
          // _checkExportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 挂载U盘失败
          // return _checkExportFileFlag;
    }
    _exportFileName.clear();
    return true;
}

/**************************************************************************************************
 * 导入xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenu::insertFileFromUSB()
{
      // 建立日志文件
    QFile filelogg(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filelogg.open(QIODevice::WriteOnly | QIODevice::Append);
    _textStream.setDevice(&filelogg);/*打开日志IO文件*/

      // 检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UNODE;  // 设备节点不存在
        return false;
    }

      // 挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit)   // 挂载失败
    {
          // _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 挂载U盘失败
          // return false;
    }
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 挂载U盘失败
        return false;
    }

      // 遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters.append("*.xml");
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        _checkImportFileFlag = ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY;  // 文件为空
        return false;
    }

    /*选择导入哪些文件*/
    d_ptr->configListImport = new IListWidget;
    d_ptr->configListImport->addItems(files);
    ImportSubWidget myImportSubWidget(files);
    bool status = myImportSubWidget.exec();
    if (status == true)   // OK
    {
  //         d_ptr->selectItemsImport = myImportSubWidget.readSelectItemsImport();
        d_ptr->remainderImportItems = d_ptr->selectItemsImport.count();
        if (d_ptr->selectItemsImport.isEmpty())
        {
            _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;  // 未选择条目
            return false;
        }
    }
    else  // close
    {
        d_ptr->selectItemsImport.clear();
        _checkImportFileFlag = ConfigExportImportMenuPrivate::CLOSE;  // 关闭选择项
        return false;
    }

      // 加载本地xml数据
    QFile fileLocal(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if (!fileLocal.open(QFile::ReadOnly | QFile::Text))
    {
        _textStream << QString("%1 open failed!!!\r\n").arg(fileLocal.fileName());
        _checkImportFileFlag = ConfigExportImportMenuPrivate::FAILED;  // 失败
        return false;
    }
    if (!_localXml.setContent(&fileLocal))
    {
        _textStream << QString("%1 parser failed!!!\r\n").arg(fileLocal.fileName());
        _checkImportFileFlag = ConfigExportImportMenuPrivate::FAILED;  // 失败
        return false;
    }
    fileLocal.close();


    d_ptr->loadConfigs();
    QList<QDomElement> importTagList;
    QDomElement  importTag = _importXml.documentElement();
    _repeatFileChooseFlag = -1;
    for (int i = 0; i < d_ptr->selectItemsImport.count(); i++)
    {

        bool checkXmlFlag = false;
        for (int j = 0; j < files.count(); j++)
        {
            if (files.at(j) == d_ptr->selectItemsImport.at(i)->text())
            {
                  // 加载导入xml文件数据
                QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(j)));
                if (!file_import.open(QFile::ReadOnly | QFile::Text))
                {
                    _textStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
                    _checkImportFileFlag = ConfigExportImportMenuPrivate::FAILED;  // 失败
                    return false;
                }
                if (!_importXml.setContent(&file_import))
                {
                    _textStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
                    _checkImportFileFlag = ConfigExportImportMenuPrivate::FAILED;  // 失败
                    return false;
                }
                file_import.close();
                break;
            }
        }
        importTagList.clear();
        importTag = _importXml.documentElement();
        checkXmlFlag = checkXMLContent(importTagList, importTag);


          // 导入文件内容不合数据处理
        if (checkXmlFlag == false)
        {

            _textStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(d_ptr->selectItemsImport.at(i)->text());

            if (checkXmlFlag == false)
            {
                _checkImportFileFlag = ConfigExportImportMenuPrivate::FAILED;
                _failedImportXmlName = d_ptr->selectItemsImport.at(i)->text();
            }
            return false;
        }
        else
        {
            bool isExist = false;
            int repeatFileChooseFlag = 1;

            if (QFile::exists(QString("%1%2").arg(CONFIG_DIR).arg(d_ptr->selectItemsImport.at(i)->text())))
            {
                isExist = true;
            }

            if (_repeatFileChooseFlag == 3)   // 全选覆盖
            {
                isExist = false;
            }
            else if (_repeatFileChooseFlag == 2 && isExist == true)   // 全选取消
            {
                isExist = false;
                repeatFileChooseFlag = 0;
            }
            else
            {
            }

            if (isExist == true)   // 发现同名文件
            {
                QStringList stringList;
                QString stringtemp = trs("IfSelectTheSameNameFile");
                stringList.append(trs("NotRepeated"));  // 0
                stringList.append(trs("Repeated"));  // 1
                stringList.append(trs("AllNotRepeated"));  // 2
                stringList.append(trs("AllRepeated"));  // 3
                IMessageBox messageBox(trs("Import"),
                                       trs(QString("%1\r\n%2?").arg(d_ptr->selectItemsImport.at(i)->text()).arg(stringtemp)),
                                       stringList);
                messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
                repeatFileChooseFlag = messageBox.exec();
                _repeatFileChooseFlag = repeatFileChooseFlag;
            }

            if (repeatFileChooseFlag == 1 || repeatFileChooseFlag == 3)   // yes
            {
                QFile::copy(QString("%1/%2").arg(USB0_PATH_NAME).arg(d_ptr->selectItemsImport.at(i)->text()),
                            QString("%1%2").arg(CONFIG_DIR).arg(d_ptr->selectItemsImport.at(i)->text()));
                  // 信息加入日志
                _textStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(d_ptr->selectItemsImport.at(i)->text());
                  // 装载导入文件  导入配置未完成
                ConfigManager::UserDefineConfigInfo  userDefine;
                userDefine.fileName = d_ptr->selectItemsImport.at(i)->text();
                userDefine.name     = d_ptr->selectItemsImport.at(i)->text().replace(".xml", "");
                bool configFlag = false;
                for (int configIndex = 0; configIndex < d_ptr->configList->count(); configIndex++)
                {
                    if (d_ptr->configs.at(configIndex).fileName == userDefine.fileName)
                    {
                        configFlag = true;
                        break;
                    }
                }
                if (configFlag == false)
                {
                    d_ptr->configs.append(userDefine);
                }
                if (d_ptr->configs.count() >= CONFIG_MAX_NUM)
                {
                    d_ptr->importBtn->setEnabled(false);
                    break;
                }
            }
            else if (repeatFileChooseFlag == 0 || repeatFileChooseFlag == 2)   // no
            {
                d_ptr->remainderImportItems--;
                if (d_ptr->remainderImportItems == 0)
                {
                    _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;  // 未选择条目
                    return false;
                }
            }
        }
        filelogg.flush();
    }
    d_ptr->updateConfigList();
    configManager.saveUserConfigInfo(d_ptr->configs);

      // 解挂U盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit)   // 挂载失败
    {
          //  _checkImportFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UDISK;  // 解挂U盘失败
          //  return false;
    }

    filelogg.close();

    return true;
}

QDomElement ConfigExportImportMenu::tagFindElement(const QStringList &list)
{
    QDomElement tag;

    if (_localXml.isNull())
    {
        return tag;
    }

    if (list.size() < 1)
    {
        return tag;
    }

    tag = _localXml.firstChildElement();

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


bool ConfigExportImportMenu::compareTagAttribute(QDomElement importTag, QDomElement localTag)
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
                _textStream << QString("Inserted xml files NodeName(%1)`s number value parser failed !!!\r\n").arg(localTag.nodeName());
                float floatMin = localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat();
                float floatMax = localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat();
                _textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(floatMin).arg(floatMax);
                return false;
            }
        }
        else/*字符串属性 直接比较是否相等*/
        {
            if (importTag.nodeValue() != localTag.nodeValue())
            {
                  // 信息加入日志
                _textStream << QString("Inserted xml files NodeName(%1)`s string value parser failed !!!\r\n").arg(localTag.nodeName());
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
                    _textStream << QString("Inserted xml files NodeName(%1)`s attribute1 parser failed !!!\r\n").arg(localTag.nodeName());
                    return false;
                }
            }
            else if (localDomNameNodeMap.item(i).nodeName() == PARAMETER_PREFIX1) /*Min、Max属性*/
            {
                if (importTag.text().toFloat() < localDomNameNodeMap.namedItem("Min").nodeValue().toFloat() || \
                        importTag.text().toFloat() > localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                      // 信息加入日志
                    _textStream << QString("Inserted xml files NodeName(%1)`s attribute2 parser failed !!!\r\n").arg(localTag.nodeName());
                    float fl = localDomNameNodeMap.namedItem("Min").nodeValue().toFloat();
                    float floa = localDomNameNodeMap.namedItem("Max").nodeValue().toFloat();
                    _textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(fl).arg(floa);
                    return false;
                }
            }
        }
    }
    return true;
}
  // 检查内容是否有效。 有效返回true；无效返回false；
  // 递归方式查询，注意递归结束条件的有效性
static bool attrCheckFlag = true;
bool ConfigExportImportMenu::checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag)
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

void ConfigExportImportMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());

    if (btn == d_ptr->exportBtn)
    {
        exportFileToUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (_checkExportFileFlag)
        {
        case ConfigExportImportMenuPrivate::FAILED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = QString("%1:%2").arg(trs("ExportFilefailed")).arg(_failedExportXmlName);
            break;
        case ConfigExportImportMenuPrivate::SUCCEED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("ExportFileCompleted");
            break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UNODE:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundDeviceNode");
            break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UDISK:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundUDisk");
            break;
        case ConfigExportImportMenuPrivate::CLOSE:
            isClose = true;
            break;
        case ConfigExportImportMenuPrivate::NOT_SELECT_FILES:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseSelectFiles");
            break;
        case ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseAddExportFiles");
            break;
        }
        _checkExportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
            messageBox.exec();
        }
          // d_ptr->selectItems.clear();
    }
    else if (btn == d_ptr->importBtn)
    {
        insertFileFromUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (_checkImportFileFlag)
        {
        case ConfigExportImportMenuPrivate::FAILED:
            paramTitle = trs("Import");
            paraText = QString("%1:\n%2").arg(trs("ImportFilefailed")).arg(_failedImportXmlName);
            isClose = false;
            break;
        case ConfigExportImportMenuPrivate::SUCCEED:
            paramTitle = trs("Import");
            paraText = trs("ImportFileCompleted");
            isClose = false;
            break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UNODE:
            paramTitle = trs("Import");
            paraText = trs("NotFoundDeviceNode");
            isClose = false;
            break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UDISK:
            paramTitle = trs("Import");
            paraText = trs("NotFoundUDisk");
            isClose = false;
            break;
        case ConfigExportImportMenuPrivate::CLOSE:
            isClose = true;
            break;
        case ConfigExportImportMenuPrivate::NOT_SELECT_FILES:
            paramTitle = trs("Import");
            paraText = trs("PleaseSelectFiles");
            isClose = false;
            break;
        case ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY:
            paramTitle = trs("Import");
            paraText = trs("PleaseAddImportFiles");
            isClose = false;
            break;
        }
        _checkImportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
            messageBox.exec();
        }
        d_ptr->selectItemsImport.clear();
    }
}


