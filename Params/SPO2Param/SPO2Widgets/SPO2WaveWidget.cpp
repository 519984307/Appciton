/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#include "SPO2WaveWidget.h"
#include "SPO2Param.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "PopupList.h"
#include "IConfig.h"
#include "Debug.h"
#include "WindowManager.h"
#include "SystemManager.h"
#include <QPainter>

/**************************************************************************************************
 * wave is enable。
 *************************************************************************************************/
bool SPO2WaveWidget::waveEnable()
{
    return spo2Param.isEnabled();
}

/**************************************************************************************************
 * 响应窗体大小调整事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void SPO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    if (!_name || !_notify)
    {
        return;
    }

    _name->move(0, 0);

    // 设定为固定的高度和宽度后，居中显示。
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void SPO2WaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    _name->setFocus();
}

void SPO2WaveWidget::paintEvent(QPaintEvent *e)
{
    WaveWidget::paintEvent(e);

    if (!spo2Param.isShowSignalIQ() || bufIsEmpty())
    {
        return;
    }

    // draw signal IQ
    QPainter painter(this);
    QPen oldPen = painter.pen();
    QPen pen(Qt::white);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    // 获得需要更新的区域
    QVector<QRect> rects = e->region().rects();
    foreach(QRect rect, rects)
    {
        int leftIndex = qMax(0, xToIndex(rect.left() - 2) - 1);
        int rightIndex = qMin(bufSize() - 1, xToIndex(rect.right() + 2) + 1);
        int beginIndex;
        int endIndex;
        if (bufHead() > bufTail())
        {
            // 缓冲连续情况
            beginIndex = qMax(leftIndex, bufTail());
            endIndex = qMin(rightIndex, bufHead() - 1);
            bool drawFlag = true;
            for (int i = beginIndex; i <= endIndex; i++)
            {
                int flag = flagBuf(i);
                unsigned char IQFlag = flag & 0xff;
                if ((IQFlag) & SPO2_IQ_FLAG_BIT)
                {
                    if (drawFlag)   // 一个波峰只画一次IQ
                    {
                        // has IQ value,以波形高度为100%来绘画
                        unsigned char iqValue = flag & (~SPO2_IQ_FLAG_BIT); // IQ值（%）
                        int pointY = this->rect().height() * iqValue / 100;
                        QPoint start(xBuf(i), this->rect().height() - pointY);
                        QPoint end(xBuf(i), this->rect().height() - 5);
                        painter.drawLine(start, end);
                        drawFlag = false;
                    }
                }
                else
                {
                    drawFlag = true;
                }
            }
            // draw IQ base Line
            QPoint start(xBuf(beginIndex), this->rect().height() - 5);
            QPoint end(xBuf(endIndex), this->rect().height() - 5);
            painter.drawLine(start, end);
        }
        else
        {
            // 缓冲不连续情况
            // 绘画ringBuff前一轮的数据
            beginIndex = qMax(leftIndex, bufTail());
            endIndex = qMin(rightIndex, bufSize() - 1);
            bool drawFlag = true;
            for (int i = beginIndex; i <= endIndex; i++)
            {
                int flag = flagBuf(i);
                unsigned char IQFlag = flag & 0xff;
                if ((IQFlag) & SPO2_IQ_FLAG_BIT)
                {
                    if (drawFlag)
                    {
                        // has IQ value,以波形高度为100%来绘画
                        unsigned char iqValue = flag & (~SPO2_IQ_FLAG_BIT); // IQ值（%）
                        int pointY = this->rect().height() * iqValue / 100;
                        QPoint start(xBuf(i), this->rect().height() - pointY);
                        QPoint end(xBuf(i), this->rect().height() - 5);
                        painter.drawLine(start, end);
                        drawFlag = false;
                    }
                }
                else
                {
                    drawFlag = true;
                }
            }

            // 绘画ringBuff下一轮的数据
            beginIndex = qMax(leftIndex, 0);
            endIndex = qMin(rightIndex, bufHead() - 1);
            drawFlag = true;
            for (int i = beginIndex; i <= endIndex; i++)
            {
                int flag = flagBuf(i);
                unsigned char IQFlag = flag & 0xff;
                if ((IQFlag) & SPO2_IQ_FLAG_BIT)
                {
                    if (drawFlag)
                    {
                        // has IQ value,以波形高度为100%来绘画
                        unsigned char iqValue = flag & (~SPO2_IQ_FLAG_BIT); // IQ值（%）
                        int pointY = this->rect().height() * iqValue / 100;
                        QPoint start(xBuf(i), this->rect().height() - pointY - 10);
                        QPoint end(xBuf(i), this->rect().height() - 5);
                        painter.drawLine(start, end);
                        drawFlag = false;
                    }
                    else
                    {
                        drawFlag = true;
                    }
                }
            }
            // draw IQ base Line
            if (bufHead() != bufTail())
            {
                QPoint start(xBuf(beginIndex), this->rect().height() - 5);
                QPoint end(xBuf(endIndex), this->rect().height() - 5);
                painter.drawLine(start, end);
            }
        }
    }
    painter.setPen(oldPen);
}

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void SPO2WaveWidget::loadConfig(void)
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);

    SPO2WaveVelocity speed = (SPO2WaveVelocity)spo2Param.getSweepSpeed();

    if (speed == SPO2_WAVE_VELOCITY_62D5)
    {
        setWaveSpeed(6.25);
    }
    else if (speed == SPO2_WAVE_VELOCITY_125)
    {
        setWaveSpeed(12.5);
    }
    else if (speed == SPO2_WAVE_VELOCITY_250)
    {
        setWaveSpeed(25.0);
    }
}

void SPO2WaveWidget::setNotify(bool enable, QString str)
{
    // demo模式notify下强制更新为正常标志
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        enable = false;
    }

    if (enable)
    {
        _notify->setVisible(true);
        _notify->setText(str);
    }
    else
    {
        if (_notify->text() == str || str == " ")
        {
            // 清空和当前字符串相同的，或者字符为空的情况
            _notify->setVisible(false);
            _notify->setText(" ");
        }
    }
}


/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
SPO2WaveWidget::SPO2WaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title)
    , _notify(NULL)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_SPO2);
//    setValueRange(64, 192);

    int fontSize = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, fontH);
    _name->setText(title);

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(fontSize));
    _notify->setFocusPolicy(Qt::NoFocus);
    _notify->setFixedSize(300, fontH);
    _notify->setText("");
    _notify->setVisible(false);
    addItem(_notify);
    // 加载配置
    loadConfig();

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
SPO2WaveWidget::~SPO2WaveWidget()
{
}
