#pragma once
#include "PopupWidget.h"
#include "RescueDataDefine.h"

//趋势营救事件范围选择打印界面
class IButton;
class IDropList;
class LButtonEx;
class RescueDataListWidget;
class SummaryRescueRangePrint : public PopupWidget
{
    Q_OBJECT

public:
    static SummaryRescueRangePrint &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SummaryRescueRangePrint();
        }

        return *_selfObj;
    }

    static SummaryRescueRangePrint *_selfObj;
    ~SummaryRescueRangePrint();

    // 获取路径
    void getPrintPath(QStringList &list);

    // 获取打印类型
    SummaryDataPrintType getPrintType();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _printReleased(int);
    void _printBtnRelease();
    void _upReleased();
    void _downReleased();
    void _updateWindowTitle();

private:
    SummaryRescueRangePrint();

private:
    RescueDataListWidget *_listWidget;
    IDropList *_print;
    LButtonEx *_incidentBtn;
    LButtonEx *_eventLogBtn;
    LButtonEx *_selectSnapshotBtn;
    SummaryDataPrintType _printType;

    IButton *_up;
    IButton *_down;
};
#define summaryRescueRangePrint (SummaryRescueRangePrint::construction())
#define deleteSummaryRescueRangePrint() (delete SummaryRescueRangePrint::_selfObj)
