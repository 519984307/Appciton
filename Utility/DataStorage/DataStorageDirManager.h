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
#include "DataStorageDirManagerInterface.h"

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
class DataStorageDirManager : public DataStorageDirManagerInterface
{
    Q_OBJECT
public:
    static DataStorageDirManager &getInstance();
    ~DataStorageDirManager();

    // 获取营救事件时间列表
    void getRescueEvent(QStringList &timeList);   // NOLINT

    // 获取指定的营救数据路径
    QString getRescueDataDir(int index, const QString &time);
    QString getRescueDataDir(int index);

    // 创建文件夹
    void createDir(bool createNew = false);
    void addStorage(StorageManager * storage);

    // 获取当前文件夹
    virtual QString getCurFolder() const;

    // 获取文件夹的个数
    int getDirNum() const;

    // 存储空间是否存满了
    bool isCurRescueFolderFull();

    // 增加当前数据的存储大小
    void addDataSize(unsigned size);

    // 删除所有数据
    void deleteAllData();
    void deleteData(const QString &path);   // NOLINT
    void deleteData(int index);

    // 整理上一个rescure incident存储目录中文件
    static bool cleanUpLastIncidentDir();

    // clean up incident directory
    static bool cleanUpIncidentDir(const QString &dir);
    // get the size of specific directory
    static quint64 dirSize(const QString &dir);

    /**
     * @brief cleanCurData 删除当前文件夹（病人）的数据
     */
    void cleanCurData();

private:
    DataStorageDirManager();
    int _deleteDir(const QString &path);

    /**
     * @brief _cleanupDir 清理目录下的所有文件，但不删除该目录
     * @param path 要清理的文件夹路径
     * @return 清理的文件的总大小
     */
    int _cleanupDir(const QString &path);

    // 删除较早的数据
    void _deleteOldData(void);

private:
    unsigned _curDataSize;           // 当前的存储数据大小
    unsigned _previousDataSize;     // the size of previous rescue data
    QString _curFolder;              // 当前目录
    QList<struct FolderName> _folderNameList;  // 存储目录下文件夹名称及对应的大小

    bool _createNew;
};
#define dataStorageDirManager (DataStorageDirManager::getInstance())
#define deleteDataStorageDirManager() (delete &(DataStorageDirManager::getInstance()))
