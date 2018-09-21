#include "Debug.h"
#include "LabelButton.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "BatteryIndicatorWidget.h"
#include "BatteryIconWidget.h"
#include "PowerManager.h"
#include "IConfig.h"
#include <QTimer>
#include "LayoutManager.h"

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
BatteryIndicatorWidget::BatteryIndicatorWidget() : PopupWidget()
{
    int fontSize = fontManager.getFontSize(1);
    setTitleBarText(trs("BatteryInfo"));
    setFixedWidth(windowManager.getPopMenuWidth());

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(180, 180, 180));
    setPalette(palette);

    QVBoxLayout *layoutV = new QVBoxLayout();
    layoutV->setMargin(5);
    layoutV->setSpacing(10);

    // first row.
    QLabel *iconLabel = new QLabel();
    iconLabel->setAlignment(Qt::AlignHCenter);
    iconLabel->setFont(fontManager.textFont(fontSize, true));
    iconLabel->setWordWrap(true);
    iconLabel->setText(trs("BatteryCapacityIcons"));

    layoutV->addWidget(iconLabel);

    // second row.
    QHBoxLayout *layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(10);

    BatteryIconWidget *batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(5);
    batteryImag->setTimeValue(-2);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(4);
    batteryImag->setTimeValue(6);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(3);
    batteryImag->setTimeValue(4);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(2);
    batteryImag->setTimeValue(2);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(1);
    batteryImag->setTimeValue(1);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(1);
    batteryImag->setTimeValue(-1);
    batteryImag->setFillColor(Qt::yellow);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);

    // third row.
    layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(0);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize, true));
    iconLabel->setText(trs("BatteryDisconnected"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_NOT_EXISTED);

    QVBoxLayout *disconnectV = new QVBoxLayout();
    disconnectV->setSpacing(10);
    disconnectV->setMargin(0);
    disconnectV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    disconnectV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize, true));
    iconLabel->setText(trs("BatteryCommFault"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_COM_FAULT);

    QVBoxLayout *comfaultV = new QVBoxLayout();
    comfaultV->setSpacing(10);
    comfaultV->setMargin(0);
    comfaultV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    comfaultV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    layoutH->addStretch();
    layoutH->addLayout(disconnectV);
    layoutH->addStretch();
    layoutH->addLayout(comfaultV);
    layoutH->addStretch();

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);

    // fourth row.
    layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(0);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize, true));
    iconLabel->setText(trs("BatteryError"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_ERROR);

    QVBoxLayout *errorV = new QVBoxLayout();
    errorV->setSpacing(10);
    errorV->setMargin(0);
    errorV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    errorV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize, true));
    iconLabel->setText(trs("BatteryCalibrationRequired"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_CALIBRATION_REQUIRED);

    QVBoxLayout *calibrateV = new QVBoxLayout();
    calibrateV->setSpacing(10);
    calibrateV->setMargin(0);
    calibrateV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    calibrateV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    layoutH->addStretch();
    layoutH->addLayout(errorV);
    layoutH->addStretch();
    layoutH->addLayout(calibrateV);
    layoutH->addStretch();

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);

    fontSize = 18;
    _temperatureLabel = new QLabel();
    _temperatureLabel->setFont(fontManager.textFont(fontSize));
    _temperatureLabel->setAlignment(Qt::AlignLeft);
    _temperatureLabel->setText("temperature:");

    _currentLabel = new QLabel();
    _currentLabel->setFont(fontManager.textFont(fontSize));
    _currentLabel->setAlignment(Qt::AlignLeft);
    _currentLabel->setText("current:");

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->setMargin(0);
    layout0->setSpacing(10);
    layout0->addWidget(_temperatureLabel);
    layout0->addWidget(_currentLabel);

    _voltageLabel = new QLabel();
    _voltageLabel->setFont(fontManager.textFont(fontSize));
    _voltageLabel->setAlignment(Qt::AlignLeft);
    _voltageLabel->setText("voltage:");

    _voltageADCLabel = new QLabel();
    _voltageADCLabel->setFont(fontManager.textFont(fontSize));
    _voltageADCLabel->setAlignment(Qt::AlignLeft);
    _voltageADCLabel->setText("voltage ADC:");

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setMargin(0);
    layout1->setSpacing(10);
    layout1->addWidget(_voltageLabel);
    layout1->addWidget(_voltageADCLabel);

    _averageCurrentLabel = new QLabel();
    _averageCurrentLabel->setFont(fontManager.textFont(fontSize));
    _averageCurrentLabel->setAlignment(Qt::AlignLeft);
    _averageCurrentLabel->setText("averageCurrent:");

    _relativeStateOfChargeLabel = new QLabel();
    _relativeStateOfChargeLabel->setFont(fontManager.textFont(fontSize));
    _relativeStateOfChargeLabel->setAlignment(Qt::AlignLeft);
    _relativeStateOfChargeLabel->setText("relativeStateOfCharge:");

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setMargin(0);
    layout2->setSpacing(10);
    layout2->addWidget(_averageCurrentLabel);
    layout2->addWidget(_relativeStateOfChargeLabel);

    _absoluteStateOfChargeLabel = new QLabel();
    _absoluteStateOfChargeLabel->setFont(fontManager.textFont(fontSize));
    _absoluteStateOfChargeLabel->setAlignment(Qt::AlignLeft);
    _absoluteStateOfChargeLabel->setText("absoluteStateOfCharge:");

    _remainingCapacityLabel = new QLabel();
    _remainingCapacityLabel->setFont(fontManager.textFont(fontSize));
    _remainingCapacityLabel->setAlignment(Qt::AlignLeft);
    _remainingCapacityLabel->setText("remainingCapacity:");

    _runTimeToEmptyLabel = new QLabel();
    _runTimeToEmptyLabel->setFont(fontManager.textFont(fontSize));
    _runTimeToEmptyLabel->setAlignment(Qt::AlignLeft);
    _runTimeToEmptyLabel->setText("runTimeToEmpty:");

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->setMargin(0);
    layout3->setSpacing(10);
    layout3->addWidget(_remainingCapacityLabel);
    layout3->addWidget(_runTimeToEmptyLabel);

    _averageTimeToEmptyLabel = new QLabel();
    _averageTimeToEmptyLabel->setFont(fontManager.textFont(fontSize));
    _averageTimeToEmptyLabel->setAlignment(Qt::AlignLeft);
    _averageTimeToEmptyLabel->setText("averageTimeToEmpty:");

    _averageTimeToFullLabel = new QLabel();
    _averageTimeToFullLabel->setFont(fontManager.textFont(fontSize));
    _averageTimeToFullLabel->setAlignment(Qt::AlignLeft);
    _averageTimeToFullLabel->setText("averageTimeToFull:");


    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->setMargin(0);
    layout4->setSpacing(10);
    layout4->addWidget(_averageTimeToEmptyLabel);
    layout4->addWidget(_averageTimeToFullLabel);

    _statusLabel = new QLabel();
    _statusLabel->setFont(fontManager.textFont(fontSize));
    _statusLabel->setAlignment(Qt::AlignLeft);
    _statusLabel->setText("status:");

    _modeLabel = new QLabel();
    _modeLabel->setFont(fontManager.textFont(fontSize));
    _modeLabel->setAlignment(Qt::AlignLeft);
    _modeLabel->setText("mode:");

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->setMargin(0);
    layout5->setSpacing(10);
    layout5->addWidget(_statusLabel);
    layout5->addWidget(_modeLabel);

    QVBoxLayout *layoutValue = new QVBoxLayout();
    layoutValue->setMargin(2);
    layoutValue->setSpacing(2);

    layoutValue->addLayout(layout0);
    layoutValue->addLayout(layout1);
    layoutValue->addLayout(layout2);
    layoutValue->addLayout(layout3);
    layoutValue->addLayout(layout4);
    layoutValue->addLayout(layout5);

    _recordBatteryInfo = false;
    machineConfig.getNumValue("Record|Battery", _recordBatteryInfo);
    if (_recordBatteryInfo)
    {
        layoutV->addLayout(layoutValue);
    }

    layoutV->addStretch();
    contentLayout->addLayout(layoutV);

    _timer = new QTimer();
    _timer->setInterval(500);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
BatteryIndicatorWidget::~BatteryIndicatorWidget()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

void BatteryIndicatorWidget::showEvent(QShowEvent *e)
{
    if (!_recordBatteryInfo)
    {
        PopupWidget::showEvent(e);
    }
    else
    {
        QRect r = layoutManager.getMenuArea();
        move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
    }
    _timer->start();
}

void BatteryIndicatorWidget::hideEvent(QHideEvent *event)
{
    _timer->stop();

    PopupWidget::hideEvent(event);
}

void BatteryIndicatorWidget::_timeOutSlot()
{
    int valueInt = 0;
    short valueShort = 0;
    QString str;
    unsigned char *data;
    QString InvStr = "---";

    switch(powerManager.getStatus())
    {
    case BATTERY_NORMAL:// 正常电量图标显示
    case BATTERY_ERROR:
    case BATTERY_CALIBRATION_REQUIRED:
        valueInt = powerManager.batteryMessage.getTemperature();
        str.sprintf("temperature: %.2f ", ((valueInt / 10.0) - 274.15));
        str += trs("celsius");
        _temperatureLabel->setText(str);

        valueInt = powerManager.batteryMessage.getVoltage();
        str.sprintf("%.3f", valueInt / 1000.0);
        _voltageLabel->setText("voltage: " + str + " v");

        valueInt = powerManager.batteryMessage.getVoltageADC();
        str.sprintf("%.3f", valueInt / 1000.0);
        _voltageADCLabel->setText("voltage ADC: " + str + " v");

        valueShort = powerManager.batteryMessage.getCurrent();
        str.sprintf("%.3f", valueShort / 1000.0);
        _currentLabel->setText("current: " + str + " A");

        valueShort = powerManager.batteryMessage.getAverageCurrent();
        str.sprintf("%.3f", valueShort / 1000.0);
        _averageCurrentLabel->setText("averageCurrent: " + str + " A");

        valueInt = powerManager.batteryMessage.getRelativeStateOfCharge();
        _relativeStateOfChargeLabel->setText("relativeStateOfCharge: " + QString::number(valueInt) + " %");

        valueInt = powerManager.batteryMessage.getAbsoluteStateOfCharge();
        _absoluteStateOfChargeLabel->setText("absoluteStateOfCharge: " + QString::number(valueInt) + " %");

        valueInt = powerManager.batteryMessage.getRemainingCapacity();
        _remainingCapacityLabel->setText("remainingCapacity: " + QString::number(valueInt) + " mAh");

        valueInt = powerManager.batteryMessage.getRunTimeToEmpty();
        _runTimeToEmptyLabel->setText("runTimeToEmpty: " + QString::number(valueInt) + " min");

        valueInt = powerManager.batteryMessage.getAverageTimeToEmpty();
        _averageTimeToEmptyLabel->setText("averageTimeToEmpty: " + QString::number(valueInt) + " min");

        valueInt = powerManager.batteryMessage.getAverageTimeToFull();
        _averageTimeToFullLabel->setText("averageTimeToFull: " + QString::number(valueInt) + " min");

        data = powerManager.batteryMessage.getStatus();
        str.sprintf("0x%02x, 0x%02x",data[1],data[0]);
        _statusLabel->setText("status: " + str);

        data = powerManager.batteryMessage.getMode();
        str.sprintf("0x%02x, 0x%02x",data[1],data[0]);
        _modeLabel->setText("mode: " + str);
        break;

    case BATTERY_NOT_EXISTED:
    {
        QString tmpStr = "temperature: " + InvStr;
        tmpStr += " ";
        tmpStr += trs("celsius");
        _temperatureLabel->setText(tmpStr);
        _voltageLabel->setText("voltage: " + InvStr + " v");
        _voltageADCLabel->setText("voltage: " + InvStr + " v");
        _currentLabel->setText("current: " + InvStr + " A");
        _averageCurrentLabel->setText("averageCurrent: " + InvStr + " A");
        _relativeStateOfChargeLabel->setText("relativeStateOfCharge: " + InvStr + " %");
        _absoluteStateOfChargeLabel->setText("absoluteStateOfCharge: " + InvStr + " %");
        _remainingCapacityLabel->setText("remainingCapacity: " + InvStr + " mAh");
        _runTimeToEmptyLabel->setText("runTimeToEmpty: " + InvStr + " min");
        _averageTimeToEmptyLabel->setText("averageTimeToEmpty: " + InvStr + " min");
        _averageTimeToFullLabel->setText("averageTimeToFull: " + InvStr + " min");
        _statusLabel->setText("status: " + InvStr);
        _modeLabel->setText("mode: " + InvStr);
        break;
    }

    case BATTERY_COM_FAULT:
        _temperatureLabel->setText("temperature: " + InvStr + " °K");
        _voltageLabel->setText("voltage: " + InvStr + " v");

        valueInt = powerManager.batteryMessage.getVoltageADC();
        str.sprintf("%.3f", valueInt / 1000.0);
        _voltageADCLabel->setText("voltage ADC: " + str + " v");

        _currentLabel->setText("current: " + InvStr + " A");
        _averageCurrentLabel->setText("averageCurrent: " + InvStr + " A");
        _relativeStateOfChargeLabel->setText("relativeStateOfCharge: " + InvStr + " %");
        _absoluteStateOfChargeLabel->setText("absoluteStateOfCharge: " + InvStr + " %");
        _remainingCapacityLabel->setText("remainingCapacity: " + InvStr + " mAh");
        _runTimeToEmptyLabel->setText("runTimeToEmpty: " + InvStr + " min");
        _averageTimeToEmptyLabel->setText("averageTimeToEmpty: " + InvStr + " min");
        _averageTimeToFullLabel->setText("averageTimeToFull: " + InvStr + " min");
        _statusLabel->setText("status: " + InvStr);
        _modeLabel->setText("mode: " + InvStr);
        break;

    default:
        break;
    }
}
