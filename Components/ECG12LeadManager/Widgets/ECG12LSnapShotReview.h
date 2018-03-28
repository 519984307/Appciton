#pragma once
#include <QVector>
#include "IStorageBackend.h"
#include "RescueDataReview.h"
#include "ECG12LDataStorage.h"

//12导快照数据
class ECG12LSnapShotReview : public RescueDataReview
{
    Q_OBJECT

public:
    static ECG12LSnapShotReview &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LSnapShotReview();
        }

        return *_selfObj;
    }

    static ECG12LSnapShotReview *_selfObj;
    ~ECG12LSnapShotReview();

public:
    //数据初始化
    void dataInit();

    //判断是否选择。
    bool isChecked(int index);

    // 获取打印类型
    ECG12LeadDataPrintType getPrintType();

    //获取传输类型
    ECG12LEADTransferType getTransferType();

    //获取快照选择标记
    QVector<bool> getSnapShotCheckFlag();

    //是否有选中的的快照
    bool _isSelectSnapShot();

    //获取快照选择的数量
    int _getSelectSnapShotCount();

    //打印
    void print(int type);

    //计算相关的控制成员
    void _calControlMemberInfo(void);

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif

private:
    ECG12LSnapShotReview();

    //载入当前页的数据
    void _loadCurrentPageData(void);

    //刷新列表标题
    void _refreshTitlePage(void);

signals:
    //打印快照信号
    void snapShotReviewPrint(int type);
    //传输快照信号
    void snapShotReviewTransfer(int type);

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void loadData(PageLoadType);
    void loadRescueTime(QStringList &);
    void incidentChange(int index);
    void timeOut();

private slots:
    void _updateCheckFlag(int index);
    virtual void _transferTypeReleased(int);

private:
    ECG12LDataStorage::ECG12LeadData *_data;

    int _totalData;                        // 数据的总个数。
    int _totalPage;                        // 总页数。
    int _currentPage;                      // 当前处在第几页。
    int _snapShotTimeInedx;                //快照当前的趋势
    QVector<bool> _checkFlag;              //快照选择标记
    IStorageBackend *_file;

    static const int _rowNR = 8;             // 表行数。
    static const int _colNR = 3;            // 表列数。
    static const int _tableItemHeight = 29;  // 表格高度。
};

#define ecg12LSnapShotReview (ECG12LSnapShotReview::construction())
#define deleteecg12LSnapShotReview() (delete ECG12LSnapShotReview::_selfObj)
