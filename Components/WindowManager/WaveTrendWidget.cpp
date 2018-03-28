#include "WaveTrendWidget.h"
#include "WindowManager.h"
#include <QHBoxLayout>
#include <QMap>
#include "IWidget.h"
#include "IConfig.h"

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WaveTrendWidget::WaveTrendWidget():IWidget("WaveTrendWidget")
{
    _mainLayout = new QHBoxLayout();
    _mainLayout->setMargin(0);
    _mainLayout->setSpacing(0);

    hLayoutTopBarRow = new QHBoxLayout();
    hLayoutTopBarRow->setMargin(0);
    hLayoutTopBarRow->setSpacing(0);

    _mainLayout->addLayout(hLayoutTopBarRow);

    _trendLayout();
    // 设置布局。
    setLayout(_mainLayout);

    setFocusPolicy(Qt::NoFocus);
}

/**************************************************************************************************
 * 获取配置列表
 * 布局
 *************************************************************************************************/
void WaveTrendWidget::_trendLayout(void)
{
    QStringList nodeWidgets;
    QString widgets;
    systemConfig.getStrValue("PrimaryCfg|UILayout|WidgetsOrder|WaveTrendOrder", widgets);
    nodeWidgets = widgets.split(",");

    IWidget *w = NULL;
    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        w = windowManager.getWidget(nodeWidgets[i]);
        if (w == NULL)
        {
            continue;
        }
        w->setParent(this);            // 设置父窗体。
        w->setVisible(true);           // 可见。

        if (nodeWidgets[i] == "NIBPTrendWidget")
        {
            hLayoutTopBarRow->addWidget(w,2);
        }
        else
        {
            hLayoutTopBarRow->addWidget(w,1);
        }
    }
}

WaveTrendWidget::~WaveTrendWidget()
{
}

/**************************************************************************************************
 * 布局器内部的Widget删除与显示
 *************************************************************************************************/
void WaveTrendWidget::setVisible(bool visible)
{
    if (!visible)
    {
        // 移除hLayoutTopBarRow之前的窗体。
        int trendcount = hLayoutTopBarRow->count();
        for (int i = 0; i < trendcount; i++)
        {
            QLayoutItem *item = hLayoutTopBarRow->takeAt(0);
            IWidget *widget = (IWidget *)item->widget();
            if (widget != NULL)
            {
                widget->setVisible(false);
                widget->setParent(NULL);
            }
        }
        trendcount = hLayoutTopBarRow->count();
    }
    else
    {
        _trendLayout();
    }

    QWidget::setVisible(visible);
}
