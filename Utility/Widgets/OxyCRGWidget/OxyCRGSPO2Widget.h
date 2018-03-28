#pragma once
#include "OxyCRGTrendWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGSPO2Widget: public OxyCRGTrendWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGSPO2Widget(const QString &waveName, const QString &title);
    ~OxyCRGSPO2Widget();
};
