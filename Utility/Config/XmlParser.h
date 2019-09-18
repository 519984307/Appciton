/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/11
 **/

#pragma once
#include <QString>
#include <QDomElement>
#include <QDomDocument>
#include <QMutex>
#include <QMap>
#include <QVariant>

///////////////////////////////////////////////////////////////////////////////
// XML文件读写器
///////////////////////////////////////////////////////////////////////////////
class XmlParser
{
public:
    XmlParser()
    {
    }

    bool saveToFile();
    bool saveToFile(const QString &filename);
    bool open(const QString &fileName);
    bool reload(void);

    bool addNode(const QString &indexStr, const QString &tagName, const QString &value = QString(),
                 const QMap<QString, QString> &attrs = QMap<QString, QString>());
    bool removeNode(const QString &indexStr);
    bool hasNode(const QString &indexStr);
    QStringList childElementNameList(const QString &indexStr);
    bool getValue(const QString &indexStr, QString &value);
    bool setValue(const QString &indexStr, const QString &value);
    bool getAttr(const QString &indexStr, const QString &attr, QString &value);
    bool setAttr(const QString &indexStr, const QString &attr,
                 const QString &value);
    bool getNode(const QString &indexStr, QDomElement &element);
    bool setNode(const QString &indexStr, QDomElement &tag);

    QVariantMap getConfig(const QString &indexStr);
    void setConfig(const QString &indexStr, const QVariantMap &map);


    const QString &currentFileName(void)
    {
        return _fileName;
    }

    /**
     * @brief setCurrentFilePath  设置当前的配置文件路径
     * @param path  文件路径
     */
    void setCurrentFilePath(QString &path);

private:
    QDomElement _findElement(const QString &indexStr);
    bool _copyNode(QDomElement &dstTag, QDomElement &srcTag);

private:
    QDomDocument _xml;
    QString _fileName;
    QMutex _lock;
    QDomElement _iterateElement;
    QDomElement _lastSettingElement;
    QString _lastSettingIndexStr;
};
