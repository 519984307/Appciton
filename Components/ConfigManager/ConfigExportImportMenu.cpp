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

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

#define USB0_DEVICE_NODE   ("/dev/sdb0")
#define USB0_PATH_NAME     ("/mnt/usb0")
#define CONFIG_DIR         ("/usr/local/nPM/etc/")
#define USER_DEFINE_CONFIG_PREFIX "UserDefine"
class ConfigExportImportMenuPrivate
{
public:
    ConfigExportImportMenuPrivate()
        :configList(NULL), exportBtn(NULL), importBtn(NULL),
        lastSelectItem(NULL)
    {}

    void loadConfigs();
    void updateConfigList();
    void importConfigs();

    IListWidget *configList;
    LButtonEx *exportBtn;
    LButtonEx *importBtn;

    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;

};

void ConfigExportImportMenuPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuPrivate::importConfigs()
{

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
            if(d_ptr->lastSelectItem)
            {
                d_ptr->lastSelectItem->setIcon(QIcon());
                d_ptr->importBtn->setEnabled(true);
                d_ptr->exportBtn->setEnabled(false);
                d_ptr->lastSelectItem = NULL;
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
    if(d_ptr->lastSelectItem)
    {
        d_ptr->lastSelectItem->setIcon(QIcon());
    }

    if(item != d_ptr->lastSelectItem)
    {
        item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        d_ptr->lastSelectItem = item;
    }
    else
    {
        d_ptr->lastSelectItem = NULL;
    }

    if(d_ptr->lastSelectItem)
    {
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }
}

void ConfigExportImportMenu::ExportFileToUSB()
{
    QProcess *procNew = 0;
    QString appName;
    QStringList argName;

    //打开U盘节点
    _usbFd = ::open(USB0_DEVICE_NODE, O_RDWR);
    if(_usbFd < 0)
    {
        qdebug("USB disk not found !!!\n");
    }
    //获得即将导出字符串的文件路径名称
    int count=0;
    QString *fileName;
    systemConfig.getNumAttr("ConfigManager|UserDefine","count",count);
    for(int i=0;i<count;i++)
    {
        fileName = new QString("");
        systemConfig.getStrValue(QString("ConfigManager|UserDefine|Config%1").arg(i+1),*fileName);
        _ExportFileName.append(*fileName);
    }
    //开始导出数据
    if(count==0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
        //挂载u盘
        procNew = new QProcess(this);
        appName = "mount";

        argName.append("-t");
        argName.append("vfat");
        argName.append(USB0_DEVICE_NODE);
        argName.append(USB0_PATH_NAME);
        procNew->start(appName,argName);
        procNew->waitForStarted();

        //需要添加挂载u盘失败情况处理代码

        //导出数据(.xml格式)
        QDomDocument xml;
        QString fileName;
        QString fileName1;
        for(int i=0;i<count;i++)
        {
            fileName = QString("%1%2").arg(CONFIG_DIR).arg(_ExportFileName.at(i));
            QFile file(fileName);
            if(xml.setContent(&file)==false)
            {
                file.close();
                qdebug("Read ExportFile_%d failed!\n");
                break;

            }

            fileName1 = QString("%1/pation%2").arg(USB0_PATH_NAME).arg(_ExportFileName.at(i));
            QFile file1(fileName1);
            if(!file1.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qdebug("save failed");
                break;
            }

            QTextStream writer(&file1);
            writer.setCodec("UTF-8");

            QMutexLocker locker(&_lock);
            xml.save(writer, 4, QDomNode::EncodingFromTextStream);

            file1.close();
            locker.unlock();
        }
    }

    if(procNew!=0) //解挂u盘
    {
        appName = "umount";
        argName.append("-t");
        argName.append("vfat");
        argName.append(USB0_DEVICE_NODE);
        argName.append(USB0_PATH_NAME);
        procNew->start(appName,argName);
        procNew->waitForStarted();
        delete procNew;

    }
    ::close(_usbFd);
    qdebug("Export File completed!\n");
}
//1.xml文件导入cpu缓存中，检查导入xml文件的合法性
//2.一旦xml文件合法，将xml文件名称加入到系统配置文件中
//3.一旦xml文件合法，将xml文件更新到配置窗口中
void ConfigExportImportMenu::InsertFileFromUSB()
{
    QProcess *procNew = 0,*p1 = 0;
    QString appName;
    QStringList argName;
    int i;

    //建立日志文件
    QFile filepp(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filepp.open(QIODevice::WriteOnly | QIODevice::Append);
    _TextStream.setDevice(&filepp);/*打开日志IO文件*/

    //打开U盘节点
    _usbFd = ::open(USB0_DEVICE_NODE, O_RDWR);
    if(_usbFd < 0)
    {
        qdebug("USB disk not found !!!\n");
    }

    //挂载u盘
    procNew = new QProcess(this);
    appName = "mount";

    argName.append("-t");
    argName.append("vfat");
    argName.append(USB0_DEVICE_NODE);
    argName.append(USB0_PATH_NAME);
    procNew->start(appName,argName);//异步触发方式
    procNew->waitForStarted();
    //需要添加挂载u盘失败情况处理代码


    //遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters << "*.xml";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    /*重新编写算法*/
    //加载本地xml数据
    QFile file_local;
    /*选择使用哪个文件*/

    file_local.setFileName(QString("%1usercheck.xml").arg(CONFIG_DIR));


    //加载本地校验文件
    if(!file_local.open(QFile::ReadOnly | QFile::Text))
    {
       add_Time_Logg();
       _TextStream << QString("%1 open failed!!!\r\n").arg(file_local.fileName());
    }
    if(_localXml.setContent(&file_local)<0)
    {
        add_Time_Logg();
        _TextStream << QString("%1 parser failed!!!\r\n").arg(file_local.fileName());
    }
    file_local.close();

    //加载导入xml文件数据
    QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(0)));
    if(!file_import.open(QFile::ReadOnly | QFile::Text))
    {
        add_Time_Logg();
        _TextStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
    }
    if(_importXml.setContent(&file_import)<0)
    {
        add_Time_Logg();
        _TextStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
    }
    file_import.close();
    QDomElement  importTag = _importXml.documentElement();
    QList<QDomElement> importTagList;
    importTagList.clear();
    bool checkbool = false;
    for(i=0;i<files.count();i++)
    {
        if(files.at(i).indexOf("User")>=0)
        {
            importTag = _importXml.documentElement();
            importTagList.clear();
            checkbool = checkXMLContent(importTagList,importTag);
        }
        //checkbool = true;
        //导入文件内容不合数据处理
        if(checkbool==false)
        {
            add_Time_Logg();
            _TextStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(files.at(i));
        }
        else
        {
            int nflag=0,k=0;
            for(k=0; k < d_ptr->configs.count(); k++)/*同名的文件不再导入*/
            {
                if(files.at(i)==d_ptr->configs.at(k).name)
                {
                    nflag = 1;
                    break;
                }
            }
            k = 0;

            if(nflag==0)
            {
                //申请拷贝进程
                p1 = new QProcess(this);
                appName = "cp";
                argName.clear();
                argName.append( QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(i)));
                argName.append(QString("%1").arg(CONFIG_DIR));
                p1->start(appName,argName);//异步触发方式
                p1->waitForStarted();
                //信息加入日志
                 add_Time_Logg();
                _TextStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(files.at(i));
                //装载导入文件  导入配置未完成
                d_ptr->loadConfigs();
                ConfigManager::UserDefineConfigInfo  userDefine;
                userDefine.fileName = files.at(i);
                userDefine.name     = files.at(i);

                d_ptr->configs.append(userDefine);
                d_ptr->updateConfigList();
                configManager.saveUserConfigInfo(d_ptr->configs);

                if(d_ptr->configList->count() >= CONFIG_MAX_NUM)
                {
                    d_ptr->importBtn->setEnabled(false);
                }
            }

            nflag = 0;
        }
        filepp.flush();
    }

    if(procNew!=0) //解挂u盘
    {
        delete procNew;
        appName = "umount";
        argName.clear();
        argName.append("-t");
        argName.append("vfat");
        argName.append(USB0_DEVICE_NODE);
        argName.append(USB0_PATH_NAME);
        procNew = new QProcess(this);
        procNew->start(appName,argName);//异步方式
        procNew->waitForStarted();//异步触发方式
        delete procNew;
    }
    if(p1!=0)//解除拷贝进程
    {
        delete p1;
    }
    ::close(_usbFd);
    filepp.close();
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


bool ConfigExportImportMenu::compareTagAttribute(QDomElement importtag,QDomElement localtag)
{
    #define PARAMETER_PREFIX_NUM0   (4)  /*参数前缀的长度*/
    #define PARAMETER_PREFIX_NUM1   (2)  /*参数前缀的长度*/
    #define PARAMETER_PREFIX0       ("Enable")  /*参数前缀*/
    #define PARAMETER_PREFIX1       ("Min")  /*参数前缀*/
    float numArrary[]={1.0,2.0};
    QString attrString[]={"Enable","Prio"};
    QDomNamedNodeMap localDomNameNodeMap = localtag.attributes();

    if(localDomNameNodeMap.count()==PARAMETER_PREFIX_NUM0)/*导入标签无属性值*/
    {
        if(localDomNameNodeMap.namedItem("cAttr0").nodeValue().toFloat()==0)/*数字属性 需要比较上下限*/
        {
            if(importtag.text().toFloat()<localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat()||\
               importtag.text().toFloat()>localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat())
            {
                //信息加入日志
                 add_Time_Logg();
                _TextStream << QString("Inserted xml files NodeName(%1)`s number value parser failed !!!\r\n").arg(localtag.nodeName());
                float fl = localDomNameNodeMap.namedItem("cMin").nodeValue().toFloat();
                float floa = localDomNameNodeMap.namedItem("cMax").nodeValue().toFloat();
                _TextStream << QString("(%1 %2 %3)\r\n").arg(importtag.text().toFloat()).arg(fl).arg(floa);
                return false;
            }
        }
        else/*字符串属性 直接比较是否相等*/
        {
            if(importtag.nodeValue()!=localtag.nodeValue())
            {
                //信息加入日志
                 add_Time_Logg();
                _TextStream << QString("Inserted xml files NodeName(%1)`s string value parser failed !!!\r\n").arg(localtag.nodeName());
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
                if(importtag.attributes().namedItem(attrString[i]).nodeValue().toFloat()<0||\
                   importtag.attributes().namedItem(attrString[i]).nodeValue().toFloat()>numArrary[i])
                {
                    //信息加入日志
                     add_Time_Logg();
                    _TextStream << QString("Inserted xml files NodeName(%1)`s attribute1 parser failed !!!\r\n").arg(localtag.nodeName());
                    return false;
                }
            }
            else if(localDomNameNodeMap.item(i).nodeName()==PARAMETER_PREFIX1)/*Min、Max属性*/
            {
                if(importtag.text().toFloat()<localDomNameNodeMap.namedItem("Min").nodeValue().toFloat()||\
                   importtag.text().toFloat()>localDomNameNodeMap.namedItem("Max").nodeValue().toFloat())
                {
                    //信息加入日志
                     add_Time_Logg();
                    _TextStream << QString("Inserted xml files NodeName(%1)`s attribute2 parser failed !!!\r\n").arg(localtag.nodeName());
                    float fl = localDomNameNodeMap.namedItem("Min").nodeValue().toFloat();
                    float floa = localDomNameNodeMap.namedItem("Max").nodeValue().toFloat();
                    _TextStream << QString("(%1 %2 %3)\r\n").arg(importtag.text().toFloat()).arg(fl).arg(floa);
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

        ExportFileToUSB();
    }
    else if(btn == d_ptr->importBtn)
    {

         InsertFileFromUSB();
    }
    else
    {

    }
}

