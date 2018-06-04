#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "FactoryTempMenu.h"
#include "Debug.h"
#include "IButton.h"
#include "IComboList.h"
#include "FactoryWindowManager.h"
#include "TEMPParam.h"
#include <QTimer>
#include "FactoryMaintainManager.h"

FactoryTempMenu *FactoryTempMenu::_selfObj = NULL;

QString FactoryTempMenu::_btnStr[10] =
{
    "TEMPCalibrate0",
    "TEMPCalibrate10",
    "TEMPCalibrate15",
    "TEMPCalibrate20",
    "TEMPCalibrate25",
    "TEMPCalibrate30",
    "TEMPCalibrate35",
    "TEMPCalibrate40",
    "TEMPCalibrate45",
    "TEMPCalibrate50"
};

QString FactoryTempMenu::_labelStr[10] =
{
    "TEMPCalibrate0is7409.3",
    "TEMPCalibrate10is4491.1",
    "TEMPCalibrate15is3541.3",
    "TEMPCalibrate20is2813.9",
    "TEMPCalibrate25is2252",
    "TEMPCalibrate30is1814.5",
    "TEMPCalibrate35is1471.1",
    "TEMPCalibrate40is1199.8",
    "TEMPCalibrate45is984",
    "TEMPCalibrate50is811.3"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTempMenu::FactoryTempMenu() : SubMenu(trs("TEMPCalibrate"))
{
    setDesc(trs("TEMPCalibrateDesc"));
    startLayout();
}

void FactoryTempMenu::layoutExec()
{
    int submenuW = factoryMaintainManager.getSubmenuWidth();
    int submenuH = factoryMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth - ITEM_H;


    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setAlignment(Qt::AlignTop);

    _tempChannel = new QLabel();
    _tempChannel->setFont(fontManager.textFont(fontSize));
    _tempChannel->setFixedSize(labelWidth, ITEM_H);
    _tempChannel->setAlignment(Qt::AlignRight);

    _tempValue = new QLabel("---");
    _tempValue->setFont(fontManager.textFont(fontSize));
    _tempValue->setFixedSize(labelWidth, ITEM_H);
    _tempValue->setAlignment(Qt::AlignRight);

    QGroupBox *_groupBox = new QGroupBox();
    _groupBox->setStyleSheet("border:none");
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    hLayout->addWidget(_tempChannel);
    hLayout->addWidget(_tempValue);
    _groupBox->setLayout(hLayout);

    _tempError = new QLabel();
    _tempError->setFont(fontManager.textFont(fontSize));
    _tempError->setAlignment(Qt::AlignRight);

    _stackedwidget = new QStackedWidget();
    _stackedwidget->setFixedHeight(ITEM_H);
    _stackedwidget->addWidget(_groupBox);
    _stackedwidget->addWidget(_tempError);
    labelLayout->addWidget(_stackedwidget);

    hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignHCenter);
    _channel = new IComboList(trs("TEMPChannel"));
    _channel->setFont(fontManager.textFont(fontSize));
    _channel->setLabelAlignment(Qt::AlignHCenter);
    _channel->label->setFixedSize(labelWidth, ITEM_H);
    _channel->combolist->setFixedSize(btnWidth, ITEM_H);
    _channel->addItem(trs("TEMP1"));
    _channel->addItem(trs("TEMP2"));
    connect(_channel->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_channelReleased(int)));
    hLayout->addWidget(_channel,0,Qt::AlignHCenter);
//    hLayout->addSpacing(10);
    hLayout->setContentsMargins(0, 0,0, 0);
    labelLayout->addLayout(hLayout);

    _success = QImage("/usr/local/iDM/icons/select.png");
    _success = _success.scaled(20, 20);
    _fault = QImage("/usr/local/iDM/icons/cancel.png");
    _fault = _fault.scaled(20, 20);

    for (int i = 0; i < 10; ++i)
    {
        hLayout = new QHBoxLayout();
        hLayout->setAlignment(Qt::AlignRight);

        lbtn[i] = new LabelButton(trs(_labelStr[i]));
        lbtn[i]->button->setText(trs(_btnStr[i]));
        lbtn[i]->setFont(fontManager.textFont(fontSize));
        lbtn[i]->label->setFixedSize(labelWidth, ITEM_H);
        lbtn[i]->label->setAlignment(Qt::AlignRight);
        lbtn[i]->button->setFixedSize(btnWidth, ITEM_H);
        lbtn[i]->button->setID(i);
        connect(lbtn[i]->button, SIGNAL(released(int)), this, SLOT(_btnReleased(int)));
        hLayout->addWidget(lbtn[i],0,Qt::AlignRight);

        _calibrateResult[i] = new QLabel();
        _calibrateResult[i]->setFixedSize(ITEM_H, ITEM_H);
        hLayout->addWidget(_calibrateResult[i]);

        labelLayout->addLayout(hLayout);
    }

    labelLayout->addStretch();

    mainLayout->addLayout(labelLayout);

    _timer = new QTimer();
    _timer->setInterval(2000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

}

void FactoryTempMenu::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    if (NULL != _timer)
    {
        _timer->stop();
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTempMenu::readyShow()
{
    _channel->setFocus();
    _channel->setCurrentIndex(0);
    _calibrateChannel = 0;
    _tempChannel->setText(trs("TEMP1"));

    for (int i = 0; i < 10; ++i)
    {
        _calibrateResult[i]->setPixmap(QPixmap::fromImage(QImage()));
    }

    if (tempParam.getErrorDisable())
    {
        showError(trs("TEMPModuleDisable"));
    }
    if (tempParam.getDisconnected())
    {
        showError(trs("TEMPCommunicationStop"));
    }
}

/**************************************************************************************************
 * 功能:获取校准结果
 *************************************************************************************************/
void FactoryTempMenu::getResult(int channel, int value, bool flag)
{
    _timer->stop();
    int index = _calibrateValue;
    if (_calibrateChannel == channel && _calibrateValue == value)
    {
        if (flag)
        {
            _calibrateResult[index]->setPixmap(QPixmap::fromImage(_success));
        }
        else
        {
            _calibrateResult[index]->setPixmap(QPixmap::fromImage(_fault));
        }
    }
    else
    {
        _calibrateResult[index]->setPixmap(QPixmap::fromImage(_fault));
    }
}

/**************************************************************************************************
 * 功能:显示体温值
 *************************************************************************************************/
void FactoryTempMenu::setTEMPValue(short t1, short t2)
{
    QString str;
    if (_channel->currentIndex() == 0)
    {
        str.sprintf("%.1f", t1 / 10.0);
    }
    else
    {
        str.sprintf("%.1f", t2 / 10.0);
    }
    _tempValue->setText(str);
    _stackedwidget->setCurrentIndex(0);
}

/**************************************************************************************************
 * 功能:错误信息显示
 *************************************************************************************************/
void FactoryTempMenu::showError(QString str)
{
    _tempError->setText(str);
    _stackedwidget->setCurrentIndex(1);
}

/**************************************************************************************************
 * 功能:更改体温通道
 *************************************************************************************************/
void FactoryTempMenu::_channelReleased(int channel)
{
    _calibrateChannel = channel;
    if (channel == 0)
    {
        _tempChannel->setText(trs("TEMP1"));
    }
    else
    {
        _tempChannel->setText(trs("TEMP2"));
    }
    for (int i = 0; i < 10; ++i)
    {
        _calibrateResult[i]->setPixmap(QPixmap::fromImage(QImage()));
    }
}

/**************************************************************************************************
 * 功能:按钮释放
 *************************************************************************************************/
void FactoryTempMenu::_btnReleased(int value)
{
    _calibrateValue = value;
    tempParam.sendCalibrateData(_calibrateChannel,_calibrateValue);
    _timer->start();
}

/**************************************************************************************************
 * 功能:超时
 *************************************************************************************************/
void FactoryTempMenu::_timeOut()
{
    _calibrateResult[_calibrateValue]->setPixmap(QPixmap::fromImage(_fault));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTempMenu::~FactoryTempMenu()
{

}

