#pragma once
#include "PopupWidget.h"
#include "ParamInfo.h"
#include "ISpinBox.h"
#include "IComboList.h"
#include "IWidget.h"
#include "IDropList.h"
#include "IButton.h"
#include "TrendDataDefine.h"
#include <QHBoxLayout>
#include <QScrollArea>

class SetRulerItem : public QWidget
{
    Q_OBJECT

public:
    SetRulerItem(ParamID ppid, SubParamID ssid) : QWidget()
    {
        pid = ppid;
        sid = ssid;

        combo = new IComboList("");
        downRuler = new ISpinBox();
        upRuler = new ISpinBox();
        downRuler->enableArrow(false);
        upRuler->enableArrow(false);

        QHBoxLayout *manlayout = new QHBoxLayout();
        manlayout->setSpacing(2);
        manlayout->setMargin(0);

        manlayout->addWidget(combo);
        combo->setSpacing(2);
        manlayout->addWidget(downRuler);
        manlayout->addWidget(upRuler);

        setLayout(manlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(combo);
    }
    ~SetRulerItem()
    {

    }

public:
    ParamID pid;
    SubParamID sid;

    IComboList *combo;
    ISpinBox *downRuler;
    ISpinBox *upRuler;
};

class TrendGraphSetWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TrendGraphSetWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TrendGraphSetWidget();
        }
        return *_selfObj;
    }
    static TrendGraphSetWidget *_selfObj;
    ~TrendGraphSetWidget();

public:
    /**
     * @brief getTrendGroup get trend group type
     * @return trend group type
     */
    TrendGroup getTrendGroup();

    /**
     * @brief layoutExec widget layout
     */
    void layoutExec(void);

    /**
     * @brief upDateTrendGroup update set widget trend group display
     */
    void upDateTrendGroup(void);

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);

private slots:
    /**
     * @brief _allAutoReleased all ruler auto slot funtion
     */
    void _allAutoReleased();

    /**
     * @brief _trendGroupReleased set trend group slot funtion
     */
    void _trendGroupReleased(int);

    /**
     * @brief _timeIntervalReleased set time interval slot funtion
     */
    void _timeIntervalReleased(int);

    /**
     * @brief _waveNumberReleased set waveform number slot funtion
     */
    void _waveNumberReleased(int);

    /**
     * @brief _comboListIndexChanged auto reluer switch select slot funtion
     * @param id item id
     * @param index select index
     */
    void _comboListIndexChanged(int id, int index);

    /**
     * @brief _upDownRulerChange up down ruler set slot funtion
     * @param id
     */
    void _upDownRulerChange(QString /*valueStr*/, int id);

private:
    /**
     * @brief _trendRulerLayout add ruler widget
     */
    void _trendRulerLayout(void);

    /**
     * @brief _clearRulerLayout hide ruler widget
     */
    void _clearRulerLayout(void);

    /**
     * @brief _loadOptions load init config data
     */
    void _loadOptions(void);

private:
    TrendGraphSetWidget();

    QList<SetRulerItem *> _itemList;

    QScrollArea *_mScrollArea;
    IWidget *_rulerSetWidget;

    IButton *_allAuto;
    IDropList *_trendGroupList;
    IDropList *_timeIntervalList;
    IDropList *_waveNumberList;
    TrendGroup _trendGroup;                 // 当前趋势组

    QVBoxLayout *_rulerLayout;
};
#define trendGraphSetWidget (TrendGraphSetWidget::construction())
#define deleteTrendGraphSetWidget() (delete TrendGraphSetWidget::_selfObj)
