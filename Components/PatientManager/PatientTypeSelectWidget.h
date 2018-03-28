#pragma once
#include "PopupWidget.h"
#include "IListWidget.h"
#include "PatientManager.h"

class PatientSelectWidget : public PopupWidget
{
    Q_OBJECT

public:
    PatientSelectWidget();
    ~PatientSelectWidget();

protected:
    virtual void showEvent(QShowEvent *event);

private slots:
    void _listItemClicked(QListWidgetItem *item);  // 鼠标点击IListWidget的item槽。
    void _listReleased(void);                      // enter按键释放IListWidget的item槽。

private:
    void _typeChanged(PatientType t);

    IListWidget *_list;
};
