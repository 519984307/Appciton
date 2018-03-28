#pragma once
#include "PopupWidget.h"

class IComboList;
class IButton;
class TrendDataSetWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TrendDataSetWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TrendDataSetWidget();
        }
        return *_selfObj;
    }
    static TrendDataSetWidget *_selfObj;
    ~TrendDataSetWidget();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _cancelReleased(void);

private:
    TrendDataSetWidget();

    IComboList *_resolutionRatio;
    IComboList *_trendGroup;
    IButton *_yes;
    IButton *_cancel;
};
#define trendDataSetWidget (TrendDataSetWidget::construction())
#define deleteTrendDataSetWidget() (delete TrendDataSetWidget::_selfObj)

