/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QMap>
#include <QString>
#include "ParamDefine.h"
#include "SystemManager.h"
#include <QObject>

class Provider;
class BLMProvider;
class Param;
class ParamManager : public QObject
{
    Q_OBJECT
public:

    static ParamManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ParamManager();
        }
        return *_selfObj;
    }
    static ParamManager *_selfObj;

    // 将Provider和参数对象加入进来。
    void addProvider(Provider *provider);
    void addParam(Param *param);

    /**
     * @brief setSupportParams set a list of param that will be manager by param manager
     * @param paramNameList the Param name list
     */
    void setSupportParams(const QStringList &paramNameList);

    /**
     * @brief setParamProvider set the param's provider that it will connect to when works in normal mode
     * @param param the param name
     * @param provider the provider name
     */
    void setParamProvider(const QString &param, const QString &provider);

    /**
     * @brief getParamProvider get the param's provider
     * @param param the param name
     * @return pointor to the param provider or NULL if the provider not exist
     */
    Provider *getParamProvider(const QString &param);

    /**
     * @brief getProvider get a provider base on the provider name
     * @param name provider name
     * @return pointer to the provider or NULL if not found
     */
    Provider *getProvider(const QString &name);

    /**
     * @brief getParam get a param base on the param ID
     * @param id param id
     * @return pointer to the param or NULL if not found
     */
    Param *getParam(ParamID id);

    // 获取系统的Params。
    QList<Param*> getParams();
    QList<ParamID> getParamIDs();

    /**
     * @brief connectParamProvider 将数据生产者和参数对象关联起来
     * @param mode  当前处于的工作模式
     */
    void connectParamProvider(WorkMode mode);

    // connect the paramters to the demo provider
    void connectDemoParamProvider(void);

    // 获取子参数值。
    short getSubParamValue(const QString &paramName, SubParamID id);
    short getSubParamValue(ParamID paramID, SubParamID id);

    // check whether a param is avaliable
    bool isSubParamAvaliable(ParamID paramID, SubParamID id);

    // 显示子参数值
    void showSubParamValue();

    // 获取子参数的当前单位。
    UnitType getSubParamUnit(ParamID, SubParamID id);

    // 参数是否有效。
    bool isParamEnable(ParamID id);

    // check whether the param is stopped
    bool isParamStopped(ParamID id);

    // 检查Proider的连接状态。
    void checkProviderConnection(void);

    // 获取版本号。Notify the providers to get version info
    void getVersion(void);

    /**
     * @brief disconnectParamProvider 断开参数连接
     * @param mode
     */
    void disconnectParamProvider(WorkMode mode);

    // 析构。
    ~ParamManager();

private:
    ParamManager();

    typedef QMap<QString, Provider*> ProviderMap;
    typedef QMap<QString, Param*> ParamMap;
    typedef QMap<ParamID, Param*> ParamWithIDMap;
    QMap<QString, QString> paramProviderMap;
    QStringList supportParams;
    ProviderMap _providers;
    ParamMap _params;
    ParamWithIDMap _paramWithID;
};
#define paramManager (ParamManager::construction())
#define deleteParamManager() (delete ParamManager::_selfObj)
