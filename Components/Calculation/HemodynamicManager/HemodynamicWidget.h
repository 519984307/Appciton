#pragma once
#include "PopupWidget.h"
#include "HemodynamicDefine.h"
#include "IComboList.h"
#include "QSignalMapper"
#include "ITableWidget.h"
#include <QStackedLayout>
#include <QString>

class LabelButton;
class IButton;
class HemodynamicWidget : public PopupWidget
{
    Q_OBJECT

public:
    static HemodynamicWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new HemodynamicWidget();
        }
        return *_selfObj;
    }
    ~HemodynamicWidget();

    void layoutExec(void);

    void initCalcInput(void);
    void updateData(void);

    void defaultInput(void);

protected:
    void showEvent(QShowEvent *e);

signals:
    void checkOutputSignal(int);
    void checkInputSignal(int);

private slots:
    void _btnParamReleased(int);
    void _checkOutputReleased(void);
    void _checkInputReleased(void);
    void _refRangeReleased(void);
    void _calcReleased(void);
    void _calcReviewReleased(void);

private:
    HemodynamicWidget();

private:    
    static HemodynamicWidget *_selfObj;
    static const int _itemH = 30;       // 子项高度
    int _refUnitFlag;                   // （参考范围）和（单位）之间的切换

    QSignalMapper *_signalMapper;

    QLabel *_inout;                     // 输入输出值标题

    QWidget *_inputWidget;
    QWidget *_outputWidget;
    QStackedLayout *_stackLayout;

    LabelButton *_calcParam[HEMODYNAMIC_PARAM_NR];
    QLabel *_calcParamUnit[HEMODYNAMIC_PARAM_NR];

    IButton *_calcReviewA;
    IButton *_calcReviewB;

    IButton *_checkOutput;
    IButton *_calculation;

    IButton *_checkInput;
    IButton *_reference;
    IButton *_record;

    ITableWidget *_outputTable;

};
#define hemodynamicWidget (HemodynamicWidget::construction())
#define deleteHemodynamicWidget() (delete &hemodynamicWidget)
