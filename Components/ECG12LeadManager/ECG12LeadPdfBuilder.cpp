#include "ECG12LeadPdfBuilder.h"
#include <QPrinter>
#include <QPrintEngine>
#include <QBuffer>
#include <QVariant>
#include "FontManager.h"
#include "LanguageManager.h"
#include "PrintManager.h"
#include "ECGParam.h"
#include "TimeDate.h"
#include "PatientDefine.h"
#include <QFile>
#include <QDateTime>
#include <Debug.h>
#include "StorageManager.h"
#include "DataStorageDefine.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include <QPainterPath>
#include <QApplication>
#include "Utility.h"
#include "HaruPdfWriter.h"
#include "QPrinterPdfWriter.h"
#include "USBManager.h"

#define TEMP_FILE "/tmp/tmp.pdf"
#define PAPER_INCH_HEIGHT 8.5
#define PAPER_INCH_WIDTH 11
#define PPK_CustomIODevice ((QPrintEngine::PrintEnginePropertyKey)(QPrintEngine::PPK_CustomBase + 1))
#define DPI 300

ECG12LeadPdfBuilder::ECG12LeadPdfBuilder(const QString &incidentDir, const QVector<int> &snapshotIndexs)
    :_incidentDir(incidentDir), _snapshotIndexs(snapshotIndexs),
      _data(NULL), _processFiles(0), _pdfFormat(PRINT_12LEAD_PDF_3X4_STAND),
      _2x6PdfTimeFormat(0)

{
    _pixelsPerMM = DPI / 25.4;
    currentConfig.getNumValue("ECG12L|PDFReportFormat", _pdfFormat);

    if(_pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_2X6_STAND)
    {
        currentConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", _2x6PdfTimeFormat);
    }

}

ECG12LeadPdfBuilder::ECG12LeadPdfBuilder(ECG12LDataStorage::ECG12LeadData *data)
    :_data(NULL), _processFiles(0), _pdfFormat(PRINT_12LEAD_PDF_3X4_STAND),
      _2x6PdfTimeFormat(0)
{
    //copy the data
    _data = new ECG12LDataStorage::ECG12LeadData();
    ::memcpy(_data, data, sizeof(ECG12LDataStorage::ECG12LeadData));

    _pixelsPerMM =  DPI / 25.4;
    currentConfig.getNumValue("ECG12L|PDFReportFormat", _pdfFormat);
    if(_pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_2X6_STAND)
    {
        currentConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", _2x6PdfTimeFormat);
    }
}

ECG12LeadPdfBuilder::~ECG12LeadPdfBuilder()
{
    if(_data)
    {
        delete _data;
        _data = NULL;
    }
    qDebug()<<Q_FUNC_INFO<<"ThreadID:"<<this->thread();
}

int ECG12LeadPdfBuilder::filesProcess()
{
    return _processFiles;
}

int ECG12LeadPdfBuilder::filesToBuild()
{
    if(_data)
    {
        return 1;
    }
    else
    {
        return _snapshotIndexs.count();
    }
}

static QString getSnapshotFilename(ECG12LDataStorage::ECG12LeadData *data)
{
    QString name;

    if(data)
    {
        QDateTime rescueTime = QDateTime::fromTime_t(data->commonInfo.rescueStartTime);
        QDateTime stampTime = QDateTime::fromTime_t(data->commonInfo.timeStamp);
        QString fulldisclosureNameStr = QString::fromAscii((const char*)&data->commonInfo.fulldisclosureName);
        QStringList strList = fulldisclosureNameStr.split('_', QString::KeepEmptyParts);

        name = usbManager.getUdiskMountPoint() + "/" + strList.at(0) + "_" + rescueTime.toString("yyyyMMdd") + "_" + rescueTime.toString("hhmmss") + "_TWL";
        name += "/" + strList.at(0) + "_" + stampTime.toString("yyyyMMdd") + "_" + stampTime.toString("hhmmss") + "_TWL" + ".pdf";
    }

    return name;
}


void ECG12LeadPdfBuilder::startBuilding()
{
    QByteArray content;
    QString filename;
    if(_data)
    {
        _processFiles += 1;
        //create a single file base on the data
        content = _createFile(_data);
        if(content.isEmpty() || _requestCancel)
        {
            //return empty content, might be canceled
            emit buildFinished(false);
            return;
        }
        else
        {
            //create file success
            filename = getSnapshotFilename(_data);
            emit oneFileComplete(filename, content);
        }
    }
    else if(!_incidentDir.isEmpty())
    {
        IStorageBackend * file = StorageManager::open(DATA_STORE_PATH + _incidentDir + ECG12L_FILE_NAME, QIODevice::ReadOnly);

        if(_snapshotIndexs.isEmpty())
        {
            //export all the incident
            int snapshotNum = file->getBlockNR();
            for(int i = 0; i< snapshotNum; i++)
            {
                _snapshotIndexs.append(i);
            }
        }

        ECG12LDataStorage::ECG12LeadData data;
        foreach(int snapshotIndex, _snapshotIndexs)
        {
            _processFiles += 1;
            bool isOk = file->readBlockData(snapshotIndex, (char *)&data, sizeof(ECG12LDataStorage::ECG12LeadData));
            if(isOk)
            {
                content = _createFile(&data);
                if(content.isEmpty() || _requestCancel)
                {
                    //return empty content, might be canceled
                    emit buildFinished(false);
                    return;
                }
                else
                {
                    //create file success
                    filename = getSnapshotFilename(&data);
                    emit oneFileComplete(filename, content);
                }
            }
            else
            {
                qdebug("Read data failed.");
                emit buildFinished(false);
                return;
            }
        }
        delete file;
    }

    emit buildFinished(true);
}


/***************************************************************************************************
 * calculate the font size base on the DPI
 **************************************************************************************************/
static int calculateFontH()
{
    int fonth = DPI / 6;
    return fonth;
}

QByteArray ECG12LeadPdfBuilder::_createFile(ECG12LDataStorage::ECG12LeadData *data)
{
    QByteArray file;
    if(!data)
    {
        return file;
    }

#if 0
    QPrinterPdfWriter *writer = new QPrinterPdfWriter(297, 210, 300);
    int fontH = calculateFontH();
    writer->setFont(fontManager.textFont(fontH));
#else
    HaruPdfWriter *writer = new HaruPdfWriter(297, 210);
    writer->setResolution(300);
    int fontH = calculateFontH();
    QString fontName = "";
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        fontName = writer->loadTrueTypeFont("/usr/local/iDM/fonts/DroidSansFallbackFull.ttf");
    }
    else
    {
        fontName = writer->loadTrueTypeFont("/usr/local/iDM/fonts/HuaWenXiHei.ttf");
    }
    writer->setFontAndSize(fontName, fontH);
    //writer->setFontAndSize(HaruPdfWriter::Courier, fontH);
#endif

    //获取要绘制的字符串
    QStringList strList;
    strList.clear();
    QString text = trs("ECG12LEADECGSNAPSHOT");
    strList << text;
    strList << (trs("PatientName") +": "+ QString::fromAscii((const char*)data->commonInfo.patientName));
    //病人年龄默认为-1，-1转化成字符时会发生反转，因此这里和"255"做判断
    if("255" == QString::number(data->commonInfo.patientAge))
    {
        strList << (trs("PatientAge")+": " + "");
    }
    else
    {
        strList << (trs("PatientAge")+": " + QString::number(data->commonInfo.patientAge));
    }
    if(data->commonInfo.patientSex == PATIENT_SEX_MALE)
    {
        strList << (trs("PatientSex")+": "+trs("Male"));
    }
    else if(data->commonInfo.patientSex == PATIENT_SEX_FEMALE)
    {
        strList << (trs("PatientSex")+": "+trs("Female"));
    }
    else
    {
        strList << (trs("PatientSex")+": "+ "");
    }
    strList << (trs("PatientID") +": "+ QString::fromAscii((const char*)data->commonInfo.patientID));

    if(ECG_BANDWIDTH_0525_40HZ == data->commonInfo.bandWidth)
        strList << (trs("ECGBandwidth")+": "+ "0.525 - 40 Hz");
    else if(ECG_BANDWIDTH_0525_150HZ == data->commonInfo.bandWidth)
        strList << (trs("ECGBandwidth")+": "+ "0.525 - 150 Hz");
    else
        strList << (trs("ECGBandwidth")+": "+ "unkown");

    //去掉名字后面的.crd
    QString fulldisclosureNameStr = QString::fromAscii((const char*)&data->commonInfo.fulldisclosureName);//.right(len - 4);
    int len = fulldisclosureNameStr.size();
    strList << (trs("IncidentIdentificationNumber") + ": "+ fulldisclosureNameStr.left(len - 4));

    timeDate.getDateTime(data->commonInfo.timeStamp, text, true, true);
    strList << (trs("ECG12LeadAcquired") + ": "+text);
    timeDate.getDateTime(data->commonInfo.rescueStartTime, text, true, true);
    strList << (trs("ECG12LeadRescueStarttime")+": " + text);
    strList << (trs("SupervisorDeviceIdentifier")+": " + QString::fromAscii((const char*)
                                                 data->commonInfo.deviceID));
    strList << (trs("SerialNum")+": " + QString::fromAscii((const char*)data->commonInfo.serialsNum));
    strList << (trs("SupervisorNotchFilter") + ": " + ECGSymbol::convert((ECGNotchFilter)data->commonInfo.notchFilter));

    strList << (trs("ECGGain") + ": " + ECGSymbol::convert((ECGGain)data->commonInfo.gain) + "   "+trs("PrintSpeed") + ": 25 mm/s");

    //绘制字符串
    QRect pageRect = writer->pageRect();
    int pageLeft = pageRect.left();
    int pageRight = pageRect.right();
    int pageTop = pageRect.top();
    int pageBottom = pageRect.bottom();

    int count = strList.count();
    int xoffset = pageRect.left();
    int yoffset =  0;
    int gap = fontH + fontH / 3;

    for (int i = 0; i < count; ++i)
    {
        if(i <= (count / 2))
        {
            xoffset = pageRect.left();
            yoffset =  pageTop + i * gap;
        }
        else
        {
            xoffset = pageRect.left() + pageRight * 3 / 5;
            yoffset = pageTop + (i - count / 2) * gap;
        }
        writer->drawText(QPointF(xoffset, yoffset), strList.at(i));
    }

    //grid offset
    int gridOffset = pageLeft / 4;
    //波形区的开始的Y轴坐标
    int waveStartYOffset = pageTop + (count /  2 + 1) * gap;
    int waveRegionHeight = pageBottom - waveStartYOffset - gap - gridOffset;
    int waveRegionWidth = pageRight - pageLeft - 2 * gridOffset;
    // wave segement
    int waveSegmentHeight = 0;
    //wave segment length
    int waveSegmentWidth = 0;

    if(_pdfFormat == PRINT_12LEAD_PDF_3X4_STAND || _pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA)
    {
        waveSegmentHeight = waveRegionHeight / (3 + 1);
        waveSegmentWidth = waveRegionWidth / 4;
    }
    else
    {
        waveSegmentHeight = waveRegionHeight / (6 + 1);
        waveSegmentWidth = waveRegionWidth / 2;
    }

    //draw wave time labels
    _drawWaveTimeLabel(writer, waveSegmentWidth, pageLeft, waveStartYOffset);

    //after draw the wave time label, mov the wave start y offset a little bit
    waveStartYOffset += gap;

    waveStartYOffset += gridOffset;

    //draw the grid
    QRectF gridRect;
    gridRect.setTop(waveStartYOffset);
    gridRect.setLeft(pageLeft);
    gridRect.setBottom(pageBottom);
    gridRect.setRight(pageRight);
    _drawGrid(writer, gridRect, gridOffset);

    //draw wave
    WaveGainInfoList gainlist = getGainList();
    for(int index = ECG_LEAD_I; index <= ECG_LEAD_V6; index++)
    {
        int waveSegmentStartYOffset = 0;
        if(_pdfFormat == PRINT_12LEAD_PDF_3X4_STAND || _pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA)
        {
            waveSegmentStartYOffset = waveStartYOffset + (index % 3) * waveSegmentHeight;
        }
        else
        {
            waveSegmentStartYOffset = waveStartYOffset + (index % 6) * waveSegmentHeight;
        }
        ECG12LeadWaveformInfo waveDesc = _getWaveFormCommonDesc(gainlist.at(index).first, waveSegmentStartYOffset, waveSegmentHeight, false);
        QPair<WaveformID, int > gainInfo = gainlist.at(index);
        gainInfo.second = data->commonInfo.gain;
        _drawWave(writer, index, waveSegmentWidth, pageLeft, gainInfo, waveDesc, data, false);

        if(_requestCancel)
        {
            delete writer;
            return file;
        }

        //don't run to fast, relax for a while
        Util::waitInEventLoop(10);
    }

    //draw the long wave
    int longwaveStartYOffset = 0;
    if(_pdfFormat == PRINT_12LEAD_PDF_3X4_STAND || _pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA)
    {
        longwaveStartYOffset = waveStartYOffset + 3 * waveSegmentHeight;
    }
    else
    {
        longwaveStartYOffset = waveStartYOffset + 6 * waveSegmentHeight;
    }
    int ecgIILeadIndex = ECG_LEAD_II;

    if(_pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA)
    {
        ecgIILeadIndex = ECG_LEAD_AVR;
    }
    ECG12LeadWaveformInfo waveDesc = _getWaveFormCommonDesc(WAVE_ECG_II, longwaveStartYOffset, waveSegmentHeight, true);
    QPair<WaveformID, int > gainIIinfo = gainlist.at(ecgIILeadIndex);
    gainIIinfo.second = data->commonInfo.gain;
    _drawWave(writer, ecgIILeadIndex, waveRegionWidth, pageLeft, gainIIinfo, waveDesc, data, true);

    writer->saveContent(file);
    delete writer;

    return file;
}

void ECG12LeadPdfBuilder::_drawText(PdfWriterBase *writer, const QString &text, qreal x, qreal y)
{
    if(text.isEmpty())
    {
        return;
    }
    writer->saveState();
    writer->setStorkeColor(Qt::black);
    writer->drawText(QPointF(x, y), text);
    writer->restoreState();
}

/***************************************************************************************************
 * draw the wave time label
 **************************************************************************************************/
void ECG12LeadPdfBuilder::_drawWaveTimeLabel(PdfWriterBase *writer, qreal waveSegmentLength, qreal xOffset, qreal yOffset)
{
    int index = 0;
    int fontlen = calculateFontH();
    if(_pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_2X6_STAND)
    {
        QString time;
        currentConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", index);
        if(index == 0 )
        {
            time = "0 - 5s";
        }
        else if(index == 1)
        {
            time = "2.5 - 7.5s";
        }
        else if(index == 2)
        {
            time = "5 - 10s";
        }
        for(int i = 0; i < 2;i++)
        {
            _drawText(writer,time,(waveSegmentLength + fontlen) * i + xOffset, yOffset);
        }
     }
    else
    {
        QString time[4] = {("0 - 2.5s"),("2.5 - 5s"),("5 - 7.5s"),("7.5 - 10s")};
        for(int i = 0; i < 4;i++)
        {
            _drawText(writer,time[i],(waveSegmentLength + fontlen) * i + xOffset, yOffset);
        }
    }
}

/***************************************************************************************************
 * draw a grid
 **************************************************************************************************/
void ECG12LeadPdfBuilder::_drawGrid(PdfWriterBase *writer, const QRectF &rect, int gridOffset)
{
    writer->saveState();
    writer->setLineWidth(DPI/240);
    writer->setStorkeColor(QColor(128, 128, 128));

    float offest = _pixelsPerMM * 5.00;
    QPainterPath path;

    //画竖线
    float offsetX = rect.left();
    while(offsetX <= rect.right())
    {
        path.moveTo(offsetX, rect.top() - gridOffset);
        path.lineTo(offsetX, rect.bottom() + gridOffset);
        offsetX += offest;
    }

    //画横线
    float offsetY = rect.top();
    while(offsetY <= rect.bottom())
    {
        path.moveTo(rect.left() - gridOffset, offsetY);
        path.lineTo(rect.right() + gridOffset, offsetY);
        offsetY += offest;
    }

    writer->drawPath(path);

    writer->restoreState();
}

/***************************************************************************************************
 * get the waveform gain list
 **************************************************************************************************/
WaveGainInfoList ECG12LeadPdfBuilder::getGainList()
{
    QList<int> waveID;
    WaveGainInfoList waveGainList;

    //Cabrera格式下的波形顺序
    if(_pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA)
    {
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_AVL));
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_I));
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_AVR));
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_II));
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_AVF));
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)ECG_LEAD_III));
        for(int lead = ECG_LEAD_V1;lead <= ECG_LEAD_V6;lead++)
        {
             waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        }
    }
    else//标准格式下的波形顺序
    {
        for(int lead = ECG_LEAD_I;lead <= ECG_LEAD_V6;lead++)
        {
            waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        }
    }
    int gain = -1;

    // 当前可打印的波形数
    for (int i = 0; i < waveID.count(); ++i)
    {
        WaveformID id = (WaveformID)waveID.at(i);
        switch (id)
        {
            case WAVE_ECG_I:
            case WAVE_ECG_II:
            case WAVE_ECG_III:
            case WAVE_ECG_aVR:
            case WAVE_ECG_aVL:
            case WAVE_ECG_aVF:
            case WAVE_ECG_V1:
            case WAVE_ECG_V2:
            case WAVE_ECG_V3:
            case WAVE_ECG_V4:
            case WAVE_ECG_V5:
            case WAVE_ECG_V6:
                gain = (int)ecgParam.get12LGain(ecgParam.waveIDToLeadID(id));
                //BUG:这边需要文件里的增益 而不是现场的增益
                break;
            default:
                break;
        }

        if (-1 != gain)
        {
            QPair<WaveformID, int > info;
            info.first = id;
            info.second = gain;
            waveGainList.append(info);
            gain = -1;
        }
    }

    return waveGainList;
}

/***************************************************************************************************
 * get the waveform info by the wave id
 **************************************************************************************************/
ECG12LeadWaveformInfo ECG12LeadPdfBuilder::_getWaveFormCommonDesc(WaveformID waveid, int waveStartYOffset, int waveSegmentHeight, bool longWave)
{
    ECG12LeadWaveformInfo waveDesc;
    waveDesc.reset();

    waveDesc.waveID = waveid;
    waveDesc.waveRate = waveformCache.getSampleRate(waveid);
    if(ECG_WAVE_RATE_250 == waveDesc.waveRate)
    {
        if(longWave)
        {
            waveDesc.waveDataNum = ECG_WAVE_RATE_250 * 10;
        }
        else
        {
            waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_250;
        }
    }
    else
    {
        if(longWave)
        {
            waveDesc.waveDataNum = ECG_WAVE_RATE_500 * 10;
        }
        else
        {
            waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_500;
        }
    }
    waveformCache.getRange(waveid, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
    waveformCache.getBaseline(waveid, waveDesc.waveBaseline);

    waveDesc.waveTitle = ECGSymbol::convert(ecgParam.waveIDToLeadID(waveid),
                                            ecgParam.getLeadConvention(),true , _isCabreaFormat());

    // 打印机速度/数据速度,x轴的偏移量。
    waveDesc.offsetX = (double)25 *_pixelsPerMM/ waveDesc.waveRate ;

    waveDesc.startY = waveStartYOffset;
    waveDesc.endY = waveDesc.startY  + waveSegmentHeight;
    waveDesc.mediumY = (waveDesc.endY - waveDesc.startY) / 2 + waveDesc.startY;

    return waveDesc;
}

/**************************************************************************************************
 * 判断是否是cabrea格式
 *************************************************************************************************/
bool ECG12LeadPdfBuilder::_isCabreaFormat() const
{
    return _pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA
            || _pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA;
}

void ECG12LeadPdfBuilder::_drawWave(PdfWriterBase *writer, int index, int waveSegmentLength, int xOffset,
                                    const QPair<WaveformID, int> &waveGain,
                                    const ECG12LeadWaveformInfo &waveDesc,
                                    ECG12LDataStorage::ECG12LeadData *data, bool longWave)
{
    qreal x1 = 0,y1 = 0,x2 = 0,y2 = 0;  // 需要连接的两点。
    int offset = 0;
    //用来负责处理 PDF的时间片段 在数组的偏移
    int timeoffest = 0;

    bool start = true;
    QPainterPath path;
    bool needMoveToPoint = true;

    if(index > MAX_WAVE_ECG12LEAD || data == NULL)
    {
        return;
    }

    //3x4格式
    if(_pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_3X4_STAND)
    {
        x1 = x2 = waveSegmentLength * (index/3) + xOffset;
        if(x1 > 0)
        {
            x1 +=(DPI/10)*(index/3);
            x2 +=(DPI/10)*(index/3);
        }
    }
    else//2 x 6 格式
    {
        x1 = x2 = waveSegmentLength * (index/6) + xOffset;
        if(x1 > 0)
        {
            x1 +=(DPI/10)*(index/6);
            x2 +=(DPI/10)*(index/6);
        }
    }

    if(longWave == 1)
    {
        x1 = x2 = xOffset;
        offset = waveDesc.waveDataNum;
    }
    else
    {
        if(_pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA || _pdfFormat == PRINT_12LEAD_PDF_3X4_STAND)
        {
            //标准格式有固定的数据偏移
            timeoffest = (index/3)* waveDesc.waveDataNum;
            offset = waveDesc.waveDataNum;
        }
        else//2 x 6 格式
        {
            //根据超级选项中的显示时间片段，偏移数据
            timeoffest = _2x6PdfTimeFormat * waveDesc.waveDataNum;
            offset = waveDesc.waveDataNum * 2;
        }
    }
    //绘制波形头
    _drawWaveLabel(writer, x1, waveDesc);

    writer->setLineWidth(DPI/119);
    writer->setStorkeColor(Qt::black);

    QPointF lastPoint;

    for(int i = 0 + timeoffest;i < offset + timeoffest; ++i)
    {
        if(data->wave[waveGain.first][i] & 0x40000000)
        {
            if(!lastPoint.isNull())
            {
                path.lineTo(lastPoint);
                lastPoint = QPointF();
            }

            int j = i + 1;
            // 判断后续是否有更多的无效数据
            if (j < offset + timeoffest)
            {
                while (data->wave[waveGain.first][j++] & 0x40000000)
                {
                    x2 += waveDesc.offsetX;

                    if (j >= offset + timeoffest)
                    {
                        break;
                    }
                }
                i = j - 1;
            }
            _drawDottedLine(writer, x1, (qreal)waveDesc.mediumY, x2, (qreal)waveDesc.mediumY);
            needMoveToPoint = true;
        }
        else
        {
            short wave = data->wave[waveGain.first][i] & 0xFFFF;
            if (waveGain.first == WAVE_ECG_aVR &&(_pdfFormat == PRINT_12LEAD_PDF_3X4_CABRELA
                    || _pdfFormat == PRINT_12LEAD_PDF_2X6_CABRELA))
            {
                wave = -wave;
            }
            double waveData = _mapWaveValue(waveGain, waveDesc, wave);

            // 画线。
            if (start)
            {
                y1 = waveData;
                start = false;
            }
            y2 = waveData;

            if(needMoveToPoint)
            {
                path.moveTo(x1, y1);
                needMoveToPoint = false;
                lastPoint = QPointF();
            }

            if(qFuzzyCompare(y1, y2))
            {
                lastPoint.setX(x2);
                lastPoint.setY(y2);
            }
            else if(!lastPoint.isNull())
            {
                path.lineTo(lastPoint);
                path.lineTo(x2, y2);
                lastPoint = QPointF();
            }
            else {
                path.lineTo(x2, y2);
            }


            lastPoint.setX(x2);
            lastPoint.setY(y2);

            //绘制内部起搏
            if ((data->wave[waveGain.first][i] >> 16) & ECG_INTERNAL_FLAG_BIT)//ipace
            {
                writer->saveState();
                QPointF startPos(x2, waveDesc.startY + 10);
                QPointF endPos(x2, waveDesc.endY - 10);
                QVector<quint16> pattern(2,4);
                writer->drawDashLine(startPos, endPos, pattern, 0);
                writer->restoreState();
            }

        }
        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;
    }

    if(!lastPoint.isNull())
    {
        path.lineTo(lastPoint);
    }

    if(!path.isEmpty())
    {
        writer->drawPath(path);
    }

    return;
}

/**************************************************************************************************
 * 绘制虚线
 *************************************************************************************************/
void ECG12LeadPdfBuilder::_drawDottedLine(PdfWriterBase *writer, qreal x1, qreal y1, qreal x2, qreal y2)
{
    writer->saveState();
    QVector<quint16> pattern;
    pattern<<DPI/30<<DPI/60;
    writer->setLineWidth(DPI/119);
    writer->drawDashLine(QPointF(x1, y1), QPointF(x2, y2), pattern, 0);
    writer->restoreState();
}

/**************************************************************************************************
 * 绘制波形标签
 *************************************************************************************************/
void ECG12LeadPdfBuilder::_drawWaveLabel(PdfWriterBase *writer, int xoffset, const ECG12LeadWaveformInfo &waveDesc)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return;
    }

    int fontH = calculateFontH();
    switch (waveDesc.waveID)
    {
        case WAVE_ECG_I:
        case WAVE_ECG_II:
        case WAVE_ECG_III:
        case WAVE_ECG_aVR:
        case WAVE_ECG_aVL:
        case WAVE_ECG_aVF:
        case WAVE_ECG_V1:
        case WAVE_ECG_V2:
        case WAVE_ECG_V3:
        case WAVE_ECG_V4:
        case WAVE_ECG_V5:
        case WAVE_ECG_V6:
        {
            _drawText(writer, waveDesc.waveTitle, 10 + xoffset, waveDesc.startY + fontH);
        }
             break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 映射波形值
 * value:波形实际值
 * max:波形最大值
 * min:波形最小值
 *************************************************************************************************/
double ECG12LeadPdfBuilder::_mapWaveValue(const QPair<WaveformID, int> &waveGain, const ECG12LeadWaveformInfo &waveDesc, int waveValue)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return 0;
    }

    int max = waveDesc.waveRangeMax ;
    int min = waveDesc.waveRangeMin ;
    double startY = waveDesc.startY;
    double endY = waveDesc.endY;
    double dpos = 0;
    double scaleData = 0;
    switch (waveDesc.waveID)
    {
        case WAVE_ECG_I:
        case WAVE_ECG_II:
        case WAVE_ECG_III:
        case WAVE_ECG_aVR:
        case WAVE_ECG_aVL:
        case WAVE_ECG_aVF:
        case WAVE_ECG_V1:
        case WAVE_ECG_V2:
        case WAVE_ECG_V3:
        case WAVE_ECG_V4:
        case WAVE_ECG_V5:
        case WAVE_ECG_V6:
        {
            switch (waveGain.second)
            {
                case ECG_GAIN_X0125:
                    scaleData = 0.125 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X025:
                    scaleData = 0.25 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X05:
                    scaleData = 0.5 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X10:
                    scaleData = 1.0 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X15:
                    scaleData = 1.5 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X20:
                    scaleData = 2.0 * 10 * _pixelsPerMM / 2.0;
                    break;

                case ECG_GAIN_X30:
                    scaleData = 3.0 * 10 * _pixelsPerMM / 2.0;
                    break;

                default:
                    break;
            }
            endY = waveDesc.mediumY + scaleData;
            startY = waveDesc.mediumY - scaleData;
            dpos = (max - waveValue) * ((endY - startY) / (max - min)) + startY;
            break;
        }
        default:
            return 0;
    }

    // 截顶处理
    if (dpos < waveDesc.startY)
    {
        dpos = waveDesc.startY;
    }else if (dpos > waveDesc.endY)
    {
        dpos = waveDesc.endY;
    }

    return dpos;
}
