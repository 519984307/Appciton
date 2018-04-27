#pragma once
#include <QObject>
#include <RecordPage.h>
#include <QGraphicsItem>

class RecordPageGenerator : public QObject
{
    Q_OBJECT
public:
    enum
    {
        Type = 1
    };

    RecordPageGenerator(QObject *parent = 0 );
    ~RecordPageGenerator();

    /**
     * @brief type get the type of the generator
     * @return
     */
    virtual int type() const;

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
     * @brief timerEvent handle timer event
     */
    void timerEvent(QTimerEvent *);

private:
    bool _requestStop; // request stop flag
    bool _generate;    // generate page or not
    int _timerID;      //timer id
};
