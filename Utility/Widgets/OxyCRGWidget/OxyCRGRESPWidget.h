#pragma once
#include "OxyCRGTrendWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGRESPWidget: public OxyCRGTrendWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGRESPWidget(const QString &waveName, const QString &title);
    ~OxyCRGRESPWidget();
};
