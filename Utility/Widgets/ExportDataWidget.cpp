/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/11/30
 **/


#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QBitmap>
#include <QTimer>
#include "ExportDataWidget.h"
#include "FontManager.h"
#include "SystemManager.h"
#include "Debug.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainterPath>
#include "LayoutManager.h"
#include "Framework/UI/ThemeManager.h"

//传输开始标题
static const char *startTitleString[EXPORT_DATA_NR] =
{
    "TransferRescueDataByUSB",
    "TransferRescueDataBySFTP",
    "TransferSupervisorByUSB",
    "TransferSupervisorByWIFI",
    "TransferECG12LeadByUSB",
    "TransferECG12LeadByWIFI",
    "TransferErrorLogToUSB",
};

//传输结束标题
static const char *endTitleString[EXPORT_DATA_NR] =
{
    "TransferRescueDataComplete",
    "TransferRescueDataComplete",
    "TransferSupervisorComplete",
    "TransferSupervisorComplete",
    "TransferECG12LeadComplete",
    "TransferECG12LeadComplete",
    "TransferErrorLogToUSBComplete",
};

//传输开始信息
static const char *startInfoString[EXPORT_DATA_NR] =
{
    "NotDisconnectUSB",
    "TransferRescueDataBySFTP",
    "NotDisconnectUSB",
    "TransferRescueDataByWIFI",
    "NotDisconnectUSB",
    "TransferRescueDataByWIFI",
    "TransferErrorLogToUSB"
};

//传输结束信息
static const char *endInfoString[EXPORT_DATA_NR] =
{
    "RemoveUSB",
    "SucceedRescueDataBySFTP",
    "RemoveUSB",
    "SucceedRescueDataByWIFI",
    "RemoveUSB",
    "SucceedRescueDataByWIFI",
    "SucceedErrorLogByUSB"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ExportDataWidget::ExportDataWidget(Export_Data_Type type) : Dialog()
{
    getCloseBtn()->setHidden(true);
    setFixedHeight(200);
    if (type < EXPORT_DATA_NR)
    {
        _curType = type;
    }
    else
    {
        _curType = EXPORT_RESCUE_DATA_BY_USB;
    }
    setModal(true);

    int fontSize = fontManager.getFontSize(2);
    int width;
    width = themeManager.defaultWindowSize().width() * 8 / 10;

    setWindowTitle(trs(startTitleString[_curType]));

    _info = new QLabel(trs(startInfoString[_curType]));
    _info->setFont(fontManager.textFont(fontSize));

    _bar = new QProgressBar();
    _bar->setStyleSheet("QProgressBar{background:black;border-radius:5px;}"
                        "QProgressBar::chunk{background:green;border-radius:5px;}");
    _bar->setTextVisible(false);
    _bar->setFixedWidth(width * 8 / 10);

    _cancleOrOK = new Button();
    _cancleOrOK->setText(trs("Cancel"));
    _cancleOrOK->setButtonStyle(Button::ButtonTextOnly);
    _cancleOrOK->setFixedWidth(width / 5);
    _cancleOrOK->setFixedHeight(48);
    connect(_cancleOrOK, SIGNAL(released()), this, SLOT(_cancleOrOKPressed()));

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setMargin(5);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(_info, 0, Qt::AlignHCenter);
    contentLayout->addWidget(_bar, 0, Qt::AlignHCenter);
    contentLayout->addWidget(_cancleOrOK, 0, Qt::AlignHCenter);

    setWindowLayout(contentLayout);
    setFixedWidth(width);

    _bar->setValue(0);

    _transferCancel = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ExportDataWidget::~ExportDataWidget()
{
}

/**************************************************************************************************
 * 初始化进度条的值。
 *************************************************************************************************/
void ExportDataWidget::init()
{
    setWindowTitle(trs(startTitleString[_curType]));
    _info->setText(trs(startInfoString[_curType]));
    _cancleOrOK->setText(trs("Cancel"));
    _bar->setValue(0);
    _transferCancel = false;

    return;
}

/**************************************************************************************************
 * 设置传输进度。
 *************************************************************************************************/
void ExportDataWidget::setBarValue(unsigned char value)
{
    if (value > 100)
    {
        value = 100;
    }

    if (value == _bar->value())
    {
        return;
    }

    _bar->setValue(value);

    if (value == 100)
    {
        setWindowTitle(trs(endTitleString[_curType]));
        _info->setText(trs(endInfoString[_curType]));
        _cancleOrOK->setText(trs("Yes"));
    }
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void ExportDataWidget::showEvent(QShowEvent *e)
{
    _bar->setValue(0);
    Dialog::showEvent(e);

    QRect r;
    r = layoutManager.getMenuArea();
    // Y轴多往下移动20个像素，主要是为了不遮挡后面的界面
    move(r.x() + (r.width() - width()) / 2 , r.y() + (r.height() - height()) / 2 + 20);
}

/**************************************************************************************************
 * 取消OK按钮被按下。
 *************************************************************************************************/
void ExportDataWidget::_cancleOrOKPressed()
{
    if (_bar->value() < 100)
    {
        _transferCancel = true;
        emit cancel();
        done(0);
    }
    else
    {
        done(1);
    }

    hide();
}
