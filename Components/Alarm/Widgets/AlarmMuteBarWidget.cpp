#include "AlarmMuteBarWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "AlarmIndicator.h"
#include "AlarmInfoPopListView.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QPushButton>

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmMuteBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);

    // 显示正常报警。
    if (_audioStatus == ALARM_AUDIO_NORMAL)
    {
        painter.fillRect(rect(), Qt::SolidPattern);
        return;
    }

    // 显示报警暂停/停止信息。
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);

    int border =  focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);

    int offx = (rect().width() - (rect().height() - 10)) / 2;
    QRect r = rect().adjusted(offx, 5, -offx, -5);
    if (_audioStatus == ALARM_AUDIO_OFF)
    {
        painter.drawPixmap(r, _muteAudioOff, _muteAudioOff.rect());
    }
    else if (_audioStatus == ALARM_OFF)
    {
        painter.drawPixmap(r, _muteAlarmOff, _muteAlarmOff.rect());
    }
    else if (_audioStatus == ALARM_AUDIO_SUSPEND)
    {
        painter.drawPixmap(r, _muteAlarmPause, _muteAlarmPause.rect());
    }
}

/**************************************************************************************************
 * 设置报警静音关闭状态。
 *************************************************************************************************/
void AlarmMuteBarWidget::setAudioNormal(void)
{
    _audioStatus = ALARM_AUDIO_NORMAL;
    update();
}

/**************************************************************************************************
 * 设置报警音停止打开状态。
 *************************************************************************************************/
void AlarmMuteBarWidget::setAudioOff(void)
{
    _audioStatus = ALARM_AUDIO_OFF;
    update();
}

/**************************************************************************************************
 * 设置报警停止打开状态。
 *************************************************************************************************/
void AlarmMuteBarWidget::setAlarmOff(void)
{
    _audioStatus = ALARM_OFF;
    update();
}

/**************************************************************************************************
 * 设置报警暂停。
 *************************************************************************************************/
void AlarmMuteBarWidget::setAlarmPause(void)
{
    _audioStatus = ALARM_AUDIO_SUSPEND;
    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmMuteBarWidget::AlarmMuteBarWidget() : IWidget("AlarmMuteBarWidget")
{
    setFocusPolicy(Qt::NoFocus);

    _muteAlarmPause.load("/usr/local/nPM/icons/AlarmAudioPause.png");
    _muteAudioOff.load("/usr/local/nPM/icons/AlarmAudioOff.png");
    _muteAlarmOff.load("/usr/local/nPM/icons/AlarmOff.png");

    _audioStatus = ALARM_AUDIO_NORMAL;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmMuteBarWidget::~AlarmMuteBarWidget()
{

}
