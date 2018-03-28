#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class LButton;
class CPRMenu : public SubMenu
{
    Q_OBJECT

public:
    static CPRMenu &Construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new CPRMenu();
        }
        return *_selfObj;
    }
    static CPRMenu *_selfObj;

public:
    ~CPRMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _enableSlot(int index);
    void _voicePromptSlot(int index);
    void _metronomeSlot(int index);
    void _metronomeRateSlot(void);
    void _lowRateLimitSlot(void);
    void _highRateLimitSlot(void);
    void _minimumDepthSlot(void);
    void _unitSlot(int index);

private:
    CPRMenu();
    void _activeControl(void);
    void _loadMinimumDepth(void);
    void _loadOptions(void);

    IComboList *_enable;                    // 使能开关。
    IComboList *_voicePrompt;               // 提示音开关。
    IComboList *_metronome;                 // 节拍器使能。
    LabelButton *_metronomeRate;            // 节拍器节律。
    LabelButton *_highRateLimit;            // 按压节律高限。
    LabelButton *_lowRateLimit;             // 按压节律低限。
    LabelButton *_minimumDepth;             // 目标按压深度。
    IComboList *_unit;                      // 按压深度单位。

    LButton *_returnBtn;                    // 返回
};
#define cprMenu (CPRMenu::Construction())
