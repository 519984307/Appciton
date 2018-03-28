#pragma once
#include "OxyCRGTrendWidget.h"

class OxyCRGTrendWidgetRuler;

class OxyCRGHRWidget: public OxyCRGTrendWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGHRWidget(const QString &waveName, const QString &title);
    ~OxyCRGHRWidget();
};
