#include "ShortTrendWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "TrendCanvasWidget.h"
#include "ColorManager.h"
#include "ParamDataStorageManager.h"
#include "ParamManager.h"
#include "ShortTrendWidgetLabel.h"
#include <QList>
#include "ParamInfo.h"
#include "FontManager.h"
#include "ShortTrendWidgetSelectMenu.h"

/**************************************************************************************************
 * 构建函数。
 *************************************************************************************************/
ShortTrendWidget::ShortTrendWidget() : IWidget()
{
    _interval = 1;

    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    titleLayout = new QVBoxLayout();
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->setAlignment(Qt::AlignTop);

    _trendCanvas = new TrendCanvasWidget(this);
    mainLayout->addLayout(titleLayout,1);
    mainLayout->addWidget(_trendCanvas,8);

    setLayout(mainLayout);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
ShortTrendWidget::~ShortTrendWidget()
{
    if (_trendCanvas)
    {
        delete _trendCanvas;
        _trendCanvas = NULL;
    }

    // 清除窗体
    QList<ShortTrendWidgetLabel *> winList = _labelItems.values();
    foreach (ShortTrendWidgetLabel *w, winList)
    {
        if (w && !w->parent())
        {
            delete w;
            w = NULL;
        }
    }

    _labelItems.clear();
}

/**************************************************************************************************
 * 添加子控件Item；
 * 将lable添加到_labelItems列表。
 *************************************************************************************************/
void ShortTrendWidget::addItem(SubParamID id)
{
    if (_labelItems.find(SubParamID(id)) != _labelItems.end())
    {
        return;
    }

    int fontSize = fontManager.getFontSize(7);

    QString subParamName = paramInfo.getSubParamName(id);
    ShortTrendWidgetLabel *lable = new ShortTrendWidgetLabel(subParamName, Qt::AlignCenter, this);
    lable->setFixedSize(100, 20);
    lable->setFont(fontManager.textFont(fontSize));
    lable->setText(trs(subParamName));
    lable->setFocusPolicy(Qt::StrongFocus);
    ParamID paramID = paramInfo.getParamID(id);
    QPalette palette = colorManager.getPalette(paramInfo.getParamName(paramID));
    lable->setPalette(palette);
    connect(lable, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    _labelItems.insert(id, lable);

    _trendCanvas->registerTrendData(id);
}

/**************************************************************************************************
 * 添加_labelItems列表中的项到titleLayout。
 *************************************************************************************************/
void ShortTrendWidget::addWidget()
{
    QMap<SubParamID, ShortTrendWidgetLabel *>::iterator it;
    for (it = _labelItems.begin(); it != _labelItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            titleLayout->addWidget(it.value(), 0, Qt::AlignTop);
        }
    }
    _calcInfo();
    _loadData();
}

void ShortTrendWidget::removeLabel(void)
{
    QMap<SubParamID, ShortTrendWidgetLabel *>::iterator it;
    for (it = _labelItems.begin(); it != _labelItems.end(); ++it)
    {
        titleLayout->removeWidget(it.value());
        delete it.value();
    }
    _labelItems.clear();
}

/**************************************************************************************************
 * 获取当前显示的趋势列表。
 *************************************************************************************************/
void ShortTrendWidget::getSubParamList(QList<SubParamID> &subIDlist)
{
    subIDlist.clear();

    QMap<SubParamID, ShortTrendWidgetLabel *>::iterator it;
    for (it = _labelItems.begin(); it != _labelItems.end(); ++it)
    {
        if (NULL == it.value())
        {
            continue;
        }
        subIDlist.append(it.key());
    }
}

void ShortTrendWidget::getFocusWidgetItem(QList<QWidget*> &itemWidget)
{
    itemWidget.clear();

//    QMap<SubParamID, IWidget *>::iterator it;
//    for (it = _labelItems.begin(); it != _labelItems.end(); ++it)
//    {
//        if (NULL == it.value())
//        {
//            continue;
//        }

//        QWidget *widget = (QWidget *)it.value();
//        if (NULL != widget)
//        {
//            itemWidget.append(widget);
//        }
//    }

    QList<ShortTrendWidgetLabel*> _items = _labelItems.values();
    int count = _items.count();
    if (0 == count)
    {
        return;
    }

    for (int i = count - 1; i >= 0; --i)
    {
        QWidget *widget = (QWidget *)_items.at(i);
        if (NULL != widget)
        {
            itemWidget.append(widget);
        }

    }

}

/**************************************************************************************************
 * 刷新。
 *************************************************************************************************/
void ShortTrendWidget::refresh(void)
{
    _calcInfo();
    _loadData();
}

void ShortTrendWidget::focusTrendformWidget(const SubParamID &id)
{
    QMap<SubParamID, ShortTrendWidgetLabel*>::Iterator it = _labelItems.find(id);
    if (it == _labelItems.end())
    {
        return;
    }

    it.value()->setFocus();
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void ShortTrendWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);

    addWidget();
}

void ShortTrendWidget::focusInEvent(QFocusEvent *)
{
    QMap<SubParamID, ShortTrendWidgetLabel*>::Iterator it = _labelItems.begin();
    if (it == _labelItems.end())
    {
        return;
    }

    it.value()->setFocus();
}

/**************************************************************************************************
 * 弹出框事件
 *************************************************************************************************/
void ShortTrendWidget::_releaseHandle(IWidget *widget)
{
    QPoint pos = widget->mapToGlobal(widget->rect().bottomLeft());

//    QRect prect = widget->geometry();
    QRect r = geometry();

    shortTrendWidgetSelectMenu.setWidgetName(widget->name());
    shortTrendWidgetSelectMenu.setShowPoint(pos.x() + r.x() + 50, pos.y() + r.y());
    shortTrendWidgetSelectMenu.autoShow();
}


/**************************************************************************************************
 * 计算开始时间。
 *************************************************************************************************/
void ShortTrendWidget::_calcInfo(void)
{
    _totalData = paramDataStorageManager.backend()->getBlockNR();

    if (_totalData <= 0)
    {
        return;
    }

    unsigned time = 0;
    ParamDataStorageManager::ParamBuf itemBuf;

    // 获取每条数据，每个时间为一条
    for (int i = _totalData - 1; i >= 0; --i)
    {
        _readBlockData(i, (char *) &itemBuf, sizeof(itemBuf));

        if(time == 0 && (itemBuf.item.t % 30) == 0)
        {
            //first time get here, item must align to 30s
            time = itemBuf.item.t;
            break;
        }
    }

    _30sStartTime = time;
}

/***************************************************************************************************
 * _readBlockData : read a block of data from current incident or other incident
 **************************************************************************************************/
int ShortTrendWidget::_readBlockData(int index, char *buf, int length)
{
    return  paramDataStorageManager.backend()->readBlockData(index, buf, length);
}


/**************************************************************************************************
 * 载入当前页的数据。
 *************************************************************************************************/
void ShortTrendWidget::_loadData(void)
{
    // 无数据。
    if(_totalData <= 0)
    {
        return;
    }

    ParamDataStorageManager::ParamBuf itemBuf;
    bool isValid = false;

    int index = 0;
    int startIndex = _totalData - 1;
    int lastTime = 0;

    for (int i = 0; i < _trendCanvas->getTotalTrend(); i++)
    {
        if (startIndex < 0)
        {
            break;
        }

        // 寻找下一个有效间隔数据
        for (; startIndex >= 0;)
        {
            index = startIndex;
            --startIndex;

            isValid = _readBlockData(index, (char *) &itemBuf, sizeof(itemBuf));
            if (!isValid)
            {
                break;
            }

            if (0 == qAbs(_30sStartTime - itemBuf.item.t) % (_interval * 30))
            {
                if (lastTime == 0)
                {
                    lastTime = itemBuf.item.t;
                }
                break;
            }

            isValid = false;
        }

        if (!isValid)
        {
            break;
        }

        QMap<SubParamID, ShortTrendWidgetLabel *>::iterator it;
        for (it = _labelItems.begin(); it != _labelItems.end(); ++it)
        {
            if (NULL == it.value())
            {
                continue;
            }
            if (SUB_PARAM_NIBP_SYS != it.key())
            {
                // 两个记录的时间之间超过时间间隔，补全无效值
                if (qAbs(int(lastTime - itemBuf.item.t)) > (_interval * 30))
                {
                    for (; (qAbs(int(lastTime - itemBuf.item.t)) > (_interval * 30)); lastTime -= (_interval * 30))
                    {
                        _trendCanvas->addTrendData(it.key(), InvData());
                        i++;
                        {
                            if (i > _trendCanvas->getTotalTrend())
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    _trendCanvas->addTrendData(it.key(), itemBuf.paramItem[it.key()].value);
                }
            }
        }

        lastTime = itemBuf.item.t;
    }
    _trendCanvas->update();
}
