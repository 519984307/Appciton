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
    ConfigExportImportMenuPrivate()
        :configList(NULL), configListImport(),
         exportBtn(NULL), importBtn(NULL)
    {
        selectItems.clear();
        selectItemsImport.clear();
        configs.clear();
        remainderImportItems =0;
    }

    enum{
        FAILED=-1,
        SUCCEED,
        NOT_FOUND_U,
        NOT_FOUND_UU,
        NOT_SELECT_FILES,
        CLOSE,
        IMPORTFILES_EMPTY,
    };
    void loadConfigs();
    void updateConfigList();


    IListWidget   *configList;
    IListWidget   *configListImport;
    LButtonEx     *exportBtn;
    LButtonEx     *importBtn;
    int           remainderImportItems;
    int           remainderExportItems;
    QList<QListWidgetItem*> selectItems;
    QList<QListWidgetItem*> selectItemsImport;
    QList<ConfigManager::UserDefineConfigInfo> configs;
};


void ConfigExportImportMenuPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuPrivate::updateConfigList()
{
    //remove old item
    while(configList->count())
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
    if(count)
    {
        configList->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        configList->setFocusPolicy(Qt::NoFocus);
    }

}

ConfigExportImportMenu::ConfigExportImportMenu()
    :SubMenu(trs("Export/Import")), d_ptr(new ConfigExportImportMenuPrivate)
{
    setDesc(trs("Export/Import"));
    startLayout();
}

ConfigExportImportMenu::~ConfigExportImportMenu()
{

}

bool ConfigExportImportMenu::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == d_ptr->configList)
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if(e->reason() == Qt::TabFocusReason)
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
            if(!d_ptr->selectItems.isEmpty())
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

    //configure label
    QLabel *label = new QLabel();
    label->setFont(fontManager.textFont(fontSize));
    QMargins margin = label->contentsMargins();
    margin.setTop(10);
    margin.setLeft(15);
    margin.setBottom(10);
    label->setContentsMargins(margin);
    label->setText(trs("Export/ExportConfig"));
    mainLayout->addWidget(label);

    //config list
    d_ptr->configList = new IListWidget();
    d_ptr->configList->setFont(fontManager.textFont(fontSize));
    d_ptr->configList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->configList->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->configList->setFrameStyle(QFrame::Plain);
    d_ptr->configList->setSpacing(2);
    d_ptr->configList->setUniformItemSizes(true);
    d_ptr->configList->setIconSize(QSize(16,16));

    QString configListStyleSheet = QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
    "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
    "QListWidget::item:focus {background-color: %2;}").arg("white").arg(colorManager.getHighlight().name());

    d_ptr->configList->setStyleSheet(configListStyleSheet);
    connect(d_ptr->configList, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(d_ptr->configList, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    d_ptr->configList->installEventFilter(this);
    d_ptr->configList->setFixedHeight(174); //size for 5 items
    mainLayout->addWidget(d_ptr->configList);

    //buttons
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(QMargins(15, 10, 0, 40));
    hlayout->setSpacing(10);
    d_ptr->exportBtn = new LButtonEx();
    d_ptr->exportBtn->setText(trs("Export"));
    d_ptr->exportBtn->setFont(fontManager.textFont(fontSize));

    /*更新导出按钮使能状态*/
    if(!d_ptr->selectItems.isEmpty())
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

    if(d_ptr->configList->count() >= CONFIG_MAX_NUM)
    {
        d_ptr->importBtn->setEnabled(false);
    }
    else
    {
        d_ptr->importBtn->setEnabled(true);
    }

    /*更新导出按钮使能状态*/
    if(!d_ptr->selectItems.isEmpty())
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
    if(backTab)
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
    int indexFlag = 0;
    if(!d_ptr->selectItems.isEmpty())/*链表不为空，进入比较*/
    {
        for(int index = 0; index < d_ptr->selectItems.count(); index ++)/*轮询比较是否再次选中对应选择项*/
        {
            if(d_ptr->selectItems.at(index)==item)/*只能进行指针比较，不能变量比较*/
            {
                item->setIcon(QIcon());
                d_ptr->selectItems.removeAt(index);
                indexFlag = 1;
                break;
            }
        }
        if(indexFlag == 0)
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
    if(!d_ptr->selectItems.isEmpty())
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
int ConfigExportImportMenu::exportFileToUSB()
{
    int checkFileFlag=0;

    //检测设备节点
    if(!QFile::exists(USB0_DEVICE_NODE))
    {
        checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_U;//设备节点不存在
        return checkFileFlag;
    }

    //挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if(backFlag!=QProcess::NormalExit)//挂载失败
    {
        //checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//挂载U盘失败
        //return checkFileFlag;
    }
    if(QFile::exists(USB0_PATH_NAME)==false)
    {
        checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//挂载U盘失败
        return checkFileFlag;
    }

    //遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters.append("*.xml");
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);


    //获得即将导出字符串的文件路径名称
    _exportFileName.clear();
    QMap<QString,QString> myExportFileInfo;
    for(int i = 0; i < d_ptr->selectItems.count(); i ++)
    {
        for(int j = 0; j<d_ptr->configs.count(); j ++)
        {
            if(d_ptr->selectItems.at(i)->text()==d_ptr->configs.at(j).name)
            {
                _exportFileName.append(d_ptr->configs.at(j).fileName);
                myExportFileInfo[d_ptr->selectItems.at(i)->text()]=d_ptr->configs.at(j).fileName;
            }
        }

    }

     int count = _exportFileName.count();
     d_ptr->remainderExportItems = count;
    //开始导出数据
    if(count==0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
        //导出数据(.xml格式)
        QDomDocument xml;
        QString fileName;
        QString fileNameTemp;

        for(int i=0;i<count;i++)
        {
            bool isexit = false;
            for(int selectItem = 0; selectItem<files.count(); selectItem ++)
            {
                if(myExportFileInfo[d_ptr->selectItems.at(i)->text()]==files.at(selectItem))
                {
                    isexit = true;
                    break;
                }

            }
            bool isCopy = true;
            if(isexit==true)//发现同名文件
            {
                IMessageBox messageBox;
                messageBox.iMessageBoxExport(trs("Export"),
                                       QString("%1%2").arg(d_ptr->selectItems.at(i)->text()).arg("\r\nif select the same name file?"));
                isCopy = messageBox.exec();
            }

            if(isCopy==true)//yes
            {
                fileName = QString("%1%2").arg(CONFIG_DIR).arg(_exportFileName.at(i));
                QFile file(fileName);
                if(xml.setContent(&file)==false)
                {
                    file.close();
                    checkFileFlag = ConfigExportImportMenuPrivate::FAILED;//
                    _failedExportXmlName = _exportFileName.at(i);
                    return checkFileFlag;
                }

                fileNameTemp = QString("%1/%2").arg(USB0_PATH_NAME).arg(_exportFileName.at(i));
                QFile fileTemp(fileNameTemp);
                if(!fileTemp.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    checkFileFlag = ConfigExportImportMenuPrivate::FAILED;//
                    _failedExportXmlName = _exportFileName.at(i);
                    return checkFileFlag;
                }

                QTextStream writer(&fileTemp);
                writer.setCodec("UTF-8");

                QMutexLocker locker(&_lock);
                xml.save(writer, 4, QDomNode::EncodingFromTextStream);

                fileTemp.close();
                locker.unlock();
            }
            else//no
            {
                d_ptr->remainderExportItems--;
                if(d_ptr->remainderExportItems==0)
                {
                    checkFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;//未选择条目
                    return checkFileFlag;
                }
            }

        }
    }

    //解挂载u盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if(backFlag!=QProcess::NormalExit)//解挂载失败
    {
        //checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//挂载U盘失败
        //return checkFileFlag;
    }
    _exportFileName.clear();
    return checkFileFlag;
}

/**************************************************************************************************
 * 导入xml文件。
 *************************************************************************************************/
int ConfigExportImportMenu::insertFileFromUSB()
{

    int checkFileFlag=0;
    //建立日志文件
    QFile filelogg(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filelogg.open(QIODevice::WriteOnly | QIODevice::Append);
    _textStream.setDevice(&filelogg);/*打开日志IO文件*/

    //检测设备节点
    if(!QFile::exists(USB0_DEVICE_NODE))
    {
        checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_U;//设备节点不存在
        return checkFileFlag;
    }

    //挂载u盘
    int backFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if(backFlag!=QProcess::NormalExit)//挂载失败
    {
        //checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//挂载U盘失败
        //return checkFileFlag;
    }
    if(QFile::exists(USB0_PATH_NAME)==false)
    {
        checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//挂载U盘失败
        return checkFileFlag;
    }

    //遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters.append("*.xml");
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    if(files.isEmpty())
    {
        checkFileFlag = ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY;//文件为空
        return checkFileFlag;
    }

    /*选择导入哪些文件*/
    d_ptr->configListImport = new IListWidget;
    d_ptr->configListImport->addItems(files);
    ImportSubWidget myImportSubWidget(files);
    bool status = myImportSubWidget.exec();
    if(status==true)//OK
    {
        d_ptr->selectItemsImport = myImportSubWidget.readSelectItemsImport();
        d_ptr->remainderImportItems = d_ptr->selectItemsImport.count();
        if(d_ptr->selectItemsImport.isEmpty())
        {
            checkFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;//未选择条目
            return checkFileFlag;
        }
    }
    else//close
    {
        d_ptr->selectItemsImport.clear();
        checkFileFlag = ConfigExportImportMenuPrivate::CLOSE;//关闭选择项
        return checkFileFlag;
    }

    //加载本地xml数据
    QFile fileLocal(QString("%1usercheck.xml").arg(CONFIG_DIR));
    if(!fileLocal.open(QFile::ReadOnly | QFile::Text))
    {
       addTimeLogg();
       _textStream << QString("%1 open failed!!!\r\n").arg(fileLocal.fileName());
    }
    if(_localXml.setContent(&fileLocal)<0)
    {
        addTimeLogg();
        _textStream << QString("%1 parser failed!!!\r\n").arg(fileLocal.fileName());
    }
    fileLocal.close();


    d_ptr->loadConfigs();
    bool checkXmlFlag = false;
    QList<QDomElement> importTagList;
    QDomElement  importTag = _importXml.documentElement();
    for(int i=0;i<d_ptr->selectItemsImport.count();i++)
    {

        for(int j=0;j<files.count();j++)
        {
            if(files.at(j)==d_ptr->selectItemsImport.at(i)->text())
            {
                //加载导入xml文件数据
                QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(j)));
                if(!file_import.open(QFile::ReadOnly | QFile::Text))
                {
                    addTimeLogg();
                    _textStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
                }
                if(_importXml.setContent(&file_import)<0)
                {
                    addTimeLogg();
                    _textStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
                }
                file_import.close();
                break;
            }
        }
        importTagList.clear();
        importTag = _importXml.documentElement();
        checkXmlFlag = checkXMLContent(importTagList,importTag);


        //导入文件内容不合数据处理
        if(checkXmlFlag==false)
        {
            addTimeLogg();
            _textStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(d_ptr->selectItemsImport.at(i)->text());

            if(checkXmlFlag==false)
            {
                checkFileFlag = ConfigExportImportMenuPrivate::FAILED;
                _failedImportXmlName = d_ptr->selectItemsImport.at(i)->text();
            }
            return checkFileFlag;
        }
        else
        {
            bool isexit = false;
            for(int selectItemLocal = 0; selectItemLocal<d_ptr->configs.count(); selectItemLocal ++)
            {
                if(d_ptr->selectItemsImport.at(i)->text()==d_ptr->configs.at(selectItemLocal).fileName)
                {
                    isexit = true;
                    break;
                }

            }

            bool isCopy = true;
            if(isexit==true)//发现同名文件
            {
                IMessageBox messageBox;
                messageBox.iMessageBoxExport(trs("Import"),
                                       QString("%1%2").arg(d_ptr->selectItemsImport.at(i)->text()).arg("\r\nif select the same name file?"));
                isCopy = messageBox.exec();
            }

            if(isCopy==true)//yes
            {
                QFile::copy(QString("%1/%2").arg(USB0_PATH_NAME).arg(d_ptr->selectItemsImport.at(i)->text()),
                            QString("%1%2").arg(CONFIG_DIR).arg(d_ptr->selectItemsImport.at(i)->text()));
                //信息加入日志
                 addTimeLogg();
                _textStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(d_ptr->selectItemsImport.at(i)->text());
                //装载导入文件  导入配置未完成
                ConfigManager::UserDefineConfigInfo  userDefine;
                userDefine.fileName = d_ptr->selectItemsImport.at(i)->text();
                userDefine.name     = d_ptr->selectItemsImport.at(i)->text().replace(".xml","");
                bool configFlag=false;
                for(int configIndex=0;configIndex<d_ptr->configList->count();configIndex++)
                {
                    if(d_ptr->configs.at(configIndex).fileName==userDefine.fileName)
                    {
                        configFlag = true;
                        break;
                    }
                }
                if(configFlag==false)
                {
                    d_ptr->configs.append(userDefine);
                }
                configFlag = false;
                if(d_ptr->configs.count() >= CONFIG_MAX_NUM)
                {
                    d_ptr->importBtn->setEnabled(false);
                    break;
                }
            }
            else//no
            {
                d_ptr->remainderImportItems--;
                if(d_ptr->remainderImportItems==0)
                {
                    checkFileFlag = ConfigExportImportMenuPrivate::NOT_SELECT_FILES;//未选择条目
                    return checkFileFlag;
                }
            }

        }
        filelogg.flush();
    }
    d_ptr->updateConfigList();
    configManager.saveUserConfigInfo(d_ptr->configs);

    //解挂U盘
    backFlag = QProcess::execute(QString("umount -t vfat %1 %2").arg(USB0_DEVICE_NODE).arg(USB0_PATH_NAME));
    QProcess::execute("sync");
    if(backFlag!=QProcess::NormalExit)//挂载失败
    {
       // checkFileFlag = ConfigExportImportMenuPrivate::NOT_FOUND_UU;//解挂U盘失败
       // return checkFileFlag;
    }

    filelogg.close();

    return checkFileFlag;
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


bool ConfigExportImportMenu::compareTagAttribute(QDomElement importTag,QDomElement localTag)
{
    #define PARAMETER_PREFIX_NUM0   (4)  /*参数前缀的长度*/
    #define PARAMETER_PREFIX_NUM1   (2)  /*参数前缀的长度*/
    #define PARAMETER_PREFIX0       ("Enable")  /*参数前缀*/
    #define PARAMETER_PREFIX1       ("Min")  /*参数前缀*/
    float numArrary[]={1.0,2.0};
    QString attrString[]={"Enable","Prio"};
    QDomNamedNodeMap localDomNameNodeMap = localTag.attributes();

    if(localDomNameNodeMap.count()==PARAMETER_PREFIX_NUM0)/*导入标签无属性值*/
    {
        if(localDomNameNodeMap.namedItem("cAttr0").nodeValue().toFloat()==0)/*数字属性 需要比较上下限*/
        {
            if(importTag.text().toFloat()<localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat()||\
               importTag.text().toFloat()>localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat())
            {
                //信息加入日志
                 addTimeLogg();
                _textStream << QString("Inserted xml files NodeName(%1)`s number value parser failed !!!\r\n").arg(localTag.nodeName());
                float floatMin = localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat();
                float floatMax = localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat();
                _textStream << QString("(%1 %2 %3)\r\n").arg(importTag.text().toFloat()).arg(floatMin).arg(floatMax);
                return false;
            }
        }
        else/*字符串属性 直接比较是否相等*/
        {
            if(importTag.nodeValue()!=localTag.nodeValue())
            {
                //信息加入日志
                 addTimeLogg();
                _textStream << QString("Inserted xml files NodeName(%1)`s string value parser failed !!!\r\n").arg(localTag.nodeName());
                return false;
            }
        }
    }
    else if(localDomNameNodeMap.count()==PARAMETER_PREFIX_NUM1)/*导入标签有属性值的*/
    {
        int localAttrbuiteSize = localDomNameNodeMap.count();
        for(int i=0;i<localAttrbuiteSize;i++)
        {
            if(localDomNameNodeMap.item(i).nodeName()==PARAMETER_PREFIX0)/*Enable、Prio属性*/
            {
                if(importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat()<0||\
                   importTag.attributes().namedItem(attrString[i]).nodeValue().toFloat()>numArrary[i])
                {
                    //信息加入日志
                     addTimeLogg();
                    _textStream << QString("Inserted xml files NodeName(%1)`s attribute1 parser failed !!!\r\n").arg(localTag.nodeName());
                    return false;
                }
            }
            else if(localDomNameNodeMap.item(i).nodeName()==PARAMETER_PREFIX1)/*Min、Max属性*/
            {
                if(importTag.text().toFloat()<localDomNameNodeMap.namedItem("Min").nodeValue().toFloat()||\
                   importTag.text().toFloat()>localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                    //信息加入日志
                     addTimeLogg();
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
//检查内容是否有效。 有效返回true；无效返回false；
//递归方式查询，注意递归结束条件的有效性
static bool attrCheckFlag = true;
bool ConfigExportImportMenu::checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag)
{
     if(importTag.isNull())
     {
         QStringList nameList;

         for(int i=0;i<importTagList.count();i++)
         {
             nameList.append(importTagList.at(i).nodeName());
         }

         QDomElement localTag = tagFindElement(nameList);
         if(!localTag.isNull())
         {
            bool attrFlag = compareTagAttribute(importTagList.at(importTagList.count()-1),localTag);
            //添加屏蔽处理WiFi.Local..等可变参数
            if(nameList.at(1)=="WiFi")
            {
                attrFlag = true;
            }
            else if(nameList.at(1)=="Local")
            {
                attrFlag = true;
            }

            if(attrFlag==false)
            {
                attrCheckFlag = false;
                return attrCheckFlag;
            }
         }

         if(importTagList.empty())
         {
             attrCheckFlag = true;
             return attrCheckFlag;
         }
         importTag = importTagList.last().nextSiblingElement();//下一个同类子节点
         importTagList.removeLast();//移除最后一个子节点
         if(importTagList.empty())
         {
             attrCheckFlag = true;
             return attrCheckFlag;
         }

         if(importTag.isNull())
         {
            importTag = importTagList.last();
            importTag = importTag.nextSiblingElement();//下一个同类子节点
            importTagList.removeLast();//移除最后一个子节点
         }
         checkXMLContent(importTagList,importTag);

     }
     else
     {
        importTagList.append(importTag);
        importTag = importTag.firstChildElement();
        checkXMLContent(importTagList,importTag);
     }

     return attrCheckFlag;

}

void ConfigExportImportMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());
    if(btn == d_ptr->exportBtn)
    {
        int exportFileflag = exportFileToUSB();
        IMessageBox messageBox;
        bool isClose = false;
        switch (exportFileflag)
        {
        case ConfigExportImportMenuPrivate::FAILED:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), QString("ExportFilefailed:%1").arg(_failedExportXmlName));
        break;
        case ConfigExportImportMenuPrivate::SUCCEED:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), "ExportFileCompleted");
        break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_U:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), "NotFoundDeviceNode");
        break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UU:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), "NotFoundUDisk");
        break;
        case ConfigExportImportMenuPrivate::CLOSE:
            isClose = true;
        break;
        case ConfigExportImportMenuPrivate::NOT_SELECT_FILES:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), "PleaseSelectFiles");
        break;
        case ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY:
            isClose = false;
            messageBox.iMessageBox(trs("Export"), "PleaseAddExportFiles");
        break;
        }
        if(isClose==false)
        {
            messageBox.exec();
        }
        //d_ptr->selectItems.clear();

    }
    else if(btn == d_ptr->importBtn)
    {
        int importFileflag = insertFileFromUSB();
        IMessageBox messageBox;
        bool isClose = false;
        switch (importFileflag)
        {
        case ConfigExportImportMenuPrivate::FAILED:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), QString("ImportFilefailed:\n%1").arg(_failedImportXmlName));
        break;
        case ConfigExportImportMenuPrivate::SUCCEED:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), "ImportFileCompleted");
        break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_U:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), "NotFoundDeviceNode");
        break;
        case ConfigExportImportMenuPrivate::NOT_FOUND_UU:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), "NotFoundUDisk");
        break;
        case ConfigExportImportMenuPrivate::CLOSE:
            isClose = true;
        break;
        case ConfigExportImportMenuPrivate::NOT_SELECT_FILES:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), "PleaseSelectFiles");
        break;
        case ConfigExportImportMenuPrivate::IMPORTFILES_EMPTY:
            isClose = false;
            messageBox.iMessageBox(trs("Import"), "PleaseAddImportFiles");
        break;
        }
        if(isClose==false)
        {
            messageBox.exec();
        }
        d_ptr->selectItemsImport.clear();
    }

}


