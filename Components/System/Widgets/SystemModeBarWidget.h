#pragma once
#include "IWidget.h"
#include "SystemManager.h"

class QLabel;
/***************************************************************************************************
 * 显示当前的工作模式。
 **************************************************************************************************/
class SystemModeSelectWidget;
class SystemModeBarWidget : public IWidget
{
    Q_OBJECT

public:
    SystemModeBarWidget(QWidget *parent = 0);
    void setMode(UserFaceType mode);

protected:
    void paintEvent(QPaintEvent */*e*/);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_modeLabel;
    SystemModeSelectWidget *_systemModeSelectWidget;
};
