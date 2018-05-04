#include "ContinuousPageGenerator.h"
#include "LanguageManager.h"
#include "PatientManager.h"

class ContinuousPageGeneratorPrivate
{
public:
    enum GenerateState {
        Title,
        Trend,
        Wave,
    };

    ContinuousPageGeneratorPrivate()
        :state(Title)
    {

    }

    int state;
};
ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate())
{

}

ContinuousPageGenerator::~ContinuousPageGenerator()
{

}

int ContinuousPageGenerator::type() const
{
    return Type;
}

RecordPage *ContinuousPageGenerator::createPage()
{
    switch(d_ptr->state)
    {
    case ContinuousPageGeneratorPrivate::Title:
        d_ptr->state += 1;
        return createTitlePage(QString(trs("RealtimeSegmentWaveRecording")), patientManager.getPatientInfo(), 0);
    default:
        break;
    }
    return NULL;
}
