#pragma once
#include "OxyCRGTrendWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGRRWidget: public OxyCRGTrendWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGRRWidget(const QString &waveName, const QString &title);
    ~OxyCRGRRWidget();
};
