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
#include <QTimer>

class BatteryIconWidgetPrivate
{
public:
    explicit BatteryIconWidgetPrivate(QColor color);
    ~BatteryIconWidgetPrivate(){}

    static const int boarderWidth = 2;
    static const int radius = 4;
    static const int wCount = 5;

    QString printString;                     // 用于打印的字符串
    BatteryPowerLevel batteryVolume;         // 当前电量等级
    int lastBatteryVolume;
    BatteryRemainTime timeRelative;          // 当前剩余时间
    BatteryRemainTime lastTimeRelative;
    QColor fillColor;                        // 填充颜色
    QColor lastFillColor;
    BatteryIconStatus batteryStatus;         // 电池状态
    BatteryIconStatus lastBatteryStatus;
    QColor iconBlackGroundColor;
    QTimer *chargingTimer;                   // 充电动画定时器
    BatteryPowerLevel chargingVolume;       // 充电动画电量等级

    /**
     * @brief saveCurrentParaValue 保存当前参数值
     */
    void saveCurrentParaValue(void);

    /**
     * @brief drawBatteryImage 颜色填充和字符串显示
     * @param painter
     * @param rect
     */
    void drawBatteryImage(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryError 画电池故障图标
     * @param painter
     * @param rect
     */
    void drawBatteryError(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryCalibrate 画电池需要校准图标
     * @param painter
     * @param rect
     */
    void drawBatteryCalibrate(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryNo 画电池不存在图标
     * @param painter
     * @param rect
     */
    void drawBatteryNo(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryCommFault 画电池通讯错误图标
     * @param painter
     * @param rect
     */
    void drawBatteryCommFault(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryFillRect 画正常电量显示图标
     * @param painter
     * @param rect
     */
    void drawBatteryFillRect(QPainter &painter, QRect &rect, BatteryPowerLevel volume);

    /**
     * @brief drawBatteryFrame 上电池底色, 并返回电池边框路径
     * @param painter
     * @param rect
     * @return
     */
    QPainterPath drawBatteryFrame(QPainter &painter, QRect &rect);

    /**
     * @brief drawBatteryFrameLine 画电池边框
     * @param painter
     * @param rect
     * @param path
     */
    void drawBatteryFrameLine(QPainter &painter, QRect &rect, QPainterPath &path);
};

/**************************************************************************************************
 * 设置电池状态
 *************************************************************************************************/
void BatteryIconWidget::setStatus(BatteryIconStatus status)
{
    d_ptr->batteryStatus = status;

    if ((d_ptr->batteryStatus == d_ptr->lastBatteryStatus)
            && (d_ptr->batteryVolume == d_ptr->lastBatteryVolume)
            && (d_ptr->timeRelative == d_ptr->lastTimeRelative))
    {
        return;
    }

    // 刷新
    update();
}

/**************************************************************************************************
 * 设置电量参数level = 0-5.
 *************************************************************************************************/
void BatteryIconWidget::setVolume(BatteryPowerLevel volume)
{
    if (d_ptr->batteryVolume != volume)
    {
        d_ptr->chargingVolume = volume;
    }
    d_ptr->batteryVolume = volume;
}

/**************************************************************************************************
 * 设置填充颜色
 *************************************************************************************************/
void BatteryIconWidget::setFillColor(const QColor &color)
{
    d_ptr->fillColor = color;
}

/**************************************************************************************************
 * 设置剩余时间参数
 *************************************************************************************************/
void BatteryIconWidget::setTimeValue(BatteryRemainTime time)
{
    switch (time)
    {
    case BAT_REMAIN_TIME_NULL:
        d_ptr->printString = "";
        break;
    case BAT_REMAIN_TIME_LOW:
        d_ptr->printString = "low";
        break;
    case BAT_REMAIN_TIME_1_HOUR:
        d_ptr->printString = "1:00+";
        break;
    case BAT_REMAIN_TIME_2_HOUR:
        d_ptr->printString = "2:00+";
        break;
    case BAT_REMAIN_TIME_5_HOUR:
        d_ptr->printString = "5:00+";
        break;
    case BAT_REMAIN_TIME_7_HOUR:
        d_ptr->printString = "7:00+";
        break;
    case BAT_REMAIN_TIME_8_HOUR:
        d_ptr->printString = "8:00+";
        break;
    default:
        break;
    };
    d_ptr->timeRelative = time;
}

void BatteryIconWidget::charging()
{
    if (!d_ptr->chargingTimer->isActive())
    {
        d_ptr->chargingTimer->start();
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
    if (d_ptr->iconBlackGroundColor.isValid())
    {
        if (d_ptr->iconBlackGroundColor.isValid())
            painter.fillRect(r , d_ptr->iconBlackGroundColor);
        else
            painter.fillRect(r, Qt::black);
    }
    else
        painter.eraseRect(r);

    painter.setRenderHint(QPainter::Antialiasing, true);

    // 上电池底色
    rectFramePath = d_ptr->drawBatteryFrame(painter, r);

    // 根据传递的参数进行绘图
    d_ptr->drawBatteryImage(painter, r);

    // 保存当前参数值
    d_ptr->saveCurrentParaValue();

    // 画电池边框
    d_ptr->drawBatteryFrameLine(painter, r, rectFramePath);
}

void BatteryIconWidget::chargingTimeOut()
{
    if (d_ptr->batteryVolume == BAT_VOLUME_5)
    {
        d_ptr->chargingTimer->stop();
        return;
    }
    int volume = static_cast<int>(d_ptr->chargingVolume) + 1;
    d_ptr->chargingVolume = static_cast<BatteryPowerLevel>(volume);
    if (d_ptr->chargingVolume > BAT_VOLUME_5)
    {
        d_ptr->chargingVolume = d_ptr->batteryVolume;
    }
    update();
}

void BatteryIconWidgetPrivate::saveCurrentParaValue(void)
{
    lastBatteryStatus = batteryStatus;
    lastTimeRelative = timeRelative;
    lastBatteryVolume = batteryVolume;
}

void BatteryIconWidgetPrivate::drawBatteryImage(QPainter &painter, QRect &rect)
{
    // 设置电池内字体格式
    QFont font;
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(QPen(Qt::white, boarderWidth));

    switch (batteryStatus)
    {
    case BATTERY_NORMAL:

        // 正常电量图标显示
        drawBatteryFillRect(painter, rect, batteryVolume);
        // 剩余时间显示成字符串,
        painter.drawText(rect, Qt::AlignCenter, printString);
        break;

    case BATTERY_NOT_EXISTED:

        // 电池不存在
        drawBatteryNo(painter, rect);
        break;

    case BATTERY_ERROR:

        // 电池故障
        drawBatteryError(painter, rect);
        break;

    case BATTERY_COM_FAULT:

        // 电池通讯错误
        drawBatteryCommFault(painter, rect);
        break;
    case BATTERY_CALIBRATION_REQUIRED:
        // 电池需要校准
        drawBatteryCalibrate(painter, rect);
        break;
    case BATTERY_CHARGING:
        drawBatteryFillRect(painter, rect, chargingVolume);
        break;
    default:
        break;
    }
}

void BatteryIconWidgetPrivate::drawBatteryFrameLine(QPainter &painter, QRect &rect, QPainterPath &path)
{
    Q_UNUSED(rect)
    painter.setPen(QPen(QColor(178, 178, 178), boarderWidth, Qt::SolidLine));
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

QPainterPath BatteryIconWidgetPrivate::drawBatteryFrame(QPainter &painter, QRect &rect)
{
    rect = rect.adjusted(1, 1, 0, 0);
    QPoint topleft = rect.topLeft();
    QPoint topright = rect.topRight();
    QPoint bottomleft = rect.bottomLeft();
    QPoint bottomright = rect.bottomRight();

    int wCount = 15;
    // 设置电池边框路径
    QPainterPath mypath;
    mypath.moveTo(bottomleft - QPoint(0, radius));
    mypath.lineTo(topleft + QPoint(0, radius));
    mypath.quadTo(topleft, topleft + QPoint(radius, 0));
    mypath.lineTo(topright.x() - rect.width() / wCount - radius / 2, topright.y());
    mypath.quadTo(topright.x() - rect.width() / wCount, topright.y(), topright.x() -
                  rect.width() / wCount, topright.y()  + radius / 2);
    mypath.lineTo(topright.x() - rect.width() / wCount, topright.y() + rect.height() * 3 / 10);
    mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - rect.height() * 3 / 10);
    mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - radius / 2);
    mypath.quadTo(bottomright.x() - rect.width() / wCount, bottomright.y(),
                  bottomright.x() - rect.width() / wCount - radius / 2, bottomright.y());
    mypath.lineTo(bottomleft + QPoint(radius, 0));
    mypath.quadTo(bottomleft, bottomleft - QPoint(0, radius));

    // 用黑色作为路径填充色
    if (iconBlackGroundColor.isValid())
        painter.fillPath(mypath, QBrush(iconBlackGroundColor));
    else
        painter.fillPath(mypath, QBrush(Qt::black));


    // 返回该路径
    return mypath;
}

void BatteryIconWidgetPrivate::drawBatteryNo(QPainter &painter, QRect &rect)
{
    // 获得新的矩形顶点坐标
    QRect rectAdjust = rect.adjusted(rect.width() * 8 / 21, rect.height() / 3,
                                     -rect.width() * 3 / 7, -rect.height() / 3);
    QPoint topleft = rectAdjust.topLeft();
    QPoint topright = rectAdjust.topRight();
    QPoint bottomleft = rectAdjust.bottomLeft();
    QPoint bottomright = rectAdjust.bottomRight();

    // 画一个白色“X”样式图
    QPen pen(QColor(220, 220, 220), boarderWidth);
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

void BatteryIconWidgetPrivate::drawBatteryError(QPainter &painter, QRect &rect)
{
    // 获得新的矩形顶点坐标
    QRect rectAdjust = rect.adjusted(rect.width() / 6, rect.height() / 4,
                                     -rect.width() / 5, -rect.height() / 5);
    QPoint topleft = rectAdjust.topLeft();
    QPoint topright = rectAdjust.topRight();
    QPoint bottomleft = rectAdjust.bottomLeft();
    QPoint bottomright = rectAdjust.bottomRight();

    painter.setPen(QPen(Qt::red, boarderWidth));
    painter.drawLine(topleft.x(), topleft.y(), bottomright.x(), bottomright.y());
    painter.drawLine(bottomleft.x(), bottomleft.y(), topright.x(), topright.y());
}

void BatteryIconWidgetPrivate::drawBatteryCalibrate(QPainter &painter, QRect &rect)
{
    painter.drawText(rect, Qt::AlignCenter, "?");
}

void BatteryIconWidgetPrivate::drawBatteryCommFault(QPainter &painter, QRect &rect)
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

void BatteryIconWidgetPrivate::drawBatteryFillRect(QPainter &painter, QRect &rect, BatteryPowerLevel volume)
{
    if (volume <= 0)
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
    mypath.moveTo(bottomleft - QPoint(0, radius));
    mypath.lineTo(topleft + QPoint(0, radius));
    mypath.quadTo(topleft, topleft + QPoint(radius, 0));

    if (volume <= wCount - 1)
    {
        mypath.lineTo(topleft.x() - 2.5 + volume * rect.width() / wCount, topleft.y());
        mypath.lineTo(bottomleft.x() + 2.5  + volume * rect.width() / wCount, bottomleft.y());
    }
    else
    {
        int wCount = 15;
        mypath.lineTo(topright.x() - rect.width() / wCount - radius / 2, topright.y());
        mypath.quadTo(topright.x() - rect.width() / wCount, topright.y(), topright.x() -
                      rect.width() / wCount, topright.y()  + radius / 2);

        mypath.lineTo(topright.x() - rect.width() / wCount, topright.y() + rect.height() * 3 / 10);
        mypath.lineTo(topright.x(), topright.y() + rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x(), bottomright.y() - rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() -
                      rect.height() * 3 / 10);
        mypath.lineTo(bottomright.x() - rect.width() / wCount, bottomright.y() - radius / 2);
        mypath.quadTo(bottomright.x() - rect.width() / wCount, bottomright.y(),
                      bottomright.x() - rect.width() / wCount - radius / 2, bottomright.y());
    }
    mypath.lineTo(bottomleft + QPoint(radius, 0));
    mypath.quadTo(bottomleft, bottomleft - QPoint(0, radius));

    // 选择填充色
    painter.fillPath(mypath, fillColor);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BatteryIconWidget::BatteryIconWidget(QColor iconColor)
    : QWidget(NULL),
      d_ptr(new BatteryIconWidgetPrivate(iconColor))
{
    // 固定电池框大小
    setFixedSize(64, 32);
    d_ptr->chargingTimer = new QTimer();
    d_ptr->chargingTimer->setInterval(1000 * 2);     // 两秒刷新一次
    connect(d_ptr->chargingTimer, SIGNAL(timeout()), this, SLOT(chargingTimeOut()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BatteryIconWidget::~BatteryIconWidget()
{
    delete d_ptr->chargingTimer;
    delete d_ptr;
}

BatteryIconWidgetPrivate::BatteryIconWidgetPrivate(QColor color)
    : printString(QString()),
      batteryVolume(BAT_VOLUME_NONE),
      lastBatteryVolume(0),
      timeRelative(BAT_REMAIN_TIME_NULL),
      lastTimeRelative(BAT_REMAIN_TIME_NULL),
      fillColor(QColor(0, 128, 0)),
      lastFillColor(QColor(0, 128, 0)),
      batteryStatus(BATTERY_NORMAL),
      lastBatteryStatus(BATTERY_NONE),
      iconBlackGroundColor(color),
      chargingTimer(NULL),
      chargingVolume(BAT_VOLUME_NONE)
{
}
