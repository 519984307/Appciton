/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include <QPixmap>
#include <QPainter>
#include "PatientStatusBarWidget.h"
#include "FontManager.h"
#include "PatientManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include "NetworkManager.h"
#include "MenuManager.h"
#include "WindowManager.h"

PatientStatusBarWidget *PatientStatusBarWidget::_selfObj = NULL;
#define ICON_WIDTH 32

/**************************************************************************************************
 * 隐藏图标。
 *************************************************************************************************/
void PatientStatusBarWidget::hideIcon(PatientIconLabel iconlabel)
{
    if (iconlabel >= PATIENT_ICON_LABEL_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if (l)
    {
        l->setPixmap(_icons[PATIENT_ICON_NONE]);
        _iconMap.value(iconlabel)->setFocusPolicy(Qt::NoFocus);
    }
}

/**************************************************************************************************
 * 函数说明:
 *         修改图标。
 * 参数说明:
 *         iconlabel:对应图标标签。
 *         status:具体图标。
 *         enableFocus:是否可以获取焦点。
 *************************************************************************************************/
void PatientStatusBarWidget::changeIcon(PatientIconLabel iconlabel, int status, bool enableFocus)
{
    if (iconlabel >= PATIENT_ICON_LABEL_NR)
    {
        return;
    }

    if (status >= PATIENT_ICON_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if (l)
    {
        l->setPixmap(_icons[(PatientStatusIcon)status]);
        IWidget *w = _iconMap.value(iconlabel);
        if (NULL != w)
        {
            if (enableFocus)
            {
                if (w->focusPolicy() == Qt::NoFocus)
                {
                    w->setFocusPolicy(Qt::StrongFocus);
                    windowManager.setFocusOrder();
                }
            }
            else
            {
                if (w->focusPolicy() == Qt::StrongFocus)
                {
                    w->setFocusPolicy(Qt::NoFocus);
                    windowManager.setFocusOrder();
                }
            }
        }
    }
}

/**************************************************************************************************
 * 函数说明:
 *         获取焦点子控件。
 *************************************************************************************************/
void PatientStatusBarWidget::getSubFocusWidget(QList<QWidget *> &subWidgets) const
{
    subWidgets.clear();

    for (int i = 0; i < PATIENT_ICON_LABEL_NR; i++)
    {
        IWidget *subWidget = _iconMap.value((PatientIconLabel)i);
        if (NULL != subWidget)
        {
            subWidgets.append(subWidget);
        }
    }
}

/**************************************************************************************************
 * 图标点击事件。
 *************************************************************************************************/
void PatientStatusBarWidget::onIconClicked(int iconLabel)
{
    Q_UNUSED(iconLabel)
}

/**************************************************************************************************
 * 绘画事件。
 *************************************************************************************************/
void PatientStatusBarWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    // do nothing
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void PatientStatusBarWidget::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    // pacer icon
    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(PATIENT_ICON_LABEL_PACER)->children().at(1));
    if (l)
    {
        if (patientManager.getPacermaker())
        {
            l->setPixmap(_icons[PATIENT_ICON_PACER_ON]);
        }
        else
        {
            l->setPixmap(_icons[PATIENT_ICON_PACER_OFF]);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientStatusBarWidget::PatientStatusBarWidget() : IWidget("PatientStatusBarWidget"),
    _signalMapper(new QSignalMapper(this))
{
    _icons[PATIENT_ICON_NONE] = QPixmap();
    _icons[PATIENT_ICON_PACER_OFF] = QPixmap("/usr/local/nPM/icons/PaceMarkerOff.png");
    _icons[PATIENT_ICON_PACER_ON] = QPixmap("");// "/usr/local/nPM/icons/Pacemarkeron.png");打开时不需要图标。

    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < PATIENT_ICON_LABEL_NR; i++)
    {
        IWidget *w = new IWidget(QString("icon%1").arg(i + 1));
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setFocusPolicy(Qt::NoFocus);
        w->setFixedWidth(ICON_WIDTH);
        connect(w, SIGNAL(released()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(w, i);
        QHBoxLayout *layout = new QHBoxLayout(w);
        layout->setContentsMargins(0, 0, 0, 0);
        QLabel *l = new QLabel();
        l->setPixmap(_icons[PATIENT_ICON_NONE]);
        layout->addWidget(l, 0, Qt::AlignCenter);
        mainLayout->addWidget(w);
        _iconMap.insert((PatientIconLabel)i, w);
    }
    mainLayout->addStretch();
    connect(_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(iconClicked(int)));
    connect(this, SIGNAL(iconClicked(int)), this, SLOT(onIconClicked(int)));

    _timer.start(1000, this);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientStatusBarWidget::~PatientStatusBarWidget()
{
    _timer.stop();
}
