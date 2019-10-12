/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

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

Provider *ParamManager::getProvider(const QString &name)
{
    return _providers.value(name, NULL);
}

Param *ParamManager::getParam(ParamID id)
{
    return _paramWithID.value(id, NULL);
}

/**************************************************************************************************
 * 功能： 获取系统的Params。
 * 参数：
 *      params： 带回参数对象。
 *************************************************************************************************/
void ParamManager::getParams(QList<Param *> &params)
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
        if (it.key() > PARAM_O2)
        {
            continue;
        }

        paramID.append(it.key());
    }
}

/**************************************************************************************************
 * 功能： 将数据生产者和参数对象关联起来。
 *************************************************************************************************/
void ParamManager::connectParamProvider(WorkMode mode)
{
    // 从配置文件获取机器支持的参数。
    QString str;
    Provider *demoProvider = NULL;
    if (mode == WORK_MODE_DEMO)
    {
        demoProvider = _providers.value("DemoProvider", NULL);
    }
    machineConfig.getStrValue("AllParams", str);
    QStringList param_list = str.split(',');
    // trim text
    for (QStringList::iterator iter = param_list.begin(); iter != param_list.end(); ++iter)
    {
        *iter = (*iter).trimmed();
    }

    Provider *provider = NULL;

    for (int i = 0; i < param_list.size(); i++)
    {
        // 查找Param对象。
        Param *param = _params.value(param_list[i], NULL);
        if (param == NULL)
        {
            continue;
        }

        // deattach the demo provider if the param is return from the demo mode
        if (demoProvider)
        {
            demoProvider->detachParam(*param);
            param->exitDemo();
        }


        // 获取参数当前的Provider。
        machineConfig.getStrValue(param_list[i], str);

        QStringList provider_list = str.split(',');
        bool providerHasSet = false;
        for (QStringList::iterator iter = provider_list.begin(); iter != provider_list.end(); ++iter)
        {
            *iter = (*iter).trimmed();
            // 查找Provider对象。
            provider = _providers.value(*iter, NULL);
            if (provider)
            {
                // 将Provider对象与Param对象连接起来。
                if (mode == WORK_MODE_STANDBY && provider->connected())
                {
                    provider->reconnected();
                    provider->stopCheckConnect(false);
                }
                else
                {
                    providerHasSet = true;
                }
                provider->attachParam(*param);
            }
        }

        if (providerHasSet)
        {
            param->initParam();
        }
    }
}

void ParamManager::connectDemoParamProvider()
{
    Provider *provider = NULL;
    provider = _providers.value("DemoProvider", NULL);
    if (provider)
    {
        // 从配置文件获取机器支持的参数。
        QString str;
        machineConfig.getStrValue("AllParams", str);
        QStringList param_list = str.split(',');
        // trim text
        for (QStringList::iterator iter = param_list.begin(); iter != param_list.end(); ++iter)
        {
            *iter = (*iter).trimmed();
        }

        for (int i = 0; i < param_list.size(); i++)
        {
            // 查找Param对象。
            Param *param = _params.value(param_list[i], NULL);
            if (param == NULL)
            {
                continue;
            }

            // 使用demo模块数据，暂停接收其他模块数据
            machineConfig.getStrValue(param_list[i], str);
            Provider *otherProvider = _providers.value(str, NULL);
            if (otherProvider)
            {
                otherProvider->detachParam(*param);
            }

            provider->attachParam(*param);
            param->initParam();
            // 停止参数更新定时器
            param->stopParamUpdateTimer();
        }
    }
    else
    {
        qdebug("Demo provider not found");
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

bool ParamManager::isSubParamAvaliable(ParamID paramID, SubParamID id)
{
    ParamWithIDMap::Iterator it = _paramWithID.find(paramID);
    if (it == _paramWithID.end())
    {
        return 0;
    }

    return it.value()->isSubParamAvaliable(id);
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
        // no such param, return the default unit
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
 * check whether the param is stopped
 *************************************************************************************************/
bool ParamManager::isParamStopped(ParamID id)
{
    ParamWithIDMap::iterator it = _paramWithID.find(id);
    if (it == _paramWithID.end())
    {
        return true;
    }

    return !it.value()->isConnected();
}

/**************************************************************************************************
 * 功能：检查Proider的连接状态。
 *************************************************************************************************/
void ParamManager::checkProviderConnection(void)
{
    foreach(Provider *provider, _providers)
    {
        provider->checkConnection();
    }
}

/**************************************************************************************************
 * 功能：获取版本号。
 *************************************************************************************************/
void ParamManager::getVersion(void)
{
    foreach(Provider *provider, _providers)
    {
        provider->sendVersion();
    }
}

void ParamManager::disconnectParamProvider(WorkMode mode)
{
    if (mode != WORK_MODE_STANDBY)
    {
        return;
    }
    QString str;
    machineConfig.getStrValue("AllParams", str);
    QStringList param_list = str.split(',');
    // trim text
    for (QStringList::iterator iter = param_list.begin(); iter != param_list.end(); ++iter)
    {
        *iter = (*iter).trimmed();
    }

    Provider *provider = NULL;
    for (int i = 0; i < param_list.size(); ++i)
    {
        Param *param = _params.value(param_list.at(i), NULL);
        if (param == NULL)
        {
            continue;
        }
        machineConfig.getStrValue(param_list[i], str);
        QStringList provider_list = str.split(',');
        for (int j = 0; j < provider_list.size(); ++j)
        {
            provider_list[j] = provider_list[j].trimmed();
            provider = _providers.value(provider_list[j], NULL);
            if (provider)
            {
                // detach Param
                if (provider->connectedToParam())
                {
                    provider->disconnected();
                    provider->detachParam(*param);
                    provider->stopCheckConnect(true);
                }
            }
        }
    }
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
    _providers.clear();

    ParamMap::iterator pmit = _params.begin();
    for (; pmit != _params.end(); ++pmit)
    {
        delete pmit.value();
    }
    _params.clear();

    _paramWithID.clear();
}
