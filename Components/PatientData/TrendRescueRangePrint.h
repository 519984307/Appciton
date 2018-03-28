#pragma once
#include "PopupWidget.h"

//趋势营救事件范围选择打印界面
class IButton;
class LButtonEx;
class RescueDataListWidget;
class TrendRescueRangePrint : public PopupWidget
{
    Q_OBJECT

public:
    static TrendRescueRangePrint &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new TrendRescueRangePrint();
        }

        return *_selfObj;
    }

    static TrendRescueRangePrint *_selfObj;
    ~TrendRescueRangePrint();

    void getPrintPath(QStringList &list);

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _printReleased();
    void _upReleased();
    void _downReleased();
    void _updateWindowTitle();

private:
    TrendRescueRangePrint();

private:
    RescueDataListWidget *_listWidget;
    LButtonEx *_printBtn;
    LButtonEx *_backBtn;
    IButton *_up;
    IButton *_down;
};
#define trendRescueRangePrint (TrendRescueRangePrint::construction())
#define deleteTrendRescueRangePrint() (delete TrendRescueRangePrint::_selfObj)
