#pragma once
#include "PopupWidget.h"
#include "PatientDefine.h"
#include "IComboList.h"

class LabelButton;
class ISpinBox;
class IButton;
class PatientInfoMenu : public PopupWidget
{
    Q_OBJECT

public:
    static PatientInfoMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PatientInfoMenu();
        }
        return *_selfObj;
    }
    static PatientInfoMenu *_selfObj;
    ~PatientInfoMenu();

    void layoutExec(void);
    void widgetChange(void);
    void relieveStatus(bool);
    void newPatientStatus(bool);

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _idReleased(void);
    void _nameReleased(void);
    void _ageReleased(void);
    void _heightReleased(void);
    void _weightReleased(void);
    void _relieveReleased(void);
    void _saveInfoReleased(void);

private:
    PatientInfoMenu();

private:
    static const int _itemH = 30;     // 子项高度
    LabelButton *_id;                 // ID。
    IComboList *_type;                // 病人类型。
    IComboList *_sex;                 // 性别。
    IComboList *_blood;               // 血型。
    LabelButton *_name;               // 姓名。
    IComboList *_pacer;               // 起搏分析。
    LabelButton *_age;                // 年龄。
    LabelButton *_height;             // 身高。
    LabelButton *_weight;             // 体重。
    IButton *_relievePatient;         // 病人信息获取
    IButton *_savePatientInfo;        // 保存病人信息

    bool _infoChange;                 //settting has been change
    bool _patientNew;                 //新建病人标志
    bool _relieveFlag;                //解除病人标志
};
#define patientInfoMenu (PatientInfoMenu::construction())
#define deletePatientInfoMenu() (delete PatientInfoMenu::_selfObj)
