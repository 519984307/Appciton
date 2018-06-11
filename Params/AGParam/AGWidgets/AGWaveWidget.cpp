#include "AGWaveWidget.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "AGWaveRuler.h"
#include "AGSymbol.h"
#include "ComboListPopup.h"
#include "WaveWidgetSelectMenu.h"
#include "AGParam.h"

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGWaveWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        _name->setText(getTitle());
    }
    else
    {
        _name->setText(AGSymbol::convert(type));
    }
}

/**************************************************************************************************
 * 添加波形数据。
 *************************************************************************************************/
void AGWaveWidget::addWaveformData(short data, int flag)
{
    addData(data, flag);
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void AGWaveWidget::setLimit(int low, int high)
{
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标尺的标签值。
 *************************************************************************************************/
void AGWaveWidget::setRuler(AGDisplayZoom zoom)
{
    float zoomValue = 0;
    switch (zoom)
    {
    case AG_DISPLAY_ZOOM_4:
        _ruler->setRuler(4.0, 2.0, 0);
        zoomValue = 4.0;
        break;
    case AG_DISPLAY_ZOOM_8:
        _ruler->setRuler(8.0, 4.0, 0);
        zoomValue = 8.0;
        break;
    case AG_DISPLAY_ZOOM_15:
        _ruler->setRuler(15.0, 7.5, 0);
        zoomValue = 15.0;
        break;
    default:
        break;
    }

    QString str;
    str.sprintf("0.0~%.1f", zoomValue);
    str += " ";
    str += trs("percent");
    _zoom->setText(str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGWaveWidget::AGWaveWidget(WaveformID id, const QString &waveName, const AGTypeGas gasType)
    : WaveWidget(waveName, AGSymbol::convert(gasType))
{
    setFocusPolicy(Qt::NoFocus);
    setID(id);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(7);
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, 30);
    _name->setText(getTitle());
    connect(_name, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    _ruler = new AGWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);

    _zoom = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _zoom->setFont(fontManager.textFont(fontSize));
    _zoom->setFixedSize(120, 30);
    addItem(_zoom);
    connect(_zoom, SIGNAL(released(IWidget*)), this, SLOT(_zoomChangeSlot(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGWaveWidget::~AGWaveWidget()
{
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void AGWaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(margin(), margin());
    _zoom->move(margin() + _name->rect().width(), margin());
    _ruler->resize(2, margin(), width() - 2, height() - margin() * 2);
    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 焦点进入。
 *************************************************************************************************/
void AGWaveWidget::focusInEvent(QFocusEvent *)
{
    if (Qt::NoFocus != _name->focusPolicy())
    {
        _name->setFocus();
    }
}

void AGWaveWidget::_releaseHandle(IWidget *)
{
    QWidget *p = (QWidget*)parent();
    if (p == NULL)
    {
        return;
    }

    QRect prect = p->geometry();
    QRect r = geometry();

    waveWidgetSelectMenu.setTopWaveform(false);
    waveWidgetSelectMenu.setWaveformName(name());
    waveWidgetSelectMenu.setShowPoint(prect.x() + r.x() + 50, prect.y() + r.y());
    waveWidgetSelectMenu.autoShow();
}

/**************************************************************************************************
 * 标尺改变。
 *************************************************************************************************/
void AGWaveWidget::_zoomChangeSlot(IWidget *widget)
{
    if (NULL == _gainList)
    {
        AGDisplayZoom zoom = AG_DISPLAY_ZOOM_4;
        int maxZoom = AG_DISPLAY_ZOOM_NR;
        _gainList = new ComboListPopup(widget, POPUP_TYPE_USER, maxZoom, zoom);
        _gainList->setBorderColor(colorManager.getBarBkColor());
        _gainList->setBorderWidth(5);
        _gainList->popupUp(true);
        float zoomArray[] = {4.0, 8.0, 15.0};
        QString str;
        for (int i = 0; i < maxZoom; i ++)
        {
            str.clear();
            str.sprintf("0.0~%.1f", zoomArray[i]);
            str += " ";
            str += trs("percent");
            _gainList->addItemText(str);
        }
        _gainList->setItemDrawMark(false);
        _gainList->setFont(fontManager.textFont(14));
        connect(_gainList, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
    }
    _gainList->show();
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void AGWaveWidget::_popupDestroyed()
{
    int index = _gainList->getCurIndex();
    if (index == -1)
    {
        _gainList = NULL;
        return;
    }

    agParam.setDisplayZoom((AGDisplayZoom)index);
    _gainList = NULL;
}
