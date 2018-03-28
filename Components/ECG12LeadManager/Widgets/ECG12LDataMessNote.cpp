#include "ECG12LDataMessNote.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "TimeDate.h"
#include "MenuManager.h"

ECG12LDataMessNote *ECG12LDataMessNote::_selfObj = NULL;
//12L操作提示信息栏
const QString ECG12LNoteString[ECG_OPERATION_TIPS_NR] =
{
    trs("USBTransferHasCanceled"),
    trs("ECG12LeadAcquisitionHalted"),
    trs("ECG12LTransfersuccess"),
    trs("ECG12LTransferfail"),
    trs("TransferErrorLogtoUSB"),
};

/**************************************************************************************************
*构造函数
 *************************************************************************************************/
ECG12LDataMessNote::ECG12LDataMessNote():PopupWidget(true)
{
    int fontSize = 15;
    int width = windowManager.getPopMenuWidth() * 8 / 10;

    QPalette p;
    p.setColor(QPalette::Window, Qt::white);
    setPalette(p);

    //还没有制作翻译软件
    _title = new QLabel("");
    _title->setFont(fontManager.textFont(fontSize));
    _title->setFixedWidth(width * 8 / 10);
    _title->setFixedHeight(width * 2 / 10);
    _title->setAlignment(Qt::AlignVCenter |  Qt::AlignHCenter);


    QHBoxLayout *hlayout = new QHBoxLayout();
    contentLayout->addWidget(_title, 5, Qt::AlignHCenter);
    contentLayout->addLayout(hlayout);

    _timer = new QTimer();
    _timer->setInterval(1500);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_windowProcess()));
}

/**************************************************************************************************
*折构
**************************************************************************************************/
ECG12LDataMessNote::~ECG12LDataMessNote()
{
    if (_timer != NULL)
    {
        delete _timer;
    }
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ECG12LDataMessNote::showEvent(QShowEvent *e)
{
    _timer->start();
    PopupWidget::showEvent(e);
}

/**************************************************************************************************
 * 隐藏。
 *************************************************************************************************/
void ECG12LDataMessNote::hideEvent(QHideEvent */*e*/)
{
    _exit();
}

/**************************************************************************************************
 * 处理消息窗口
 * 参数:
 *value:设置值
 *************************************************************************************************/
void ECG12LDataMessNote::_windowProcess()
{
    hide();
}

/**************************************************************************************************
 * 退出
 *************************************************************************************************/
void ECG12LDataMessNote::_exit()
{
    _timer->stop();
}

/**************************************************************************************************
 * 设置标题
 *************************************************************************************************/
void ECG12LDataMessNote::settitle(ECGNoteType type)
{
    if (USB_CANCEL > type || (ECG_OPERATION_TIPS_NR <= type))
    {
        return ;
    }

    _curType = type;

    _title->setText(ECG12LNoteString[_curType]);
}


/**************************************************************************************************
 * 消息弹出框提示
 *************************************************************************************************/
void ECG12LDataMessNote::showMessage(ECGNoteType _curType)
{
    QRect r = windowManager.getMenuArea();
    settitle(_curType);
    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    ecg12LDataMessNote.show();
}
