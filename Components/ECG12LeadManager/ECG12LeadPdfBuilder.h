#pragma once
#include "ECG12LeadFileBuilderBase.h"
#include "ECG12LDataStorage.h"
#include <QVector>
#include <QByteArray>
#include <QPainter>
#include <QPair>
#include <QList>
#include "ParamDefine.h"
#include "PrintECG12LeadLayout.h"

typedef QList< QPair<WaveformID, int> > WaveGainInfoList;

class PdfWriterBase;
class ECG12LeadPdfBuilder: public ECG12LeadFileBuilderBase
{
public:
    // build pdf files from the incident file, if the snapshotIndexs is empty, the builder will export
    // all the snapshots in the incident
    ECG12LeadPdfBuilder(const QString& incidentDir, const QVector<int> &snapshotIndexs = QVector<int>());

    // build pdf from the 12 data, the builder will make a copy of the data
    ECG12LeadPdfBuilder(ECG12LDataStorage::ECG12LeadData *data);

    ~ECG12LeadPdfBuilder();

    Type type() const { return PDF;}

    int filesProcess();
    int filesToBuild();

    void startBuilding();

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif

private:
    QByteArray _createFile(ECG12LDataStorage::ECG12LeadData *data);
    void _drawText(PdfWriterBase *writer, const QString &text, qreal x, qreal y);
    void _drawWaveTimeLabel(PdfWriterBase *writer, qreal waveSegmentLength, qreal xOffset, qreal yOffset);
    void _drawGrid(PdfWriterBase *writer, const QRectF &rect, int gridOffset);
    WaveGainInfoList getGainList();
    ECG12LeadWaveformInfo _getWaveFormCommonDesc(WaveformID waveid, int waveStartYOffset, int waveSegmentHeight,
                                                 bool longWave);
    bool _isCabreaFormat() const;
    void _drawWave(PdfWriterBase *writer, int index, int waveSegmentLength, int xOffset,
                   const QPair<WaveformID, int> &waveGain, const ECG12LeadWaveformInfo &waveDesc,
                   ECG12LDataStorage::ECG12LeadData *data, bool longWave);
    void _drawDottedLine(PdfWriterBase *writer, qreal x1, qreal y1,
                         qreal x2, qreal y2);
    void _drawWaveLabel(PdfWriterBase *writer, int xoffset, const ECG12LeadWaveformInfo &waveDesc);
    double _mapWaveValue(const QPair<WaveformID, int> &waveGain,
                         const ECG12LeadWaveformInfo &waveDesc,int waveValue);

    QString _incidentDir;
    QVector<int> _snapshotIndexs;
    ECG12LDataStorage::ECG12LeadData *_data;
    float _pixelsPerMM;
    int _processFiles;
    int _pdfFormat;
    int _2x6PdfTimeFormat;
};
