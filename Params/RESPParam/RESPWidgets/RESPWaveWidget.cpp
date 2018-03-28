#include "RESPWaveWidget.h"
#include "RESPParam.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "ComboListPopup.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void RESPWaveWidget::_releaseHandle(IWidget *)
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
 * 增益改变。
 *************************************************************************************************/
void RESPWaveWidget::_respZoom(IWidget *widget)
{
    if (NULL == _gainList)
    {
        _gainList = new ComboListPopup(widget, POPUP_TYPE_USER, RESP_ZOOM_NR, respParam.getZoom());
        _gainList->setBorderColor(colorManager.getBarBkColor());
        _gainList->setBorderWidth(5);
        _gainList->popupUp(true);
        for (int i = 0; i < RESP_ZOOM_NR; i++)
        {
            _gainList->addItemText(RESPSymbol::convert(RESPZoom(i)));
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
void RESPWaveWidget::_popupDestroyed(void)
{
    int index = _gainList->getCurIndex();
    if (index == -1)
    {
        _gainList = NULL;
        return;
    }

    respParam.setZoom((RESPZoom)index);

    _gainList = NULL;
}

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
RESPWaveWidget::RESPWaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title), _notify(NULL), _gain(NULL), _gainList(NULL)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_RESP);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);
    
    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
//    _name = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFixedSize(130, fontH);
    _name->setFont(fontManager.textFont(infoFont));
    _name->setText(title);
//    addItem(_name);
    connect(_name, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(infoFont));
    _gain->setFixedSize(120, fontH);
    _gain->setText("");
    addItem(_gain);
    connect(_gain, SIGNAL(released(IWidget*)), this, SLOT(_respZoom(IWidget*)));

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(infoFont));
    _notify->setFixedHeight(fontH);
    _notify->setFocusPolicy(Qt::NoFocus);
    addItem(_notify);

    // 加载配置
    _loadConfig();

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
RESPWaveWidget::~RESPWaveWidget()
{

}

/**************************************************************************************************
 * 设置波形放大倍数。
 *************************************************************************************************/
void RESPWaveWidget::setZoom(int zoom)
{
    switch (zoom)
    {
        case RESP_ZOOM_X025:
            setValueRange(-0x4000 * 4, 0x3FFF * 4);
            break;

        case RESP_ZOOM_X050:
            setValueRange(-0x4000 * 2, 0x3FFF * 2);
            break;

        case RESP_ZOOM_X100:
            setValueRange(-0x4000, 0x3FFF);
            break;

        case RESP_ZOOM_X200:
            setValueRange(-0x2000, 0x1FFF);
            break;

        case RESP_ZOOM_X300:
            setValueRange(-0x1555, 0x1554);
            break;

        case RESP_ZOOM_X400:
            setValueRange(-0x1000, 0xFFF);
            break;

        case RESP_ZOOM_X500:
            setValueRange(-0xCCC, 0xCCB);
            break;

        default:
            zoom = RESP_ZOOM_X100;
            break;
    }

    _gain->setText(RESPSymbol::convert((RESPZoom)zoom));
}

/**************************************************************************************************
 * 电极脱落
 *************************************************************************************************/
void RESPWaveWidget::leadoff(bool flag)
{
    if (flag)
    {
        _notify->setText(trs("RESPCheckElectrodes"));
    }
    else
    {
        _notify->setText("");
    }
}

/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool RESPWaveWidget::waveEnable()
{
    return respParam.isEnabled();
}

/**************************************************************************************************
 * 加载配置。
 *************************************************************************************************/
void RESPWaveWidget::_loadConfig()
{
    RESPSweepSpeed speed = respParam.getSweepSpeed();
    if (speed == RESP_SWEEP_SPEED_6_25)
    {
        setWaveSpeed(6.25);
    }
    else if (speed == RESP_SWEEP_SPEED_12_5)
    {
        setWaveSpeed(12.5);
    }
    else if (speed == RESP_SWEEP_SPEED_25_0)
    {
        setWaveSpeed(25.0);
    }

    setZoom((int)respParam.getZoom());
}

/**************************************************************************************************
 * 功能： resize事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void RESPWaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    _name->move(0, 0);
    _gain->move(_name->rect().width(), 0);

    // 居中显示。
    _notify->setFixedWidth(width() / 2);
    _notify->move((width() - _notify->width()) / 2,
         qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void RESPWaveWidget::focusInEvent(QFocusEvent */*e*/)
{
    _name->setFocus();
}

/**************************************************************************************************
 * 功能： hide event。
 *************************************************************************************************/
void RESPWaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
}
