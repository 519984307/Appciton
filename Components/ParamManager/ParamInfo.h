#pragma once
#include <stddef.h>
#include "ParamDefine.h"
#include "UnitManager.h"

class ParamInfo
{
public:
    static ParamInfo &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ParamInfo();
        }
        return *_selfObj;
    }
    static ParamInfo *_selfObj;
    ~ParamInfo();

public:
    // 根据参数的ID查询对应的名字。
    const char *getParamName(ParamID id);
    ParamID getParamID(const QString &name);

    // Param 与 SubParam之间转换。
    ParamID getParamID(SubParamID id);
    void getSubParams(ParamID id, QList<SubParamID> &ids);
    const char *getSubParamName(SubParamID id, bool ignoreModuleConfig = false);
    const char *getSubParamName(SubDupParamID id);

    // 获取波形ID与字串名之间的转换。
    const char *getParamWaveformName(WaveformID id);

    // 获取参数存放趋势数据时的放大倍数。
    int getMultiOfSubParam(SubParamID id);
    int getMultiOfSubParam(const char *name);

    // 获取子参数的默认单位。
    UnitType getUnitOfSubParam(SubParamID id);

    // 查询子参数的默认单位。
    UnitType getUnitOfSubParam(SubParamID id, UnitType &t0, UnitType &t1);

    // sort the param id
    static void sortSubParamId(QList<SubParamID> &subIDlist);

private:
    ParamInfo();
};
#define paramInfo (ParamInfo::construction())
#define deleteParamInfo() (delete ParamInfo::_selfObj)
