#pragma once
#include "StorageManager.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "ECGSymbol.h"
#include "ECGDefine.h"
#include <QMutex>

struct BuffItem
{
    BuffItem():datatype(""), data(NULL), length(0){}
    BuffItem(QString type, unsigned char *data, int len):datatype(type), data(data), length(len){}
    QString datatype;
    unsigned char *data;
    int length;
};


// 数据文件。
struct FileData
{
    FileData()
    {
        currentItemList = 0;
        buff1 = 0;
        buff2 = 0;
        _provideList[currentItemList].clear();
        _provideList[!currentItemList].clear();
        _txtOpen = false;
    }

    ~FileData()
    {
        _provideList[currentItemList].clear();
        _provideList[!currentItemList].clear();
    }

    QString name;
    QList<BuffItem> _provideList[2];
    bool _txtOpen;                                 //文件是否打开的标志
    quint8 currentItemList;
    QString _fileName;                                 //txt文件名
    QString _folderPath;                            //文件夹路径
    QString _filePath;                             //文件路径
    QFile *_pfile;                                 //文件
    QTextStream *_textStream;
    int _data_1;
    int _data_2;
    int buff1,buff2;
    int _numData;
    QMutex _mutex;
};

class ECG12LeadStorageManagerPrivate;
class RawDataCollectionTxt : public StorageManager
{
public:
    static RawDataCollectionTxt &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RawDataCollectionTxt();
        }
        return *_selfObj;
    }
    static RawDataCollectionTxt *_selfObj;

    ~RawDataCollectionTxt();

    // 循环运行
    void run();

    //向缓冲区中写入数据
    void PushData(QString name, const unsigned char *data, int len);

private:
    RawDataCollectionTxt();


    void openFile(FileData* channel);              //打开U盘文件
    void closeFile(FileData* channel);             //关闭文件
    void _buffCreate(QString name);                //buff是否存在
    void _switchList(FileData* channel);           //buff双缓冲切换
    bool _folderCreate(FileData* channel);         //文件夹的创建
    bool _openTxt(FileData* channel);              //打开U盘文件
    void _saveData(void);                           //保存数据到U盘
    void exportUdisk(FileData* channel, unsigned char *data, int len);     //将数据写入TXT文件

    // 数据缓存映射。
    typedef QMap<QString, FileData*> ProviderMap;
    ProviderMap _provider;

};
#define rawDataCollectionTxt (RawDataCollectionTxt::construction())
#define deleteRawDataCollectionTxt() (delete RawDataCollectionTxt::_selfObj)
