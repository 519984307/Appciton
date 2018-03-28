#pragma once
#include "PopupWidget.h"
#include "RescueDataDefine.h"

//帮助信息高度
#define RESCUE_HELP_ITEM_H (30)



//传输类型
enum TransferTypeECG12Lead
{
    ECG12Lead_TYPE_USB,
    ECG12Lead_TYPE_WIFI,
    ECG12Lead_TYPE_PRINT,
    ECG12Lead_TYPE_NR
};
class QTableWidget;
class IComboList;
class ComboList;
class IButton;
class LButtonEx;
class IDropList;
class ITableWidget;
class QTimer;
class RescueDataReview : public PopupWidget
{
    Q_OBJECT

public:
    RescueDataReview(RescueDataType type = TREND_RESCUE_DATA);
    ~RescueDataReview();

    int getMaxWidth() const {return _maxWidth - 14;}
    int getMaxHeight() const {return _maxHeight - RESCUE_HELP_ITEM_H - 2;}

    void setSolutionVisiable(bool isVisible);
    void setPrintVisiable(bool isVisible);
    void setPrintParamVisiable(bool isVisible);
    bool isPrintParamVisiable() const;
    void setIncidentVisiable(bool isVisible);
    void settransferTypeVisiable(bool isVisible);
    void setPrintBtnVisiable(bool isVisible);
    void setBackBtnVisiable(bool isVisible);
    bool isIncidentVisiable() const;
    bool isPrintVisiable() const;

    //添加打印类型
    void addPrintType(const QString typeStr);

    //添加打印参数
    void addPrintParam(const QString &paramStr);

    //获取打印类型
    int getPrintIndex();

    //获取营救索引
    int getIncidentIndex();

    //获取传输索引
    int getTransferIndex();

    //改变打印营救时间的抬头
    void printTitleSet(const QString &paramStr);

    //添加传送子项目
    void addTransfer(const QString &paramStr);

    ITableWidget *table;

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif
protected:
    void keyPressEvent(QKeyEvent *e);
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    virtual void loadData(PageLoadType pageType);
    virtual void loadRescueTime(QStringList &strList);
    virtual void solutionChange(int index);
    virtual void incidentChange(int index);
    virtual void printParamChange(int index);
    virtual void print(int type);
    virtual void timeOut();

    //纯虚函数 传输过程，让子类实现
    virtual bool TransferProcess(ECG12LEADTransferType type);

protected slots:
    virtual void onPrintBtnRelease();
    virtual void onBackBtnRelease();

private slots:
    void _upReleased(void);
    void _downReleased(void);
    void _printReleased(int);
    void _printParamReleased(int);
    void _rangeReleased(int);
    void _rescueTimeReleased(int);
    void _solutionTimeReleased(int);
    void _timeOutSlot();
    virtual void _transferTypeReleased(int);

private:
    void _readShow();

private:
    QTimer *_timer;
    IComboList *_incident;
    IDropList *_solution;
    IDropList *_print;
    IDropList *_transferType;
    IDropList *_printParam;
    LButtonEx *_printBtn;
    LButtonEx *_backBtn;
    IButton *_up;
    IButton *_down;

    RescueDataType _type;              //回顾数据类型
    RescueDataPrintType _selectType;   //打印类型
    ECG12LEADTransferType _transtype;  //传输类型
    int _maxWidth;                     //最大宽度
    int _maxHeight;                    //最大高度
};


