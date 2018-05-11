#include "OxyCRGEventWaveWidget.h"
#include "FontManager.h"
#include <QPainter>

#define WAVEFORM_SCALE_HIGH      30

class OxyCRGEventWaveWidgetPrivate
{    
public:
    OxyCRGEventWaveWidgetPrivate(){}
    ~OxyCRGEventWaveWidgetPrivate(){}



    QVector<WaveformDataSegment *> waveSegments;
    QVector<TrendDataSegment *> trendSegments;
    float startTime;
    float startX;
    float endX;

};

OxyCRGEventWaveWidget::OxyCRGEventWaveWidget()
    :d_ptr(new OxyCRGEventWaveWidgetPrivate())
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);
}

OxyCRGEventWaveWidget::~OxyCRGEventWaveWidget()
{

}

void OxyCRGEventWaveWidget::setWaveTrendSegments(const QVector<WaveformDataSegment *> waveSegments, const QVector<TrendDataSegment *> trendSegments)
{
    d_ptr->waveSegments = waveSegments;
    d_ptr->trendSegments = trendSegments;
}

void OxyCRGEventWaveWidget::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::white));
    int fonSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fonSize);
    painter.setFont(font);

    painter.drawLine(0, (height() - WAVEFORM_SCALE_HIGH)/3, width(), (height() - WAVEFORM_SCALE_HIGH)/3);
    painter.drawLine(0, (height() - WAVEFORM_SCALE_HIGH)/3 * 2, width(), (height() - WAVEFORM_SCALE_HIGH)/3 * 2);
    painter.drawLine(0, (height() - WAVEFORM_SCALE_HIGH)/3 * 3, width(), (height() - WAVEFORM_SCALE_HIGH)/3 * 3);
    painter.drawLine(width()/4*3, 0, width()/4*3, (height() - WAVEFORM_SCALE_HIGH)/3 * 3);
}
