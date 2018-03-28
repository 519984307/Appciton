#pragma once
#include <QString>
#include <QDomElement>
#include <QDomDocument>
#include <QMutex>
#include <QMap>

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

    bool addNode(const QString &indexStr, const QString &tagName, const QString &value = QString(), const QMap<QString, QString> &attrs = QMap<QString, QString>());
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
    bool getFirstValue(const QString &indexStr, QString &index, QString &value);
    bool getNextValue(QString &index, QString &value);

    const QString &currentFileName(void)
    {
        return _fileName;
    }

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
