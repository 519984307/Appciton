#pragma once
#include "IWidget.h"

class QLabel;
class QHBoxLayout;

/***************************************************************************************************
 * 显示流逝的时间。
 **************************************************************************************************/
class ElapseTimeWidget : public IWidget
{
    Q_OBJECT

public:
    ElapseTimeWidget(QWidget *parent = 0);
    void setText(const QString &str);

private:
    QLabel *_elapseLabel;
};
