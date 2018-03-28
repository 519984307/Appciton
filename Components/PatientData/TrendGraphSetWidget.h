#pragma once
#include "PopupWidget.h"
#include "ParamInfo.h"
#include "ISpinBox.h"
#include "IComboList.h"
#include "IWidget.h"
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

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);

private:
    TrendGraphSetWidget();

    QList<SetItem *> _itemList;

    QScrollArea *_mScrollArea;
    IWidget *_rulerSetWidget;
};
#define trendGraphSetWidget (TrendGraphSetWidget::construction())
#define deleteTrendGraphSetWidget() (delete TrendGraphSetWidget::_selfObj)
