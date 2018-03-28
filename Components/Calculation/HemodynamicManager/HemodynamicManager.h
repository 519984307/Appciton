#pragma once
#include <QObject>
#include <QList>
#include "HemodynamicDefine.h"

#define YELLOW_BACKGROUND       QBrush(QColor(255,255,0))
#define WHITE_BACKGROUND        QBrush(QColor(255,255,255))

#define UP_ARROW                QChar(0x2191)
#define DOWN_ARROW              QChar(0x2193)

class HemodynamicManager : public QObject
{
    Q_OBJECT
public:
    static HemodynamicManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new HemodynamicManager();
        }
        return *_selfObj;
    }
    ~HemodynamicManager();

    static ParamLimitInfo outputLimit[HEMODYNAMIC_OUTPUT_NR];
    static ParamLimitInfo inputLimit[HEMODYNAMIC_PARAM_NR];
    static int outputAccuracy[HEMODYNAMIC_OUTPUT_NR];
    static int inputAccuracy[HEMODYNAMIC_PARAM_NR];

public:
    // 获取计算结果
    HemodynamicInfo getCalcValue(void);

    // 设置计算参数和结果
    void setCalcValue(HemodynamicInfo);

    // 获取回顾数据
    QList<HemodynamicInfo> getReviewData(void);

    // 设置输入值
    void setCalcInput(int, float);

    // 设置输出值
    void setCalcOutput(int, float);

    // 血液动力学计算
    void calcAction(void);

    // 插入数据
    void insertReviewData(HemodynamicInfo);

private:    
    static HemodynamicManager *_selfObj;
    HemodynamicManager();
    HemodynamicInfo _hemodynamicInfo;
    QList<HemodynamicInfo> _reviewData;

};
#define hemodynamicManager (HemodynamicManager::construction())
#define deleteHemodynamicManager() (delete &hemodynamicManager)
