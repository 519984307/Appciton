#pragma once
#include "SubMenu.h"
#include <QScopedPointer>
#include <QList>

class ConfigExportImportMenuPrivate;
class ConfigExportImportMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigExportImportMenu();
    ~ConfigExportImportMenu();

    enum Import_XML_Type
    {
        SYSTEM_CONFIG_XML=0,
        MACHINE_CONFIG_XML,
        USER_CONFIG_XML,
        XML_NR
    };

    bool eventFilter(QObject *obj, QEvent *ev);

    int _usbfd;//挂载U盘的设备节点

    QList<QString> _ExportFileName;//导出的XML文件名称的链表

    QMutex _lock;//原子锁 用于共享内存读取写入的保护

    QList<QDomDocument> _Insertxml;//导入的XML文件的链表

    QTextStream       _TextStream;//日志接口

    void ExportFileToUSB();//导出文件到U盘接口

    void InsertFileFromUSB();//从U盘导入文件的接口

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onExitList(bool);
    void onConfigClick();
    void onBtnClick();

private:
    bool CheckXMLContent(QStringList files,int index,int type);//检查导入的xml文件是否有效 返回：ture-有效;false-无效；

    bool CompareTagAttribute(QDomElement importtag,QDomElement localtag);//检查每个元素节点下属性名称的匹配和属性值的有效性

    QScopedPointer<ConfigExportImportMenuPrivate> d_ptr;

};
#define   add_Time_Logg()   ( _TextStream << QString("The recording time is %1.%2.%3/%4.%5.%6\r\n").arg(timeDate.getDateYear()).arg(\
                                           timeDate.getDateMonth()).arg(timeDate.getDateDay()).arg(timeDate.getTimeHour()).arg(\
                                           timeDate.getTimeMinute()).arg(timeDate.getTimeSenonds()))







