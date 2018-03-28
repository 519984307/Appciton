#pragma once
#include "StorageManager.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "ECGSymbol.h"
#include "ECGDefine.h"
#include <QMutex>
#include <QByteArray>

class ECG12LeadStorageManagerPrivate;
class RawDataCollection : public StorageManager
{
public:
    static RawDataCollection &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RawDataCollection();
        }
        return *_selfObj;
    }
    static RawDataCollection *_selfObj;

    ~RawDataCollection();

    // 循环运行
    void run();

    //向缓冲区中写入数据
    void pushData(QString name, const unsigned char *data, int len);

private:
    RawDataCollection();

    void _closeFile(QString name);             //关闭文件
//    void closeFile(QString name);             //关闭文件
    bool _folderCreate(QString name);         //文件夹的创建
    bool _openTxt(QString name, QString folderPath);              //打开U盘文件
    void _saveData(void);                           //保存数据到U盘
    void _exportUdisk(QString name, const QByteArray &data);     //将数据写入TXT文件

    typedef QMap<QString, QByteArray> ProviderBuff;
    ProviderBuff _providerBuff;

    typedef QMap<QString, QByteArray> ProviderBuffBk;
    ProviderBuffBk _providerBuffBk;

    typedef QMap<QString, QFile*> ProviderFile;
    ProviderFile _providerFile;

    QMutex _mutex;

    int datanum;
};
#define rawDataCollection (RawDataCollection::construction())
#define deleteRawDataCollection() (delete RawDataCollection::_selfObj)
