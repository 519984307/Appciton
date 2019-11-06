/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/



#pragma once
#include <QObject>
#include <QBasicTimer>
#include "RecordPage.h"
#include "PatientDefine.h"
#include "TrendDataDefine.h"
#include <QStringList>
#include <QVector>
#include "PrintDefine.h"
#include "ECGDefine.h"
#include "RESPDefine.h"
#include "SPO2Define.h"
#include "CO2Define.h"
#include "IBPDefine.h"
#include "AGDefine.h"
#include "SystemDefine.h"
#include "EventDataDefine.h"
#include "Framework/Storage/IStorageBackend.h"

struct RecordWaveSegmentInfo
{
    WaveformID id;                  // wave id
    int startYOffset;               // start Y Offset of wave;
    int middleYOffset;              // yoffset middle value
    int endYOffset;                 // end y offset of the wave
    int minWaveValue;               // minimum wave value
    int maxWaveValue;               // maximum wave value
    int waveBaseLine;               // wave base line
    int sampleRate;                 // wave sample rate
    QVector<WaveDataType> secondWaveBuff;  // wave buffer of one second, the size should equal to sample

    struct {
        qreal prevSegmentLastYpos;  // the y value on the previous segment page edge
        qreal curPageFirstXpos;     // the first wave point's x position
        qreal dashOffset;           // dash offset when draw the dash line
        unsigned short lastWaveFlags;  // the wave flags of the last draw wave point
        int captionPixLength;       // store the caption pixel length of the wave caption
        QString caption;            // wave caption string
    } drawCtx;                      // wave draw context

    union {
        struct {
            ECGGain gain;
            bool in12LeadMode;
            Display12LeadFormat _12LeadDisplayFormat;
        } ecg;

        struct {
            RESPZoom zoom;
        } resp;

        struct {
            CO2DisplayZoom zoom;
        } co2;

        struct {
            int low;
            int high;
            bool isAuto;
            IBPPressureName pressureName;
            UnitType unit;
        } ibp;

        struct {
            AGDisplayZoom zoom;
        } ag;
    } waveInfo;
};


struct GraphAxisInfo
{
    QString caption;
    QPointF origin;
    qreal height;
    qreal validHeight;
    qreal width;
    qreal validWidth;
    qreal marginLeft;
    int xSectionWidth;
    int ySectionHeight;
    int xSectionNum;
    int ySectionNum;
    int tickLength;
    bool drawArrow;
    QStringList yLabels;
    QStringList xLabels;
};

class RecordPageGenerator : public QObject
{
    Q_OBJECT
public:
    enum
    {
        Type = 1
    };

    enum PageType
    {
        TitlePage,
        TrendPage,
        TrendTablePage,
        TrendGraphPage,
        TrendOxyCRGPage,
        WaveScalePage,
        WaveSegmentPage,
        EventListPage,
        EndPage,
        NullPage,
    };

    enum PrintPriority
    {
        PriorityNone = 0,
        PriorityContinuous = 1,
        PriorityReview = 2,
        PriorityTrigger = 3,
    };

    explicit RecordPageGenerator(QObject *parent = 0);
    ~RecordPageGenerator();

    /**
     * @brief setTrigger mark this generator as a trigger generator
     * @param flag
     */
    void setTrigger(bool flag);

    /**
     * @brief isTrigger check whether this generator is a trigger generator
     * @return
     */
    bool isTrigger() const;

    /**
     * @brief getPriority get the print prority of this generator
     * @return
     */
    virtual PrintPriority getPriority() const;

    /**
     * @brief type get the type of the generator
     * @return
     */
    virtual int type() const;

    /**
     * @brief font get the print font
     * @return  the print font
     */
    static QFont font();

    /**
     * @brief setPrintTime
     * @param timeSec
     */
    virtual void setPrintTime(PrintTime timeSec);

public slots:

    /**
     * @brief start create pages with specific time interval
     * @param interval in unit of ms,
     */
    void start(int interval = 200);

    /**
     * @brief stop stop page generate
     */
    void stop();

    /**
     * @brief generateControl control page generating
     * @param pause pause generating page or not
     */
    void pageControl(bool pause);

public:
    /**
     * @brief drawGraphAxis draw the graph axis info
     * @param painter painter
     * @param axisInfo the axis info
     */
    static void drawGraphAxis(QPainter * painter, const GraphAxisInfo &axisInfo);

    /**
     * @brief drawTrendGraph draw the trend graph
     * @param painter painter
     * @param axisInfo the trend graph axis info
     * @param graphInfo the trend graph info
     */
    static void drawTrendGraph(QPainter *painter, const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo);

    static void drawTrendGraphEventSymbol(QPainter *painter, const GraphAxisInfo &axisInfo,
                                          const TrendGraphInfo &graphInfo, const QList<BlockEntry> &eventList);

    /**
     * @brief getWaveInfos get the waveinfo for each wave in @waves base on the current running config
     * @param waves a list of wave ids
     * @return a list fo wave infos
     */
    static QList<RecordWaveSegmentInfo> getWaveInfos(const QList<WaveformID> &waves);

signals:
    /**
     * @brief generatePage emit when one page is generate
     * @param page
     */
    void generatePage(RecordPage *page);

    /**
     * @brief stopped emit when stop
     */
    void stopped();


protected:
    /**
     * @brief createPage create page process, the default implement will create
     *                   empty pages continuously, subclass should reinplement this function
     * @return the record page, if the page is NULL, the process is completed
     */
    virtual RecordPage *createPage();

    /**
     * @brief createTitlePage create a title page
     *          title page contain the title, patient info, and the recording time
     * @param title the page title
     * @param patInfo patient info struct
     * @param timestamp recording time, use current time when the value equal to 0
     * @return a record page
     */
    static RecordPage *createTitlePage(const QString &title, const PatientInfo &patInfo, unsigned timestamp = 0);

    /**
     * @brief createTrendPage create a trend page
     * @param trendData trend data
     * @param showEventTime show the event time or not
     * @param timeStringCaption override the default time string, if the string is empty, use the default string
     * @param trendPageTitle the trend page title string, if the string is empty, display nothing.
     * @return a record page
     */
    static RecordPage *createTrendPage(const TrendDataPackage& trendData, bool showEventTime = false,
                                       const QString  &timeStringCaption = QString(),
                                       const QString &trendPageTitle = QString(),
                                       const QString &extraInfo = QString());

    /**
     * @brief getTrendStringList get a trend string list from the trend data,
     *                           each string contain the subparam name, value and unit
     * @param trendData the trend data struct
     * @return  string list
     */
    static QStringList getTrendStringList(const TrendDataPackage& trendData);

    /**
     * @brief createWaveScalePage create the wave scale page
     * @param waveInfos wave infos
     * @param speed print speed
     * @return page
     */
    static RecordPage *createWaveScalePage(const QList<RecordWaveSegmentInfo> & waveInfos, PrintSpeed speed);

    /**
     * @brief createWaveSegments create the wave segments
     * @param waveInfos wave infos
     * @param segmentIndex the segment index,
     * @param speed print speed
     * @return page
     */
    static RecordPage *createWaveSegments(QList<RecordWaveSegmentInfo> *waveInfos, int segmentIndex, PrintSpeed speed);


    /**
     * @brief createOxyCRGGraph create the OxyCRG graph page
     * @param trendGraphInfo the trend info, the list usually contian 2 items, hr and spo2
     * @param waveInfo the OxyCRG wave info, resp or co2 wave, contains all the wave data for the OxyCRG duration
     * @return page
     */
    static RecordPage *createOxyCRGGraph(const QList<TrendGraphInfo> &trendGraphInfo, const OxyCRGWaveInfo &waveInfo);


    /**
     * @brief createStringListSegemnts create a page with the string list, each string will be draw a row
     *                                all the string will be left align
     * @note if contains too many string, some string will not be drawn on the page because the page height is limit
     * @param strList the string list
     * @return page
     */
    static RecordPage *createStringListSegemnt(const QStringList &strList);

    /**
     * @brief createEndPage create a finished page
     * @return  page
     */
    static RecordPage *createEndPage();

    /**
     * @brief timerEvent handle timer event
     */
    void timerEvent(QTimerEvent *);

    /**
     * @brief onStartGenerate call when start generate
     */
    virtual void onStartGenerate() {}

    /**
     * @brief call when stop generate page
     */
    virtual void onStopGenerate() {}



private:
    bool _requestStop;  // request stop flag
    bool _generate;     // generate page or not
    bool _trigger;      // store the trigger flag
    QBasicTimer _timer;  // timer to provide timer event
    int _timerInterval;  // timer interval
};
