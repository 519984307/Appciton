/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/11
 **/

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMutexLocker>
#include <QDomAttr>
#include <QDomText>
#include "XmlParser.h"
#include <QDebug>

/*******************************************************************************
 * 功能： 更新数据到磁盘文件
 ******************************************************************************/
bool XmlParser::saveToFile()
{
    if (_xml.isNull())
    {
        return false;
    }

    QFile file(_fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        return false;
    }

    // Constructs a QTextStream（writer） that operates on device（file）
    QTextStream writer(&file);
    // 编码方式
    writer.setCodec("UTF-8");
    // 加锁
    QMutexLocker locker(&_lock);
    // 把_filename字符串保存为XML文件
    _xml.save(writer, 4, QDomNode::EncodingFromTextStream);
    // 关闭文件
    file.close();
    return true;
}

bool XmlParser::saveToFile(const QString &filename)
{
    if (_xml.isNull())
    {
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << Q_FUNC_INFO << "save to"
                 << filename <<"failed:" << file.errorString();
        return false;
    }

    QTextStream writer(&file);
    writer.setCodec("UTF-8");

    QMutexLocker locker(&_lock);
    _xml.save(writer, 4, QDomNode::EncodingFromTextStream);

    file.close();
    return true;
}

/*******************************************************************************
 * 功能： 打开XML文件。
 * 参数：
 *      fileName文件名。
 * 返回：
 *      true，成功；false，打开失败或XML文件解析失败。
 ******************************************************************************/
bool XmlParser::open(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return false;
    }

    QMutexLocker locker(&_lock);
    // 将filename包装成一个File格式的文件
    QFile file(fileName);
    // 以文本方式打开只读
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    // 初始化_xml对象
    _xml.clear();
    // 判断是否读取XML文件成功
    if (!_xml.setContent(&file))
    {
        qDebug() << Q_FUNC_INFO << "Parse config file failed!";

        file.close();
        return false;
    }
    // 更新XML文件路径
    _fileName = fileName;
    return true;
}

/*******************************************************************************
 * 功能： 重新加载文件的内容。
 * 返回：
 *      true，成功；false，打开失败或XML文件解析失败。
 ******************************************************************************/
bool XmlParser::reload(void)
{
    QMutexLocker locker(&_lock);
    QFile file(_fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }

    _xml.clear();
    //再次读取文件内容
    if (!_xml.setContent(&file))
    {
        file.close();
        return false;
    }

    return true;
}

/*******************************************************************************
* 功能： 添加一个结点
 * 参数：
 *      indexStr：父结点路径，格式：string1|string2|string3；
 *      tagName：结点名字
 *      value: value
 *      attrs: 属性字典
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::addNode(const QString &indexStr, const QString &tagName, const QString &value,
                        const QMap<QString, QString> &attrs)
{
    QMutexLocker rlocker(&_lock);

    QDomElement tag = _findElement(indexStr);

    if (tag.isNull())
    {
        return false;
    }
    rlocker.unlock();


    QDomElement newTag = _xml.createElement(tagName);

    QMap<QString, QString>::ConstIterator iter;
    for (iter = attrs.constBegin(); iter != attrs.constEnd(); iter++)
    {
        newTag.setAttribute(iter.key(), iter.value());
    }

    QMutexLocker wlocker(&_lock);

    if (tag.appendChild(newTag).isNull())
    {
        return false;
    }
    wlocker.unlock();
    return setValue(indexStr + "|" + tagName, value);
}

/*******************************************************************************
* 功能： 删除一个结点。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 * 返回：
 *      true，成功；false，标签或父标签不存在。
 ******************************************************************************/
bool XmlParser::removeNode(const QString &indexStr)
{
    QMutexLocker locker(&_lock);
    QDomElement tag = _findElement(indexStr);
    if (!tag.isNull() && !tag.parentNode().isNull())
    {
        QDomNode node = tag.parentNode().removeChild(tag);
        return !node.isNull();
    }
    return false;
}

/*******************************************************************************
* 功能： 检查结点是否存在。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 * 返回：
 *      true，存在；false，不存在。
 ******************************************************************************/
bool XmlParser::hasNode(const QString &indexStr)
{
    return !_findElement(indexStr).isNull();
}

/*******************************************************************************
* 功能： 获取子Element名字列表
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 * 返回：
 *      名字列表
 ******************************************************************************/
QStringList XmlParser::childElementNameList(const QString &indexStr)
{
    QStringList namelist;
    QDomElement tag = _findElement(indexStr);
    if (!tag.isNull())
    {
        QDomNodeList nodelist = tag.childNodes();
        for (int i = 0; i < nodelist.size(); i++)
        {
            if (nodelist.at(i).nodeType() == QDomNode::ElementNode)
            {
                namelist.append(nodelist.at(i).nodeName());
            }
        }
    }
    return namelist;
}

/*******************************************************************************
* 功能： 读取标签元素。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      value：返回读取的字符串。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
// 获取XML文件中的指定路径的值
bool XmlParser::getValue(const QString &indexStr, QString &value)
{
    QMutexLocker locker(&_lock);
    QDomElement tag = _findElement(indexStr);
    if (!tag.isNull())
    {
        value = tag.text();
        return true;
    }

    return false;
}

/*******************************************************************************
 * 功能： 设置标签元素。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      value：要设定的值。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::setValue(const QString &indexStr, const QString &value)
{
    QMutexLocker locker(&_lock);

    if (_lastSettingIndexStr != indexStr)
    {
        _lastSettingIndexStr = indexStr;
        _lastSettingElement = _findElement(indexStr);
    }
    if (!_lastSettingElement.isNull())
    {
        QDomText domText = _lastSettingElement.firstChild().toText();
        if (!domText.isNull())
        {
            domText.setData(value);
        }
        else
        {
            // 节点没有内容时，需要新增一个节点
            _lastSettingElement.appendChild(_xml.createTextNode(value));
        }

        return true;
    }

    return false;
}

/*******************************************************************************
 * 功能： 读取标签属性。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      attr：标签属性名称；
 *      value：返回读取的字符串。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::getAttr(const QString &indexStr, const QString &attr,
                        QString &value)
{
    QMutexLocker locker(&_lock);
    QDomElement tag = _findElement(indexStr);
    if (!tag.isNull())
    {
        if (tag.hasAttribute(attr))
        {
            value = tag.attribute(attr);
            return true;
        }
    }

    value = "";
    return false;
}

/*******************************************************************************
 * 功能： 设置标签属性。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      attr：标签属性名称；
 *      value：要设定的值。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::setAttr(const QString &indexStr, const QString &attr,
                        const QString &value)
{
    QMutexLocker locker(&_lock);
    QDomElement tag = _findElement(indexStr);
    if (!tag.isNull())
    {
        if (tag.hasAttribute(attr))
        {
            tag.setAttribute(attr, value);
            return true;
        }
    }

    return false;
}

/*******************************************************************************
 * 功能： 读取标签。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      value：返回读取的字符串。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::getNode(const QString &indexStr, QDomElement &element)
{
    QMutexLocker locker(&_lock);
    element = _findElement(indexStr);

    if (element.isNull())
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*******************************************************************************
 * 功能： 设置标签。
 * 参数：
 *      indexStr：标签元素所在的路径，格式：string1|string2|string3；
 *      value：要设定的值。
 * 返回：
 *      true，成功；false，标签元素不存在。
 ******************************************************************************/
bool XmlParser::setNode(const QString &indexStr, QDomElement &srctag)
{
    QMutexLocker locker(&_lock);
    QDomElement tag = _findElement(indexStr);

    return _copyNode(tag, srctag);
}

static QVariantMap parseDomElement(const QDomElement &ele)
{
    QVariantMap varMap;

    QDomNamedNodeMap attrNodes = ele.attributes();
    for (int i = 0; i < attrNodes.count(); i++)
    {
        QDomAttr attr = attrNodes.item(i).toAttr();
        varMap[QString("@%1").arg(attr.name())] = qVariantFromValue(attr.value());
    }

    if (ele.firstChild().isText())
    {
        varMap["@text"] = qVariantFromValue(ele.text());
    }
    else if (ele.firstChild().isElement())
    {
        QDomNodeList list = ele.childNodes();
        for (int i = 0; i < list.count(); i++)
        {
            QDomNode::NodeType type = list.at(i).nodeType();
            if (type == QDomNode::ElementNode)  // Element Node only
            {
                QDomElement childEle = list.at(i).toElement();
                QVariantMap childMap = parseDomElement(childEle);
                if (varMap.contains(childEle.tagName()))
                {
                    QVariantList l;
                    if (varMap[childEle.tagName()].type() != QVariant::List)
                    {
                        l.append(varMap[childEle.tagName()]);
                    }
                    else
                    {
                        l = varMap[childEle.tagName()].toList();
                    }

                    l.append(childMap);
                    varMap[childEle.tagName()] = l;
                }
                else
                {
                    varMap[childEle.tagName()] = childMap;
                }
            }
        }
    }
    return varMap;
}

/*******************************************************************************
 * 功能： get a QVariantMap object from the specific xml node
 * 参数：
 *      index：XML的元素名；
 * 返回：
 *      QVariantMap represent xml dom tree
 ******************************************************************************/
QVariantMap XmlParser::getConfig(const QString &indexStr)
{
    QMutexLocker locker(&_lock);
    QDomElement ele = _findElement(indexStr);
    QVariantMap map;
    if (!ele.isNull())
    {
        map = parseDomElement(ele);
    }
    return map;
}

static QDomElement createDomElement(const QString &tagName, const QVariantMap &map, QDomDocument &doc)  // NOLINT
{
    QDomElement ele = doc.createElement(tagName);

    QVariantMap::ConstIterator iter = map.constBegin();
    for (; iter != map.constEnd(); ++iter)
    {
        if (iter.key().at(0) == '@')
        {
            if (iter.key().midRef(1) == "text")
            {
                ele.appendChild(doc.createTextNode(iter.value().toString()));
            }
            else
            {
                ele.setAttribute(iter.key().midRef(1).toString(), iter.value().toString());
            }
        }
        else
        {
            QVariant var = iter.value();
            if (var.type() == QVariant::List)
            {
                QVariantList list = var.toList();
                QVariantList::ConstIterator listIter;
                for (listIter = list.constBegin(); listIter != list.constEnd(); ++listIter)
                {
                    ele.appendChild(createDomElement(iter.key(), (*listIter).toMap(), doc));
                }
            }
            else if (var.type() == QVariant::Map)
            {
                ele.appendChild(createDomElement(iter.key(), iter.value().toMap(), doc));
            }
        }
    }
    return ele;
}

/*******************************************************************************
 * 功能： Create a QVariantMap object from the specific xml node
 * 参数：
 *      index：XML的元素名；
 *      map: QVariantMap represent xml dom tree
 ******************************************************************************/
void XmlParser::setConfig(const QString &indexStr, const QVariantMap &map)
{
    if (indexStr.isEmpty())
    {
        return;
    }
    QMutexLocker locker(&_lock);

    QDomElement ele = _findElement(indexStr);
    QString tagName;
    QDomNode parent;
    // remove the element first
    if (!ele.isNull())
    {
        parent = ele.parentNode();
        parent.removeChild(ele);
    }

    int index = indexStr.lastIndexOf('|');
    QString parentIndex;
    if (index > 0)
    {
        parentIndex = indexStr.left(index);
        tagName = indexStr.right(indexStr.size() - index - 1);
        parent = _findElement(parentIndex);
    }
    else
    {
        tagName = indexStr.trimmed();
        parent = _xml.firstChildElement();
    }

    if (parent.isNull())
    {
        // the parent not exist
        return;
    }

    parent.appendChild(createDomElement(tagName, map, _xml));
}

void XmlParser::setCurrentFilePath(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }
   _fileName = path;
}

/*******************************************************************************
 * 功能： 根据标签元素路径查找对应的标签元素对象。
 * 参数：
 *      indexStr：返回XML的元素名；
 * 返回：
 *      QDomElement对象。
 ******************************************************************************/
QDomElement XmlParser::_findElement(const QString &indexStr)
{
    QDomElement tag;
    if (_xml.isNull())
    {
        return tag;
    }

    QStringList list = indexStr.split('|', QString::SkipEmptyParts);
    if (list.size() < 1)
    {
        return tag;
    }

    tag = _xml.firstChildElement();

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

/*******************************************************************************
 * 功能： 标签元素拷贝。
 * 参数：
 *      indexStr：返回XML的元素名；
 * 返回：
 *      QDomElement对象。
 ******************************************************************************/
bool XmlParser::_copyNode(QDomElement &dstTag, QDomElement &srcTag)
{
    QDomNodeList list1 = dstTag.childNodes();
    QDomNodeList list2 = srcTag.childNodes();
    if (list1.count() != list2.count())
    {
        return false;
    }

    int count = list1.count();
    QDomElement tag1, tag2;
    for (int i = 0; i < count; ++i)
    {
        tag1 = list1.item(i).toElement();
        tag2 = list2.item(i).toElement();

        bool tag1HasChild = tag1.hasChildNodes();
        bool tag2HasChile = tag2.hasChildNodes();
        if (tag1HasChild && tag2HasChile)
        {
            if (tag1.hasAttributes())
            {
                QDomNamedNodeMap map = tag2.attributes();
                for (int j = 0; j < map.count(); ++j)
                {
                    tag1.setAttribute(map.item(j).nodeName(), map.item(j).nodeValue());
                }
            }

            QDomText domText = tag1.firstChild().toText();
            if (!domText.isNull())
            {
                domText.setData(tag2.text());
            }

            _copyNode(tag1, tag2);
        }
        else if (tag1HasChild)
        {
            tag1.firstChild().toText().setData(QString());
        }
    }

    return true;
}
