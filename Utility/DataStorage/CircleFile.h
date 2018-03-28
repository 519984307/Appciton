#pragma once
#include <QFile>
#include <QList>
#include "DataStorageDefine.h"

// 变长数据索引条目
struct BlockEntryItem
{
    BlockEntryItem()
        : pos(0)
        , len(0)
        , checksum(0)
    {
    }

    int sum()
    {
        return (pos + len);
    }

    void updateCheckSum()
    {
        checksum = sum();
    }

    bool isValid()
    {
        return (sum() == checksum);
    }

    int pos;    // 文件偏移位置
    int len;    // 数据长度
    int checksum;
};

/***************************************************************************************************
 * 环回文件:
 * 与RingBuff实现类似，文件缓存大小保持在设定值以内。
 * 与RingBuff实现不同，当数据大小超过文件缓存最大值时，覆盖最老的数据(环回写入，RingBuff实现是丢弃最新的数据)。
 **************************************************************************************************/
class CircleFile
{
public:
    CircleFile(const QString &fileName, int addInfoSize = 0);
    ~CircleFile();

    // 写数据
    bool appendBlockData(const char *data, int len);

    // 读数据
    bool readBlockData(int index, char *data, int len);

    // 获取指定块的大小
    void readEntryData(int index, BlockEntryItem &entry);

    // 清除文件数据(缓存及保留空间)
    void clear();

    // 写附加信息。
    bool writeAdditionalData(const char *data, int len);

    // 读附加信息。
    int readAdditionalData(char *data, int len);

    // 获取数据项总数
    int getBlockNR(void);

    // 获取指定目录文件的项数
    int getBlockNR(QString &path);

    // 重新加载文件
    void reload(const QString &fileName, int addInfoSize = 0);

private:

    // 清除文件数据(缓存及保留空间)
    void _clear();

    // 更新文件中变长类型数据索引条目
    void _updateItems();

    // 写数据
    int _write(QFile &file, const char *data, int pos, int len);

    // 读数据
    int _read(QFile &file, char *data, int pos, int len);

private:
    unsigned _writeIndex;            // 写索引
    unsigned _additionalInfoSize;    // 文件附加信息大小
    QFile _dataFile;            // 文件句柄
    QFile _entryFile;            // 文件句柄
    QList<BlockEntryItem> _entries;
};
