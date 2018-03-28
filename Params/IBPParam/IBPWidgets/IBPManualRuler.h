#pragma once
#include <QLabel>
#include "PopupWidget.h"
#include "IBPWaveWidget.h"

class LabelButton;
class IBPManualRuler: public PopupWidget
{
    Q_OBJECT

public:
    static IBPManualRuler &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new IBPManualRuler();
        }
        return *_selfObj;
    }

    void layoutExec(void);

    IBPManualRuler();
    ~IBPManualRuler();

public:
    // 设置手动标尺的波形窗口对象
    void setWaveWidget(IBPWaveWidget *widget);

    // 设置上下标尺默认值
    void setHighLowRuler(int high, int low);

private slots:
    void _highRulerReleased(void);
    void _lowRulerReleased(void);
    void _yesReleased(void);

private:
    static IBPManualRuler *_selfObj;
    static const int _itemH = 30;       // 子项高度

    int _high;
    int _low;

    IBPWaveWidget *_waveWidget;

    LabelButton *_lowLimit;
    LabelButton *_highLimit;

    IButton *_yes;
};
#define ibpManualRuler (IBPManualRuler::construction())
#define deleteIBPManualRuler() (delete &ibpManualRuler)
