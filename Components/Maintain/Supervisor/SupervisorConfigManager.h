#pragma once
#include "IConfig.h"
#include "ConfigDefine.h"

//导入配置文件名
#define IMPORT_CONFIG_FILE ("/nPM_import_supervisor_config.xml")

//超级配置文件内容管理
class SupervisorConfigManager
{
public:
    static SupervisorConfigManager &construction()
    {
        if (NULL == _selfobj)
        {
            _selfobj = new SupervisorConfigManager();
        }

        return *_selfobj;
    }
    static SupervisorConfigManager *_selfobj;

    //加载默认配置
    void loadDefaultConfig();

    //导出配置
    bool exportConfig();

    //导入配置
    bool importConfig();

private:
    SupervisorConfigManager()
    {

    }

    //检查导入配置文件的内容
    bool _checkImportFile(QString file, QString &error);
    bool _checkRegexp(const QString &pattern, const QString &value);
};
#define supervisorConfigManager (SupervisorConfigManager::construction())
#define deleteSupervisorConfigManager() (delete SupervisorConfigManager::_selfobj)
