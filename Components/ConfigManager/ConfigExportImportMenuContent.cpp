/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#include "ConfigExportImportMenu.h"
#include "ConfigMaintainMenuGrp.h"
#include "ConfigEditMenuGrp.h"
#include "ConfigManager.h"
#include "IListWidget.h"
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
#include "ConfigExportImportMenuContent.h"
#include <QGridLayout>
#include "ComboBox.h"
#include <QMap>
#include "Button.h"
#include <QDomComment>
#include <QHBoxLayout>
#include "FontManager.h"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

#define USB0_DEVICE_NODE   ("/dev/sda2")
#define USB0_PATH_NAME     ("/mnt/usb0")
#define CONFIG_DIR         ("/usr/local/nPM/etc/")
#define USER_DEFINE_CONFIG_PREFIX "UserDefine"


class ConfigExportImportMenuContentPrivate
{
public:
    /**
     * @brief ConfigExportImportMenuContentPrivate  构造方法
     */
    ConfigExportImportMenuContentPrivate(): checkExportFileFlag(0),
        checkImportFileFlag(0),
        usbFd(0),
        repeatFileChooseFlag(0),
        remainderImportItems(0),
        remainderExportItems(0),
        configListImport(NULL),
        failedImportXmlName(""),
        failedExportXmlName(""),
        sameImportXmlName(""),
        sameExportXmlName(""),
        attrCheckFlag(true)
    {
        selectItems.clear();
        selectItemsImport.clear();
        configs.clear();
        exportFileName.clear();
        localXml.clear();
        importXml.clear();
        ltws.clear();
        btns.clear();
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
    enum MenuItem
    {
        ITEM_LTW_CONFIG_LIST = 0,

        ITEM_BTN_EXPORT_CONFIG = 0,
        ITEM_BTN_IMPORT_CONFIG,
    };
    /**
     * @brief loadConfigs  装载数据方法
     */
    void loadConfigs();
    /**
     * @brief updateConfigList  更新配置列表
     */
    void updateConfigList();

    int               checkExportFileFlag;

    int               checkImportFileFlag;

    int               usbFd;

    int               repeatFileChooseFlag;

    int               remainderImportItems;

    int               remainderExportItems;

    IListWidget       *configListImport;

    QList<QListWidgetItem *> selectItems;

    QList<QListWidgetItem *> selectItemsImport;

    QList<ConfigManager::UserDefineConfigInfo> configs;

    QList<QString>    exportFileName;//  导出的XML文件名称的链表

    QTextStream       textStream;//  日志接口

    QDomDocument      localXml;//  本地检查文件

    QDomDocument      importXml;//  导入检查文件

    QString           failedImportXmlName;//  首个失败的导入文件名称

    QString           failedExportXmlName;//  首个失败的导出文件名称

    QString           sameImportXmlName;//  首个同名的导入文件名称

    QString           sameExportXmlName;//  首个同名的导出文件名称

    QMap <MenuItem, IListWidget *> ltws;

    QMap <MenuItem, Button *> btns;

    bool attrCheckFlag;
};


void ConfigExportImportMenuContentPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuContentPrivate::updateConfigList()
{
    //  remove old item
    while (ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->count())
    {
        QListWidgetItem *item = ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->takeItem(0);
        delete item;
    }

    for (int i = 0; i < configs.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(configs.at(i).name,
                ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]);
        item->setSizeHint(QSize(CONFIG_LIST_ITME_W, CONFIG_LIST_ITEM_H));
    }

    int count = configs.count();
    if (count)
    {
        ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setFocusPolicy(Qt::NoFocus);
    }
}

ConfigExportImportMenuContent::ConfigExportImportMenuContent()
    : MenuContent(trs("ExportImport"), trs("ExportImportDesc")),
      d_ptr(new ConfigExportImportMenuContentPrivate)

{
}

ConfigExportImportMenuContent::~ConfigExportImportMenuContent()
{
}

bool ConfigExportImportMenuContent::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST])
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if (e->reason() == Qt::TabFocusReason)
            {
                d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setCurrentRow(0);
            }
            else if (e->reason() == Qt::BacktabFocusReason)
            {
                d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setCurrentRow(
                    d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->count() - 1);
            }
        }

        if (ev->type() == QEvent::Hide)
        {
            if (!d_ptr->selectItems.isEmpty())
            {
                d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG]->setEnabled(true);
                d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(false);
                d_ptr->selectItems.clear();
            }
        }
    }
    return false;
}

void ConfigExportImportMenuContent::layoutExec()
{
    if (layout())
    {
        //  already install layout
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label = new QLabel(trs("ExportImportConfig"));
    layout->addWidget(label, d_ptr->ltws.count(), 0);

    IListWidget *listWidget = new IListWidget;
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setFrameStyle(QFrame::Plain);
    listWidget->setSpacing(2);
    listWidget->setUniformItemSizes(true);
    listWidget->setIconSize(QSize(16, 16));
    QString configListStyleSheet =
        QString("QListWidget { margin-left: 0px; border:0px solid #808080; border-radius: 0px; background-color: transparent; outline: none; }\n "
                "QListWidget::item {padding: 0px; border-radius: 0px; border: none; background-color: %1;}\n"
                "QListWidget::item:focus {background-color: %2;}").arg("gray").arg("blue");
    listWidget->setStyleSheet(configListStyleSheet);
    d_ptr->ltws.insert(ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST, listWidget);
    layout->addWidget(listWidget, d_ptr->ltws.count(), 0);
    connect(listWidget, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(listWidget, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    listWidget->installEventFilter(this);

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button;

    button = new Button(trs("Export"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    d_ptr->btns.insert(ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG, button);
    connect(d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG], SIGNAL(released()), this,
            SLOT(onBtnClick()));

    button = new Button(trs("Import"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    d_ptr->btns.insert(ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG, button);
    connect(d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG], SIGNAL(released()), this,
            SLOT(onBtnClick()));

    layout->addLayout(hl, (d_ptr->ltws.count() + 1), 0);

    layout->setRowStretch((d_ptr->ltws.count() + 2), 1);

    //  update export status
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(true);
    }
    else
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(false);
    }
}

void ConfigExportImportMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();

    if (d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->count() >= CONFIG_MAX_NUM)
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG]->setEnabled(false);
    }
    else
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG]->setEnabled(true);
    }

    /*更新导出按钮使能状态*/
    if (!d_ptr->selectItems.isEmpty())
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(true);
    }
    else
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(false);
    }
}

void ConfigExportImportMenuContent::onExitList(bool backTab)
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

void ConfigExportImportMenuContent::onConfigClick()
{
    QListWidgetItem *item = d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->currentItem();
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
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(true);
    }
    else
    {
        d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG]->setEnabled(false);
    }
}

/**************************************************************************************************
 * 导出xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenuContent::exportFileToUSB()
{
    //  检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UNODE;//设备节点不存在
        return false;
    }

    //  挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit) //挂载失败
    {
        //  d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//挂载U盘失败
        //  return false;
    }
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//  挂载U盘失败
        return false;
    }

    //  获得即将导出字符串的文件路径名称
    d_ptr->exportFileName.clear();
    for (int i = 0; i < d_ptr->selectItems.count(); i ++)
    {
        for (int j = 0; j < d_ptr->configs.count(); j ++)
        {
            if (d_ptr->selectItems.at(i)->text() == d_ptr->configs.at(j).name)
            {
                d_ptr->exportFileName.append(d_ptr->configs.at(j).fileName);
            }
        }
    }

    int count = d_ptr->exportFileName.count();
    d_ptr->remainderExportItems = count;
    //  开始导出数据
    if (count == 0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
        //  导出数据(.xml格式)
        QDomDocument xml;
        QString fileName;
        QString fileNameTemp;
        d_ptr->repeatFileChooseFlag = -1;

        for (int i = 0; i < count; i++)
        {
            bool isExist = false;
            int repeatFileChooseFlag = 1;

            if (QFile::exists(QString("%1/%2").arg(USB0_PATH_NAME).arg(d_ptr->exportFileName.at(i))))
            {
                isExist = true;
            }

            if (d_ptr->repeatFileChooseFlag == 3) //  全选覆盖
            {
                isExist = false;
            }
            else if (d_ptr->repeatFileChooseFlag == 2 && isExist == true) //  全选取消
            {
                isExist = false;
                repeatFileChooseFlag = 0;
            }
            else
            {
            }

            if (isExist == true) //  发现同名文件
            {
                QStringList stringList;
                QString stringtemp = trs("IfSelectTheSameNameFile");
                stringList.append(trs("NotRepeated"));//  0
                stringList.append(trs("Repeated"));//  1
                stringList.append(trs("AllNotRepeated"));//  2
                stringList.append(trs("AllRepeated"));//  3
                IMessageBox messageBox(trs("Export"),
                                       trs(QString("%1\r\n%2?").arg(d_ptr->selectItems.at(i)->text()).arg(stringtemp)),
                                       stringList);
                messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
                d_ptr->repeatFileChooseFlag = messageBox.exec();
                d_ptr->repeatFileChooseFlag = repeatFileChooseFlag;
            }

            if (d_ptr->repeatFileChooseFlag == 1 || d_ptr->repeatFileChooseFlag == 3) // yes
            {
                fileName = QString("%1%2").arg(CONFIG_DIR).arg(d_ptr->exportFileName.at(i));
                QFile file(fileName);
                if (xml.setContent(&file) == false)
                {
                    file.close();
                    d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//
                    d_ptr->failedExportXmlName = d_ptr->exportFileName.at(i);
                    return false;
                }

                fileNameTemp = QString("%1/%2").arg(USB0_PATH_NAME).arg(d_ptr->exportFileName.at(i));
                QFile fileTemp(fileNameTemp);
                if (!fileTemp.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//
                    d_ptr->failedExportXmlName = d_ptr->exportFileName.at(i);
                    return false;
                }

                QTextStream writer(&fileTemp);
                writer.setCodec("UTF-8");
                xml.save(writer, 4, QDomNode::EncodingFromTextStream);
                fileTemp.close();
            }
            else if (d_ptr->repeatFileChooseFlag == 0 || d_ptr->repeatFileChooseFlag == 2) // no
            {
                d_ptr->remainderExportItems--;
                if (d_ptr->remainderExportItems == 0)
                {
                    d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::NOT_SELECT_FILES;//  未选择条目
                    return false;
                }
            }
        }
    }

    //  解挂载u盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit) //  解挂载失败
    {
        //  d_ptr->checkExportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//挂载U盘失败
        //  return _d_ptr->checkExportFileFlag;
    }
    d_ptr->exportFileName.clear();
    return true;
}

/**************************************************************************************************
 * 导入xml文件。
 *************************************************************************************************/
bool ConfigExportImportMenuContent::insertFileFromUSB()
{
    //  建立日志文件
    QFile filelogg(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filelogg.open(QIODevice::WriteOnly | QIODevice::Append);
    d_ptr->textStream.setDevice(&filelogg);/*打开日志IO文件*/

    //  检测设备节点
    if (!QFile::exists(USB0_DEVICE_NODE))
    {
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UNODE;//设备节点不存在
        return false;
    }

    //  挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit) //  挂载失败
    {
        //  _d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//挂载U盘失败
        //  return false;
    }
    if (QFile::exists(USB0_PATH_NAME) == false)
    {
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//  挂载U盘失败
        return false;
    }

    //  遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters.append("*.xml");
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::IMPORTFILES_EMPTY;//  文件为空
        return false;
    }

    /*选择导入哪些文件*/
    d_ptr->configListImport = new IListWidget;
    d_ptr->configListImport->addItems(files);
    ImportSubWidget myImportSubWidget(files);
    bool status = myImportSubWidget.exec();
    if (status == true) //  OK
    {
        d_ptr->selectItemsImport = myImportSubWidget.readSelectItemsImport();
        d_ptr->remainderImportItems = d_ptr->selectItemsImport.count();
        if (d_ptr->selectItemsImport.isEmpty())
        {
            d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_SELECT_FILES;// 未选择条目
            return false;
        }
    }
    else//  close
    {
        d_ptr->selectItemsImport.clear();
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::CLOSE;//  关闭选择项
        return false;
    }

    //  加载本地xml数据
    QFile fileLocal(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if (!fileLocal.open(QFile::ReadOnly | QFile::Text))
    {
        d_ptr->textStream << QString("%1 open failed!!!\r\n").arg(fileLocal.fileName());
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//  失败
        return false;
    }
    if (!d_ptr->localXml.setContent(&fileLocal))
    {
        d_ptr->textStream << QString("%1 parser failed!!!\r\n").arg(fileLocal.fileName());
        d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//  失败
        return false;
    }
    fileLocal.close();


    d_ptr->loadConfigs();
    QList<QDomElement> importTagList;
    QDomElement  importTag = d_ptr->importXml.documentElement();
    d_ptr->repeatFileChooseFlag = -1;
    for (int i = 0; i < d_ptr->selectItemsImport.count(); i++)
    {

        for (int j = 0; j < files.count(); j++)
        {
            if (files.at(j) == d_ptr->selectItemsImport.at(i)->text())
            {
                //  加载导入xml文件数据
                QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(j)));
                if (!file_import.open(QFile::ReadOnly | QFile::Text))
                {
                    d_ptr->textStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
                    d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//  失败
                    return false;
                }
                if (!d_ptr->importXml.setContent(&file_import))
                {
                    d_ptr->textStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
                    d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;//  失败
                    return false;
                }
                file_import.close();
                break;
            }
        }
        bool checkXmlFlag;
        importTagList.clear();
        importTag = d_ptr->importXml.documentElement();
        checkXmlFlag = checkXMLContent(importTagList, importTag);


        //  导入文件内容不合数据处理
        if (checkXmlFlag == false)
        {

            d_ptr->textStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(d_ptr->selectItemsImport.at(
                                  i)->text());

            if (checkXmlFlag == false)
            {
                d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::FAILED;
                d_ptr->failedImportXmlName = d_ptr->selectItemsImport.at(i)->text();
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

            if (d_ptr->repeatFileChooseFlag == 3) //  全选覆盖
            {
                isExist = false;
            }
            else if (d_ptr->repeatFileChooseFlag == 2 && isExist == true) //  全选取消
            {
                isExist = false;
                repeatFileChooseFlag = 0;
            }
            else
            {
            }

            if (isExist == true) //  发现同名文件
            {
                QStringList stringList;
                QString stringtemp = trs("IfSelectTheSameNameFile");
                stringList.append(trs("NotRepeated"));//  0
                stringList.append(trs("Repeated"));//  1
                stringList.append(trs("AllNotRepeated"));//  2
                stringList.append(trs("AllRepeated"));//  3
                IMessageBox messageBox(trs("Import"),
                                       trs(QString("%1\r\n%2?").arg(d_ptr->selectItemsImport.at(i)->text()).arg(stringtemp)),
                                       stringList);
                messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
                repeatFileChooseFlag = messageBox.exec();
                d_ptr->repeatFileChooseFlag = repeatFileChooseFlag;
            }

            if (d_ptr->repeatFileChooseFlag == 1 || d_ptr->repeatFileChooseFlag == 3) //  yes
            {
                QFile::copy(QString("%1/%2").arg(USB0_PATH_NAME).arg(d_ptr->selectItemsImport.at(i)->text()),
                            QString("%1%2").arg(CONFIG_DIR).arg(d_ptr->selectItemsImport.at(i)->text()));
                //  信息加入日志
                d_ptr->textStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(d_ptr->selectItemsImport.at(
                                      i)->text());
                //  装载导入文件  导入配置未完成
                ConfigManager::UserDefineConfigInfo  userDefine;
                userDefine.fileName = d_ptr->selectItemsImport.at(i)->text();
                userDefine.name     = d_ptr->selectItemsImport.at(i)->text().replace(".xml", "");
                bool configFlag = false;
                for (int configIndex = 0;
                        configIndex < d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->count(); configIndex++)
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
                    d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG]->setEnabled(false);
                    break;
                }
            }
            else if (d_ptr->repeatFileChooseFlag == 0 || d_ptr->repeatFileChooseFlag == 2) //  no
            {
                d_ptr->remainderImportItems--;
                if (d_ptr->remainderImportItems == 0)
                {
                    d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_SELECT_FILES;//  未选择条目
                    return false;
                }
            }
        }
        filelogg.flush();
    }
    d_ptr->updateConfigList();
    configManager.saveUserConfigInfo(d_ptr->configs);

    //  解挂U盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if (backFlag != QProcess::NormalExit) //  挂载失败
    {
        // _d_ptr->checkImportFileFlag = ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK;//解挂U盘失败
        // return false;
    }

    filelogg.close();

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

    if (btn == d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_EXPORT_CONFIG])
    {
        exportFileToUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (d_ptr->checkExportFileFlag)
        {
        case ConfigExportImportMenuContentPrivate::FAILED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = QString("%1:%2").arg(trs("ExportFilefailed")).arg(d_ptr->failedExportXmlName);
            break;
        case ConfigExportImportMenuContentPrivate::SUCCEED:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("ExportFileCompleted");
            break;
        case ConfigExportImportMenuContentPrivate::NOT_FOUND_UNODE:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundDeviceNode");
            break;
        case ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("NotFoundUDisk");
            break;
        case ConfigExportImportMenuContentPrivate::CLOSE:
            isClose = true;
            break;
        case ConfigExportImportMenuContentPrivate::NOT_SELECT_FILES:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseSelectFiles");
            break;
        case ConfigExportImportMenuContentPrivate::IMPORTFILES_EMPTY:
            isClose = false;
            paramTitle = trs("Export");
            paraText = trs("PleaseAddExportFiles");
            break;
        }
        d_ptr->checkExportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
            messageBox.exec();
        }
    }
    else if (btn == d_ptr->btns[ConfigExportImportMenuContentPrivate::ITEM_BTN_IMPORT_CONFIG])
    {
        insertFileFromUSB();
        QString paramTitle = "";
        QString paraText = "";
        bool isClose = false;
        switch (d_ptr->checkImportFileFlag)
        {
        case ConfigExportImportMenuContentPrivate::FAILED:
            paramTitle = trs("Import");
            paraText = QString("%1:\n%2").arg(trs("ImportFilefailed")).arg(d_ptr->failedImportXmlName);
            isClose = false;
            break;
        case ConfigExportImportMenuContentPrivate::SUCCEED:
            paramTitle = trs("Import");
            paraText = trs("ImportFileCompleted");
            isClose = false;
            break;
        case ConfigExportImportMenuContentPrivate::NOT_FOUND_UNODE:
            paramTitle = trs("Import");
            paraText = trs("NotFoundDeviceNode");
            isClose = false;
            break;
        case ConfigExportImportMenuContentPrivate::NOT_FOUND_UDISK:
            paramTitle = trs("Import");
            paraText = trs("NotFoundUDisk");
            isClose = false;
            break;
        case ConfigExportImportMenuContentPrivate::CLOSE:
            isClose = true;
            break;
        case ConfigExportImportMenuContentPrivate::NOT_SELECT_FILES:
            paramTitle = trs("Import");
            paraText = trs("PleaseSelectFiles");
            isClose = false;
            break;
        case ConfigExportImportMenuContentPrivate::IMPORTFILES_EMPTY:
            paramTitle = trs("Import");
            paraText = trs("PleaseAddImportFiles");
            isClose = false;
            break;
        }
        d_ptr->checkImportFileFlag = 0;
        if (isClose == false)
        {
            IMessageBox messageBox(paramTitle, paraText, false);
            messageBox.setFixedSize(configMaintainMenuGrp.getSubmenuWidth() * 2 / 3, configMaintainMenuGrp.getSubmenuHeight() / 3);
            messageBox.exec();
        }
        d_ptr->selectItemsImport.clear();
    }
}

void ConfigExportImportMenuContent::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        d_ptr->ltws[ConfigExportImportMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setFont(fontManager.textFont(
                    font().pixelSize()));
    }
}


