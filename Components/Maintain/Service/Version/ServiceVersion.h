#pragma once
#include <QWidget>
#include <QPushButton>
#include "LabelButton.h"
#include <QTextEdit>
#include "LabeledLabel.h"

class ServiceVersion : public QWidget
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
    LabeledLabel *_NIBPDaemon;
    LabeledLabel *_SPO2;
    LabeledLabel *_TEMP;
    LabeledLabel *_nPMBoard;
    LabeledLabel *_PRT72;
    LabeledLabel *_CORE;

    LButtonEn *_btnReturn;               //返回按钮

    QString _ECGVersion;
    QString _NIBPVersion;
    QString _NIBPDaemonVersion;
    QString _SPO2Version;
    QString _TEMPVersion;
    QString _nPMBoardVersion;
    QString _PRT72Version;

    void _coreVersion(void);
};
#define serviceVersion (ServiceVersion::Construation())
