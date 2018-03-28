#include "AlarmPhyInfoBarWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "AlarmIndicator.h"
#include "AlarmInfoPopListView.h"
#include <QPainter>

static AlarmPhyInfoBarWidget *_selfObj = NULL;

/**************************************************************************************************
 * 绘制背景。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::_drawBackground(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    if (_alarmPriority == ALARM_PRIO_HIGH)
    {
        painter.setBrush(QColor(255, 0, 0));
    }
    else if (_alarmPriority == ALARM_PRIO_PROMPT)
    {
        painter.setBrush(QColor(0, 175, 219));
    }
    else
    {
        painter.setBrush(QColor(255, 255, 0));
    }

    int border =  focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);
}

/**************************************************************************************************
 * 绘制文本。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::_drawText(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    if (_alarmPriority == ALARM_PRIO_HIGH)
    {
        painter.setPen(Qt::white);
    }
    else
    {
        painter.setPen(Qt::black);
    }

    QRect r = this->rect();
    if (_acknowledge)
    {
        QImage image("/usr/local/nPM/icons/select.png");
        painter.drawImage(QRect(focusedBorderWidth() + 6, (height() - 16) / 2, 16, 16), image);
        r.adjust(focusedBorderWidth() + 6 + 16 + 6, 0, 0, 0);
    }
    else
    {
        r.adjust(focusedBorderWidth() + 6, 0, 0, 0);
    }

    QRect SizeFont;
    SizeFont.setHeight(height() * 0.66);
    int fontSize = fontManager.adjustTextFontSize(SizeFont);
    painter.setFont(fontManager.textFont(fontSize));
//    painter.setFont(fontManager.textFont(fontManager.getFontSize(1)));
    painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, trs(_text));

    if (_type != _alarmType && 0 != _pauseTime)
    {
        r.adjust(0, 0, -6, 0);
        QImage image("/usr/local/nPM/icons/MutePause.png");
        painter.drawImage(QRect(width() - 60, (height() - 16) / 2, 16, 16), image);
//        painter.drawText(r, Qt::AlignVCenter | Qt::AlignRight, QString::number(_pauseTime) + "s");
    }
}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    _drawBackground();
    _drawText();
}

void AlarmPhyInfoBarWidget::_releaseHandle(IWidget *)
{
    //报警少于一个时，不显示。
    int total = alarmIndicator.getAlarmCount(_alarmType);
    if (total < 2)
    {
        return;
    }

    if (NULL == _alarmList)
    {
        _alarmList = new AlarmInfoPopListVIew(&alarmPhyInfoBarWin.getSelf(),_alarmType);
        connect(_alarmList, SIGNAL(listHide()), this, SLOT(_alarmListHide()));
        _alarmList->show();
    }
    else if (_alarmList->isHidden())
    {
        _alarmList->show();
    }
    else
    {
        return;
    }

    update();
}

void AlarmPhyInfoBarWidget::_alarmListHide()
{
    updateList();
}

/**************************************************************************************************
 * 清除信息。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::clear(void)
{
    _alarmPriority = ALARM_PRIO_LOW;
    _text.clear();
    _type = _alarmType;
    _latch = false;
    _pauseTime = 0;
    update();
    updateList();
}

/**************************************************************************************************
 * 设置报警提示信息。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::display(AlarmInfoNode &node)
{
    _alarmPriority = node.alarmPriority;
    _text = node.alarmMessage;
    _type = node.alarmType;
    _latch = node.latch;
    _acknowledge = node.acknowledge;
    _pauseTime = node.pauseTime;
    update();
    updateList();
}

/**************************************************************************************************
 * 获取对象。
 *************************************************************************************************/
AlarmPhyInfoBarWidget &AlarmPhyInfoBarWidget::getSelf()
{
    return *_selfObj;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmPhyInfoBarWidget::AlarmPhyInfoBarWidget(const QString &name) :
    IWidget(name),
    _alarmType(ALARM_TYPE_PHY)
{
    _selfObj = this;
//    setFocusPolicy(Qt::NoFocus);

    _alarmList = NULL;
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmPhyInfoBarWidget::~AlarmPhyInfoBarWidget()
{

}

void AlarmPhyInfoBarWidget::updateList()
{
    if (NULL != _alarmList && !_alarmList->isHidden())
    {
        _alarmList->loadData();
    }
}
