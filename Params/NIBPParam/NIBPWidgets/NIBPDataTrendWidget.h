#pragma once
#include "TrendWidget.h"
#include <QHBoxLayout>
#include "BaseDefine.h"
#include "NIBPParam.h"
#include <QMap>

// 趋势缓存数据
struct NIBPTrendCacheData
{
    NIBPTrendCacheData()
    {
        lastNibpMeasureTime = 0;
        sysvalue = InvData();
        diavalue = InvData();
        mapvalue = InvData();
        valueIsDisplay = false;
        sysAlarm = false;
        diaAlarm = false;
        mapAlarm = false;
        errorCode = NIBP_ONESHOT_NONE;
    }

    unsigned lastNibpMeasureTime;
    TrendDataType sysvalue;
    TrendDataType diavalue;
    TrendDataType mapvalue;
    bool valueIsDisplay;
    bool sysAlarm;
    bool diaAlarm;
    bool mapAlarm;
    NIBPOneShotType errorCode;
};

class QLabel;
class ITableWidget;
class NIBPDataTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新HR和ST的值。
    void setHRValue(short hr, bool isHR);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    void collectNIBPTrendData(unsigned t);

    // 显示参数值
    void showValue(void);

    NIBPDataTrendWidget();
    ~NIBPDataTrendWidget();

protected:
    void resizeEvent(QResizeEvent *e);

    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);
    void _releaseHandle();

private:
    ITableWidget *_table;
    QString _hrString;
    bool _isAlarm;
    int _rowNR;
    int _tableItemHeight;  // 表格高度。

    // 子参数ID、数值映射。
    typedef QMap<unsigned, NIBPTrendCacheData> NIBPTrendCacheMap;
    NIBPTrendCacheMap _nibpNrendCacheMap;
};
