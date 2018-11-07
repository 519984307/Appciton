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
#include "MenuContent.h"
#include <QDomDocument>


class FactoryImportExportMenuContentPrivate;

class FactoryImportExportMenuContent : public MenuContent
{
    Q_OBJECT
public:
    FactoryImportExportMenuContent();
    ~FactoryImportExportMenuContent();

    enum Import_XML_Type
    {
        SYSTEM_CONFIG_XML = 0,
        MACHINE_CONFIG_XML,
        USER_CONFIG_XML,
        XML_NR
    };
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
    QDomElement tagFindElement(const QStringList &list);

protected:
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief readyShow
     */
    virtual void readyShow();

    /* reimplment */
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief onBtnClick
     */
    void onBtnClick();
    /**
     * @brief updateBtnStatus
     */
    void updateBtnStatus();

    /**
     * @brief onTimeOut  定时器触发函数
     */
    void onTimeOut(void);

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
    bool compareTagAttribute(QDomElement importtag, QDomElement localtag);


    FactoryImportExportMenuContentPrivate *const d_ptr;
};
