/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/23
 **/


#include "BatteryIconWidget.h"
#include "qpainter.h"
#include "Debug.h"

/**************************************************************************************************
 * 设置电池状态
 *************************************************************************************************/
void BatteryIconWidget::setStatus(BatteryIconStatus status)
{
    _batteryStatus = status;

    if ((_batteryStatus == _lastBatteryStatus) && (_batteryVolume == _lastBatteryVolume)
            && (_timeRelative == _lastTimeRelative))
    {
        return;
    }

    // 刷新
    update();
}

/**************************************************************************************************
 * 设置电量参数level = 0-5.
 *************************************************************************************************/
void BatteryIconWidget::setVolume(int volume)
{
    _batteryVolume = volume;
}

/**************************************************************************************************
 * 设置填充颜色
 *************************************************************************************************/
void BatteryIconWidget::setFillColor(const QColor &color)
{
    _fillColor = color;
}

/**************************************************************************************************
 * 设置剩余时间参数
 *************************************************************************************************/
void BatteryIconWidget::setTimeValue(int time)
{
    time = (time >= 6) ? 6 : time;

    _timeRelative = time;

    // 将剩余可运行时间转变为字符串
    if (_timeRelative == 0)
    {
        _printString = "<0:30";
    }
    else if (_timeRelative == -1)
    {
        _printString = "low";
    }
    else if (_timeRelative == -2)
    {
        _printString = "";
    }
    else
    {
        if (_timeRelative % 2 == 1)
        {
            _printString = QString("%1%2%3")
                           .arg(_timeRelative / 2).arg(":").arg("30+");
        }
        else
        {
            _printString = QString("%1%2%3")
                           .arg(_timeRelative / 2).arg(":").arg("00+");
        }
    }
}

/**************************************************************************************************
 * 绘画
 *************************************************************************************************/
void BatteryIconWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    QPainterPath rectFramePath;
    QRect r = this->rect();
    if (_iconBlackGroundColor.isValid())
    {
        if (_iconBlackGroundColor.isValid())
            painter.fillRect(r , _iconBlackGroundColor);
        else
            painter.fillRect(r, Qt::black);
    }
    else
        painter.eraseRect(r);

    painter.setRenderHint(QPainter::Antialiasing, true);

    // 上电池底色
    rectFramePath = _drawBatteryFrame(painter, r);

    // 根据传递的参数进行绘图
    _drawBatteryImage(painter, r);

    // 保存当前参数值
    _saveCurrentParaValue();

    // 画电池边框
    _drawBatteryFrameLine(painter, r, rectFramePath);
}

/**************************************************************************************************
 * 保存当前参数值
 *************************************************************************************************/
void BatteryIconWidget::_saveCurrentParaValue(void)
{
    _lastBatteryStatus = _batteryStatus;
    _lastTimeRelative = _timeRelative;
    _lastBatteryVolume = _batteryVolume;
}

/**************************************************************************************************
 * 颜色填充和字符串显示
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryImage(QPainter &painter, QRect &rect)
{
    // 设置电池内字体格式
    QFont font;
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(QPen(Qt::white, _boarderWidth));

    switch (_batteryStatus)
    {
    case BATTERY_NORMAL:

        // 正常电量图标显示
        _drawBatteryFillRect(painter, rect);
        // 剩余时间显示成字符串,
        if (_batteryVolume > 0 && _batteryVolume < 5)
        {
            painter.drawText(rect, Qt::AlignCenter, _printString);
        }
        break;

    case BATTERY_NOT_EXISTED:

        // 电池不存在
        _drawBatteryNo(painter, rect);
        break;

    case BATTERY_ERROR:

        // 电池故障
        _drawBatteryError(painter, rect);
        break;

    case BATTERY_COM_FAULT:

        // 电池通讯错误
        _drawBatteryCommFault(painter, rect);
        break;
    case BATTERY_CALIBRATION_REQUIRED:
        // 电池需要校准
        _drawBatteryCalibrate(painter, rect);

    default:
        break;
    }
}

/**************************************************************************************************
 * 画电池边框
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryFrameLine(QPainter &painter, QRect &/*rect*/, QPainterPath &path)
{
    painter.setPen(QPen(QColor(178, 178, 178), _boarderWidth, Qt::SolidLine));
    painter.drawPath(path);

    /* // 带虚线的电池边框
        rect = rect.adjusted(1, 1, 0, 0);
        QPoint topright = rect.topRight();
        QPoint bottomright = rect.bottomRight();

        if (_batteryStatus == BATTERY_NOT_EXISTED)
        {
            painter.setPen(QPen(QColor(220, 220, 220), _boarderWidth , Qt::DotLine));
        }
        else
        {
            painter.setPen(QPen(QColor(220, 220, 220), _boarderWidth , Qt::SolidLine));
        }

        painter.drawPath(path);
        painter.setPen(QPen(QColor(220, 220, 220), _boarderWidth , Qt::SolidLine));
        painter.drawLine(topright.x() - rect.width() / _wCount, topright.y() + rect.height() * 3 / 10,
                topright.x(), topright.y() + rect.height() * 3 / 10);
        painter.drawLine(topright.x(), topright.y() + rect.height() * 3 / 10,
                bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
        painter.drawLine(bottomright.x(), bottomright.y() - rect.height() * 3 / 10,
                bottomright.x() - rect.width() / _wCount, bottomright.y() - rect.height() * 3 / 10);
     */
}

/**************************************************************************************************
 * 上电池底色, 并返回电池边框路径
 *************************************************************************************************/
QPainterPath BatteryIconWidget::_drawBatteryFrame(QPainter &painter, QRect &rect)
{
    rect = rect.adjusted(1, 1, 0, 0);
    QPoint topleft = rect.topLeft();
    QPoint topright = rect.topRight();
    QPoint bottomleft = rect.bottomLeft();
    QPoint bottomright = rect.bottomRight();

    int wCount = 15;
    // 设置电池边框路径
    QPainterPath mypath;
    mypath.moveTo(bottomleft - QPoint(0, _radius));
    mypath.lineTo(topleft + QPoint(0, _radius));
    mypath.quadTo(topleft, topleft + QPoint(_radius, 0));
    mypath.lineTo(topright.x() - rect.width() / wCount - _radius / 2, topright.y());
    mypath.quadTo(topright.x() - rect.width() / wCount, topright.y(), topright.x() -
                  rect.width() / wCount, topright.y()  + _radius / 2);
    mypath.lineTo(topright.x() - rect.width() / wCount, topright.y() + rect.height() * 3 / 10);
    mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - _radius / 2);
    mypath.quadTo(bottomright.x() - rect.width() / wCount, bottomright.y(),
                  bottomright.x() - rect.width() / wCount - _radius / 2, bottomright.y());
    mypath.lineTo(bottomleft + QPoint(_radius, 0));
    mypath.quadTo(bottomleft, bottomleft - QPoint(0, _radius));

    // 用黑色作为路径填充色
    if (_iconBlackGroundColor.isValid())
        painter.fillPath(mypath, QBrush(_iconBlackGroundColor));
    else
        painter.fillPath(mypath, QBrush(Qt::black));


    // 返回该路径
    return mypath;
}

/**************************************************************************************************
 * 画电池不存在图标
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryNo(QPainter &painter, QRect &rect)
{
    // 获得新的矩形顶点坐标
    QRect rectAdjust = rect.adjusted(rect.width() * 8 / 21, rect.height() / 3,
                                     -rect.width() * 3 / 7, -rect.height() / 3);
    QPoint topleft = rectAdjust.topLeft();
    QPoint topright = rectAdjust.topRight();
    QPoint bottomleft = rectAdjust.bottomLeft();
    QPoint bottomright = rectAdjust.bottomRight();

    // 画一个白色“X”样式图
    QPen pen(QColor(220, 220, 220), _boarderWidth);
    pen.setWidth(1);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(pen);
    painter.drawLine(topleft.x(), topleft.y(), bottomright.x(), bottomright.y());
    painter.drawLine(bottomleft.x(), bottomleft.y(), topright.x(), topright.y());
    painter.setRenderHint(QPainter::Antialiasing, true);

    /*  // 复制原电池边框路径，并画成虚线
        QPoint topleft = rect.topLeft();
        QPoint topright = rect.topRight();
        QPoint bottomleft = rect.bottomLeft();
        QPoint bottomright = rect.bottomRight();

        painter.setPen(QPen(Qt::white, _boarderWidth, Qt::SolidLine));
        QPainterPath mypath;
        mypath.moveTo(bottomleft - QPoint(0, _radius));
        mypath.lineTo(topleft + QPoint(0, _radius));
        mypath.quadTo(topleft, topleft + QPoint(_radius, 0));
        mypath.lineTo(topright.x() - rect.width() / 10 - _radius / 4, topright.y());
        mypath.quadTo(topright.x() - rect.width() / 10, topright.y(),topright.x() - rect.width() / 10,
                topright.y() + rect.height() * 3 / 10 + _radius / 4);
        mypath.lineTo(topright.x() - rect.width() / 10, topright.y() +
                      rect.height() * 3 / 10);
        mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / 10, bottomright.y() - rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / 10, bottomright.y() - _radius / 4);
        mypath.quadTo(bottomright.x() - rect.width() / 10, bottomright.y(),
                      bottomright.x() - rect.width() / 10 - _radius / 4, bottomright.y());
        mypath.lineTo(bottomleft + QPoint(_radius, 0));
        mypath.quadTo(bottomleft,bottomleft - QPoint(0, _radius));
        painter.fillPath(mypath, QBrush(Qt::black));
        painter.setPen(QPen(Qt::white, _boarderWidth, Qt::DotLine));
        painter.drawPath(mypath);
        painter.fillRect(rect.x() + rect.width() * 9 / 10 -_boarderWidth, rect.y() + rect.height() * 3 / 10,
                rect.width() / 10, rect.height() - rect.height() * 6 / 10, QBrush(Qt::black));
    */
}

/**************************************************************************************************
 * 画电池故障图标
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryError(QPainter &painter, QRect &rect)
{
    // 获得新的矩形顶点坐标
    QRect rectAdjust = rect.adjusted(rect.width() / 6, rect.height() / 4,
                                     -rect.width() / 5, -rect.height() / 5);
    QPoint topleft = rectAdjust.topLeft();
    QPoint topright = rectAdjust.topRight();
    QPoint bottomleft = rectAdjust.bottomLeft();
    QPoint bottomright = rectAdjust.bottomRight();

    painter.setPen(QPen(Qt::red, _boarderWidth));
    painter.drawLine(topleft.x(), topleft.y(), bottomright.x(), bottomright.y());
    painter.drawLine(bottomleft.x(), bottomleft.y(), topright.x(), topright.y());
}

/**************************************************************************************************
 * 画电池需要校准图标
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryCalibrate(QPainter &painter, QRect &rect)
{
    painter.drawText(rect, Qt::AlignCenter, "?");
}

/**************************************************************************************************
 * 画电池通讯错误图标
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryCommFault(QPainter &painter, QRect &rect)
{
    QPoint topleft = rect.topLeft();
    QPoint topright = rect.topRight();
    QPoint bottomright = rect.bottomRight();

    painter.setPen(QPen(Qt::white, 1));
    QPainterPath mypath;
    int wCount = 15;
    mypath.moveTo(topright.x() - rect.width() * 3 / wCount,
                  topleft.y() + rect.height() / 3);
    mypath.lineTo(topright.x() - rect.width() * 3 / wCount,
                  topleft.y() + rect.height() / 3 + rect.height() / 10 - 1);
    mypath.lineTo(topleft.x() + rect.width() * 2 / wCount,
                  topleft.y() + rect.height() / 3 + rect.height() / 10 - 1);
    mypath.lineTo(topleft.x() + rect.width() * 3 / wCount,
                  topleft.y() + rect.height() / 3 - rect.height() / 10 - 1);
    mypath.lineTo(topleft.x() + rect.width() * 3 / wCount + rect.width() / 20,
                  topleft.y() + rect.height() / 3 - rect.height() / 10 - 1);
    mypath.lineTo(topleft.x() + rect.width() * 3 / wCount,
                  topleft.y() + rect.height() / 3);
    mypath.lineTo(topright.x() - rect.width() * 3 / wCount,
                  topleft.y() + rect.height() / 3);
    painter.drawPath(mypath);
    painter.fillPath(mypath, QBrush(Qt::white));

    mypath.moveTo(topleft.x() + rect.width() * 2 / wCount,
                  topleft.y() + rect.height() * 2 / 3);
    mypath.lineTo(topleft.x() + rect.width() * 2 / wCount,
                  topleft.y() + rect.height() * 2 / 3 - rect.height() / 10);
    mypath.lineTo(topright.x() - rect.width() * 3 / wCount,
                  topleft.y() + rect.height() * 2 / 3 - rect.height() / 10);
    mypath.lineTo(topright.x() - rect.width() * 4 / wCount,
                  topleft.y() + rect.height() * 2 / 3 + rect.height() / 10);
    mypath.lineTo(topright.x() - rect.width() * 4 / wCount - rect.width() / 20,
                  topleft.y() + rect.height() * 2 / 3 + rect.height() / 10);
    mypath.lineTo(topright.x() - rect.width() * 4 / wCount,
                  topleft.y() + rect.height() * 2 / 3);
    mypath.lineTo(topleft.x() + rect.width() * 2 / wCount,
                  topleft.y() + rect.height() * 2 / 3);
    painter.drawPath(mypath);
    painter.fillPath(mypath, QBrush(QColor(220, 220, 220)));

    // 画红色斜线
    QPen pen = QPen(Qt::red, rect.height() / 6);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawLine((topleft.x() + topright.x() - rect.width() / wCount) / 2 +
                     rect.width() / wCount, topright.y() + rect.height() / 6,
                     (topleft.x() + topright.x() - rect.width() / wCount) / 2 - rect.width() / wCount ,
                     bottomright.y() - rect.height() / 6);

    // 画两条黑色斜线
    painter.setPen(QPen(Qt::black, rect.height() / 12));
    painter.drawLine((topleft.x() + topright.x() - rect.width() / wCount) / 2 + rect.width() / wCount
                     - rect.height() / 10, topright.y() + rect.height() / 6, (topleft.x() + topright.x()
                             - rect.width() / wCount) / 2 - rect.width() / wCount - rect.height() / 10, bottomright.y()
                     - rect.height() / 6);
    painter.drawLine((topleft.x() + topright.x() - rect.width() / wCount) / 2 + rect.width() / wCount
                     + rect.height() / 10, topright.y() + rect.height() / 6, (topleft.x() + topright.x()
                             - rect.width() / wCount) / 2 - rect.width() / wCount + rect.height() / 10, bottomright.y()
                     - rect.height() / 6);
}

/**************************************************************************************************
 * 画正常电量显示图标
 *************************************************************************************************/
void BatteryIconWidget::_drawBatteryFillRect(QPainter &painter, QRect &rect)
{
    if (_batteryVolume <= 0)
    {
        return;
    }
    // 获得新的矩形顶点坐标
    QRect rectAdjust = rect;
    QPoint topleft = rectAdjust.topLeft();
    QPoint topright = rectAdjust.topRight();
    QPoint bottomleft = rectAdjust.bottomLeft();
    QPoint bottomright = rectAdjust.bottomRight();

    // 设置填充路径
    QPainterPath mypath;
    mypath.moveTo(bottomleft - QPoint(0, _radius));
    mypath.lineTo(topleft + QPoint(0, _radius));
    mypath.quadTo(topleft, topleft + QPoint(_radius, 0));
//    mypath.lineTo(topleft.x() + rect.width() * 2 / _wCount, topleft.y());

    if (_batteryVolume <= _wCount - 1)
    {
        mypath.lineTo(topleft.x() - 2.5 + _batteryVolume * rect.width() / _wCount, topleft.y());
        mypath.lineTo(bottomleft.x() + 2.5  + _batteryVolume * rect.width() / _wCount, bottomleft.y());
    }
    else
    {
        int wCount = 15;
        mypath.lineTo(topright.x() - rect.width() / wCount - _radius / 2, topright.y());
        mypath.quadTo(topright.x() - rect.width() / wCount, topright.y(), topright.x() -
                      rect.width() / wCount, topright.y()  + _radius / 2);

        mypath.lineTo(topright.x() - rect.width() / wCount, topright.y() + rect.height() * 3 / 10);
        mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() -
                      rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - _radius / 2);
        mypath.quadTo(bottomright.x() - rect.width() / wCount, bottomright.y(),
                      bottomright.x() - rect.width() / wCount - _radius / 2, bottomright.y());
    }
//    if (_batteryVolume <= _wCount - 2)
//    {
//        mypath.lineTo(topleft.x() + rect.width() * 2 / _wCount +
//                _batteryVolume * rect.width() * 11 / (13 * _wCount), topleft.y());
//        mypath.lineTo(bottomleft.x() + rect.width() * 2 / _wCount +
//                _batteryVolume * rect.width() * 11 / (13 * _wCount), bottomleft.y());
//    }
//    else if (_batteryVolume == _wCount - 1)
//    {
//        mypath.lineTo(topright.x() - rect.width() / _wCount - _radius / 2, topright.y());
//        mypath.quadTo(topright.x() - rect.width() / _wCount, topright.y(),topright.x() -
//                rect.width() / _wCount, topright.y()  + _radius / 2);
//        mypath.lineTo(bottomright.x() - rect.width() / _wCount, bottomright.y() - _radius / 2);
//        mypath.quadTo(bottomright.x() - rect.width() / _wCount, bottomright.y(),
//                      bottomright.x() - rect.width() / _wCount - _radius / 2, bottomright.y());
//    }
//    else
//    {
//        mypath.lineTo(topright.x() - rect.width() / _wCount - _radius / 2, topright.y());
//        mypath.quadTo(topright.x() - rect.width() / _wCount, topright.y(),topright.x() -
//                rect.width() / _wCount, topright.y()  + _radius / 2);
//        mypath.lineTo(topright.x() - rect.width() / _wCount, topright.y() + rect.height() * 3 / 10);
//        mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
//        mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
//        mypath.lineTo(bottomright.x() - rect.width() / _wCount, bottomright.y() -
//                rect.height() * 3 / 10);
//        mypath.lineTo(bottomright.x() - rect.width() / _wCount, bottomright.y() - _radius / 2);
//        mypath.quadTo(bottomright.x() - rect.width() / _wCount, bottomright.y(),
//                      bottomright.x() - rect.width() / _wCount - _radius / 2, bottomright.y());
//    }
//    mypath.lineTo(bottomleft.x() + rect.width() * 2 / _wCount, bottomleft.y());
    mypath.lineTo(bottomleft + QPoint(_radius, 0));
    mypath.quadTo(bottomleft, bottomleft - QPoint(0, _radius));
// printf("_batteryVolume = %d\n_timeRelative = %d\n",_batteryVolume,_timeRelative);
// if (_fillColor == QColor(0, 128, 0))
// {
//    printf("current fillcolor is white\n");
// }
// else
// {
//    printf("current fillcolor is yellow\n");
// }
    // 选择填充色
    painter.fillPath(mypath, _fillColor);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BatteryIconWidget::BatteryIconWidget(QColor iconColor) : QWidget(NULL)
{
    // 固定电池框大小
    if (!iconColor.isValid())    // if draw battery info window's icon
        setFixedSize(64, 32);
    _printString = "";
    _batteryVolume = 0;             // 当前电量等级
    _lastBatteryVolume = 0;
    _timeRelative = -2;
    _lastTimeRelative = -2;
    _fillColor = QColor(0, 128, 0);
    _lastFillColor = QColor(0, 128, 0);
    _batteryStatus = BATTERY_NORMAL;
    _lastBatteryStatus = BATTERY_NONE;
    _iconBlackGroundColor = iconColor;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BatteryIconWidget::~BatteryIconWidget()
{
}
