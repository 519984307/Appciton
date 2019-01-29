/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/

#pragma once
#include <QString>

class MergeConfig
{
public:
    static MergeConfig &construction()
    {
        if (NULL == _object)
        {
            _object = new MergeConfig();
        }

        return *_object;
    }

    static MergeConfig *_object;
    ~MergeConfig() {}

    // check and merge config
    void checkConfig();

    // back up old config
    void backupOldConfig();

    // check and mount the configuration partition
    bool checkAndMountConfigPartition();

    // get the configuration partition mount point
    QString configPartitionMountPoint() const;

private:
    MergeConfig();
    void _loadSupervisorConfig();
    void _loadMachineConfig();
    void _loadSystemConfig();

    // back up machine and supervisor file path
    QString _bkMachinePath() const;
    QString _bkSuperPath() const;
    QString _bkSystemPath() const;

    void config_compare(QString attr);

private:
    QString _configPartitionMountPoint;  // configuration partition mount point
};
#define mergeConfig (MergeConfig::construction())
#define deleteMergeConfig() (delete MergeConfig::_object)
