#pragma once
#include <QList>
#include "PopupWidget.h"
#include <qscrollarea.h>
#include "LabelButton.h"

class QTimer;
class BatteryIndicatorWidget: public PopupWidget
{
    Q_OBJECT

public:
    BatteryIndicatorWidget();
    ~BatteryIndicatorWidget();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *event);

private slots:
    void _timeOutSlot();

private:
    QLabel *_temperatureLabel;               // 获取电池温度
    QLabel *_voltageLabel;                   // 获取电池电压
    QLabel *_voltageADCLabel;                // 获取电池ADC采集电压
    QLabel *_currentLabel;                   // 获取电池电流
    QLabel *_averageCurrentLabel;            // 获取每分钟平均电流
    QLabel *_relativeStateOfChargeLabel;     // 获取相对电荷状态，即相对电池剩余百分比
    QLabel *_absoluteStateOfChargeLabel;     // 获取绝对电荷状态，即绝对电池剩余百分比
    QLabel *_remainingCapacityLabel;         // 获取剩余电池容量
    QLabel *_runTimeToEmptyLabel;            // 获取电池剩余运行时间
    QLabel *_averageTimeToEmptyLabel;        // 获取电池平均剩余运行时间
    QLabel *_averageTimeToFullLabel;         // 获取电池平均充满时间
    QLabel *_statusLabel;                    // 获取电池状态信息
    QLabel *_modeLabel;                      // 获取电池模式

    QTimer *_timer;

    bool _recordBatteryInfo;
};
