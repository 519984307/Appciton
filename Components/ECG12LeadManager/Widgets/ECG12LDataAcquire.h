#pragma once

#include "PopupWidget.h"
#include "IDropList.h"
#include "RescueDataReview.h"
#include "qprogressbar.h"
#include "IButton.h"
#include "ECG12LDataStorage.h"
#include "ECG12LExportDataWidget.h"

class ECG12LDataAcquire : public PopupWidget
{
    Q_OBJECT

public:
    static ECG12LDataAcquire &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LDataAcquire();
        }

        return *_selfObj;
    }

    static ECG12LDataAcquire *_selfObj;

    ECG12LDataAcquire();

    ~ECG12LDataAcquire();

public:
    //数据初始化
    void dataInit();

    //是否正在获取
    bool isAcquiring();

    //获取进度条的值
    unsigned char getBarValue();

    //退出获取
    void _exit();

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    QLabel *_title;                            //标题
    QProgressBar *_bar;                        //进度条
    IButton *_cancelBtn;                       //取消获取按钮
    IDropList *_transferBtn;                   //传送按钮
    IButton *_printBtn;                        //打印按钮
    IButton *_closeBtn;                        //关闭按钮
    QTimer *_timer;                            //定时器

signals:
    void acquire12LDataFinish();               //快照捕获完成信号
    void acquire12LDataTransfer(int type);     //快照捕获传输信号
    void acquire12LDataPrint();                //快照捕获打印信号

private slots:
    void _setBarValue();                       //设置进度条槽函数
    void _cancelBtnSlot();                     //取消按钮槽函数
    void _transBtnSlot(int type);             //传输按钮槽函数
    void _printBtnSlot();                      //打印按钮槽函数
    void _closeBtnSlot();                      //关闭按钮槽函数
    void keyPressEvent(QKeyEvent *e);          //按键处理

private:
    bool _acquiring;                           //是否正在传送
    TransferTypeECG12Lead _transtype;          //传输类型

    unsigned int _t;
};

#define ecg12LDataAcquire (ECG12LDataAcquire::construction())
#define deleteecg12LDataAcquire() (delete ECG12LDataAcquire::_selfObj)
