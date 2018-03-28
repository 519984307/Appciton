#include <QString>
#include "Debug.h"
#include "CircleFile.h"

/**************************************************************************************************
 * 功能： 构造。
 * 参数：
 *      fileName: 文件名。
 *      limitSize: 文件大小限定值。
 *************************************************************************************************/
CircleFile::CircleFile(const QString &fileName, int addInfoSize) :
    _writeIndex(addInfoSize)
    , _additionalInfoSize(addInfoSize)
{
    if (fileName.isEmpty())
    {
        return;
    }

    _entries.clear();

    _dataFile.setFileName(fileName + FILE_DATA_SUFFIX);
    _entryFile.setFileName(fileName + FILE_ENTRY_SUFFIX);

    bool openFileError = false;
    if (!_entryFile.open(QFile::ReadWrite))
    {
        openFileError = true;
        debug("Fail to open or create file %s!", qPrintable(_entryFile.fileName()));
    }

    if (!_dataFile.open(QFile::ReadWrite))
    {
        openFileError = true;
        debug("Fail to open or create file %s!", qPrintable(_dataFile.fileName()));
    }

    if (openFileError)
    {
        debug("Fail to open or create file!");
        _clear();
        return;
    }

    // 新文件或文件数据错误
    if ((0 == _entryFile.size()) || (0 != (_entryFile.size() % sizeof(BlockEntryItem))))
    {
        debug("New data file or file error %s!", qPrintable(_entryFile.fileName()));
        _clear();
        return;
    }

    // 变长类型数据存储
    int totalItems = _entryFile.size() / sizeof(BlockEntryItem);
    if (0 == totalItems)
    {
        debug("New data file %s!", qPrintable(_entryFile.fileName()));
        _clear();
        return;
    }

    BlockEntryItem lastEntry;
    if (sizeof(BlockEntryItem) != _read(_entryFile, (char *)&lastEntry,
            _entryFile.size() - sizeof(BlockEntryItem),
            sizeof(BlockEntryItem)))
    { // 文件出错
        _clear();
        return;
    }

    // 最后一个entry有效性检查
    if (!lastEntry.isValid())
    { // 文件数据损坏
        _clear();
        return;
    }

    BlockEntryItem entry[1000];
    int len = 0;
    int count = 0;
    for (int i = 0; i < totalItems; i += 1000)
    {
        len = _read(_entryFile, (char *)&entry, i * sizeof(BlockEntryItem),
                1000 * sizeof(BlockEntryItem));

        if (0 != (len % sizeof(BlockEntryItem)))
        {
            clear();
            return;
        }

        count = len / sizeof(BlockEntryItem);
        for(int j = 0; j < count; ++j)
        {
            _entries.append(entry[j]);
        }
    }

    _writeIndex = _entries.at(totalItems - 1).pos + _entries.at(totalItems - 1).len;

    debug("%s:WIndex %d,additionSize %d",
          qPrintable(fileName), _writeIndex, _additionalInfoSize);
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
CircleFile::~CircleFile()
{
    if (_entryFile.isOpen())
    {
        _entryFile.close();
    }

    if (_dataFile.isOpen())
    {
        _dataFile.close();
    }

    _entries.clear();
}

/**************************************************************************************************
 * 功能： 清除文件数据(缓存及保留空间)
 *************************************************************************************************/
void CircleFile::clear()
{
    _clear();
    _entries.clear();
}

/**************************************************************************************************
 * 功能： 读取指定index项的数据到data中，返回实际读取数据的长度。
 * 参数：
 *      data: 存放读取数据的缓冲区。
 *      index:指定的项。
 *      len:  需读出数据的长度(或数据缓冲区长度)。
 * 返回：读取成功返回true，否则返回false。
 *************************************************************************************************/
bool CircleFile::readBlockData(int index, char *data, int len)
{
    if ((NULL == data) || (index >= _entries.count()) || (len <= 0) || (index < 0))
    {
        debug("Invalid arguments!");
        return false;
    }

    BlockEntryItem entry = _entries.at(index);

    if ((unsigned)entry.pos > _writeIndex)
    {
        debug("*************File Error!!!!!*************");
        clear();
        return false;
    }

    len = qMin(entry.len, len);

    // 偏移位置有效性检查
    if ((unsigned)(entry.pos + len) > _writeIndex)
    {
        len = _writeIndex - entry.pos;
    }

    if (!_entryFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_entryFile.fileName()));
        return false;
    }

    if (!_dataFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_dataFile.fileName()));
        return false;
    }

    if (len != _read(_dataFile, data, entry.pos, len))
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 功能： 从文件缓存写索引端添加len个字节数据。
 * 参数：
 *      data: 待写入数据的缓冲区。
 *      len:  待写入数据的长度。
 * 返回：true写入成功，false写入失败
 *************************************************************************************************/
bool CircleFile::appendBlockData(const char *data, int len)
{
    if ((NULL == data) || (len <= 0))
    {
        debug("Invalid arguments!");
        return false;
    }

    if (!_entryFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_entryFile.fileName()));
        return false;
    }

    if (!_dataFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_dataFile.fileName()));
        return false;
    }

    BlockEntryItem entry;
    entry.pos = _writeIndex;
    entry.len = len;
    entry.updateCheckSum();

    // 直接写入
    if (len != _write(_dataFile, data, _writeIndex, len))
    {
        debug("File write error!");
        return false;
    }

    if (sizeof(BlockEntryItem) !=
            _write(_entryFile, (char *)&entry, _entryFile.size(), sizeof(BlockEntryItem)))
    {
        debug("File write error!");
        return false;
    }

    _entries.append(entry);
    _writeIndex += len;

    return true;
}

/**************************************************************************************************
 * 功能： 读取指定index项的数据。
 * 参数：
 *      index:指定的项, 从0开始。
 *************************************************************************************************/
void CircleFile::readEntryData(int index, BlockEntryItem &entry)
{
    if (index >= _entries.count())
    {
        return;
    }

    entry = _entries.at(index);
}

/**************************************************************************************************
 * 写附加信息。
 *************************************************************************************************/
bool CircleFile::writeAdditionalData(const char *data, int len)
{
    if ((NULL == data) || (len <= 0))
    {
        debug("Invalid arguments!");
        return false;
    }

    if ((unsigned)len != _additionalInfoSize)
    {
        debug("Invalid len!");
        return false;
    }

    if (!_entryFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_entryFile.fileName()));
        return false;
    }

    if (!_dataFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_dataFile.fileName()));
        return false;
    }

    if (len != _write(_dataFile, data, 0, len))
    {
        debug("File write error");
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 读附加信息。
 *************************************************************************************************/
int CircleFile::readAdditionalData(char *data, int len)
{
    if ((NULL == data) || (len <= 0))
    {
        debug("Invalid arguments!");
        return 0;
    }

    if ((unsigned)len != _additionalInfoSize)
    {
        debug("Invalid len!");
        return 0;
    }

    if (!_entryFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_entryFile.fileName()));
        return 0;
    }

    if (!_dataFile.isOpen())
    {
        debug("File %s not opened!", qPrintable(_dataFile.fileName()));
        return 0;
    }

    return _read(_dataFile, data, 0, len);
}

/***************************************************************************************************
 * 获取数据项数
 **************************************************************************************************/
int CircleFile::getBlockNR()
{
    return _entries.count();
}

/**************************************************************************************************
 * 获取指定目录中的数据项的个数
 *************************************************************************************************/
int CircleFile::getBlockNR(QString &path)
{
    int number = 0;
    if (path.isEmpty())
    {
        return number;
    }

    QString fileStr = path + FILE_ENTRY_SUFFIX;
    QFile file(fileStr);
    if (!file.exists())
    {
        return number;
    }

    if (!file.open(QFile::ReadOnly))
    {
        return number;
    }

    int size = file.size();
    if (0 == size % sizeof(BlockEntryItem))
    {
        number = size / sizeof(BlockEntryItem);
    }
    else
    {
        BlockEntryItem entry[500];
        int len = 0;
        while ((len = file.read((char *)entry, sizeof(entry))) != 0)
        {
            len /= sizeof(BlockEntryItem);
            for (int i = 0; i < len; ++i)
            {
                if (entry[i].isValid())
                {
                    ++number;
                }
            }
        }
    }

    file.close();

    return number;
}

/***************************************************************************************************
 * 重新加载文件
 **************************************************************************************************/
void CircleFile::reload(const QString &fileName, int additionSize)
{
    _additionalInfoSize = additionSize;
    _entries.clear();

    if (_dataFile.isOpen())
    {
        _dataFile.close();
    }

    if (_entryFile.isOpen())
    {
        _entryFile.close();
    }

    _dataFile.setFileName(fileName + FILE_DATA_SUFFIX);
    _entryFile.setFileName(fileName + FILE_ENTRY_SUFFIX);

    if (!_dataFile.exists())
    {
        debug("file is not exists %s!", qPrintable(_dataFile.fileName()));
        return;
    }

    if (!_entryFile.exists())
    {
        debug("file is not exists %s!", qPrintable(_entryFile.fileName()));
        return;
    }

    if (!_entryFile.open(QFile::ReadOnly))
    {
        debug("Fail to open or create file %s!", qPrintable(_entryFile.fileName()));
        return;
    }

    if (!_dataFile.open(QFile::ReadOnly))
    {
        debug("Fail to open or create file %s!", qPrintable(_dataFile.fileName()));
        return;
    }

    if ((0 == _entryFile.size()) || (0 != (_entryFile.size() % sizeof(BlockEntryItem))))
    { // 新文件或文件数据错误
        debug("New data file or file error %s!", qPrintable(_entryFile.fileName()));
        _clear();
        return;
    }

    // 变长类型数据存储
    int totalItems = _entryFile.size() / sizeof(BlockEntryItem);
    if (0 == totalItems)
    {
        debug("New data file %s!", qPrintable(_entryFile.fileName()));
        _clear();
        return;
    }

    BlockEntryItem lastEntry;
    if (sizeof(BlockEntryItem) != _read(_entryFile, (char *)&lastEntry,
                                       _entryFile.size() - sizeof(BlockEntryItem),
                                       sizeof(BlockEntryItem)))
    { // 文件出错
        _clear();
        return;
    }

    // 最后一个entry有效性检查
    if (!lastEntry.isValid())
    { // 文件数据损坏
        _clear();
        return;
    }

    BlockEntryItem entry[1000];
    int len = 0;
    int count = 0;
    for (int i = 0; i < totalItems; i += 1000)
    {
        len = _read(_entryFile, (char *)&entry, i * sizeof(BlockEntryItem),
                    1000 * sizeof(BlockEntryItem));

        if (0 != (len % sizeof(BlockEntryItem)))
        {
            clear();
            return;
        }

        count = len / sizeof(BlockEntryItem);
        for(int j = 0; j < count; ++j)
        {
            _entries.append(entry[j]);
        }
    }

    _writeIndex = _entries.at(totalItems - 1).pos + _entries.at(totalItems - 1).len;


    debug("%s: WIndex %d,additionSize %d",
          qPrintable(fileName), _writeIndex, _additionalInfoSize);
}

/***************************************************************************************************
 * 清除文件内容
 **************************************************************************************************/
void CircleFile::_clear()
{
    _dataFile.resize(0);
    _entryFile.resize(0);
    _writeIndex = _additionalInfoSize;
}

/***************************************************************************************************
 * 更新文件中变长类型数据索引条目
 **************************************************************************************************/
void CircleFile::_updateItems()
{
    int count = _entries.size();
    if (0 == count)
    {
        return;
    }

    _entryFile.resize(0);
    BlockEntryItem items[count];

    for (int i = 0; i < count; ++i)
    {
        items[i] = _entries.at(i);
    }


    _write(_entryFile, (const char *)items, 0, sizeof(items));
}

/***************************************************************************************************
 * 写数据
 **************************************************************************************************/
int CircleFile::_write(QFile &file, const char *data, int pos, int len)
{
    if (!file.isOpen())
    {
        return 0;
    }

    if (!file.seek(pos))
    {
        return 0;
    }

    int writeLen = file.write(data, len);
    file.flush();

    return writeLen;

}

/***************************************************************************************************
 * 读数据
 **************************************************************************************************/
int CircleFile::_read(QFile &file, char *data, int pos, int len)
{
    if (!file.isOpen())
    {
        return 0;
    }

    if (!file.seek(pos))
    {
        return 0;
    }

    return file.read(data, len);
}
