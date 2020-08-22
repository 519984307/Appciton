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
#include "AlarmDefine.h"
#include "EventDataParseContext.h"
#include "Framework/Storage/IStorageBackend.h"

// nibp数据包
struct NibpDataPacket
{
    NibpDataPacket()
        : isAlarm(false),
          priority(ALARM_PRIO_PROMPT),
          value(InvData())
    {}
    bool isAlarm;
    AlarmPriority priority;
    TrendDataType value;
};

// 趋势缓存数据
struct NIBPTrendCacheData
{
    NIBPTrendCacheData()
    {
        lastNibpMeasureTime = 0;
        prvalue = InvData();
        valueIsDisplay = false;
        prAlarm = false;
        errorCode = NIBP_ONESHOT_NONE;
    }

    unsigned lastNibpMeasureTime;
    TrendDataType prvalue;
    bool valueIsDisplay;
    bool prAlarm;
    NibpDataPacket sys;
    NibpDataPacket dia;
    NibpDataPacket map;
    NIBPOneShotType errorCode;
};

class QLabel;
class QTableWidget;
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

    virtual void updatePalette(const QPalette &pal);

    /**
     * @brief updateWidgetConfig
     */
    void updateWidgetConfig();

    /**
     * @brief clearListData 清除列表数据
     */
    void clearListData();

    /* reimplement */
    void updateLimit();

    void updateUnit(UnitType unit);

    void getTrendNIBPlist();

    bool parseEventData(int dataIndex);

    NIBPDataTrendWidget();
    ~NIBPDataTrendWidget();

protected:
    void resizeEvent(QResizeEvent *e);

    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);
    void _releaseHandle();

private:
    QTableWidget *_table;
    QString _hrString;
    bool _isAlarm;
    int _rowNR;
    int _tableItemHeight;  // 表格高度。

    EventDataPraseContext ctx;
    IStorageBackend *backend;
    int eventNum;
    /**
     * @brief getPriotityColor 根据优先级获取显示颜色
     * @param prio
     * @return
     */
    QString getPriotityColor(AlarmPriority prio);

    // 子参数ID、数值映射。
    typedef QMap<unsigned, NIBPTrendCacheData> NIBPTrendCacheMap;
    NIBPTrendCacheMap _nibpNrendCacheMap;

    QString moduleStr;
    int columnNR;
    bool _showNIBPValue;   // show nibp trend data
};
