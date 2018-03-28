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

    //check and merge config
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

private:
    QString _configPartitionMountPoint;  //configuration partition mount point
};
#define mergeConfig (MergeConfig::construction())
#define deleteMergeConfig() (delete MergeConfig::_object)
