#pragma once
#include "PopupWidget.h"

class ITableWidget;
class HistoryDataReviewWidget : public PopupWidget
{
    Q_OBJECT

public:
    static HistoryDataReviewWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new HistoryDataReviewWidget();
        }

        return *_selfObj;
    }

    static HistoryDataReviewWidget *_selfObj;
    ~HistoryDataReviewWidget();

    /**
     * @brief setHistoryReviewIndex 设置调用的历史数据所在的索引
     * @param index 索引位置
     * @param timeStr 历史数据时间字符串
     */
    void setHistoryReviewIndex(int index, QString timeStr);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _historyTimeReleased(void);
    void _trendTableBtnReleased(void);
    void _trendGraphBtnReleased(void);
    void _eventReviewBtnReleased(void);
    void _oxyCRGEventBtnReleased(void);

private:
    HistoryDataReviewWidget();

private:
    IButton *_trendTableBtn;
    IButton *_trendGraphBtn;
    IButton *_eventReviewBtn;
    IButton *_oxyCRGEventBtn;
    IButton *_historyTimeBtn;

    QString _selHistoryDataPath;

};

#define historyDataReviewWidget (HistoryDataReviewWidget::construction())
#define deleteHistoryDataReviewWidget() (delete HistoryDataReviewWidget::_selfObj)
