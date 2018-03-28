#pragma once
#include "IWidget.h"

class QLabel;
class DateTimeWidget : public IWidget
{
    Q_OBJECT
public:
    DateTimeWidget(QWidget *parent = 0);
    void setText(const QString &str);

private:
    QLabel *_datetimeLabel;
};
