#pragma once
#include "PopupWidget.h"
#include "ITableWidget.h"
#include "IButton.h"
#include "HemodynamicDefine.h"
#include "FreezeTableWidget.h"
#include <QStandardItemModel>

class HemodynamicReviewWidget : public PopupWidget
{
    Q_OBJECT

public:
    static HemodynamicReviewWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new HemodynamicReviewWidget();
        }
        return *_selfObj;
    }
    ~HemodynamicReviewWidget();

    void layoutExec(void);

    void updateReviewTable(void);
    void defaultSelectColumn(void);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _upReleased(void);
    void _downReleased(void);
    void _leftReleased(void);
    void _rightReleased(void);
    void _returnReleased(void);
    void _referenceReleased(void);

private:    
    static HemodynamicReviewWidget *_selfObj;
    static const int _itemH = 30;       // 子项高度
    int _refUnitFlag;                   // (参考范围）和（单位）之间的切换
    int _selectColumn;                  // 选中的列所在位置index
    int _curVScroller;

    FreezeTableWidget *_reviewTable;

    QStandardItemModel *_model;
    QStringList _headList;

    IButton *_up;
    IButton *_down;
    IButton *_left;
    IButton *_right;
    IButton *_return;
    IButton *_reference;

    HemodynamicReviewWidget();

private:


};
#define hemodynamicReviewWidget (HemodynamicReviewWidget::construction())
#define deleteHemodynamicReviewWidget() (delete &hemodynamicReviewWidget)
