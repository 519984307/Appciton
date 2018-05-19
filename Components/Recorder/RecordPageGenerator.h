#pragma once
#include <QObject>
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
    QVector<WaveDataType> secondWaveBuff; // wave buffer of one second

    struct {
        qreal prevSegmentLastYpos;  // the y value on the previous segment page edge
        qreal curPageFirstXpos;     // the first wave point's x position
        qreal dashOffset;           // dash offset when draw the dash line
        unsigned short lastWaveFlags; // the wave flags of the last draw wave point
        int captionPixLength;       // store the caption pixel length of the wave caption
        char caption[64];           // wave caption string
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
            SPO2Gain gain;
        } spo2;

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
        WaveScalePage,
        WaveSegmentPage,
        EndPage,
        NullPage,
    };

    RecordPageGenerator(QObject *parent = 0 );
    ~RecordPageGenerator();

    /**
     * @brief type get the type of the generator
     * @return
     */
    virtual int type() const;

    /**
     * @brief font get the print font
     * @return  the print font
     */
    QFont font() const;


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
     * @return a record page
     */
    static RecordPage *createTrendPage(const TrendDataPackage& trendData, bool showEventTime = false);

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
    static RecordPage *createWaveSegments(QList<RecordWaveSegmentInfo> &waveInfos, int segmentIndex, PrintSpeed speed);


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

private:
    bool _requestStop; // request stop flag
    bool _generate;    // generate page or not
    int _timerID;      // timer id
};
