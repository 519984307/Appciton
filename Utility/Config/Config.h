#pragma once
#include <QString>
#include <typeinfo>
#include <QVariant>
#include <QVector>
#include "Debug.h"
#include "XmlParser.h"
#include <QStringList>
#include <QHash>
#include <QReadWriteLock>

class Config
{
public:
    Config(const QString &configPath);

    /***************************************************************************
     * 功能： 读取数字值, 支持数据类型int, bool和float。
     * 参数：
     *      indexStr：字段在XML文件中的索引；
     *      value：返回模板定义的类型值。
     * 返回：
     *      如果数据类型不支持或者找不到对应的数值，函数调用失败。
     **************************************************************************/
    template<typename T>
    bool getNumValue(const QString &indexStr, T &value)
    {
        //访问共享缓冲区时进行加锁操作
        _cacheLock.lock();
        //访问共享缓冲区
        QString strText = _configCache.value(indexStr);
        //访问结束时进行解锁操作
        _cacheLock.unlock();
        //当访问的缓冲区为空时，开始读取.xml文件
        if(strText.isNull())
        {
            //no found in cache
            if (_xmlParser.getValue(indexStr, strText))
            {
                //add to cache
                _cacheLock.lock();
                //缓冲区数据倒入操作
                _configCache.insert(indexStr,strText);
                _cacheLock.unlock();
            }
            else
            {
                return false;
            }
        }

        //访问共享缓冲区或.xml文件成功找到指定元素时，开始进行下列逻辑操作
        bool ok = false;
        if (typeid(value) == typeid(int))
        {
            value = strText.toInt(&ok);
        }
        else if (typeid(value) == typeid(unsigned))
        {
            value = strText.toUInt(&ok);
        }
        else if (typeid(value) == typeid(bool))
        {
            value = QVariant(strText).toBool();
            ok = true;
        }
        else if (typeid(value) == typeid(float))
        {
            value = strText.toFloat(&ok);
        }
        else if (typeid(value) == typeid(double))
        {
            value = strText.toDouble(&ok);
        }
        else
        {
            debug("Wrong value type for '%s'", qPrintable(indexStr));
        }

        return ok;
    }

    /***************************************************************************
     * 功能： 设置数字值, 支持数据类型int, bool和float。
     * 参数：
     *      indexStr：字段在XML文件中的索引；
     *      value：返回模板定义的类型值。
     * 返回：
     *      如果数据类型不支持或者找不到对应的数值，函数调用失败。
     **************************************************************************/
    template<typename T>
    bool setNumValue(const QString &indexStr, T value)
    {
        T curValue = 0;
        //如果.xml文件指定位置读取值时失败，则退出该函数接口
        if (!getNumValue(indexStr, curValue))
        {
            return false;
        }

        if (value != curValue)
        {
            save();
            QString strValue = QVariant(value).toString();
            _cacheLock.lock();
            _configCache.insert(indexStr, strValue);
            _cacheLock.unlock();
            return _xmlParser.setValue(indexStr, strValue);
        }

        return true;
    }

    /***************************************************************************
     * 功能： 设置多项数字值, 支持数据类型int和float。
     * 参数：
     *      paths：保存路径的容器
     *      values：保存数据的容器
     * 返回：
     *      返回值：如果数据类型不支持或者找不到对应的数值，函数调用失败;
     *      如果当前数据值等于将要设置的数据值则什么也不做.
     **************************************************************************/
    template<typename T>
    bool setNumValues(const QVector<QString> &paths, const QVector<T> &values)
    {
        int size = paths.size();
        if (size != values.size())
        {
            return false;
        }

        for (int i = 0; i < size; i++)
        {
            if (!setNumValue(paths.at(i), values.at(i)))
            {
                return false;
            }
        }

        return true;
    }

    // 读取字符串值
    bool getStrValue(const QString &indexStr, QString &value);

    // 设置字符串值
    bool setStrValue(const QString &indexStr, const QString &value);
    bool setStrValues(const QVector<QString> &paths,
            const QVector<QString> &values);

    // 获取属性字串
    bool getStrAttr(const QString &indexStr, const QString &attr,
            QString &value);

    // 设置属性字串
    bool setStrAttr(const QString &indexStr, const QString &attr,
            const QString &value);

    // 设置字段
    bool setNodeValue(const QString &indexStr, Config &config);
    bool getNodeValue(const QString &indexStr, QDomElement &element);

    // 添加结点
    bool addNode(const QString &indexStr, const QString &tagName,
                 const QString &value = QString(), const QMap<QString, QString> &attrs = QMap<QString, QString>());
    // 删除结点
    bool removeNode(const QString &indexStr);
    // 检查结点是否存在
    bool exist(const QString &indexStr);

    //获取子配置项名字
    QStringList getChildNodeNameList(const QString &indexStr);

    /***************************************************************************
     * 功能： 获取属性字串
     * 参数：
     *      indexStr：字段索引路径
     *      attr：属性值
     *      value：返回的数据类型数值
     * 返回：
     *      成功读取返回true，否则返回false
     **************************************************************************/
    template<typename T>
    bool getNumAttr(const QString &indexStr, const QString &attr, T &value)
    {
        QString curText;
        if (!getStrAttr(indexStr, attr, curText))
        {
            return false;
        }

        bool ok = false;
        if (typeid(value) == typeid(int))
        {
            value = curText.toInt(&ok);
        }
        else
        {
            value = curText.toFloat(&ok);
        }

        return ok;
    }

    /***************************************************************************
     * 功能： 设置属性字串
     * 参数：
     *      indexStr：字段索引路径
     *      attr：属性值
     *      value：要设置的数据类型数值
     * 返回：
     *      成功读取返回true，否则返回false
     **************************************************************************/
    template<typename T>
    bool setNumAttr(const QString &indexStr, const QString &attr,
            const T &value)
    {
        T curValue = 0;
        if (!getNumAttr(indexStr, attr, curValue))
        {
            return false;
        }

        if (value != curValue)
        {
            save();
            QString strValue = QVariant(value).toString();
            return _xmlParser.setAttr(indexStr, attr, strValue);
        }

        return true;
    }

    /***************************************************************************
     * 功能： 存取以“CurrentOption”标识的配置
     * 参数：
     *      indexStr：字段索引路径
     *      value：返回的数据类型数值
     * 返回：
     *      成功读取返回true，否则返回false
     **************************************************************************/
    template<typename T>
    bool getCurrentNumValue(const QString &indexStr, T &value)
    {
        int index = 0;
        if (!getNumAttr(indexStr, "CurrentOption", index))
        {
            return false;
        }

        return getNumValue(indexStr + QString("|Opt%1").arg(index), value);
    }

    // 读取以“CurrentOption”标识的字串配置
    bool getCurrentStrValue(const QString &indexStr, QString &value);

    // 读取全部可选字段
    bool getOptionList(const QString &indexStr, QVector<QString> &options);

    // 存储文件。
    void save(void);

    // 存储配置文件到磁盘中。
    void saveToDisk(void);

    // 加载指定文件的内容。
    bool load(const QString &configPath);

    // 重新加载文件的内容。
    void reload(){_xmlParser.reload();}

    //获取保存状态
    bool getSaveStatus() {return _requestToSave;}

    //get current file name
    QString getFileName() {return _xmlParser.currentFileName();}

    //save the config to file
    bool saveToFile(const QString &filepath);

private:
    QString _getBackupFileName(const QString &fileName);
    QString _getFactoryFileName(const QString &fileName);

    void _newBackupFile(const QString &fileName);
    void _deleteBackupFile(const QString &fileName);
    void _checkBKFile(const QString &fileName);
    void _restoreBackupFile(const QString &fileName);

    void _restoreOrigFile(const QString &configPath);

private:
    XmlParser _xmlParser;
    QHash<QString, QString> _configCache;
    QMutex _cacheLock;
    bool _requestToSave;
    QMutex _requestToSaveMutex;
};
