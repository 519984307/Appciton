#include "ConfigExportImportMenu.h"
#include "ConfigMaintainMenuGrp.h"
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
//#include <io.h>
//#include<iostream>
#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

#define USB0_DEVICE_NODE   ("/dev/sdb0")
#define USB0_PATH_NAME     ("/mnt/usb0")
#define CONFIG_DIR         ("/usr/local/nPM/etc/")

class ConfigExportImportMenuPrivate
{
public:
    ConfigExportImportMenuPrivate()
        :configList(NULL), exportBtn(NULL), importBtn(NULL),
        lastSelectItem(NULL)
    {}

    void loadConfigs();
    void updateConfigList();

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
    QProcess *p0 = 0;
    QString app;
    QStringList arg;

    int ti = timeDate.time();

    //打开U盘节点
    _usbfd = ::open(USB0_DEVICE_NODE, O_RDWR);
    if(_usbfd < 0)
    {
        qdebug("USB disk not found !!!\n");
    }
    //获得即将导出字符串的文件路径名称
    int count=0,i=0;
    QString *FileName;
    systemConfig.getNumAttr("ConfigManager|UserDefine","count",count);
    for(i=0;i<count;i++)
    {
        FileName = new QString("");
        systemConfig.getStrValue(QString("ConfigManager|UserDefine|Config%1").arg(i+1),*FileName);
        _ExportFileName.append(*FileName);
    }
    //开始导出数据
    if(count==0)
    {
        qdebug("The ExportFile not found !!!\n");
    }
    else
    {
        //挂载u盘
        p0 = new QProcess(this);
        app = "mount";

        arg.append("-t");
        arg.append("vfat");
        arg.append(USB0_DEVICE_NODE);
        arg.append(USB0_PATH_NAME);
        p0->start(app,arg);

        //需要添加挂载u盘失败情况处理代码

        //导出数据(.xml格式)
        QDomDocument xml;
        QString filename,filename1;
        for(i=0;i<count;i++)
        {
            filename = QString("%1%2").arg(CONFIG_DIR).arg(_ExportFileName.at(i));
            QFile file(filename);
            if(xml.setContent(&file)==false)
            {
                file.close();
                qdebug("Read ExportFile_%d failed!\n",i+1);
                break;

            }

            filename1 = QString("%1/pation%2").arg(USB0_PATH_NAME).arg(_ExportFileName.at(i));
            QFile file1(filename1);
            if(!file1.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qdebug("save failed : %s", qPrintable(file1.errorString()));
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

    if(p0!=0) //解挂u盘
    {
        app = "umount";
        arg.append("-t");
        arg.append("vfat");
        arg.append(USB0_DEVICE_NODE);
        arg.append(USB0_PATH_NAME);
        p0->start(app,arg);
        delete p0;

    }
    ::close(_usbfd);
    ::sleep(5);
    qdebug("Export File completed!\n",i+1);
    qDebug()<<"Export File used time ="<<timeDate.time()-ti<<"s"<<endl;
}
//1.xml文件导入cpu缓存中，检查导入xml文件的合法性
//2.一旦xml文件合法，将xml文件名称加入到系统配置文件中
//3.一旦xml文件合法，将xml文件更新到配置窗口中
void ConfigExportImportMenu::InsertFileFromUSB()
{
    QProcess *p0 = 0;
    QString app;
    QStringList arg;
    int i;

    //建立日志文件
    QFile filepp(QString("%1%2").arg(CONFIG_DIR).arg("logg.txt"));
    filepp.open(QIODevice::WriteOnly | QIODevice::Append);
    _TextStream.setDevice(&filepp);/*打开日志IO文件*/

    //打开U盘节点
    _usbfd = ::open(USB0_DEVICE_NODE, O_RDWR);
    if(_usbfd < 0)
    {
        qdebug("USB disk not found !!!\n");
    }

    //挂载u盘
    p0 = new QProcess(this);
    app = "mount";

    arg.append("-t");
    arg.append("vfat");
    arg.append(USB0_DEVICE_NODE);
    arg.append(USB0_PATH_NAME);
    p0->start(app,arg);//异步触发方式
    p0->waitForStarted();
    //需要添加挂载u盘失败情况处理代码


    //遍历U盘USB0目录下内所有xml格式文件
    QDir dir(QString("%1%2").arg(USB0_PATH_NAME).arg("/"));
    QStringList nameFilters;
    nameFilters << "*.xml";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    bool checkbool = false;
    for(i=0;i<files.count();i++)
    {
        if(files.at(i).indexOf("user")>=0)
        {
            checkbool = CheckXMLContent(files,i,USER_CONFIG_XML);
        }
        else if(files.at(i).indexOf("system")>=0)
        {
            checkbool = CheckXMLContent(files,i,SYSTEM_CONFIG_XML);
        }
        else if(files.at(i).indexOf("machine")>=0)
        {
            checkbool = CheckXMLContent(files,i,MACHINE_CONFIG_XML);
        }
        else
        {
            checkbool = false;
        }
        //导入文件内容不合数据处理
        if(checkbool==false)
        {
            add_Time_Logg();
            _TextStream << QString("Inserted xml files(%1) parser failed !!!\r\n").arg(files.at(i));
        }
        else
        {
             add_Time_Logg();
            _TextStream << QString("Inserted xml files(%1) parser succeed ^O^\r\n").arg(files.at(i));
        }
        filepp.flush();
    }

    if(p0!=0) //解挂u盘
    {
        app = "umount";
        arg.append("-t");
        arg.append("vfat");
        arg.append(USB0_DEVICE_NODE);
        arg.append(USB0_PATH_NAME);
        p0->start(app,arg);//异步方式，待改进
        p0->waitForStarted();//异步触发方式
        delete p0;
    }

    filepp.close();
}

//检查内容是否有效。 有效返回true；无效返回false；
//下一版本接口可以改为递归方式查询，注意递归结束条件的有效性
bool ConfigExportImportMenu::CheckXMLContent(QStringList files,int index,int type)
{

    QDomDocument         importxml,localxml;
    QDomElement          importtag,localtag;//第一层标签
    QDomElement          importtagnext1,localtagnext1;//第二层标签
    QDomElement          importtagnext2,localtagnext2;//第三层标签
    QDomElement          importtagnext3,localtagnext3;//第四层标签
    QDomElement          importtagnext4,localtagnext4;//第五层标签
    QDomNodeList         importlist,locallist;//第一层标签链表
    QDomNodeList         importlistnext1,locallistnext1;//第二层标签链表
    QDomNodeList         importlistnext2,locallistnext2;//第三层标签链表
    QDomNodeList         importlistnext3,locallistnext3;//第四层标签链表
    QDomNodeList         importlistnext4,locallistnext4;//第五层标签链表
    //QDomNamedNodeMap     importmap,localmap;
    int i,j,z,k,m;

     //加载本地xml数据
     QFile file_local;
     if(type==SYSTEM_CONFIG_XML)
     {
         file_local.setFileName(QString("%1systemcheck.xml").arg(CONFIG_DIR));
     }
     else if(type==MACHINE_CONFIG_XML)
     {
         file_local.setFileName(QString("%1machinecheck.xml").arg(CONFIG_DIR));
     }
     else
     {
         file_local.setFileName(QString("%1usercheck.xml").arg(CONFIG_DIR));
     }

     //加载本地校验文件
     if(!file_local.open(QFile::ReadOnly | QFile::Text))
     {
        add_Time_Logg();
        _TextStream << QString("%1 open failed!!!\r\n").arg(file_local.fileName());
     }
     if(localxml.setContent(&file_local)<0)
     {
         add_Time_Logg();
         _TextStream << QString("%1 parser failed!!!\r\n").arg(file_local.fileName());
     }
     file_local.close();

     //加载导入xml文件数据
     QFile file_import(QString("%1/%2").arg(USB0_PATH_NAME).arg(files.at(index)));
     if(!file_import.open(QFile::ReadOnly | QFile::Text))
     {
         add_Time_Logg();
         _TextStream << QString("%1 open failed!!!\r\n").arg(file_import.fileName());
     }
     if(importxml.setContent(&file_import)<0)
     {
         add_Time_Logg();
         _TextStream << QString("%1 parser failed!!!\r\n").arg(file_import.fileName());
     }

     //第一层标签  此时的localtag为Supervision
     localtag = localxml.firstChildElement();
     importtag = importxml.firstChildElement();
     if(0!=localtag.nodeName().compare(importtag.nodeName()))//元素名称不匹配
     {
         add_Time_Logg();
         _TextStream << QString("%1:%2`s name parser failed!!!\r\n").arg(files.at(index)).arg(importtag.nodeName());
         return false;//解析失败
     }
     else//比较元素的属性名称、属性值
     {
        if(false==CompareTagAttribute(importtag,localtag))
        {
            add_Time_Logg();
            _TextStream << QString("%1:%2`s attribute parser failed!!!\r\n").arg(files.at(index)).arg(importtag.nodeName());
            return false;//解析失败
        }
     }

     //第二层标签
     locallist = localtag.childNodes();/*获取第二层标签链表*/
     importlist = importtag.childNodes();
     if(locallist.count()!=importlist.count())/*对数量中的加不加注释要求很严格*/
     {
         add_Time_Logg();
         _TextStream << QString("%1:%2`s size parser failed!!!\r\n").arg(files.at(index)).arg(importtag.nodeName());
         return false;//解析失败
     }

     //第二层标签 比较每个节点下的元素名称；最外层元素名称循环  此时locallist的节点名称：General BasicDefib ...
     for(i=0;i<locallist.count();i++)
     {
         if(locallist.at(i).nodeName().compare(importlist.at(i).nodeName()))
         {
             add_Time_Logg();
             _TextStream << QString("%1:%2`s name parser failed!!!\r\n").arg(files.at(index)).arg(importlist.at(i).nodeName());
             return false;//解析失败
         }
     }


     //第二层标签   比较第二层标签属性名称、属性值；
     for(i=0;i<locallist.count();i++)//第二层标签循环  此时locallist的节点名称：General BasicDefib ...
     {
         if(false==CompareTagAttribute(importtag.firstChildElement(importlist.item(i).nodeName()),localtag.firstChildElement(locallist.item(i).nodeName())))
         {
             qdebug("%s parser6 failed !!!\n",files.at(index));
             //return false;//解析失败
         }
     }

     //比较每个节点下的属性名称、属性值；
     for(i=0;i<locallist.count();i++)//此时locallist的节点名称：General BasicDefib AlarmSource...
     {
        //此时locallistnext1的节点名称：SuperPassword Password ...
        locallistnext1 = localtag.firstChildElement(locallist.item(i).nodeName()).childNodes();//获取
        importlistnext1 = importtag.firstChildElement(importlist.item(i).nodeName()).childNodes();//获取下一层链表节点
        for(j=0;j<locallistnext1.count();j++)//此时locallistnext1的节点名称：SuperPassword Password Adult...
        {
            localtagnext1 = localtag.firstChildElement(locallist.item(i).nodeName()) ;//此时localtagnext1的元素名称：General BasicDefib ...
            importtagnext1 = importtag.firstChildElement(importlist.item(i).nodeName()) ;//更新第二层标签
           qDebug()<< localtagnext1.nodeName() <<endl;
            if(false==CompareTagAttribute(importtagnext1 ,localtagnext1))//解析SuperPassword Adult下的属性匹配  比较属性匹配
            {
                qdebug("%s parser7 failed !!!\n",files.at(index));
                return false;//解析失败
            }
            if(locallistnext1.at(j).nodeName().compare(importlistnext1.at(j).nodeName()))//比较节点名称
            {
                qdebug("%s parser8 failed !!!\n",files.at(index));
                return false;//解析失败
            }
            if(locallistnext1.at(j).nodeValue()!=importlistnext1.at(j).nodeValue())//比较元素值
            {
                qdebug("%s parser9 failed !!!\n",files.at(index));
                return false;//解析失败
            }

            //此时locallistnext2的节点名称：HR_PR RR_BR SPO2 NIBP_SYS ...
            locallistnext2 = localtagnext1.firstChildElement(locallistnext1.item(j).nodeName()).childNodes();//获取
            importlistnext2 = importtagnext1.firstChildElement(importlistnext1.item(j).nodeName()).childNodes();//获取下一层链表节点
            qDebug()<< locallistnext1.item(j).nodeName()<<endl;
            qDebug()<< locallistnext2.item(0).nodeName()<<endl;
            int ghjy = locallistnext2.count();int tyh = importlistnext2.count();
            for(z=0;z<locallistnext2.count();z++)
            {

                localtagnext2 = localtagnext1.firstChildElement(locallistnext1.item(j).nodeName()) ;//此时localtagnext2的元素名称：HR_PR RR_BR SPO2 NIBP_SYS ...
                importtagnext2 = importtagnext1.firstChildElement(importlistnext1.item(j).nodeName()) ;

                if(false==CompareTagAttribute(importtagnext2 ,localtagnext2))//解析HR_PR RR_BR SPO2 NIBP_SYS ...下的属性匹配  比较属性匹配
                {
                    qdebug("%s parser7 failed !!!\n",files.at(index));
                    return false;//解析失败
                }
                if(locallistnext2.at(z).nodeName().compare(importlistnext2.at(z).nodeName()))//比较节点名称
                {
                    qdebug("%s parser8 failed !!!\n",files.at(index));
                    return false;//解析失败
                }
                if(locallistnext2.at(z).nodeValue()!=importlistnext2.at(z).nodeValue())//比较元素值
                {
                    qdebug("%s parser9 failed !!!\n",files.at(index));
                    return false;//解析失败
                }

                //此时locallistnext3的节点名称：bpm rpm  percent Sensitivity...
                locallistnext3 = localtagnext2.firstChildElement(locallistnext2.item(z).nodeName()).childNodes();//获取
                importlistnext3 = importtagnext2.firstChildElement(importlistnext2.item(z).nodeName()).childNodes();//获取下一层链表节点
                for(k=0;k<locallistnext3.count();k++)
                {
                    localtagnext3 = localtagnext2.firstChildElement(locallistnext2.item(z).nodeName()) ;//此时localtagnext3的元素名称：bpm rpm  percent Sensitivity......
                    importtagnext3 = importtagnext2.firstChildElement(importlistnext2.item(z).nodeName()) ;//
                    if(false==CompareTagAttribute(importtagnext3 ,localtagnext3))//解析bpm rpm  percent Sensitivity...下的属性匹配  比较属性匹配
                    {
                        qdebug("%s parser7 failed !!!\n",files.at(index));
                        return false;//解析失败
                    }
                    if(locallistnext3.at(k).nodeName().compare(importlistnext3.at(k).nodeName()))//比较节点名称
                    {
                        qdebug("%s parser8 failed !!!\n",files.at(index));
                        return false;//解析失败
                    }
                    if(locallistnext3.at(k).nodeValue()!=importlistnext3.at(k).nodeValue())//比较元素值
                    {
                        qdebug("%s parser9 failed !!!\n",files.at(index));
                        return false;//解析失败
                    }

                    //此时locallistnext4的节点名称：Low High Step Scale...
                    locallistnext4 = localtagnext3.firstChildElement(locallistnext3.item(k).nodeName()).childNodes();//获取
                    importlistnext4 = importtagnext3.firstChildElement(importlistnext3.item(k).nodeName()).childNodes();//获取下一层链表节点
                    for(m=0;m<locallistnext4.count();m++)
                    {
                        localtagnext4 = localtagnext3.firstChildElement(locallistnext3.item(k).nodeName()) ;//此时locallistnext4的节点名称：Low High Step Scale...
                        importtagnext4 = importtagnext3.firstChildElement(importlistnext3.item(k).nodeName()) ;//
                        qDebug()<< localtagnext4.nodeName()<<endl;
                        qDebug()<< locallistnext4.at(m).nodeName()<<endl;
                        if(false==CompareTagAttribute(importtagnext4 ,localtagnext4))//解析Low High Step Scale...下的属性匹配  比较属性匹配
                        {
                            qdebug("%s parser7 failed !!!\n",files.at(index));
                            return false;//解析失败
                        }
                        if(locallistnext4.at(m).nodeName().compare(importlistnext4.at(m).nodeName()))//比较节点名称
                        {
                            qdebug("%s parser8 failed !!!\n",files.at(index));
                            return false;//解析失败
                        }
                        if(locallistnext4.at(m).nodeValue()!=importlistnext4.at(m).nodeValue())//比较元素值
                        {
                            qdebug("%s parser9 failed !!!\n",files.at(index));
                            return false;//解析失败
                        }
                    }
                }
            }

        }

      }
}

bool ConfigExportImportMenu::CompareTagAttribute(QDomElement importtag,QDomElement localtag)
{
    int i;
    QDomNamedNodeMap     importmap,localmap;
    if(localtag.hasAttributes()==true)
    {
        if(importtag.hasAttributes()==true)
        {
             localmap = localtag.attributes();
             importmap = importtag.attributes();
             if(localmap.count()==importmap.count())
             {
                 for(i=0;i<localmap.count();i++)
                 {
                     if(0==localmap.item(i).nodeName().compare(importmap.item(i).nodeName()))
                     {
                         if(localmap.item(i).nodeValue()==importmap.item(i).nodeValue())
                         {
                             //属性名称、属性值匹配
                             //待加入属性值的有效性检查。。。
                         }
                         else
                         {
                             qdebug("parser8 failed !!!\n");
                             return false;//解析失败
                         }
                     }
                     else
                     {
                        qdebug("parser7 failed !!!\n");
                         return false;//解析失败
                     }
                 }
             }
             else
             {
                 qdebug("parser9 failed !!!\n");
                 return false;//解析失败
             }
        }
        else
        {
            qdebug("parser5 failed !!!\n");
            return false;//解析失败
        }
    }
    else if(importtag.hasAttributes()==false)
    {
        //无属性
        return true;
    }
    else
    {
        qdebug("parser4 failed !!!\n");
        //return false;//解析失败
    }
    return true;
}

void ConfigExportImportMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());
    if(btn == d_ptr->exportBtn)
    {
        qdebug("Unknown signal sender exportBtn !");
        ExportFileToUSB();
    }
    else if(btn == d_ptr->importBtn)
    {
         qdebug("Unknown signal sender importBtn !");
         InsertFileFromUSB();
    }
    else
    {
        qdebug("Unknown signal sender!");
    }
}

