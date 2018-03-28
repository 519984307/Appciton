#include "ParamManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include "Provider.h"
#include "BLMProvider.h"
#include "Param.h"
#include "IConfig.h"
#include <QStringList>
#include <iostream>

ParamManager *ParamManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 从配置文件中读取Provider和Param的对应关系。
 *************************************************************************************************/
//void ParamManager::_load_param_provider_info(void)
//{
//    // 读取数据到_provider_param_table表中。
//    QString str;
//    machine_config.ReadStringValue("AllProviders", str);
//    QStringList provider_list = str.split(',');
//    QStringList param_list;
//    for (int i = 0; i < provider_list.size(); i++)
//    {
//        machine_config.ReadStringValue(provider_list[i], str);
//        param_list = str.split(',');
//        for (int j = 0; j < param_list.size(); j++)
//        {
//            _provider_param_table.insert(provider_list[i], param_list[j]);
//        }
//    }
//    // 读取数据到_param_provider_table表中。
//    machine_config.ReadStringValue("AllParams", str);
//    param_list = str.split(',');
//    for (int i = 0; i < param_list.size(); i++)
//    {
//        machine_config.ReadStringValue(param_list[i], str);
//        _param_provider_table.insert(param_list[i], str);
//    }
//}

/**************************************************************************************************
 * 功能： 将数据生产者对象加入进来。
 * 参数：
 *      provider： 数据生产者。
 *************************************************************************************************/
void ParamManager::addProvider(Provider &provider)
{
    _providers.insert(provider.getName(), &provider);
}

/**************************************************************************************************
 * 功能： 将参数对象加入进来。
 * 参数：
 *      param： 参数对象。
 *************************************************************************************************/
void ParamManager::addParam(Param &param)
{
    _params.insert(param.getName(), &param);
    _paramWithID.insert(param.getParamID(), &param);
}

/**************************************************************************************************
 * 功能： 获取系统的Params。
 * 参数：
 *      params： 带回参数对象。
 *************************************************************************************************/
void ParamManager::getParams(QList<Param*> &params)
{
    params.clear();
    ParamMap::Iterator it = _params.begin();
    for (; it != _params.end(); ++it)
    {
        params.append(it.value());
    }
}

/**************************************************************************************************
 * 功能： 获取系统的ParamID。
 * 参数：
 *      paramID: 带回参数的ID。
 *************************************************************************************************/
void ParamManager::getParams(QList<ParamID> &paramID)
{
    paramID.clear();
    ParamWithIDMap::Iterator it = _paramWithID.begin();
    for (; it != _paramWithID.end(); ++it)
    {
        if (it.key() > PARAM_CO)
        {
            continue;
        }

        paramID.append(it.key());
    }
}

/**************************************************************************************************
 * 功能： 将数据生产者和参数对象关联起来。
 *************************************************************************************************/
void ParamManager::connectParamProvider(void)
{
    // 从配置文件获取机器支持的参数。
    QString str;
    machineConfig.getStrValue("AllParams", str);
    QStringList param_list = str.split(',');
    //trim text
    for(QStringList::iterator iter = param_list.begin(); iter != param_list.end(); iter++)
    {
        *iter = (*iter).trimmed();
    }
    Provider *provider = NULL;
    Param *param = NULL;

    for (int i = 0; i < param_list.size(); i++)
    {
        // 查找Param对象。
        param = _params.value(param_list[i], NULL);
        if (param == NULL)
        {
            continue;
        }

        // 获取参数当前的Provider。
        machineConfig.getStrValue(param_list[i], str);

        QStringList provider_list = str.split(',');
        bool providerHasSet = false;
        for(QStringList::iterator iter = provider_list.begin(); iter != provider_list.end(); iter++)
        {
            *iter = (*iter).trimmed();
            // 查找Provider对象。
            provider = _providers.value(*iter, NULL);
            if(provider)
            {
                // 将Provider对象与Param对象连接起来。
                provider->attachParam(*param);
                providerHasSet = true;
            }

            BLMProvider *blmProvider = static_cast<BLMProvider*>(provider);
            if(blmProvider)
            {
                _blmProviders.insert(param->getParamID(), blmProvider);
            }
        }

        if(providerHasSet)
        {
            param->initParam();
        }
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 * 参数:
 *     paramname:参数名称
 *     id:子参数ID
 *     返回:子参数值
 *************************************************************************************************/
short ParamManager::getSubParamValue(const QString &paramName, SubParamID id)
{
    ParamMap::Iterator it = _params.find(paramName);
    if (it == _params.end())
    {
        return InvData();
    }

    return it.value()->getSubParamValue(id);
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 * 参数:
 *     paramID:参数ID
 *     id:子参数ID
 *     返回:子参数值
 *************************************************************************************************/
short ParamManager::getSubParamValue(ParamID paramID, SubParamID id)
{
    ParamWithIDMap::Iterator it = _paramWithID.find(paramID);
    if (it == _paramWithID.end())
    {
        return 0;
    }

    return it.value()->getSubParamValue(id);
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ParamManager::showSubParamValue()
{
    ParamMap::Iterator it = _params.begin();
    while (it != _params.end())
    {
        Param *param = it.value();
        if (NULL != param)
        {
            if (isParamEnable(param->getParamID()))
            {
                param->showSubParamValue();
            }
        }

        ++it;
    }
}

/**************************************************************************************************
 * 功能： 获取子参数的当前单位。
 * 参数:
 *      paramID:参数ID
 *      id:子参数ID
 * 返回:
 *      子参数值的当前单位。
 *************************************************************************************************/
UnitType ParamManager::getSubParamUnit(ParamID paramID, SubParamID id)
{
    ParamWithIDMap::Iterator it = _paramWithID.find(paramID);
    if (it == _paramWithID.end())
    {
        //no such param, return the default unit
        return paramInfo.getUnitOfSubParam(id);
    }

    return it.value()->getCurrentUnit(id);
}

/**************************************************************************************************
 * 参数是否有效。
 *************************************************************************************************/
bool ParamManager::isParamEnable(ParamID id)
{
    // 找到对应的param对象。
    ParamWithIDMap::iterator it = _paramWithID.find(id);
    if (it == _paramWithID.end())
    {
        return false;
    }

    return it.value()->isEnabled();
}

/**************************************************************************************************
 * 功能：检查Proider的连接状态。
 *************************************************************************************************/
void ParamManager::checkProviderConnection(void)
{
    foreach (Provider *provider, _providers)
    {
        provider->checkConnection();

    }
}

/**************************************************************************************************
 * 功能：获取版本号。
 *************************************************************************************************/
void ParamManager::getVersion(void)
{
    foreach (Provider *provider, _providers)
    {
        provider->sendVersion();
    }
}

/***************************************************************************************************
 * getBLMProvider : get BLMProvider by param id
 **************************************************************************************************/
BLMProvider *ParamManager::getBLMProvider(ParamID paramId)
{
    return _blmProviders.value(paramId, NULL);
}

/**************************************************************************************************
 * 功能：构造。
 *************************************************************************************************/
ParamManager::ParamManager()
{

}

/**************************************************************************************************
 * 功能：析构。
 *************************************************************************************************/
ParamManager::~ParamManager()
{
    // 删除对象。
    ProviderMap::iterator pit = _providers.begin();
    for (; pit != _providers.end(); ++pit)
    {
        delete pit.value();
    }

    ParamMap::iterator pmit = _params.begin();
    for (; pmit != _params.end(); ++pmit)
    {
        delete pmit.value();
    }
}
