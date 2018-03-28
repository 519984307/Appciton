#pragma once
#include "PopupWidget.h"
#include "ITableWidget.h"

class IButton;

//可能用到wifi压缩包
struct EMAILCOMPRESSION
{
    QMap<QString, QByteArray> content;
    int addressIndex;
};

//email 信息
struct EMAILINFO
{
    QStringList fileName;
    int addressIndex;
};

//压缩打包
struct CompressPackage
{
    QString packageData;
    bool isrealTime;
    EMAILINFO wifiPackage;
};


//压缩过程的错误码

class MailAddressWidget : public PopupWidget
{
    Q_OBJECT

public:
    static MailAddressWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new MailAddressWidget();
        }

        return *_selfObj;
    }
    static MailAddressWidget *_selfObj;
    ~MailAddressWidget();

    int getMaxWidth() const {return _maxWidth - 20;}
    int getMaxHeight() const {return _maxHeight - 30;}

    //获取E-mail地址偏移量
    int getEmailAddressOffset();

    //获取E-mail地址个数
    int getCheckFlagCount() {return _checkFlag.count();}

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif

private:
    //获取mail地址
    void _showMailPath();
    void _loadData();
    bool _isRealTimeTransfer(const QVariant &para);

protected:
    void showEvent(QShowEvent *e);

private:
    MailAddressWidget();
    void _exit();

signals:
    void startToSend();

private slots:
    void _mailReleased();
    void _updateCheckFlag(int index);
    void keyPressEvent(QKeyEvent *e);

private:
    //RescueDataListWidget *_listWidget;
    ITableWidget *table;
    IButton *_email;

    int _totalData;                          // 数据的总个数。
    int _totalPage;                          // 总页数。
    int _currentPage;                        // 当前处在第几页。
    QVector<bool> _checkFlag;

    int _maxWidth;                           //最大宽度
    int _maxHeight;                          //最大高度
    int _addressoffest;                      //email 地址偏移
    static const int _rowNR = 5;             // 表行数。
    static const int _colNR = 4;             // 表列数。
    static const int _tableItemHeight = 29;  // 表格高度。
};
#define mailAddressWidget (MailAddressWidget::construction())
#define deletemailAddressWidget() (delete MailAddressWidget::_selfObj)
