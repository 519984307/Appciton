#pragma once
#include "PopupWidget.h"

//12L操作提示类型枚举
enum ECGNoteType
{
    USB_CANCEL,                      //U盘取消提示
    DATA_ACQUIRE_CANCEL,             //数据取消提示
    TRANSFER_OK,                     //传送成功
    TRANSFER_ERROR,                  //传送失败
    ERROR_LOG_BY_USB,                //Export ErrorLog through USB
    ECG_OPERATION_TIPS_NR
};
class QLabel;
/*
 * 描述：此类是用来处理 12导联数获取或者usb传送中断时后，给予用户的提示的，比如12倒获取数据中断提示： 12-Lead acqusition Halted
 * USB传输中断时候：提示USB has canceled tansfered etc.
 * **********************************************************************************************/

class ECG12LDataMessNote : public PopupWidget
{
    Q_OBJECT
public:
    static ECG12LDataMessNote &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LDataMessNote();
        }

        return *_selfObj;
    }

    static ECG12LDataMessNote *_selfObj;
    ~ECG12LDataMessNote();

public:

    void settitle( ECGNoteType _curType);
    void showMessage(ECGNoteType _curType);
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void _exit();

private:
    ECG12LDataMessNote();
    QLabel *_title;                // 标题
    QTimer *_timer;                // 定时器

private slots:
    void _windowProcess();

private:
    unsigned int _t;
    unsigned int _ms;
    ECGNoteType _curType;

};
#define ecg12LDataMessNote (ECG12LDataMessNote::construction())
#define deleteecg12LDataMessNote() (delete ECG12LDataMessNote::_selfObj)
