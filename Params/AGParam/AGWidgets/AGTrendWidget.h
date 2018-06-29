#pragma once
#include "TrendWidget.h"
#include "AGDefine.h"
#include <QLabel>

class AGTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置麻醉剂类型
    void setAnestheticType(AGAnaestheticType type);

    // 设置测量实时数据。
    void setEtData(short etValue);
    void setFiData(short fiValue);

    // 构造与析构。
    AGTrendWidget(const QString &trendName, const AGTypeGas gasType);
    ~AGTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:

    QLabel *_etValue;
    QLabel *_fiName;
    QLabel *_fiValue;

    QString _etStr;
    QString _fiStr;

    AGTypeGas _gasType;

};
