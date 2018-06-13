#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QBitmap>
#include <QScrollBar>
#include <QPainter>
#include <QTimer>
#include "ColorManager.h"
#include "AlarmIndicator.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "AlarmInfoPopListView.h"
#include "ISpinBox.h"
#include "IButton.h"

/**************************************************************************************************
 * 构造函数。
 * 参数:
        parent:关联父控件
        type:报警类型
 *************************************************************************************************/
AlarmInfoPopListVIew::AlarmInfoPopListVIew(QWidget *parent, AlarmType alarmType) :
        QDialog(0, Qt::FramelessWindowHint), _parent(parent), _alarmType(alarmType)
{
    _total = 0;
    _isPressed = false;

    setModal(true);

    int fontSize = fontManager.getFontSize(1);

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);
    _mainLayout->setSpacing(0);

    for (int i = 0; i < _pageSize; ++i)
    {
        _label[i].setFixedSize(parent->width() - 6, parent->height() - 4);
        _label[i].setFont(fontManager.textFont(fontSize));
        _mainLayout->addWidget(&_label[i]);
    }

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(1);

    _page = new ISpinBox("", false);
    _page->setVisible(false);
    _page->setFixedHeight(parent->height() - 4);
    _page->setValueFont(fontManager.textFont(fontSize));
    _page->enableSendRealSignal(true);
    _page->setMode(ISPIN_MODE_INT);
    _page->setStep(1);
    _page->enableArrow(false);
    connect(_page, SIGNAL(valueChange(QString,int)), this, SLOT(_pageChange(QString)));

    _close = new IButton("X");
    _close->setVisible(false);
    _close->setBorderEnabled(false, true);
    _close->setFont(fontManager.textFont(fontSize));
    _close->setFixedSize(parent->height() - 4, parent->height() - 4);
    connect(_close, SIGNAL(realReleased()), this, SLOT(_closeSlot()));

    hlayout->addWidget(_page, 9);
    hlayout->addWidget(_close, 1);

    _mainLayout->addLayout(hlayout);

    setLayout(_mainLayout);
    setAttribute(Qt::WA_TranslucentBackground);

    _curPage = 1;
    _totalPage = 1;

    _timer = new QTimer();
    _timer->setInterval(15000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(close()));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
AlarmInfoPopListVIew::~AlarmInfoPopListVIew()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/**************************************************************************************************
 * 处理键盘和飞棱事件, 切换聚集或菜单项翻页。
 *************************************************************************************************/
void AlarmInfoPopListVIew::keyPressEvent(QKeyEvent *e)
{
    if (_totalPage > 1)
    {
        return;
    }

    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Enter:
        case Qt::Key_Return:
            _isPressed = true;
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 处理键盘和飞棱事件, 切换聚集或菜单项翻页。
 *************************************************************************************************/
void AlarmInfoPopListVIew::keyReleaseEvent(QKeyEvent *e)
{
    if (_totalPage > 1)
    {
        return;
    }

    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (_isPressed)
            {
                setVisible(false);
                _isPressed = false;
            }
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 显示事件
 *************************************************************************************************/
void AlarmInfoPopListVIew::showEvent(QShowEvent */*e*/)
{
    loadData();
    QPoint pos = _parent->mapToGlobal(_parent->rect().bottomLeft());
    pos.setY(pos.y() - 3);
    pos.setX(pos.x() + 3);
    move(pos);
    _timer->start();

    if (_page->isVisible())
    {
        _page->setFocus();
    }
}

/**************************************************************************************************
 * 显示事件
 *************************************************************************************************/
void AlarmInfoPopListVIew::hideEvent(QHideEvent *e)
{
    _timer->stop();

    QDialog::hideEvent(e);
}

/**************************************************************************************************
 * 加载数据
 *************************************************************************************************/
void AlarmInfoPopListVIew::loadData()
{
    int total = alarmIndicator.getAlarmCount();
    if (total <= 1)
    {
        _total = total;
        hide();
        return;
    }

    _total = total;
    _loadData();
}

/**************************************************************************************************
 * 加载数据
 *************************************************************************************************/
void AlarmInfoPopListVIew::_loadData()
{
    if (_total <= 0)
    {
        return;
    }

    AlarmInfoNode node;
    QList<AlarmInfoNode> highnode;
    QList<AlarmInfoNode> midnode;
    QList<AlarmInfoNode> lownode;
    QList<AlarmInfoNode> promptnode;

    for (int i = _total - 1; i >= 0; --i)
    {
        alarmIndicator.getAlarmInfo(i, node);
        if (node.alarmType != _alarmType)
        {
            continue;
        }

        switch (node.alarmPriority)
        {
            case ALARM_PRIO_HIGH:
                highnode.append(node);
                break;
            case ALARM_PRIO_MED:
                midnode.append(node);
                break;
            case ALARM_PRIO_LOW:
                lownode.append(node);
                break;
            default:
                promptnode.append(node);
                break;
        }
    }

    highnode.append(midnode);
    highnode.append(lownode);
    highnode.append(promptnode);
    int count = highnode.count();

    int start = 0, end = 0;
    int totalPage = (0 == count % _pageSize) ? (count / _pageSize) : (count / _pageSize + 1);
    if (_totalPage != totalPage)
    {
        _totalPage = totalPage;
        _page->setRange(1, _totalPage);
        if (_curPage > _totalPage)
        {
            _curPage = _totalPage;
        }

    }
    _page->setSuffix("/" + QString::number(_totalPage));
    _page->setValue(_curPage);

    if (_page->isHidden() && _close->isHidden())
    {
        _page->setVisible(true);
        _close->setVisible(true);
        _page->setFocus();
    }

    start = (_curPage - 1) * _pageSize;
    if (count > _pageSize)
    {
        end = start + _pageSize;
        if (end > count)
        {
            end = count;
            start = count - _pageSize;
        }
    }
    else
    {
        end = _pageSize;
    }

    for (int i = start; i < end; ++i)
    {
        if (i < count)
        {
            node = highnode.at(i);
            _label[i - start].setVisible(true);
            _label[i - start].setText(trs(node.alarmMessage));
            _label[i - start].setLatchMark(node.latch);
            _label[i - start].setAcknowledgeMark(node.acknowledge);
            _label[i - start].setPauseTime(node.pauseTime);
            _label[i - start].setAlarmType(node.alarmType);
            _label[i - start].setAlarmPriority(node.alarmPriority);
            switch (node.alarmPriority)
            {
                case ALARM_PRIO_HIGH:
                    _label[i - start].setBgColor(Qt::red);
                    _label[i - start].setTxtColor(Qt::white);
                    break;
                case ALARM_PRIO_MED:
                    _label[i - start].setBgColor(Qt::yellow);
                    _label[i - start].setTxtColor(Qt::black);
                    break;
                case ALARM_PRIO_LOW:
                    _label[i - start].setBgColor(QColor(0, 175, 219));
                    _label[i - start].setTxtColor(Qt::black);
                    break;
                case ALARM_PRIO_PROMPT:
                    _label[i - start].setBgColor(QColor(150, 200, 200));
                    _label[i - start].setTxtColor(Qt::black);
                    break;
                default:
                    break;
            }
        }
        else
        {
            _label[i - start].setVisible(false);
        }
    }

    setFixedHeight((_pageSize + 1) * _label[0].height());

    highnode.clear();
    midnode.clear();
    lownode.clear();
    promptnode.clear();

    update();
}

/**************************************************************************************************
 * 页改变
 *************************************************************************************************/
void AlarmInfoPopListVIew::_pageChange(QString value)
{
    _curPage = value.toInt();

    _loadData();
}

/**************************************************************************************************
 * 关闭
 *************************************************************************************************/
void AlarmInfoPopListVIew::_closeSlot()
{
    setVisible(false);
}

/**************************************************************************************************
 * 显示/隐藏
 *************************************************************************************************/
void AlarmInfoPopListVIew::setVisible(bool flag)
{
    QDialog::setVisible(flag);

    if (!flag)
    {
        emit listHide();
    }
}

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
AlarmInfoLabel::AlarmInfoLabel():QLabel()
{
    _bgColor = Qt::black;
    _txtColor = Qt::white;
    _latch = false;
    _acknowledge = false;
    _pauseTime = 0;
    _type = ALARM_TYPE_TECH;
    _priority = ALARM_PRIO_PROMPT;
}

/**************************************************************************************************
 * 绘画。
 *************************************************************************************************/
void AlarmInfoLabel::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(_bgColor);

    painter.setPen(QPen(Qt::white, 2));
    painter.drawRoundedRect(rect(), 5.0, 5.0);

    painter.setPen(_txtColor);

    QRect r = this->rect();
    r.adjust(1 + 6, 0, 0, 0);
//    if (_acknowledge)
//    {
//        QImage image("/usr/local/nPM/icons/select.png");
//        painter.drawImage(QRect(1 + 6, (height() - 16) / 2, 16, 16), image);
//        r.adjust(1 + 6 + 16 + 6, 0, 0, 0);
//    }
//    else
//    {
//        r.adjust(1 + 6, 0, 0, 0);
//    }

    painter.setFont(fontManager.textFont(fontManager.getFontSize(1)));
    painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, text());

//    if (_type != ALARM_TYPE_TECH && 0 != _pauseTime)
//    {
//        r.adjust(0, 0, -4, 0);
//        painter.drawText(r, Qt::AlignVCenter | Qt::AlignRight, QString::number(_pauseTime) + "s");
//    }
}

