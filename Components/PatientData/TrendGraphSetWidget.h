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

class SetItem : public QWidget
{
public:
    SetItem(ParamID ppid, SubParamID ssid) : QWidget()
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
    ~SetItem()
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
    TrendGroup getTrendGroup();
    void setTrendGroup(TrendGroup g);

    void layoutExec(void);

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);

private slots:
    void _trendGroupReleased(int);
    void _timeIntervalReleased(int);
    void _waveNumberReleased(int);

private:
    void _trendRulerLayout(void);
    void _clearRulerLayout(void);

private:
    TrendGraphSetWidget();

    QList<SetItem *> _itemList;

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
