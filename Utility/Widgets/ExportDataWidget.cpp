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
#include "WindowManager.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "SystemManager.h"
#include "Debug.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainterPath>
#include "LayoutManager.h"


/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ExportDataWidget::ExportDataWidget(Export_Data_Type type) : QDialog(0, Qt::FramelessWindowHint)
{
    if (type < EXPORT_DATA_NR)
    {
        _curType = type;
    }
    else
    {
        _curType = EXPORT_RESCUE_DATA_BY_USB;
    }
    setAttribute(Qt::WA_NoSystemBackground); // draw background in paintEvent
    setModal(true);

    int fontSize = fontManager.getFontSize(1);
    int width;
    width = windowManager.getPopWindowWidth() * 8 / 10;


    QPalette p;
    p.setColor(QPalette::Window, Qt::white);
    setPalette(p);

    // 标题栏。
    _title = new QLabel(startTitleString[_curType]);
    _title->setAlignment(Qt::AlignCenter);
    _title->setFixedHeight(_titleBarHeight);
    _title->setFont(fontManager.textFont(fontSize, true));
    p.setColor(QPalette::Foreground, Qt::white);
    _title->setPalette(p);

    _info = new QLabel(startInfoString[_curType]);
    _info->setFont(fontManager.textFont(fontSize));

    _bar = new QProgressBar();
    _bar->setStyleSheet("QProgressBar{background:black;border-radius:5px;}QProgressBar::chunk{background:green;border-radius:5px;}");
    _bar->setTextVisible(false);
    _bar->setFixedWidth(width * 8 / 10);

    _cancleOrOK = new LButton();
    _cancleOrOK->setText(trs("Cancel"));
    _cancleOrOK->setFont(fontManager.textFont(fontSize));
    _cancleOrOK->setFixedWidth(width / 5);
    connect(_cancleOrOK, SIGNAL(realReleased()), this, SLOT(_cancleOrOKPressed()));

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setMargin(5);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(_title, 0, Qt::AlignHCenter);
    contentLayout->addWidget(_info, 0, Qt::AlignHCenter);
    contentLayout->addWidget(_bar, 0, Qt::AlignHCenter);
    contentLayout->addWidget(_cancleOrOK, 0, Qt::AlignHCenter);

    setLayout(contentLayout);
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
    _title->setText(startTitleString[_curType]);
    _info->setText(startInfoString[_curType]);
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
        _title->setText(endTitleString[_curType]);
        _info->setText(endInfoString[_curType]);
        _cancleOrOK->setText(trs("Yes"));
    }
}

/**************************************************************************************************
 * 绘画事件。
 *************************************************************************************************/
void ExportDataWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);


    // 绘制标题栏。
    QPainter barPainter(this);
    barPainter.setRenderHint(QPainter::Antialiasing);
    barPainter.setClipPath(clipPath);

    // 绘制边框。
    QPen pen;
    pen.setColor(colorManager.getBarBkColor());
    pen.setWidth(8);
    barPainter.setPen(pen);
    barPainter.setBrush(palette().window());
    barPainter.drawRect(rect());

    if (_title != NULL)
    {
        QRect r = rect();
        r.setBottom(_titleBarHeight);
        barPainter.fillRect(r, colorManager.getBarBkColor());
    }
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void ExportDataWidget::showEvent(QShowEvent *e)
{
    _bar->setValue(0);
    QDialog::showEvent(e);

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
