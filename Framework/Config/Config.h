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
#include <typeinfo>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include <QHash>
#include <QDomElement>

class ConfigPrivate;
class Config
{
public:
    explicit Config(const QString &configPath);

    ~Config();

    /***************************************************************************
     * 功能： 读取数字值, 支持数据类型int, bool和float。
     * 参数：
     *      indexStr：字段在XML文件中的索引；
     *      value：返回模板定义的类型值。
     * 返回：
     *      如果数据类型不支持或者找不到对应的数值，函数调用失败。
     **************************************************************************/
    template<typename T>
    bool getNumValue(const QString &indexStr, T &value)  // NOLINT
    {
        // 当访问的缓冲区为空时，开始读取.xml文件
        QString strText = _getCacheValue(indexStr);
        if (strText.isNull())
        {
            // no found in cache
            if (_getValue(indexStr, strText))
            {
                _setCacheValue(indexStr, strText);
            }
            else
            {
                return false;
            }
        }

        // 访问共享缓冲区或.xml文件成功找到指定元素时，开始进行下列逻辑操作
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
            /* Invalid type */
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
        // 如果.xml文件指定位置读取值时失败，则退出该函数接口
        if (!getNumValue(indexStr, curValue))
        {
            return false;
        }

        if (value != curValue)
        {
            QString strValue = QVariant(value).toString();
            _setCacheValue(indexStr, strValue);
            return _setValue(indexStr, strValue);
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
    bool getStrValue(const QString &indexStr, QString &value);  // NOLINT

    // 设置字符串值
    bool setStrValue(const QString &indexStr, const QString &value);
    bool setStrValues(const QVector<QString> &paths,
                      const QVector<QString> &values);

    // 获取属性字串
    bool getStrAttr(const QString &indexStr, const QString &attr,
                    QString &value);  // NOLINT

    // 设置属性字串
    bool setStrAttr(const QString &indexStr, const QString &attr,
                    const QString &value);

    // 设置字段
    bool setNodeValue(const QString &indexStr, Config &config);  // NOLINT
    bool getNodeValue(const QString &indexStr, QDomElement &element);  // NOLINT

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
    bool getNumAttr(const QString &indexStr, const QString &attr, T &value)  // NOLINT
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
            QString strValue = QVariant(value).toString();
            return _setAttr(indexStr, attr, strValue);
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
    bool getCurrentNumValue(const QString &indexStr, T &value)  // NOLINT
    {
        int index = 0;
        if (!getNumAttr(indexStr, "CurrentOption", index))
        {
            return false;
        }

        return getNumValue(indexStr + QString("|Opt%1").arg(index), value);
    }

    // 读取以“CurrentOption”标识的字串配置
    bool getCurrentStrValue(const QString &indexStr, QString &value);  // NOLINT

    // 读取全部可选字段
    bool getOptionList(const QString &indexStr, QVector<QString> &options);  // NOLINT

    // request save file
    void requestSave(void);

    // 存储配置文件到磁盘中。
    void saveToDisk(void);

    // 加载指定文件的内容。
    bool load(const QString &configPath);

    /**
     * @brief periodlySaveToDisk contorl periodly save file to the flash
     * @param enable true to enable, false to disable
     */
    void periodlySaveToDisk(bool enable);

    /**
     * @brief setCurrentFilePath  设置当前的配置文件路径
     * @param path  文件路径
     */
    void setCurrentFilePath(const QString &path);

    // 重新加载文件的内容。
    void reload();

    // 获取保存状态
    bool getSaveStatus();

    // get current file name
    QString getFileName();

    // save the config to file
    bool saveToFile(const QString &filepath);

    // get the config by return a QVariant object
    QVariantMap getConfig(const QString &indexStr);

    // set the config with the config object
    void setConfig(const QString &indexStr, const QVariantMap &config);

    // get default file name
    QString getDefaultFileName(const QString &fileName);
private:
    QString _getBackupFileName(const QString &fileName);
    QString _getFactoryFileName(const QString &fileName);

    void _newBackupFile(const QString &fileName);
    void _deleteBackupFile(const QString &fileName);
    void _checkBKFile(const QString &fileName);
    void _restoreBackupFile(const QString &fileName);

    void _restoreOrigFile(const QString &configPath);

    /**
     * @brief _getCacheValue get value from the internal cache
     * @param indexStr the index string
     * @return  the value in cache or empty string if not found
     */
    QString _getCacheValue(const QString &indexStr);

    /**
     * @brief _setCacheValue cache value
     * @param indexStr the index string
     * @param value the cache value
     */
    void _setCacheValue(const QString &indexStr, const QString &value);

    /**
     * @brief _setAttr set the attribute
     * @param indexStr the index string
     * @param attr the attr name
     * @param valueStr the value string
     */
    bool _setAttr(const QString &indexStr, const QString &attr, const QString &valueStr);

    /**
     * @brief _setValue set the value string
     * @param indexStr the index string
     * @param valueStr the value string
     * @return true if setting successfully, otherwise, false
     */
    bool _setValue(const QString &indexStr, const QString &valueStr);

    /**
     * @brief _getValue set the value string
     * @param indexStr the index string
     * @param valueStr store the value string
     * @return true if getting successfully, otherwise, false
     */
    bool _getValue(const QString &indexStr, QString &valueStr);  // NOLINT

private:
    Q_DISABLE_COPY(Config)
    ConfigPrivate *const d_ptr;
};
