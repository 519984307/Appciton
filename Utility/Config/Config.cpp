#include <QFile>
#include "Config.h"
#include "unistd.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"

/**************************************************************************************************
 * 功能： 构建原始文件的备份文件名
 * 参数：
 *      fileName：原始文件名
 *************************************************************************************************/
QString Config::_getBackupFileName(const QString &fileName)
{
    QString backupFileName;
    if (".XML" == fileName.right(4).toUpper())
    {
        backupFileName = fileName.left(fileName.size() - 4);
        backupFileName += "-bk.xml";
    }

    return backupFileName;
}

/**************************************************************************************************
 * 功能： 构建原始文件的备份文件名
 * 参数：
 *      fileName：原始文件名
 *************************************************************************************************/
QString Config::_getFactoryFileName(const QString &fileName)
{
    QString factoryFileName;
    if (".XML" == fileName.right(4).toUpper())
    {
        factoryFileName = fileName.left(fileName.size() - 4);
        factoryFileName += ".Original.xml";
    }

    return factoryFileName;
}

/**************************************************************************************************
 * 功能： 新建备份文件。
 * 参数：
 *      fileName：备份文件的原始文件名
 *************************************************************************************************/
void Config::_newBackupFile(const QString &fileName)
{
    QString backupFileName = _getBackupFileName(fileName);
    if (QFile::exists(backupFileName))
    {
        QFile::remove(backupFileName);
    }

   QFile::copy(fileName, backupFileName);
}

/**************************************************************************************************
 * 功能： 删除备份文件。
 * 参数：
 *      fileName：备份文件的原始文件名
 *************************************************************************************************/
void Config::_deleteBackupFile(const QString &fileName)
{
    QString backupFileName = _getBackupFileName(fileName);
    QFile::remove(backupFileName);
}

/**************************************************************************************************
 * 功能： 检查备份文件是否存在，如果存在则使用，这表明当前的文件可能时损坏的。
 * 参数：
 *      configPath：配置文件的名称。
 *************************************************************************************************/
void Config::_checkBKFile(const QString &fileName)
{
    QString backupFileName = _getBackupFileName(fileName);
    if (QFile::exists(backupFileName))
    {
        if (_xmlParser.open(backupFileName))
        {
            //merge backup file
            QFile::remove(fileName);
            QFile::copy(backupFileName, fileName);
            QFile::remove(backupFileName);
        }
    }
}

/**************************************************************************************************
 * 功能： 将备份文件恢复成正常文件。
 * 参数：
 *      fileName： 配置文件的名称。
 *************************************************************************************************/
void Config::_restoreBackupFile(const QString &fileName)
{
    QString backupFileName = _getBackupFileName(fileName);
    QFile::remove(fileName);
    QFile::copy(backupFileName, fileName);
}

/**************************************************************************************************
 * 功能： 恢复出厂的设置默认设置。
 * 参数：
 *      configPath：配置文件的名称。
 *************************************************************************************************/
void Config::_restoreOrigFile(const QString &configPath)
{
    QString factoryConfigPath = _getFactoryFileName(configPath);
    if (QFile::exists(factoryConfigPath))
    {
        QFile::remove(configPath);
        QFile::copy(factoryConfigPath, configPath);

        ErrorLogItem *item = new ErrorLogItem();
        item->setName("Load default config");
        QString str = factoryConfigPath;
        str += "\r\n";
        item->setLog(str);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
    }
}

/**************************************************************************************************
 * 功能： 读取以“CurrentOption”标识的字串配置
 * 参数：
 *      indexStr：字段索引路径；
 *      value：读取成功的字串。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::getCurrentStrValue(const QString &indexStr, QString &value)
{
    int index = 0;
    if (!getNumAttr(indexStr, "CurrentOption", index))
    {
        return false;
    }

    return getStrValue(indexStr + QString("|Opt%1").arg(index), value);
}

/**************************************************************************************************
 * 功能： 读取全部可选字段。
 * 参数：
 *      indexStr：字段索引路径；
 *      value：返回保存的字段
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::getOptionList(const QString &indexStr, QVector<QString> &options)
{
    int count = 0;
    if (!getNumAttr(indexStr, "OptionCount", count))
    {
        return false;
    }

    QString text;
    options.clear();
    for (int i = 0; i < count; i++)
    {
        if (!getStrValue(indexStr + QString("|Opt%1").arg(i), text))
        {
            return false;
        }

        options.append(text);
    }

    return true;
}

/**************************************************************************************************
 * 功能： 请求存储文件到磁盘。
 *************************************************************************************************/
void Config::save(void)
{
    QMutexLocker lock(&_requestToSaveMutex);
    _requestToSave = true;
}

/**************************************************************************************************
 * 功能： 存储配置文件到磁盘中。
 *************************************************************************************************/
void Config::saveToDisk(void)
{
    QMutexLocker lock(&_requestToSaveMutex);
    if (!_requestToSave)
    {
        return;
    }
    _requestToSave = false;
    lock.unlock();

    // 保存数据时先将文件备份。
    QString fileName = _xmlParser.currentFileName();
    _newBackupFile(fileName);
    if (!_xmlParser.saveToFile())
    {
        _restoreBackupFile(fileName);
        _xmlParser.reload();
    }
    _deleteBackupFile(fileName);
    sync();
}

/**************************************************************************************************
 * 功能：加载指定文件的内容。
 *************************************************************************************************/
bool Config::load(const QString &configPath)
{
    if (!QFile::exists(configPath))
    {
        return false;
    }

    // 先将目标文件替换。
    if (!QFile::remove(_xmlParser.currentFileName()))
    {
        return false;
    }

    if (!QFile::copy(configPath, _xmlParser.currentFileName()))
    {
        return false;
    }

    _requestToSave = false;
    sync();
    usleep(10);

    _cacheLock.lock();
    _configCache.clear();
    _cacheLock.unlock();
    return _xmlParser.reload();
}

//save the config to file
bool Config::saveToFile(const QString &filepath)
{
    if(_xmlParser.saveToFile(filepath))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
 * 功能： 读取字符串值。
 * 参数：
 *      indexStr：字段索引路径；
 *      value：返回的字符串。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::getStrValue(const QString &indexStr, QString &value)
{
    _cacheLock.lock();
    QString cacheValue = _configCache.value(indexStr);
    _cacheLock.unlock();
    if(cacheValue.isNull())
    {
        bool flag = _xmlParser.getValue(indexStr, value);
        if(flag)
        {
            _cacheLock.lock();
            _configCache.insert(indexStr, value);
            _cacheLock.unlock();
        }
        return flag;
    }
    else
    {
        value = cacheValue;
        return true;
    }
}

/**************************************************************************************************
 * 功能： 设置字符串值。
 * 参数：
 *      indexStr：字段索引路径；
 *      value：需要设置的字符串。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::setStrValue(const QString &indexStr, const QString &value)
{
    QString curText;
    if (!getStrValue(indexStr, curText))
    {
        return false;
    }

    if (value != curText)
    {
        save();
        _cacheLock.lock();
        _configCache.insert(indexStr, value);
        _cacheLock.unlock();
        return _xmlParser.setValue(indexStr, value);
    }

    return true;
}

/**********************************************************************************************
 * 功能： 读取标签字段。
 * 参数：
 *      indexStr：字段在XML文件中的索引；
 * 返回：
 *      标签tag。
 *********************************************************************************************/
bool Config::getNodeValue(const QString &indexStr, QDomElement &element)
{
    return _xmlParser.getNode(indexStr, element);
}

/**********************************************************************************************
 * 功能： 添加一个结点
 * 参数：
 *      indexStr：XML文件中的索引；
 *      tagName: 结点名
 *      value: 结点值
 *      attrs: 结点属性
 * 返回：
 *      创建是否成功
 *********************************************************************************************/
bool Config::addNode(const QString &indexStr, const QString &tagName, const QString &value, const QMap<QString, QString> &attrs)
{
    return _xmlParser.addNode(indexStr, tagName, value, attrs);
}

/**********************************************************************************************
 * 功能： 删除一个结点
 * 参数：
 *      indexStr：XML文件中的索引；
 * 返回：
 *      删除是否成功
 *********************************************************************************************/
bool Config::removeNode(const QString &indexStr)
{
    _cacheLock.lock();
    _configCache.clear();
    _cacheLock.unlock();
    return _xmlParser.removeNode(indexStr);
}

/**********************************************************************************************
 * 功能： 检查结点是否存在
 * 参数：
 *      indexStr：XML文件中的索引；
 * 返回：
 *      是否存在
 *********************************************************************************************/
bool Config::exist(const QString &indexStr)
{
    return _xmlParser.hasNode(indexStr);
}

/**********************************************************************************************
 * 功能： 获取子配置项名字列表
 * 参数：
 *      indexStr：XML文件中的索引；
 * 返回：
 *      名字列表， 空表示无子结点或父结点不存在
 *********************************************************************************************/
QStringList Config::getChildNodeNameList(const QString &indexStr)
{
    return _xmlParser.childElementNameList(indexStr);
}

/**********************************************************************************************
 * 功能： 设置标签字段。
 * 参数：
 *      indexStr：字段在XML文件中的索引；
 *      value：返回模板定义的类型值。
 * 返回：
 *      如果数据类型不支持或者找不到对应的数值，函数调用失败。
 *********************************************************************************************/
bool Config::setNodeValue(const QString &indexStr, Config &config)
{
    QDomElement element;
    if (!config.getNodeValue(indexStr, element))
    {
        return false;
    }

    save();
    _cacheLock.lock();
    _configCache.clear();
    _cacheLock.unlock();
    return _xmlParser.setNode(indexStr, element);
}

/**************************************************************************************************
 * 功能： 设置字符串值。
 * 参数：
 *      paths: 字段索引路径容器；
 *      values: 需要设置的字符串容器。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::setStrValues(const QVector<QString> &paths, const QVector<QString> &values)
{
    int size = paths.size();
    if (size != values.size())
    {
        return false;
    }

    for (int i = 0; i < size; i++)
    {
        if (!setStrValue(paths.at(i), values.at(i)))
        {
            return false;
        }
    }

    return true;
}

/**************************************************************************************************
 * 功能： 获取属性字串。
 * 参数：
 *      indexStr：字段索引路径；
 *      attr：属性值；
 *      value：要设置的字串。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::getStrAttr(const QString &indexStr, const QString &attr, QString &value)
{
    return _xmlParser.getAttr(indexStr, attr, value);
}

/**************************************************************************************************
 * 功能： 设置属性字串。
 * 参数：
 *      indexStr：字段索引路径；
 *      attr：属性值；
 *      value：要设置的字串。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Config::setStrAttr(const QString &indexStr, const QString &attr, const QString &value)
{
    QString curText;
    if (!getStrAttr(indexStr, attr, curText))
    {
        return false;
    }

    if (value != curText)
    {
        save();
        return _xmlParser.setAttr(indexStr, attr, value);
    }

    return true;
}

/**************************************************************************************************
 * 功能：构造函数。
 *************************************************************************************************/
Config::Config(const QString &configPath)
{
    _requestToSave = false;

    _checkBKFile(configPath);
    if (!_xmlParser.open(configPath))
    {
        _restoreOrigFile(configPath);
        _xmlParser.open(configPath);
    }
}
