/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/28
 **/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStylePainter>
#include "WindowWidget.h"
#include "WindowManager.h"
#include "PatientManager.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "IButton.h"
#include "EnglishPanel.h"
#include "KeyBoardPanel.h"
#include "RelievePatientWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"

struct DemoWaveDataDesc
{
    const char *waveformName;       // 对应的波形名称。
    const WaveformID waveID;        // 波形标识。
    Param *param;                   // 处理数据的Param对象。
    short sampleRate;               // 采样率。
    int error;                      // 数据产生时的误差记录。
};

static DemoWaveDataDesc _demoWaveData[WAVE_NR] =
{
    {"ECG-I",     WAVE_ECG_I,     NULL, 250, 0},
    {"ECG-II",    WAVE_ECG_II,    NULL, 250, 0},
    {"ECG-III",   WAVE_ECG_III,   NULL, 250, 0},
    {"ECG-aVR",   WAVE_ECG_aVR,   NULL, 250, 0},
    {"ECG-aVL",   WAVE_ECG_aVL,   NULL, 250, 0},
    {"ECG-aVF",   WAVE_ECG_aVF,   NULL, 250, 0},
    {"ECG-V1",    WAVE_ECG_V1,    NULL, 250, 0},
    {"ECG-V2",    WAVE_ECG_V2,    NULL, 250, 0},
    {"ECG-V3",    WAVE_ECG_V3,    NULL, 250, 0},
    {"ECG-V4",    WAVE_ECG_V4,    NULL, 250, 0},
    {"ECG-V5",    WAVE_ECG_V5,    NULL, 250, 0},
    {"ECG-V6",    WAVE_ECG_V6,    NULL, 250, 0},
    {"RESP",      WAVE_RESP,      NULL, 250, 0},
    {"SPO2",      WAVE_SPO2,      NULL, 50, 0},
    {"CO2",       WAVE_CO2,       NULL, 20, 0},
    {"N2O",       WAVE_N2O,       NULL, 50, 0},
    {"AA1",       WAVE_AA1,       NULL, 50, 0},
    {"AA2",       WAVE_AA2,       NULL, 50, 0},
    {"O2",        WAVE_O2,        NULL, 50, 0},
    {"ART",       WAVE_ART,       NULL, 50, 0},
    {"PA",        WAVE_PA,        NULL, 50, 0},
    {"CVP",       WAVE_CVP,       NULL, 50, 0},
    {"LAP",       WAVE_LAP,       NULL, 50, 0},
    {"RAP",       WAVE_RAP,       NULL, 50, 0},
    {"ICP",       WAVE_ICP,       NULL, 50, 0},
    {"AUXP1",     WAVE_AUXP1,     NULL, 50, 0},
    {"AUXP2",     WAVE_AUXP2,     NULL, 50, 0},
};

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void WindowWidget::layoutExec()
{
    for (int i = 0; i < 48; i++)
    {
        _pixmap[i] = new QPixmap(QSize(width() / 6 - 5, height() / 8 - 5));
        _pixmap[i]->fill(Qt::red);
    }

    // ECG_I
    _wavePixmap[WAVE_ECG_I] = new QPixmap(QSize(width() / 6 * 4 - 5, height() / 8 - 5));
    _wavePixmap[WAVE_ECG_I]->fill(Qt::black);
    _painter = new QPainter(_wavePixmap[WAVE_ECG_I]);
    _drawCurves(WAVE_ECG_I, _painter, 255);

    // WAVE_RESP
    _wavePixmap[WAVE_RESP] = new QPixmap(QSize(width() / 6 * 4 - 5, height() / 8 - 5));
    _wavePixmap[WAVE_RESP]->fill(Qt::black);
    _painter = new QPainter(_wavePixmap[WAVE_RESP]);
    _drawCurves(WAVE_RESP, _painter, 255);

    // SPO2
    _wavePixmap[WAVE_SPO2] = new QPixmap(QSize(width() / 6 * 4 - 5, height() / 8 - 5));
    _wavePixmap[WAVE_SPO2]->fill(Qt::black);
    _painter = new QPainter(_wavePixmap[WAVE_SPO2]);
    _drawCurves(WAVE_SPO2, _painter, 255);

    // WAVE_CO2
    _wavePixmap[WAVE_CO2] = new QPixmap(QSize(width() / 6 * 4 - 5, height() / 8 - 5));
    _wavePixmap[WAVE_CO2]->fill(Qt::black);
    _painter = new QPainter(_wavePixmap[WAVE_CO2]);
    _drawCurves(WAVE_CO2, _painter, 50);


//    int wi = windowManager.getPopMenuWidth() * 0.8;
//    int w = this->width();

    resultImage = new QImage(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);
}

void WindowWidget::focusOrder(bool flag)
{
    if (flag)
    {
        _focusNum = 0;
    }
    else
    {
        _focusNum = 47;
    }
}

void WindowWidget::keyPressEvent(QKeyEvent *e)
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
}

void WindowWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStylePainter painter(this);

    QPainter combinepainter(resultImage);

    combinepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//    for (int y = 0; y < 8; y++)
//    {
//        for (int x = 0; x < 6; x++)
//        {
//            combinepainter.drawImage(((width() - 4) / 6) * x + 2, ((height() - 4) / 8) * y + 2, _pixmap[(y * 6) + x]->toImage());
//        }
//    }
    combinepainter.drawImage(2, ((height() - 4) / 8) * 0 + 2, _wavePixmap[WAVE_ECG_I]->toImage());
    combinepainter.drawImage(2, ((height() - 4) / 8) * 1 + 2, _wavePixmap[WAVE_SPO2]->toImage());
    combinepainter.drawImage(2, ((height() - 4) / 8) * 2 + 2, _wavePixmap[WAVE_RESP]->toImage());
    combinepainter.drawImage(2, ((height() - 4) / 8) * 3 + 2, _wavePixmap[WAVE_CO2]->toImage());

    combinepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    _drawGrid(&combinepainter);
    combinepainter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    combinepainter.fillRect(resultImage->rect(), Qt::black);
    combinepainter.end();

    painter.drawImage(0, 0, *resultImage);
}

void WindowWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
    if (hasFocus())
    {
        if (_focusNum >= 0 && _focusNum <= 47)
        {
            QPainter painter(_pixmap[_focusNum]);
            painter.setPen(QPen(Qt::white, _focusedBorderWidth / 2));
            painter.setRenderHint(QPainter::Antialiasing, true);
            QRect r = _pixmap[_focusNum]->rect();
            r.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                     -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
            painter.drawRoundedRect(r, _focusedBorderWidth, _focusedBorderWidth);
        }
    }
    update();
}

bool WindowWidget::focusPreviousChild()
{
    QPainter painterPre(_pixmap[_focusNum]);
    painterPre.setPen(QPen(Qt::red, _focusedBorderWidth / 2));
    painterPre.setRenderHint(QPainter::Antialiasing, true);
    QRect painterPreR = _pixmap[_focusNum]->rect();
    painterPreR.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                       -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
    painterPre.drawRoundedRect(painterPreR, _focusedBorderWidth, _focusedBorderWidth);
    update();

    if (_focusNum <= 0)
    {
        return QWidget::focusPreviousChild();
    }
    else
    {
        _focusNum--;
        QPainter painterNext(_pixmap[_focusNum]);
        painterNext.setPen(QPen(Qt::white, _focusedBorderWidth / 2));
        painterNext.setRenderHint(QPainter::Antialiasing, true);
        QRect painterNextR = _pixmap[_focusNum]->rect();
        painterNextR.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                            -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
        painterNext.drawRoundedRect(painterNextR, _focusedBorderWidth, _focusedBorderWidth);
        update();
        return false;
    }
}

bool WindowWidget::focusNextChild()
{
    QPainter painterPre(_pixmap[_focusNum]);
    painterPre.setPen(QPen(Qt::red, _focusedBorderWidth / 2));
    painterPre.setRenderHint(QPainter::Antialiasing, true);
    QRect painterPreR = _pixmap[_focusNum]->rect();
    painterPreR.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                       -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
    painterPre.drawRoundedRect(painterPreR, _focusedBorderWidth, _focusedBorderWidth);
    update();


    if (_focusNum >= 47)
    {
        return QWidget::focusNextChild();
    }
    else
    {
        _focusNum++;
        QPainter painterNext(_pixmap[_focusNum]);
        painterNext.setPen(QPen(Qt::white, _focusedBorderWidth / 2));
        painterNext.setRenderHint(QPainter::Antialiasing, true);
        QRect painterNextR = _pixmap[_focusNum]->rect();
        painterNextR.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                            -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
        painterNext.drawRoundedRect(painterNextR, _focusedBorderWidth, _focusedBorderWidth);
        update();
        return false;
    }
}

void WindowWidget::_drawCurves(WaveformID id, QPainter *painter, int maxWaveform)
{
    QString path("/usr/local/nPM/demodata/");

    _demoFile[id].setFileName(path + _demoWaveData[id].waveformName);
    _demoFile[id].open(QFile::ReadOnly);

    if (!_demoFile[id].isOpen())
    {
        return;
    }

    int elapsed = 1;
    char data;
    int len = _demoWaveData[id].sampleRate * elapsed + _demoWaveData[id].error;
    Q_UNUSED(len)
    QPolygon polyline;

    for (int x = 0; x < (width() / 6 * 4 * 2); x++)
    {
        int y;
        int ret = 0;
        short d;
        ret = _demoFile[id].read(&data, 1);
        if (ret <= 0)
        {
            _demoFile[id].seek(0);
            _demoFile[id].read(&data, 1);
        }
        d = (unsigned char)(data);
        y = (height() / 8 - 6) - (d * height() / 8 - 6) / maxWaveform;

        if (y < 0)
        {
            y = 0;
        }

        polyline.append(QPoint(x / 2, y));
    }

//    QPen linePen(colorManager.getColor(paramInfo.getParamName(PARAM_ECG)),1);
    QPen linePen(Qt::white, 1, Qt::SolidLine);
    painter->setPen(linePen);
    painter->drawPolyline(polyline);

    painter->drawText(0, 0, 50, 25, Qt::AlignTop | Qt::AlignLeft, _demoWaveData[id].waveformName);
}

void WindowWidget::_drawGrid(QPainter *painter)
{
    double row = (height() - 4) / 8;
    double col = (width() - 4) / 6;

    QPen gridPen(Qt::white, 0.5, Qt::DotLine);
    painter->setPen(gridPen);
    double cur_row_pos = 2 + row;
    double cur_col_pos = 2 + col;
    while (cur_row_pos < height() - 2)
    {
        painter->drawLine(2, cur_row_pos, width() - 2, cur_row_pos);
        cur_row_pos += row;
    }

    while (cur_col_pos < width() - 2)
    {
        painter->drawLine(cur_col_pos, 2, cur_col_pos, height() - 2);
        cur_col_pos += col;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WindowWidget::WindowWidget() : QWidget()
{
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(windowManager.getPopMenuWidth() * 0.8, windowManager.getPopMenuHeight() * 0.8);
    layoutExec();

    _focusNum = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WindowWidget::~WindowWidget()
{
}
