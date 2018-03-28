#pragma once
#include "IWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "UnitManager.h"
#include <QString>
#include <QLabel>
#include <QPalette>

class QHBoxLayout;
class QVBoxLayout;
class TrendWidgetLabel;
class TrendWidget: public IWidget
{
    Q_OBJECT
public:
    TrendWidget(const QString &widgetName, bool vertical = false);
    virtual ~TrendWidget();

    // 窗口是否使能
    virtual bool enable() {return true;}

    // 更新报警标志
    void updateAlarm(bool alarmFlag);

    // 获取显示的名称。
    const QString &getTitle(void) const
    {
        return _title;
    }

private slots:
    void _releaseHandle(IWidget *widget);

protected:
    void setName(const QString &name);
    void setUnit(const QString &unit);
    void setCalcLeadName(const QString &calcLead);

    void setNameFont(int size, bool isBold = false);
    void setUnitFont(int size, bool isBold = false);

    QHBoxLayout *contentLayout;
    QHBoxLayout *mLayout;

protected:
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    //字体大小设置
    virtual void setTextSize(void) = 0;

protected:
    static const int barHeight = 21;

    TrendWidgetLabel *nameLabel;
    QLabel *calcLeadLabel;
    QLabel *unitLabel;

    QString _title;                           // 趋势控制标名

    QPalette normalPalette(QPalette psrc);
    QPalette alarmPalette(QPalette psrc);
};
