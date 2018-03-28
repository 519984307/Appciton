#include "EventReviewWindow.h"
#include <QListWidget>
#include "EventWaveWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "EventTrendItemDelegate.h"
#include "ColorManager.h"
#include "EventStorageManager.h"
#include <QBoxLayout>
#include "Debug.h"
#include <QFile>

struct EventDataPraseContex
{
    EventDataPraseContex()
        :eventDataBuf(NULL),
          infoSegment(NULL),
          trendSegment(NULL),
          almSegment(NULL),
          codeMarkerSegment(NULL)
    {
    }

    void reset()
    {
        if(eventDataBuf)
        {
            qFree(eventDataBuf);
            eventDataBuf = NULL;
        }
        infoSegment = NULL;
        trendSegment = NULL;
        waveSegments.clear();
        almSegment = NULL;
        codeMarkerSegment = NULL;
    }

    ~EventDataPraseContex()
    {
        if(eventDataBuf)
        {
            qFree(eventDataBuf);
        }
    }

    char *eventDataBuf; //buffer for the event data
    EventInfoSegment *infoSegment;  //pointor of the info segment
    TrendDataSegment *trendSegment; //pointer of the trend segment
    QVector<WaveformDataSegment *> waveSegments;    //pointers of the wave segments
    AlarmInfoSegment *almSegment;   //pointer to the alarm segment
    CodeMarkerSegment *codeMarkerSegment;   //pointer to the code marker segment
};

class EventReviewWindowPrivate
{
public:
    EventReviewWindowPrivate()
        :trendListWidget(NULL),
          waveWidget(NULL),
          backend(NULL),
          curParseIndex(-1)
    {
        backend = eventStorageManager.backend();
    }

    //prase the event data from the backend
    bool parseEventData(int dataIndex)
    {
        if(!backend || dataIndex >= (int) backend->getBlockNR() || dataIndex < 0)
        {
            return false;
        }

        quint32 length  = backend->getBlockDataLen(dataIndex);

        char *buf = (char *)qMalloc(length);
        if(!buf)
        {
            return false;
        }

        if (backend->readBlockData(dataIndex, buf, length) != length)
        {
            qFree(buf);
            return false;
        }

        ctx.reset();

        //clear the wave with empyt wave segments
        waveWidget->setWaveSegments(ctx.waveSegments);

        ctx.eventDataBuf = buf;

        char *parseBuffer = buf;
        bool parseEnd = false;
        while(!parseEnd)
        {
            EventSegmentType *eventType = (EventSegmentType *)parseBuffer;
            switch (*eventType) {
            case EVENT_INFO_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.infoSegment = (EventInfoSegment *) parseBuffer;
                //find the location of the next event type
                parseBuffer += sizeof(EVENT_INFO_SEGMENT);
                break;
            case EVENT_TRENDDATA_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.trendSegment = (TrendDataSegment *)parseBuffer;
                //find the location of the next event type
                parseBuffer += sizeof(TrendDataSegment) + ctx.trendSegment->trendValueNum * sizeof(TrendValueSegment);
                break;
            case EVENT_WAVEFORM_SEGMENT:
                //skip the offset of the segment type field
                parseBuffer += sizeof(EventSegmentType);
                ctx.waveSegments.append((WaveformDataSegment *) parseBuffer);
                //find the location of the next event type
                parseBuffer += sizeof(WaveformDataSegment) + ctx.waveSegments.last()->waveNum * sizeof(WaveDataType);
                break;
            case EVENT_ALARM_INFO_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.almSegment = (AlarmInfoSegment *) parseBuffer;
                parseBuffer += sizeof(AlarmInfoSegment);
                break;
            case EVENT_CODEMARKER_SEGMENT:
                parseBuffer += sizeof(EventSegmentType);
                ctx.codeMarkerSegment = (CodeMarkerSegment *) parseBuffer;
                parseBuffer += sizeof(CodeMarkerSegment);
                break;
            default:
                qdebug("unknown segment type %d, stop parsing.",  *eventType);
                parseEnd = true;
                break;
            }
        }

        if(parseBuffer >= buf + length)
        {
            parseEnd = true;
        }
    }


    void loadTestWaveSegment()
    {
        QFile f("/usr/local/nPM/demodata/SPO2");
        f.open(QIODevice::ReadOnly);
        QByteArray d = f.readAll();
        f.close();
        
        WaveformDataSegment *seg = new WaveformDataSegment;
    }

    QListWidget *trendListWidget;
    EventWaveWidget *waveWidget;
    EventDataPraseContex ctx;
    IStorageBackend *backend;
    int curParseIndex;
};

EventReviewWindow::EventReviewWindow()
    :PopupWidget(), d_ptr(new EventReviewWindowPrivate())
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Event Review");

    int windowWidth = windowManager.getPopMenuWidth();
    int windowHeight = windowManager.getPopMenuHeight();


    QHBoxLayout *hlayout =new QHBoxLayout();

    d_ptr->waveWidget = new EventWaveWidget;
    d_ptr->waveWidget->setWaveSegments(d_ptr->ctx.waveSegments);
    hlayout->addWidget(d_ptr->waveWidget, 3);
    hlayout->addSpacing(4);

    d_ptr->trendListWidget = new QListWidget;
    QPalette pal = d_ptr->trendListWidget->palette();
    pal.setColor(QPalette::Base, Qt::black);
    d_ptr->trendListWidget->setPalette(pal);
    d_ptr->trendListWidget->setFrameShape(QFrame::NoFrame);
    d_ptr->trendListWidget->setItemDelegate(new EventTrendItemDelegate(d_ptr->trendListWidget));
    d_ptr->trendListWidget->setResizeMode(QListWidget::Adjust);

    QListWidgetItem *item;
    QStringList valueList;
    QStringList titleList;
    QStringList unitList;
    valueList << "60" <<"98" << "120/90/(100)" << "4.6" << "14" << "120/90(100)" << "20" <<"36.5" << "36.4" <<"0.1";
    titleList <<"HR" <<"SPO2" <<"NIBP" <<"CO2" <<"RR" <<"ART" <<"CVP" <<"T1" <<"T2" <<QString::fromUtf8("ΔT");
    unitList <<"bpm" <<"%" <<"mmHg" <<"%" <<"rpm"<<"mmHg"<<"mmHg" << QString::fromUtf8("°C")
               << QString::fromUtf8("°C")<< QString::fromUtf8("°C");

    QStringList paramNames;
    paramNames << "ECG" <<"SPO2" <<"NIBP" << "CO2" <<"RESP" <<"IBP" <<"IBP"<<"TEMP"<<"TEMP"<<"TEMP";

    QFont valueFont = fontManager.numFont(32);
    QFont titleFont = fontManager.textFont(16);
    QFont unitFont = fontManager.textFont(12);

    for(int i = 0; i< valueList.length(); i++)
    {
        item = new QListWidgetItem(d_ptr->trendListWidget);

        if(valueList.at(i).length() >= 10)
        {
            item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(fontManager.numFont(28)));
        }
        else
        {
            item->setData(EventTrendItemDelegate::ValueFontRole, qVariantFromValue(valueFont));
        }
        item->setData(EventTrendItemDelegate::TitleFontRole, qVariantFromValue(titleFont));
        item->setData(EventTrendItemDelegate::UnitFontRole, qVariantFromValue(unitFont));

        item->setData(EventTrendItemDelegate::ValueTextRole, valueList.at(i));
        item->setData(EventTrendItemDelegate::TitleTextRole, titleList.at(i));
        item->setData(EventTrendItemDelegate::UnitTextRole, unitList.at(i));

        item->setData(EventTrendItemDelegate::TrendAlarmRole, false);

        item->setTextColor(colorManager.getColor(paramNames.at(i)));
        item->setFlags(Qt::NoItemFlags);
    }

    hlayout->addWidget(d_ptr->trendListWidget, 1);

    contentLayout->addLayout(hlayout);


    setFixedSize(windowWidth, windowHeight);
}

EventReviewWindow::~EventReviewWindow()
{

}
