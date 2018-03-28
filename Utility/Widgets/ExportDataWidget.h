#pragma once
#include <QDialog>
#include "LanguageManager.h"
#include "qprogressbar.h"

//数据类型及传输方式
enum Export_Data_Type
{
    EXPORT_RESCUE_DATA_BY_USB,       //通过U盘导出营救数据
    EXPORT_RESCUE_DATA_BY_SFTP,      //通过WIFI导出营救数据
    EXPORT_CONFIG_DATA_BY_USB,       //通过U盘导出配置数据
    EXPORT_CONFIG_DATA_BY_WIFI,      //通过WIFI导出配置数据
    EXPORT_ECG12LEAD_DATA_BY_USB,    //通过U盘导出12导心电数据
    EXPORT_ECG12LEAD_DATA_BY_WIFI,    //通过WIFI导出12导心电数据
    EXPORT_ERROR_LOG_BY_USB,         //Export ErrorLog through USB
    EXPORT_DATA_NR
};

//传输开始标题
const QString startTitleString[EXPORT_DATA_NR] =
{
    trs("TransferRescueDataByUSB"),
    trs("TransferRescueDataBySFTP"),
    trs("TransferSupervisorByUSB"),
    trs("TransferSupervisorByWIFI"),
    trs("TransferECG12LeadByUSB"),
    trs("TransferECG12LeadByWIFI"),
    trs("TransfeErrorLogToUSB"),
};

//传输结束标题
const QString endTitleString[EXPORT_DATA_NR] =
{
    trs("TransferRescueDataComplete"),
    trs("TransferRescueDataComplete"),
    trs("TransferSupervisorComplete"),
    trs("TransferSupervisorComplete"),
    trs("TransferECG12LeadComplete"),
    trs("TransferECG12LeadComplete"),
    trs("TransferErrorLogToUSBComplete"),
};

//传输开始信息
const QString startInfoString[EXPORT_DATA_NR] =
{
    trs("NotDisconnectUSB"),
    trs("TransferRescueDataBySFTP"),
    trs("NotDisconnectUSB"),
    trs("TransferRescueDataByWIFI"),
    trs("NotDisconnectUSB"),
    trs("TransferRescueDataByWIFI"),
};

//传输结束信息
const QString endInfoString[EXPORT_DATA_NR] =
{
    trs("RemoveUSB"),
    trs("SucceedRescueDataBySFTP"),
    trs("RemoveUSB"),
    trs("SucceedRescueDataByWIFI"),
    trs("RemoveUSB"),
    trs("SucceedRescueDataByWIFI"),
};

//数据传输基类
class QLabel;
class LButton;
class QProgressBar;
class QTimer;
class ExportDataWidget : public QDialog
{
    Q_OBJECT

public:
    ExportDataWidget(Export_Data_Type type);
    ~ExportDataWidget();

    //是否传输取消
    bool isTransferCancel() const {return _transferCancel;}

    //获取进度条的值
    int getBarValue() {return _bar->value();}

    //初始化进度条的值
    void init();

public slots:
    //设置进度条的值
    void setBarValue(unsigned char value);

protected:
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);

private slots:
    void _cancleOrOKPressed();

signals:
    void cancel();

protected:
    static const int _titleBarHeight = 30;

    QLabel *_title;
    QLabel *_info;
    QProgressBar *_bar;
    LButton *_cancleOrOK;

    bool _transferCancel;
    Export_Data_Type _curType;
};
