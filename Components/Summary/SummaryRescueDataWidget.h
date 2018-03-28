#pragma once
#include <QVector>
#include "RescueDataReview.h"

//概要报告
class IStorageBackend;
class SummaryRescueDataWidget : public RescueDataReview
{
    Q_OBJECT

public:
    static SummaryRescueDataWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SummaryRescueDataWidget();
        }

        return *_selfObj;
    }

    static SummaryRescueDataWidget *_selfObj;
    ~SummaryRescueDataWidget();

    // 获取打印路径
    void getPrintPath(QStringList &list);

    // 判断是否选择
    bool isChecked(int index);

    // return the checked flags of all items
    QVector<bool> getCheckFlags() const;

    // 获取打印类型
    SummaryDataPrintType getPrintType();

    //set the incident index
    void setIncidentIndex(int index);

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void loadData(PageLoadType);
    void loadRescueTime(QStringList &);
    void incidentChange(int index);
    void print(int type);
    void timeOut();
    void onPrintBtnRelease();
    void onBackBtnRelease();

private slots:
    void _updateCheckFlag(int index);

private:
    SummaryRescueDataWidget();
    void _calcInfo(void);
    void _loadData(void);
    void _refreshTitlePage(void);

private:
    int _totalData;                        // 数据的总个数。
    int _totalPage;                        // 总页数。
    int _currentPage;                      // 当前处在第几页。
    int _incidentIndex;                    //incident index
    QVector<bool> _checkFlag;              // 选择标志

    IStorageBackend *_file;

    static const int _rowNR = 8;             // 表行数。
    static const int _colNR = 3;            // 表列数。
    static const int _tableItemHeight = 29;  // 表格高度。
};
#define summaryRescueDataWidget (SummaryRescueDataWidget::construction())
#define deleteSummaryRescueDataWidget() (delete SummaryRescueDataWidget::_selfObj)
