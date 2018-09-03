/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#pragma once
#include <QWidget>
#include <QMap>

enum BatteryIconStatus
{
    BATTERY_NONE = -1,
    BATTERY_NORMAL,                 //正常
    BATTERY_NOT_EXISTED,            //无电池
    BATTERY_COM_FAULT,              //通信错误
    BATTERY_ERROR,                  //电池错误
    BATTERY_CALIBRATION_REQUIRED,   //电池需要校准
};

/***************************************************************************************************
 * 系统状态栏，主要显示一些系统提示图标。
 **************************************************************************************************/
class BatteryIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryIconWidget(QColor iconBackGround = QColor());
    ~BatteryIconWidget();

    // 设置电池状态
    void setStatus(BatteryIconStatus status);

    // 设置电量参数level = 0-15.
    void setVolume(int level);

    // 设置填充颜色
    void setFillColor(const QColor &color);

    // 设置剩余时间参数
    void setTimeValue(int time);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void _saveCurrentParaValue(void);
    void _drawBatteryImage(QPainter &painter, QRect &rect);
    void _drawBatteryError(QPainter &painter, QRect &rect);
    void _drawBatteryCalibrate(QPainter &painter, QRect &rect);
    void _drawBatteryNo(QPainter &painter, QRect &rect);
    void _drawBatteryCommFault(QPainter &painter, QRect &rect);
    void _drawBatteryFillRect(QPainter &painter, QRect &rect);
    QPainterPath _drawBatteryFrame(QPainter &painter, QRect &rect);
    void _drawBatteryFrameLine(QPainter &painter, QRect &rect, QPainterPath &path);

    static const int _boarderWidth = 2;
    static const int _radius = 4;
    static const int _wCount = 5;

    QString _printString;                        // 用于打印的字符串
    int _batteryVolume;                          // 当前电量等级
    int _lastBatteryVolume;
    int _timeRelative;                           // 当前剩余时间
    int _lastTimeRelative;
    QColor _fillColor;                        // 填充颜色
    QColor _lastFillColor;
    BatteryIconStatus _batteryStatus;            // 电池状态
    BatteryIconStatus _lastBatteryStatus;
    QColor _iconBlackGroundColor;
};
