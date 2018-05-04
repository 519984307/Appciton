#include "SPO2WaveWidget.h"
#include "SPO2Param.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "ComboListPopup.h"
#include "IConfig.h"
#include "Debug.h"

/**************************************************************************************************
 * 设置波形增益
 *************************************************************************************************/
void SPO2WaveWidget::setGain(SPO2Gain gain)
{
    if (!_gain)
    {
        return;
    }

    _initValueRange(gain);
    QString text = NULL;
    switch (gain)
    {
        case SPO2_GAIN_X10:
        case SPO2_GAIN_X20:
        case SPO2_GAIN_X40:
        case SPO2_GAIN_X80:
            text += SPO2Symbol::convert(gain);
            break;

        default:
            text += "X1";
            break;
    }

    _gain->setText(text);
}

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void SPO2WaveWidget::_releaseHandle(IWidget *)
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
void SPO2WaveWidget::_spo2Gain(IWidget *widget)
{
    if (NULL == _gainList)
    {
        _gainList = new ComboListPopup(widget, POPUP_TYPE_USER, SPO2_GAIN_NR, spo2Param.getGain());
        _gainList->setBorderColor(colorManager.getBarBkColor());
        _gainList->setBorderWidth(5);
        _gainList->popupUp(true);
        for (int i = 0; i < SPO2_GAIN_NR; i++)
        {
            _gainList->addItemText(SPO2Symbol::convert(SPO2Gain(i)));
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
void SPO2WaveWidget::_popupDestroyed()
{
    int index = _gainList->getCurIndex();
    if (index == -1)
    {
        _gainList = NULL;
        return;
    }

    spo2Param.setGain((SPO2Gain)index);

    _gainList = NULL;
}

/**************************************************************************************************
 * 获取极值。
 *************************************************************************************************/
void SPO2WaveWidget::getGainToValue(SPO2Gain gain, int &min, int &max)
{
    int mid = spo2Param.getSPO2MaxValue() / 2;
    int diff;
    switch(gain)
    {
        case SPO2_GAIN_X10:
            min = 0;
            max = spo2Param.getSPO2MaxValue();
            break;

        case SPO2_GAIN_X20:
            diff = spo2Param.getSPO2MaxValue() / 4;
            min = mid - diff;
            max = mid + diff;
            break;

        case SPO2_GAIN_X40:
            diff = spo2Param.getSPO2MaxValue() / 8;
            min = mid - diff;
            max = mid + diff;
            break;

        case SPO2_GAIN_X80:
            diff = spo2Param.getSPO2MaxValue() / 16;
            min = mid - diff;
            max = mid + diff;
            break;

        default:
            min = 0;
            max = spo2Param.getSPO2MaxValue();
            break;
    }
}

/**************************************************************************************************
 * wave is enable。
 *************************************************************************************************/
bool SPO2WaveWidget::waveEnable()
{
    return spo2Param.isEnabled();
}

void SPO2WaveWidget::_initValueRange(SPO2Gain gain)
{
    int min = 0;
    int max = 0;
    getGainToValue(gain, min, max);
    setValueRange(min, max);
}

/**************************************************************************************************
 * 响应窗体大小调整事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void SPO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    if (!_name || !_gain || !_notify)
    {
        return;
    }

    _name->move(0, 0);
    _gain->move(0 + _name->rect().width(), 0);

    // 设定为固定的高度和宽度后，居中显示。
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);

    _initValueRange(spo2Param.getGain());

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
void SPO2WaveWidget::focusInEvent(QFocusEvent */*e*/)
{
    _name->setFocus();
}

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void SPO2WaveWidget::_loadConfig(void)
{
    SPO2WaveVelocity speed = (SPO2WaveVelocity)spo2Param.getSweepSpeed();

    if (speed == SPO2_WAVE_VELOCITY_62D5)
    {
        setWaveSpeed(6.25);
    }
    else if (speed == SPO2_WAVE_VELOCITY_125)
    {
        setWaveSpeed(12.5);
    }
    else if (speed == SPO2_WAVE_VELOCITY_250)
    {
        setWaveSpeed(25.0);
    }

    setGain(spo2Param.getGain());
}

void SPO2WaveWidget::setNotify(bool enable, QString str)
{
    if(enable)
    {
        _notify->setVisible(true);
        _notify->setText(str);
    }
    else
    {
        _notify->setVisible(false);
        _notify->setText(" ");
    }
}


/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
SPO2WaveWidget::SPO2WaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title)
    , _gain(NULL)
    , _notify(NULL)
    , _gainList(NULL)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_SPO2);
//    setValueRange(64, 192);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);

    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
//    _name = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(title);
//    addItem(_name);
    connect(_name, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(infoFont));
    _gain->setFixedSize(120, fontH);
    _gain->setText("");
    connect(_gain, SIGNAL(released(IWidget*)), this, SLOT(_spo2Gain(IWidget*)));
    addItem(_gain);

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(infoFont));
    _notify->setFocusPolicy(Qt::NoFocus);
    _notify->setFixedSize(200, fontH);
    _notify->setText("");
    _notify->setVisible(false);
    addItem(_notify);
    // 加载配置
    _loadConfig();

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
SPO2WaveWidget::~SPO2WaveWidget()
{

}
