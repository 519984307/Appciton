#pragma once
#include "IWidget.h"
#include <QMap>
#include <QSignalMapper>
#include <QBasicTimer>

enum PatientIconLabel
{
    PATIENT_ICON_LABEL_NONE = -1,
    PATIENT_ICON_LABEL_PACER,     // 起搏图标
    PATIENT_ICON_LABEL_NR,
};

// 图标
enum PatientStatusIcon
{
    PATIENT_ICON_NONE = -1,
    PATIENT_ICON_PACER_OFF,
    PATIENT_ICON_PACER_ON,
    PATIENT_ICON_NR,
};

class QPixmap;
/***************************************************************************************************
 * 系统状态栏，主要显示一些系统提示图标。
 **************************************************************************************************/
class PatientStatusBarWidget : public IWidget
{
    Q_OBJECT
public:
    static PatientStatusBarWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PatientStatusBarWidget();
        }
        return *_selfObj;
    }
    static PatientStatusBarWidget *_selfObj;
    ~PatientStatusBarWidget();

signals:
    void iconClicked(int iconlabel);

protected:
    void showEvent(QShowEvent *e);
    void paintEvent(QPaintEvent *e);

public:
    void hideIcon(PatientIconLabel iconlabel);
    void changeIcon(PatientIconLabel iconlabel, int status, bool enableFocus = false);

    virtual void getSubFocusWidget(QList<QWidget*> &/*subWidget*/) const;

private slots:
    void onIconClicked(int iconLabel);

private:
    PatientStatusBarWidget();

    QMap<PatientStatusIcon, QPixmap> _icons;     // 图标。
    QMap<PatientIconLabel, IWidget *> _iconMap;
    QSignalMapper *_signalMapper;
    QBasicTimer _timer;
};
#define patientStatusBar (PatientStatusBarWidget::construction())
