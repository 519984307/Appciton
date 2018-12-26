/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/26
 **/

#include "EventListPageGenerator.h"
#include "PatientManager.h"
#include "FontManager.h"
#include <QPainter>
#include <QDebug>

class EventListPageGeneratorPrivate
{
public:
    EventListPageGeneratorPrivate()
        : curPageType(RecordPageGenerator::TitlePage)
    {}

    RecordPage *drawEventListPage();

    RecordPageGenerator::PageType curPageType;
    QStringList eventList;
};

EventListPageGenerator::EventListPageGenerator(QStringList &eventList, QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new EventListPageGeneratorPrivate)
{
    d_ptr->eventList = eventList;
}

EventListPageGenerator::~EventListPageGenerator()
{
}

RecordPage *EventListPageGenerator::createPage()
{
    switch (d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = EventListPage;
        return createTitlePage(QString(trs("EventListPrint")), patientManager.getPatientInfo());
    case EventListPage:
        if (!d_ptr->eventList.isEmpty())
        {
            return d_ptr->drawEventListPage();
        }
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}

RecordPage *EventListPageGeneratorPrivate::drawEventListPage()
{
    // calculate the event text width
    int textWidth = 0;
    QFont font = fontManager.recordFont(24);
    foreach(QString eventStr, eventList)
    {
        int w = 0;
        w = fontManager.textWidthInPixels(eventStr, font);
        if (w > textWidth)
        {
            textWidth = w;
        }
    }

    int pageWidth = textWidth + font.pixelSize() * 3;
    int fontH = fontManager.textHeightInPixels(font);

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    QRect textRect(font.pixelSize(), fontH, textWidth, fontH);
    int num = 0;
    while (!eventList.isEmpty() && num <= 9)
    {
        QString eventStr = eventList.takeFirst();
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, eventStr);
        textRect.translate(0, fontH);
        num++;
    }
    return page;
}
