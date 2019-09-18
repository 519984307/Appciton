/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#include "AlarmInfoBarWidget.h"

static AlarmInfoBarWidget *phyInfoBarWidget = NULL;
static AlarmInfoBarWidget *techInfoBarWidget = NULL;

AlarmInfoBarWidget *AlarmInfoBarWidget::registerAlarmInfoBar(AlarmType alarmType, AlarmInfoBarWidget *instance)
{
    AlarmInfoBarWidget * old = NULL;
    if (alarmType == ALARM_TYPE_PHY)
    {
        old = phyInfoBarWidget;
        phyInfoBarWidget = instance;
    }
    else if (alarmType == ALARM_TYPE_TECH)
    {
        old = techInfoBarWidget;
        techInfoBarWidget = instance;
    }
    return old;
}

AlarmInfoBarWidget *AlarmInfoBarWidget::getAlarmPhyInfoBar(AlarmType alarmType)
{
    AlarmInfoBarWidget *infoBarWidget = NULL;
    if (alarmType == ALARM_TYPE_PHY)
    {
        infoBarWidget = phyInfoBarWidget;
    }
    else if (alarmType == ALARM_TYPE_TECH)
    {
        infoBarWidget = techInfoBarWidget;
    }
    return infoBarWidget;
}
