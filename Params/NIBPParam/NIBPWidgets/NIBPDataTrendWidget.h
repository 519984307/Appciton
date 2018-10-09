/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/4
 **/


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
        prvalue = InvData();
        valueIsDisplay = false;
        sysAlarm = false;
        diaAlarm = false;
        mapAlarm = false;
        prAlarm = false;
        errorCode = NIBP_ONESHOT_NONE;
    }

    unsigned lastNibpMeasureTime;
    TrendDataType sysvalue;
    TrendDataType diavalue;
    TrendDataType mapvalue;
    TrendDataType prvalue;
    bool valueIsDisplay;
    bool sysAlarm;
    bool diaAlarm;
    bool mapAlarm;
    bool prAlarm;
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

    virtual void updatePalette(const QPalette pal);

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
