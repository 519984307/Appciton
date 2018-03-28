#include <qpainter.h>
#include <qpen.h>
#include "FontManager.h"
#include "WindowManager.h"
#include "qtimer.h"
#include "IConfig.h"
#include "ColorManager.h"
#include "PMessageBox.h"

PMessageBox* PMessageBox::_selfObj = NULL;

PMessageBox::PMessageBox(QString message, QColor bgColor, QColor textColor)
    :PopupWidget(true),_message(message),_bgColor(bgColor),_textColor(textColor),_timer(NULL)
{
    int height = 22;
    int width = 220;

    int barHeight[2] = {30, 30};
    QString string;
    systemConfig.getStrValue("PrimaryCfg|UILayout|WidgetsOrder|TopBarRowHight", string);
    QStringList factors = string.split(",");
    if (factors.size() == 2)
    {
        barHeight[0] = factors[0].toInt();
        barHeight[1] = factors[1].toInt();
    }

    int screenWidth = 0;
    machineConfig.getNumValue("ScreenWidth", screenWidth);

    setGeometry(screenWidth - screenWidth * 55 / (30 + 270 + 55) - width, barHeight[0] + barHeight[1], width, height);
    setWindowFlags(Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);
    setModal(false);

    _timer = new QTimer();
    _timer->setInterval(5000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_exit()));
}

PMessageBox::~PMessageBox()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

void PMessageBox::setMessBoxInfo(QString message, QColor bgColor, QColor textColor)
{
    _message = message;
    _bgColor = bgColor;
    _textColor = textColor;
}

void PMessageBox::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    int font = 14;
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(clipPath);

    QPen pen;
    pen.setColor(_textColor);
    pen.setWidth(4);

    painter.setPen(pen);
    painter.setBrush(_bgColor);
    QRect r = rect();
    r.setBottom(rect().height());
    painter.fillRect(r, _bgColor);

    painter.setFont(fontManager.textFont(font));
    painter.drawText(rect().left() + 8, rect().top() + 2, rect().width() - 16, rect().height() - 4,
                     Qt::AlignLeft | Qt::AlignVCenter, _message);
}

void PMessageBox::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    _timer->start();
}

void PMessageBox::_exit()
{
    _timer->stop();
    hide();
}
