#pragma once
#include "MenuWidget.h"
#include <QPushButton>
#include "LabelButton.h"
#include <QTextEdit>
#include "LabeledLabel.h"

class ServiceVersion : public MenuWidget
{
    Q_OBJECT

public:
    static ServiceVersion &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceVersion();
        }

        return *_selfObj;
    }
    static ServiceVersion *_selfObj;
    ~ServiceVersion();

    //初始化
    void init();

    void getECGVersion(unsigned char *data, int len);
    void getNIBPVersion(unsigned char *data, int len);
    void getSPO2Version(unsigned char *data, int len);
    void getTEMPVersion(unsigned char *data, int len);
    void getnPMBoardVersion(unsigned char *data, int len);
    void getPRT72Version(unsigned char *data, int len);

private:
    ServiceVersion();

    QLabel *_title;                      //标题

    static const int minLen = 65;

    LabeledLabel *_nPM;
    LabeledLabel *_ECG;
    LabeledLabel *_NIBP;
    LabeledLabel *_SPO2;
    LabeledLabel *_TEMP;
    LabeledLabel *_nPMBoard;
    LabeledLabel *_PRT72;
    LabeledLabel *_CORE;

    QString _ECGVersion;
    QString _NIBPVersion;
    QString _SPO2Version;
    QString _TEMPVersion;
    QString _nPMBoardVersion;
    QString _PRT72Version;

    void _coreVersion(void);
};
#define serviceVersion (ServiceVersion::Construation())
