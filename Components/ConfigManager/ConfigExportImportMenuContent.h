/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#pragma once
#include "ConfigExportImportMenuContent.h"
#include "MenuContent.h"
#include <QDomDocument>
class ConfigExportImportMenuContentPrivate;

class ConfigExportImportMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ConfigExportImportMenuContent();
    ~ConfigExportImportMenuContent();

    enum Import_XML_Type
    {
        SYSTEM_CONFIG_XML = 0,
        MACHINE_CONFIG_XML,
        USER_CONFIG_XML,
        XML_NR
    };
    /**
     * @brief eventFilter
     * @param obj
     * @param ev
     * @return
     */
    bool eventFilter(QObject *obj, QEvent *ev);
    /**
     * @brief exportFileToUSB export XML to usb
     * @return  true-export success   false- export fail
     */
    bool exportFileToUSB();
    /**
     * @brief insertFileFromUSB    import XML from usb
     * @return true-import success  false-import fail
     */
    bool insertFileFromUSB();
    /**
     * @brief tagFindElement  find tag elements
     * @param list     the element path
     * @return         true-the element path  false--0
     */
    QDomElement tagFindElement(const QStringList &list);//寻找本地的标签

protected:
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);
private slots:
    /**
     * @brief onExitList
     */
    void onExitList(bool);
    /**
     * @brief onConfigClick
     */
    void onConfigClick();
    /**
     * @brief onBtnClick
     */
    void onBtnClick();

private:
    /**
     * @brief checkXMLContent  check validity of the import XML
     * @param importTagList    import tag list
     * @param importTag        import tag
     * @return 返回：ture-valid;false-invalid；
     */
    bool checkXMLContent(QList<QDomElement> &importTagList, QDomElement &importTag);

    /**
     * @brief compareTagAttribute  compare  attribute and value of the both of all XML nodes
     * @param importtag   import tag
     * @param localtag    local check tag
     * @return ture-valid;false-invalid；
     */
    bool compareTagAttribute(QDomElement importtag, QDomElement localtag);//


    ConfigExportImportMenuContentPrivate *const d_ptr;
};
