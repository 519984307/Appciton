#pragma once
#include "IWidget.h"

class QHBoxLayout;
class WaveTrendWidget : public IWidget
{
    Q_OBJECT
public:
    WaveTrendWidget();
    ~WaveTrendWidget();

    virtual void setVisible(bool visible);

private:
    QHBoxLayout *_mainLayout;
    QHBoxLayout *hLayoutTopBarRow;

    void _trendLayout(void);

};
