#pragma once
#include "TrendWidget.h"
#include <QLabel>

class COTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 构造和析构
    COTrendWidget(const QString &trendName);
    ~COTrendWidget();

public:
    // display C.O. and C.I. data.
    void setMeasureResult(unsigned short coData, unsigned short ciData);

    // display temp blood data.
    void setTBData(unsigned short tbData);

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:

    QLabel *_coValue;

    QLabel *_ciName;
    QLabel *_ciValue;
    QLabel *_tbName;
    QLabel *_tbValue;

    QString _coStr;
    QString _ciStr;
    QString _tbStr;

};
