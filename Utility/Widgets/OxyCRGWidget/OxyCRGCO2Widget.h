#pragma once
#include "OxyCRGTrendWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGCO2Widget: public OxyCRGTrendWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGCO2Widget(const QString &waveName, const QString &title);
    ~OxyCRGCO2Widget();
};
