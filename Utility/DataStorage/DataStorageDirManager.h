/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/30
 **/


#pragma once
#include <QString>
#include "RescueDataDefine.h"
#include "DataStorageDefine.h"
#include <QObject>

struct FolderName
{
    unsigned sequenceNum;
    QString name;

    bool operator<(const struct FolderName &other) const
    {
        if (sequenceNum < other.sequenceNum)
        {
            return true;
        }
        else if (sequenceNum == other.sequenceNum)
        {
            return name < other.name;
        }

        return false;
    }

    void getNum()
    {
        QString tmpStr("nPM");
        int pos = name.indexOf(tmpStr, 0);
        if (-1 != pos)
        {
            sequenceNum = name.left(pos).toInt();
        }
        else
        {
            sequenceNum = 0;
        }
    }

    QString getDatetimeStr()
    {
        return name.right(14);
    }
};

//数据存储目录管理
class StorageManager;
class DataStorageDirManager : public QObject
{
    Q_OBJECT
public:
    static DataStorageDirManager &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new DataStorageDirManager();
        }

        return *_selfObj;
    }
    static DataStorageDirManager *_selfObj;
    ~DataStorageDirManager();

    // 获取营救事件时间列表
    void getRescueEvent(QStringList &timeList);

    // 获取指定的营救数据路径
    QString getRescueDataDir(int index, const QString &time);
    QString getRescueDataDir(int index);

    // 创建文件夹
    void createDir(bool createNew = false);
    void addStorage(StorageManager * storage);

    // 获取当前文件夹
    QString getCurFolder() const;

    // 获取FD文件名
    QString getFDFileName() const {return _fdFileName;}

    // 获取文件夹的个数
    int getDirNum() const;

    // 存储空间是否存满了
    bool isCurRescueFolderFull();

    // 增加当前数据的存储大小
    void addDataSize(unsigned size);

    // 删除所有数据
    void deleteAllData();
    void deleteData(QString &path);
    void deleteData(int index);

    // 整理上一个rescure incident存储目录中文件
    static bool cleanUpLastIncidentDir();

    // clean up incident directory
    static bool cleanUpIncidentDir(const QString &dir);
    // get the size of specific directory
    static quint64 dirSize(const QString &dir);

signals:
    /**
     * @brief changeDirPath 改变了保存文件夹路径
     */
    void changeDirPath();

private:
    DataStorageDirManager();
    int _deleteDir(const QString &path);

    // 删除较早的数据
    void _deleteOldData(void);

    // 创建FD文件名
    void _createFDFileName(QString &fileName, unsigned time, bool newRescue);

private:
    unsigned _curDataSize;           // 当前的存储数据大小
    unsigned _previousDataSize;     // the size of previous rescue data
    QString _curFolder;              // 当前目录
    QString _fdFileName;             // fulldisclosure文件名
    QList<struct FolderName> _folderNameList;// 存储目录下文件夹名称及对应的大小

    QList<StorageManager *> _storageList;

    bool _createNew;
};
#define dataStorageDirManager (DataStorageDirManager::construction())
#define deleteDataStorageDirManager() (delete DataStorageDirManager::_selfObj)
