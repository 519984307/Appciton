#pragma once
#include "WaveWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGTrendWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGTrendWidget(const QString &waveName, const QString &title);
    ~OxyCRGTrendWidget();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    OxyCRGTrendWidgetRuler *_ruler;          // 标尺对象
};
