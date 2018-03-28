#pragma once
#include "IWidget.h"
#include "ParamDefine.h"

// 界面类型
enum ShortTrend
{
    SHORT_TREND_0,
    SHORT_TREND_1,
    SHORT_TREND_NR
};

class ShortTrendWidgetLabel;
class ShortTrendWidget;
class ShortTrendManager : public IWidget
{
    Q_OBJECT

public:
    static ShortTrendManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ShortTrendManager();
        }
        return *_selfObj;
    }
    static ShortTrendManager *_selfObj;
    ~ShortTrendManager();

    void getSubParamList(QList<SubParamID> &list);
    void getSubParamList(QStringList &list);

    // 获取trend个数
    int getTrendNR(const QString &trendform);

    // 替换波形。
    void replaceTrendform(const QString &oldTrendform, const QString &newTrendform,
        bool setFocus = true, bool order = true);

    // 插入波形。
    void insertTrendform(const QString &frontTrendform, const QString &insertedTrendform,
        bool setFocus = true, bool order = true);

    // 移除波形。
    void removeTrendform(const QString &trendform, bool setFocus = true);

protected:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);

private slots:
    void _timeOutSlot();

private:
    ShortTrendManager();

    QMap<ShortTrend, ShortTrendWidget *> _shortTrendItems;

    ShortTrendWidget *_shortTrendWidget0;
    ShortTrendWidget *_shortTrendWidget1;

    QList<SubParamID> _shortTrendSubParamID0;
    QList<SubParamID> _shortTrendSubParamID1;

    ShortTrend _shortTrend;

    QTimer *_timer;


    void _addItem();

    void getSubFocusWidget(QList<QWidget*> &subWidget) const;

    // 聚焦到指定的波形窗体。
    void _focusTrendformWidget(void);

    // 获取当前显示的波形窗体内容。
    void _getCurrentshortTrend(QStringList &shortTrend);

    // 设置当前显示的波形窗体内容。
    void _setCurrentshortTrend(const QStringList &shortTrend);

    // 设置当前的ShortTrendWidget
    void _setCurrentshortTrendWidget(const QString &shortTrend);
};
#define shortTrendManager (ShortTrendManager::construction())
