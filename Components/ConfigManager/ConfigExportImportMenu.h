/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/6
 **/
#pragma once
#include "SubMenu.h"
#include <QScopedPointer>
#include <QList>
#include "IListWidget.h"

class ConfigExportImportMenuPrivate;
class ConfigExportImportMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief ConfigExportImportMenu 构造方法
     */
    ConfigExportImportMenu();
    ~ConfigExportImportMenu();

    enum Import_XML_Type
    {
        SYSTEM_CONFIG_XML = 0,
        MACHINE_CONFIG_XML,
        USER_CONFIG_XML,
        XML_NR
    };
    /**
     * @brief eventFilter 导入导出窗口事件过滤方法
     * @param obj  导入导出窗口对象
     * @param ev   事件
     * @return     返回 false
     */
    bool eventFilter(QObject *obj, QEvent *ev);
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
    QDomElement tagFindElement(const QStringList &list);   //  寻找本地的标签

protected:
    /**
     * @brief layoutExec 布局方法
     */
    virtual void layoutExec();
    /**
     * @brief readyShowb 数据装载方法
     */
    virtual void readyShow();

private slots:
    /**
     * @brief onExitList  退出窗口时的动作方法
     */
    void onExitList(bool);
    /**
     * @brief onConfigClick  选定文件动作方法
     */
    void onConfigClick();
    /**
     * @brief onBtnClick  按钮动作方法
     */
    void onBtnClick();

private:
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
    bool compareTagAttribute(QDomElement importtag, QDomElement localtag);  //

    QList<QString> _exportFileName;  //  导出的XML文件名称的链表

    QMutex _lock;  //  原子锁 用于共享内存读取写入的保护

    QList<QDomDocument> _insertxml;  //  导入的XML文件的链表

    QTextStream       _textStream;  //  日志接口

    QDomDocument      _localXml;  //  本地检查文件

    QDomDocument      _importXml;  //  导入检查文件

    QString           _failedImportXmlName;  //  首个失败的导入文件名称

    QString           _failedExportXmlName;  //  首个失败的导出文件名称

    QString           _sameImportXmlName;  //  首个同名的导入文件名称

    QString           _sameExportXmlName;  //  首个同名的导出文件名称

    int               _checkExportFileFlag;  //  检查导出文件标志位

    int               _checkImportFileFlag;  //  检查导入文件标志位

    int               _usbFd;  //  挂载U盘的设备节点

    int               _repeatFileChooseFlag;  //  重复文件选择状态位
    QScopedPointer<ConfigExportImportMenuPrivate> d_ptr;
};








