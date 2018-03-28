#pragma once
#include "PopupWidget.h"
#include "TitrateTableDefine.h"

class IComboList;
class IButton;
class TitrateTableSetWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TitrateTableSetWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TitrateTableSetWidget();
        }
        return *_selfObj;
    }
    static TitrateTableSetWidget *_selfObj;
    ~TitrateTableSetWidget();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _yesReleased(void);

private:
    TitrateTableSetWidget();

private:
    static const int _itemH = 30;   // 子项高度

    IComboList *_step;              // 步长
    IComboList *_datumTerm;         // 基准项
    IComboList *_doseType;          // 剂量类型

    IButton *_yes;
};
#define titrateTableSetWidget (TitrateTableSetWidget::construction())
#define deleteTitrateTableSetWidget() (delete TitrateTableSetWidget::_selfObj)
