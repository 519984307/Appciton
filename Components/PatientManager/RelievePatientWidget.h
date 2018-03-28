#pragma once
#include <QLabel>
#include "ILabel.h"
#include "PopupWidget.h"
#include "PatientDefine.h"
#include "IComboList.h"

class IButton;
class RelievePatientWidget : public PopupWidget
{
    Q_OBJECT

public:
    static RelievePatientWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RelievePatientWidget();
        }
        return *_selfObj;
    }
    static RelievePatientWidget *_selfObj;
    ~RelievePatientWidget();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);
    void exit(void);

private slots:
    void _yesReleased(void);
    void _noReleased(void);

private:
    RelievePatientWidget();

private:
    static const int _itemH = 30;     // 子项高度
    QLabel *_standby;                 // 进入待机。
    IButton *_yes;                    // 确定按键
    IButton *_no;                    // 确定按键
};
#define relievePatientWidget (RelievePatientWidget::construction())
#define deleteRelievePatientWidget() (delete RelievePatientWidget::_selfObj)
