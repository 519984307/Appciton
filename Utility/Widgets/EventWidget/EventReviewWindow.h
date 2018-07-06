#pragma once
#include "PopupWidget.h"
#include <QScopedPointer>
#include <QStackedLayout>
#include <QModelIndex>
#include "EventDataDefine.h"
#include "Alarm.h"

class EventReviewWindowPrivate;
class EventReviewWindow : public PopupWidget
{
    Q_OBJECT
public:
    static EventReviewWindow &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new EventReviewWindow();
        }

        return *_selfObj;
    }

    static EventReviewWindow *_selfObj;
    ~EventReviewWindow();

    void eventInfoUpdate(void);
    void eventTrendUpdate(void);
    void eventWaveUpdate(void);

    void setWaveSpeed(int);
    void setWaveGain(int);

    /**
     * @brief setHistoryDataPath 设置历史回顾数据的文件路径
     * @param path 文件路径
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief setHistoryData 此刻的趋势表是否为历史数据
     * @param flag 标志
     */
    void setHistoryData(bool flag);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _waveInfoReleased(QModelIndex index);
    void _eventListReleased(void);
    void _upMoveEventReleased(void);
    void _downMoveEventReleased(void);
    void _eventTypeSelect(int);
    void _eventLevelSelect(int);
    void _leftMoveCoordinate(void);
    void _rightMoveCoordinate(void);
    void _leftMoveEvent(void);
    void _rightMoveEvent(void);
    void _setReleased(void);
    void _upReleased(void);
    void _downReleased(void);
    void _printRelease(void);

private:
    void _loadEventData(void);
    AlarmPriority _levelToPriority(EventLevel);

private:
    EventReviewWindow();
    QScopedPointer<EventReviewWindowPrivate> d_ptr;
};
#define eventReviewWindow (EventReviewWindow::construction())
#define deleteEventReviewWindow() (delete EventReviewWindow::_selfObj)
