#include <QTimer>
#include "ECG12LDataAcquire.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "SystemManager.h"
#include "NetworkManager.h"
#include "TimeDate.h"
#include "ECG12LDataMessNote.h"


ECG12LDataAcquire* ECG12LDataAcquire::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECG12LDataAcquire::ECG12LDataAcquire() : PopupWidget(true)
{
    _acquiring = false;
    _transtype = ECG12Lead_TYPE_USB;

    int fontSize = 15;
    int width = windowManager.getPopMenuWidth() * 8 / 10;

    QPalette p;
    p.setColor(QPalette::Window, Qt::white);
    setPalette(p);

    _title = new QLabel(trs("ECGAcquiring12L"));
    _title->setFont(fontManager.textFont(fontSize));

    _bar = new QProgressBar();
    _bar->setStyleSheet("QProgressBar{background:black;border-radius:5px;} \
                        QProgressBar::chunk{background:green;border-radius:5px;}");
    _bar->setTextVisible(false);
    _bar->setFixedWidth(width * 8 / 10);
    _bar->setValue(0);

    _cancelBtn = new IButton();
    _cancelBtn->setText(trs("Cancel"));
    _cancelBtn->setFont(fontManager.textFont(fontSize));
    _cancelBtn->setFixedWidth(width / 5);
    _cancelBtn->setBorderEnabled(true);
    connect(_cancelBtn, SIGNAL(realReleased()), this, SLOT(_cancelBtnSlot()));

    _transferBtn = new IDropList(trs("Transfer"));
    _transferBtn->setVisible(false);
    _transferBtn->setFixedSize(100, RESCUE_HELP_ITEM_H);
    _transferBtn->setFont(fontManager.textFont(fontSize));
    _transferBtn->addItem(trs("USB"));
    /* 当且仅当系统支持WIFI切WIFI功能开启时，才支持E-mail传输 */
    if(systemManager.isSupport(CONFIG_WIFI) && NetworkManager::isWifiTurnOn())
    {
        _transferBtn->addItem(trs("EMAIL"));
    }
    _transferBtn->addItem("");
    connect(_transferBtn, SIGNAL(currentIndexChange(int)), this, SLOT(_transBtnSlot(int)));

    _printBtn = new IButton();
    _printBtn->setText(trs("Print"));
    _printBtn->setFont(fontManager.textFont(fontSize));
    _printBtn->setFixedWidth(width / 5);
    _printBtn->setBorderEnabled(true);
    _printBtn->setVisible(false);
    connect(_printBtn, SIGNAL(realReleased()), this, SLOT(_printBtnSlot()));

    _closeBtn = new IButton();
    _closeBtn->setText(trs("Close"));
    _closeBtn->setFont(fontManager.textFont(fontSize));
    _closeBtn->setFixedWidth(width / 5);
    _closeBtn->setVisible(false);
    _closeBtn->setBorderEnabled(true);
    connect(_closeBtn, SIGNAL(realReleased()), this, SLOT(_closeBtnSlot()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(width / 5, 0, width / 5, 0);
    hlayout->setSpacing(0);
    hlayout->setMargin(5);
    hlayout->addWidget(_cancelBtn);
    hlayout->addWidget(_transferBtn);
    hlayout->addWidget(_printBtn);
    hlayout->addWidget(_closeBtn);

    contentLayout->setSpacing(0);
    contentLayout->addWidget(_title, 5, Qt::AlignHCenter);
    contentLayout->addWidget(_bar, 5, Qt::AlignHCenter);
    contentLayout->addLayout(hlayout);

    _timer = new QTimer();
    connect(_timer, SIGNAL(timeout()), this, SLOT(_setBarValue()));

    setFixedWidth(width);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECG12LDataAcquire::~ECG12LDataAcquire()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/**************************************************************************************************
 * 数据初始化。
 *************************************************************************************************/
void ECG12LDataAcquire::dataInit()
{
    _acquiring = false;
    _timer->stop();
    _bar->setValue(0);

    _cancelBtn->setVisible(true);
    _transferBtn->setVisible(false);
    _printBtn->setVisible(false);
    _closeBtn->setVisible(false);

    return ;
}

/**************************************************************************************************
 * 是否正在获取。
 *************************************************************************************************/
bool ECG12LDataAcquire::isAcquiring()
{
    return _acquiring;
}

/**************************************************************************************************
 * 获取进度条的值。
 *************************************************************************************************/
unsigned char ECG12LDataAcquire::getBarValue()
{
    return _bar->value();
}

/**************************************************************************************************
 * 退出获取。
 *************************************************************************************************/
void ECG12LDataAcquire::_exit()
{
    _acquiring = false;
    exitButtonRealsed();
    int value = _bar->value();
    if (value == 100)
    {
        _cancelBtn->setVisible(true);
        _transferBtn->setVisible(false);
        _printBtn->setVisible(false);
        _closeBtn->setVisible(false);
        _title->setText(trs("ECGAcquiring12L"));
    }
    else
    {
        _timer->stop();
    }

    _bar->setValue(0);

    return ;
}

/**************************************************************************************************
 * 显示事件
 *************************************************************************************************/
void ECG12LDataAcquire::showEvent(QShowEvent *e)
{
    _t = timeDate.time();

    _acquiring = true;
    _timer->start(1000);
    PopupWidget::showEvent(e);
}

/**************************************************************************************************
 * 隐藏事件
 *************************************************************************************************/
void ECG12LDataAcquire::hideEvent(QHideEvent */*e*/)
{
    _exit();

    return;
}

/**************************************************************************************************
 * 设置进度条。
 *************************************************************************************************/
void ECG12LDataAcquire::_setBarValue()
{
    int value = _bar->value();

    if (timeDate.difftime(timeDate.time(), _t) >= 1)
    {
        value += 10;
        if (value >= 100)
        {
            value = 100;
        }
    }
    _bar->setValue(value);

    if(100 == value)
    {
        _acquiring = false;
        _timer->stop();

        _cancelBtn->setVisible(false);
        _transferBtn->setVisible(true);
        _printBtn->setVisible(true);
        _closeBtn->setVisible(true);
        _closeBtn->setFocus();

        _title->setText(trs("ECGAcquired12L"));

        emit acquire12LDataFinish();
    }
}

/**************************************************************************************************
 * 取消快照获取。
 *************************************************************************************************/
void ECG12LDataAcquire::_cancelBtnSlot()
{
    ecg12LDataMessNote.showMessage(DATA_ACQUIRE_CANCEL);

    _exit();

    return ;
}

/**************************************************************************************************
 * 传输快照。
 *************************************************************************************************/
void ECG12LDataAcquire::_transBtnSlot(int type)
{
    emit acquire12LDataTransfer(type);

    return;
}

/**************************************************************************************************
 * 打印快照。
 *************************************************************************************************/
void ECG12LDataAcquire::_printBtnSlot()
{
//    ecg12LeadManager.AcquireDataPrint();
    emit acquire12LDataPrint();

    return;
}

/**************************************************************************************************
 * 关闭弹出框。
 *************************************************************************************************/
void ECG12LDataAcquire::_closeBtnSlot()
{
    _exit();

    return ;
}

/**************************************************************************************************
 * 焦点左右聚焦操作。
 *************************************************************************************************/
void ECG12LDataAcquire::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}
