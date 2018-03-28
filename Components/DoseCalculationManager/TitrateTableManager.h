#pragma once
#include <QObject>
#include "TitrateTableDefine.h"

class TitrateTableManager : public QObject
{
    Q_OBJECT
public:
    static TitrateTableManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TitrateTableManager();
        }
        return *_selfObj;
    }
    static TitrateTableManager *_selfObj;
    ~TitrateTableManager();

public:
    //
    void setStep(StepOption);

    void setDatumTerm(DatumTerm);

    void setDoseType(DoseType);

    // 获取病人信息。
    const SetTableParam &getSetTableParam(void);


private:
    TitrateTableManager();
    SetTableParam _setTableParam;
};

#define titrateTableManager (TitrateTableManager::construction())
#define deleteTitrateTableManager() (delete TitrateTableManager::_selfObj)
