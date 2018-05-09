#include "PrintPage.h"
#include "PrintECG12LeadLayout.h"
#include "TimeDate.h"
#include "TimeManager.h"
#include "DataStorageDirManager.h"
#include "ECGSymbol.h"
#include "WaveformCache.h"
#include "FontManager.h"
#include "ECGParam.h"
#include "DataStorageDirManager.h"
#include "Debug.h"
#include "IConfig.h"
#include "WindowManager.h"
#include "ECG12LSnapShotReview.h"
#include "ECG12LeadManager.h"
#include "ECG12LDataPrint.h"
#include "ECGWaveWidget.h"

PrintECG12LeadLayout *PrintECG12LeadLayout::_selfObj = NULL;

/**************************************************************************************************
 * 构造函数
 *************************************************************************************************/
PrintECG12LeadLayout::PrintECG12LeadLayout() : PrintLayoutIFace(PRINT_LAYOUY_ID_12L_ECG)
{
    _step = -1;
    _index = -1;
    _totalData = 1;
    _longWaveOffest = -1;
    _filePath.clear();
    _file = NULL;
    _fileNum= 0;
    _realstartid = 0;
    _rescuetartid = 0;
    _printStartPage = true;   //打印开始标志位
    _selectdata = NULL;
    _data = NULL;
    memset(_drawLeadFaultLen,0,sizeof(_drawLeadFaultLen));
    int leadoffFontLen = fontManager.textWidthInPixels(trs("LeadOff"),printFont());
    int leadoffFontH = fontManager.textHeightInPixels(printFont());
    _leadoffImage = new QImage(leadoffFontLen + 2,leadoffFontH + PIXELS_PER_MM,QImage::Format_MonoLSB);
    QPainter painter(_leadoffImage);
    painter.setPen(Qt::white);
    painter.setFont(printFont());
    _leadoffImage->fill(painter.brush().color());
    painter.drawText(_leadoffImage->rect(), trs("LeadOff"));

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawLine(0, leadoffFontH + 1, leadoffFontLen, leadoffFontH + 1);
}

/**************************************************************************************************
 * 析构函数
 *************************************************************************************************/
PrintECG12LeadLayout::~PrintECG12LeadLayout()
{
    if (NULL != _file)
    {
        delete _file;
    }
    _filePath.clear();
    _step = -1;
    _index = -1;
    _realstartid = 0;
    _rescuetartid = 0;
    _fileNum= 0;
    if(NULL !=_data)
    {
       delete _data;
    }
    if(NULL !=_selectdata)
    {
       delete _selectdata;
        _selectdata = NULL;
    }
    if(NULL != _leadoffImage)
    {
        delete _leadoffImage;
        _leadoffImage = NULL;
    }
}

/**************************************************************************************************
 *开始布局
 *************************************************************************************************/
void PrintECG12LeadLayout::start()
{
    if (!ecg12LDataPrint.getPrintRealTime())
    {
        ecg12LeadManager.getPrintSnapShotPath(_filePath);

        //getPrintPath();
        if (0 == _filePath.count())
        {
            return;
        }
    }

    _fileNum = 0;
    _isCabrera = 0;
    _isNeedDrawFirstPage = TRUE;
    _getWaveForm(_waveGainList);
    _printSpeed = printManager.getPrintSpeed();
    if(_printSpeed == PRINT_SPEED_250)//25MM/s
    {
        _pagewidth = 25 * PIXELS_PER_MM * 2.5 + 50;
    }
    else // PRINT_SPEED_500
    {
        _pagewidth = 50 * PIXELS_PER_MM * 2.5 + 50;
    }

    _type =(ECG12LeadDataPrintType)ecg12LSnapShotReview.getPrintIndex();

    //end
    _data = NULL;
    _selectdata = NULL;
    memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
}

/**************************************************************************************************
 * 终止
 *************************************************************************************************/
void PrintECG12LeadLayout::abort()
{
  //_paramList.clear();
    _filePath.clear();
    _printStartPage = true;
    _step = -1;
    _index = -1;
    _shortWaveOffest = -1;
    _longWaveOffest = -1;
     _isCabrera = 0;
    //这里之所以加了数据的删除，因为当打印机失效之后，打印机终止运行会导致程序段错误
    if(NULL !=_data)
    {
       delete _data;
       _data = NULL;
    }
    if(NULL !=_selectdata)
    {
       delete _selectdata;
        _selectdata = NULL;
    }
    memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintECG12LeadLayout::exec(PrintContent *content)
{
    if(content->printType == PRINT_TYPE_TRIGGER_12L_ECG)//12导联快照触发
    {
        _realtimePrint = TRUE;
        if(NULL ==_data)
        {
            _data = new ECG12LDataStorage::ECG12LeadData();

            if (NULL != content)
            {
                memcpy(_data,content->extraData,content->extraDataLen);
            }
        }
        return _rescuePage(true);
    }
    else if(content->printType == PRINT_TYPE_REVIEW)//12导存储打印
    {
         _realtimePrint = FALSE;
        switch (_type)
        {
            case PRINT_12L_SELECT_RESCUE://当前页最新的营救
                return _rescuePage(FALSE);
                break; 
            case PRINT_12L_SELECT:
                return _drawSelectPage(FALSE);
                break;
            default:
                return NULL;
         }
     }
    return NULL;
}

/**************************************************************************************************
 * 数据初始化
 *************************************************************************************************/
bool PrintECG12LeadLayout::_dataInit(bool IsRealtime)
{
    if (_index < 0)
    {
        if(!IsRealtime)//趋势打印
        {
            //int a = _filePath.count();
            while (_fileNum < _filePath.count())
            {
                QString fileStr = DATA_STORE_PATH + _filePath.at(_fileNum)
                        + ECG12L_FILE_NAME;
                _fileNum++;
                if (NULL != _file)
                {
                    _file->reload(fileStr, QIODevice::ReadOnly);
                }
                else
                {
                    // 如果是趋势所有文件，文件读写
                    _file = StorageManager::open(fileStr, QIODevice::ReadOnly);
                }
                QString GileStr =  DATA_STORE_PATH + _filePath.at(_fileNum - 1);
                QString FileStr = dataStorageDirManager.getCurFolder();

                //如果是当前路径 读取当前的个数
                if( GileStr == dataStorageDirManager.getCurFolder())
                {
                    _totalData = ecg12LDataStorage.backend()->getBlockNR();
                }
                else  //不是当前路径 读取文件内的个数
                {
                    _totalData = _file->getBlockNR();
                }
                if (_totalData > 0)
                {
                    _step = 0;
                    _index = _totalData -1;
                    _printStartPage = true;
                    break;
                }
            }
            if (_index < 0)
            {
                return false;
            }
        }
        else//快照触发
        {
             if(_step == -1)
             {
                _printStartPage = true;
                _totalData = 1;
                _realstartid ++;
                _step = 0;
             }
             return true;
        }
    }
    return true;
}

/**************************************************************************************************
 * 函数说明：获取显示的波形及其增益。
 *************************************************************************************************/
void PrintECG12LeadLayout::_getWaveForm(WaveGainList &waveList)
{
    QList<int> waveID;
    waveID.clear();  
    currentConfig.getNumValue("ECG12L|PrintSnapshotFormat", _isCabrera);
    if(_isCabrera)//Cabrera格式下的波形顺序
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
    _numOfWaveTrace = ECG_LEAD_V6 + 1;
    int gain = -1;
    int count = waveID.count();

    // 当前可打印的波形数
    waveList.clear();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
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
                //gain = (int)ecgParam.getGain(ecgParam.waveIDToLeadID(id));
                gain = (int)ecgParam.get12LGain(ecgParam.waveIDToLeadID(id));
                //这边需要文件里的增益 而不是现场的增益
                break;
            default:
                break;
        }
        if (-1 != gain)
        {
            waveList.append(WaveGain(id, gain));
            gain = -1;
        }
    }
}

/**************************************************************************************************
 * 构建数据页
 *
 * 开一个新页
 * 画打印速度和增益和导联
 * 画波形
 *************************************************************************************************/
PrintPage *PrintECG12LeadLayout::_rescuePage(bool IsRealtime)
{
    static int id = 0;
    if (!_dataInit(IsRealtime))
    {
        return NULL;
    }
    if(_printStartPage)
    {
        if(!IsRealtime)//趋势打印读取文件
        {
            if(NULL == _data)
            {
                _data = new ECG12LDataStorage::ECG12LeadData();
                _debugNewCount++;
                debug("\033[36m _debugNewCount =%d \033[m",_debugNewCount);
            }
            _rescuetartid++;//趋势头文件的索引增加
            if(NULL !=_file)
            {
                QString GileStr =  DATA_STORE_PATH + _filePath.at(_fileNum - 1);
                QString FileStr =dataStorageDirManager.getCurFolder();
                if( GileStr == dataStorageDirManager.getCurFolder()) //如果是当前路径 读取当前的内容
                {
                    _totalData = ecg12LDataStorage.backend()->readBlockData(_index,(char*)_data,
                            sizeof(ECG12LDataStorage::ECG12LeadData));
                }
                else  //不是当前路径 读取文件内参数
                {
                    _file->readBlockData(_index,(char*)_data,
                            sizeof(ECG12LDataStorage::ECG12LeadData));
                }
            }
            else
            {
                return NULL;
            }
        }
        else   //实时打印
        {
            //异常情况处理
        }
        _printStartPage = false;

        //进入开始打印页
        return _startPage(_data);
    }

    if(_step < 4) //2.5s的十二道波形
    {
        _getWaveFormShortWave();

        //需要一个宏定义 因为波形数据固定，页码宽度固定
        PrintPage *page = new PrintPage(_pagewidth + 50);
        page->setID("ECG12Lead" + QString().sprintf("%03d", id++));
        QPainter painter(page);
        painter.setFont(printFont());
        painter.setPen(Qt::white);

        // 清空所有的数
        painter.fillRect(page->rect(), Qt::SolidPattern);
        //标志打印的时间段
        _markTimeSegments(painter,_step);
        _step++;
        _shortWaveOffest = _step - 1;

        //做个保护
        if(_shortWaveOffest < 0)
        {
            return NULL;
        }
        for(int i = 3 * (_step - 1);i <= 3 * (_step - 1) + 2;i++)
        {
            _drawShortWave(i, painter, _pagewidth + 50,_shortWaveOffest,_data);
            /* 打印完一次12导数据后清空“Lead Off”偏移 */
            memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
        }
       return page;
    }
    else if(_step < 5)  //10s的II导联波形
    {
        _getWaveFormLongWave();
        int width;
        if(_printSpeed == PRINT_SPEED_250)
        {
            width = 25 * PIXELS_PER_MM * 10 + 50;
        }
        else // PRINT_SPEED_500
        {
            width = 50 * PIXELS_PER_MM * 10 + 50;
        }
        PrintPage *page = new PrintPage(width);
        page->setID("ECG12Lead" + QString().sprintf("%03d", id++));
        QPainter painter(page);
        painter.setFont(printFont());
        painter.setPen(Qt::white);
        painter.fillRect(page->rect(), Qt::SolidPattern);

        //标志打印的时间段
//        _markTimeSegments(painter,_step);

        //做一个保护
        _drawlongWave(_leadMap(ECG_LEAD_II), painter, width, _data);

        if (IsRealtime)
        {
            _step++;
        }
        else
        {
            _shortWaveOffest = -1;
            _longWaveOffest = -1;
            _step = 0;
            _printStartPage = true;
            //页面索引逐次递减
            --_index;

            if(NULL !=_file)
            {
                QString GileStr =  DATA_STORE_PATH + _filePath.at(_fileNum - 1);
                QString FileStr =dataStorageDirManager.getCurFolder();

                //如果是当前路径 读取当前的内容
                if( GileStr == dataStorageDirManager.getCurFolder())
                {
                    _totalData = ecg12LDataStorage.backend()->readBlockData(
                    _index,(char*)_data,sizeof(ECG12LDataStorage::ECG12LeadData));
                }
                else  //不是当前路径 读取文件内参数
                {
                    _file->readBlockData(_index,(char*)_data,
                            sizeof(ECG12LDataStorage::ECG12LeadData));
                }
            }
            else
            {
                if(NULL != _data)
                {
                    delete _data;
                    _data = NULL;
                    _debugNewCount--;
                }
                return NULL;
            }
        }

        return page;
    }
    //走到这个循环，表示是实时打印的结束了
    else if(_step == 5)
    {
        memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
        memset(_drawLeadFaultLen,0,sizeof(_drawLeadFaultLen));
        _printStartPage = false;
        _step = -1;
        _shortWaveOffest = -1;
        _longWaveOffest = -1;
        if(NULL != _data)
        {
            delete _data;
            _data = NULL;
            _debugNewCount--;
        }
        return NULL;
    }
    return NULL;
}

/**************************************************************************************************
 *构建 开始页
 *************************************************************************************************/
PrintPage *PrintECG12LeadLayout::_startPage(const ECG12LDataStorage::ECG12LeadData *info)
{
    //偏移
    int fontH = fontHeight();
    QStringList strList;
    strList.clear();
    if (NULL == info)
    {
        return NULL;
    }

    QString text = trs("ECG12LEADECGSNAPSHOT");
    strList << text;
    timeDate.getDateTime(info->commonInfo.timeStamp, text, true, true);

    strList << (trs("PatientName") +": "+ QString::fromAscii((const char*)info->commonInfo.patientName));
    //病人年龄默认为-1，-1转化成字符时会发生反转，因此这里和"255"做判断
    if ("255" == QString::number(info->commonInfo.patientAge))
    {
        strList << (trs("PatientAge")+": " + "");
    }
    else
    {
        strList << (trs("PatientAge")+": " + QString::number(info->commonInfo.patientAge));
    }

    if(info->commonInfo.patientSex == PATIENT_SEX_MALE)
    {
        strList << (trs("PatientSex")+": "+trs("Male"));
    }
    else if(info->commonInfo.patientSex == PATIENT_SEX_FEMALE)
    {
        strList << (trs("PatientSex")+": "+trs("Female"));
    }
    else
    {
        strList << (trs("PatientSex")+": "+ "");
    }
    strList << (trs("PatientID") +": "+QString::fromAscii((const char*)info->commonInfo.patientID));

    if(info->commonInfo.bandWidth == ECG_BANDWIDTH_0525_40HZ)
        strList << (trs("ECGBandwidth")+": "+ "0.525 - 40 Hz");
    else if(info->commonInfo.bandWidth == ECG_BANDWIDTH_0525_150HZ)
        strList << (trs("ECGBandwidth")+": "+ "0.525 - 150 Hz");
    strList << (trs("ECG12LeadAcquired") + ": "+text);
    timeDate.getDateTime(info->commonInfo.rescueStartTime, text, true, true);
    strList << (trs("ECG12LeadRescueStarttime")+": " + text);
    strList << (trs("SupervisorDeviceIdentifier")+": " + QString::fromAscii((const char*)
                                                 info->commonInfo.deviceID));
    strList << (trs("SerialNum")+": " + QString::fromAscii((const char*)info->commonInfo.serialsNum));
    strList << (trs("SupervisorNotchFilter") + ": " + ECGSymbol::convert((ECGNotchFilter)info->commonInfo.notchFilter));

    if(printManager.getPrintSpeed() == PRINT_SPEED_250)
    {
        strList << (trs("ECGGain") + ": " + ECGSymbol::convert((ECGGain)info->commonInfo.gain) + "   "+trs("PrintSpeed") + ": 25 mm/s");
    }
    else if(printManager.getPrintSpeed() == PRINT_SPEED_500)
    {
        strList << (trs("ECGGain") + ": " + ECGSymbol::convert((ECGGain)info->commonInfo.gain) + "   "+ trs("PrintSpeed") + ": 50 mm/s");
    }

    //去掉名字后面的.crd
    QString fulldisclosureNameStr = QString::fromAscii((const char*)&info->commonInfo.fulldisclosureName);//.right(len - 4);
    int len = fulldisclosureNameStr.size();
    strList << (trs("IncidentIdentificationNumber") + ": "+ fulldisclosureNameStr.left(len - 4));

    int pageWidth = 0;
    int strWidth = 0;
    int count = strList.count();
    for (int i = 0; i < count; ++i)
    {
        strWidth = fontManager.textWidthInPixels(strList.at(i), printFont());
        if (pageWidth < strWidth)
        {
            pageWidth = strWidth;
        }
    }
    text = trs("IncidentIDNum") + ":";
    pageWidth += 2 * fontH;
    PrintPage *startPage = new PrintPage(pageWidth);

    //设置12导联名称
    startPage->setID("ECG12LeadStart" + QString::number(_rescuetartid + _realstartid -1));
    QRect r = startPage->rect();
    QPainter painter(startPage);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(r, Qt::SolidPattern); // 清空所有的数据。

    count = strList.count();
    int xoffset = fontH;
    int yoffset = fontH;
    int gap = fontH + fontH / 3;

    for (int i = 0; i < count; ++i)
    {
        painter.drawText(xoffset, yoffset, strList.at(i));
        yoffset += gap;
    }
    strList.clear();
  //  _drawPDFPicture();
    return startPage;
}

/**************************************************************************************************
 * * 函数说明：画波形标签。
 *   函数参数：
 *         painter:波形需要画入的位置。
 *         id:波形ID。
 *         waveDesc:波形信息。
 *         测试发现625个数据只需要500个像素点
 *************************************************************************************************/
void PrintECG12LeadLayout::_drawWaveLabel(QPainter &painter, const ECG12LeadWaveformInfo &waveDesc)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return;
    }
//   shock flag
//    除颤标志
//    if (1)
//    {
//        QImage image("/usr/local/nPM/icons/s.png");
//        painter.drawImage(QRect(3, waveDesc.startY + 3, fontH, fontH), image);
//        //_shockDeliveried = false;
//    }
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
            if (_isNeedDrawFirstPage)
            {
                //波形标识。
                _drawText(painter, waveDesc.waveTitle, 55,waveDesc.startY);

                // ECG增益。
//                QString ecgGainStr = "1.0cm/mV";
//                _drawText(painter, ecgGainStr, 10, waveDesc.mediumY + fontH);
            }
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 函数说明：画2.5s波形及波形信息。
 * 函数参数：
 *         index：波形索引。
 *         painter:波形需要画入的位置。
 *         drawWaveInformation:波形信息数据。
 *         pageWidth:页宽。
 *************************************************************************************************/
void PrintECG12LeadLayout::_drawShortWave(int index, QPainter &painter, int pageWidth,int count,
        const ECG12LDataStorage::ECG12LeadData *info)
{
    qreal x1 = 50,y1 = 0,x2 = 50,y2 = 0;  // 需要连接的两点。
    bool start = true;
    int offset = 0;
    int flage = 0;
    if (index > _waveGainList.size() || index > _waveformDescList.size() ||
            index > MAX_WAVE_ECG12LEAD || info == NULL )
    {
        return;
    }
    WaveGain waveGain = _waveGainList.at(index);
    waveGain.gain = info->commonInfo.gain;
    ECG12LeadWaveformInfo waveDesc = _waveformDescList.at(index);
    _drawWaveLabel(painter, waveDesc);
    offset = count * WAVEWIDTHSHORT;
    for (int i = 0 + offset; i < waveDesc.waveDataNum + offset; ++i)
    {
        // invalid
        if (info->wave[waveGain.id][i] & 0x40000000)
        {
            if(flage != index)
            {
                flage = index;
            }
            int j = i + 1;

            // 判断后续是否有更多的无效数据
            if (j < waveDesc.waveDataNum + offset)
            {
                while (info->wave[waveGain.id][j++] & 0x40000000)
                {
                    x2 += waveDesc.offsetX;

                    if (j >= waveDesc.waveDataNum + offset)
                    {
                        break;
                    }
                }
                i = j - 1;
            }
            _drawDottedLine(painter, x1, (qreal)waveDesc.mediumY, x2, (qreal)waveDesc.mediumY);
        }
        else
        {
            _drawLeadFaultStart[index] = 0;
            _drawLeadFaultLen[index] = 0;
            short wave = info->wave[waveGain.id][i] & 0xFFFF;
            if (waveGain.id == WAVE_ECG_aVR && _isCabrera)
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
            QLineF line(x1, y1, x2, y2);
            if (painter.pen().style() != Qt::SolidLine)
            {
                painter.setPen(Qt::SolidLine);
            }
            painter.drawLine(line);

            // 数据有效，无效计数清0
            _drawLeadFaultStart[waveGain.id] = 0;
            QPointF center(x2, y2);

            if ((info->wave[waveGain.id][i] >> 16) & ECG_INTERNAL_FLAG_BIT)//ipace
            {
                QPoint start(x2, waveDesc.startY + 10);
                QPoint end(x2, waveDesc.endY - 10);
                ECGWaveWidget::drawIPaceMark(painter, start, end);
            }

            // 判断是否是 demand pace
            if (info->wave[waveGain.id][i] & 0x20000)
            {
                QPen pen = painter.pen();
                Qt::PenStyle style = pen.style();
                pen.setStyle(Qt::DashLine);
                QVector<qreal> darsh(2, 4);
                pen.setDashPattern(darsh);
                painter.setPen(pen);
                line = QLineF(x2, waveDesc.startY, x2, waveDesc.endY);
                painter.drawLine(line);
                pen.setStyle(style);
                painter.setPen(pen);
            }

            // 判断是否是synchronized cardioversion
            if (info->wave[waveGain.id][i] & 0x40000)
            {
                int width = fontManager.textWidthInPixels("S", printFont()) / 2;
                int x = x2 - width;
                if (x2 < width)
                {
                    x = 0;
                }
                if (x2 + width > pageWidth)
                {
                    x = pageWidth - 2 * width;
                }
                painter.drawText(x, waveDesc.startY + fontHeight(), "S");
            }
        }
        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;
    }
//    QLineF line(x1, y1, pageWidth, y1);
//    if (painter.pen().style() != Qt::SolidLine)
//    {
//        painter.setPen(Qt::SolidLine);
//    }
//    painter.drawLine(line);
    return;
}

/**************************************************************************************************
 * 函数说明：画10s波形及波形信息。
 * 函数参数：
 *         index：波形索引。
 *         painter:波形需要画入的位置。
 *         drawWaveInformation:波形信息数据。
 *         pageWidth:页宽。
 *************************************************************************************************/
void PrintECG12LeadLayout::_drawlongWave(int index,QPainter &painter, int pageWidth,
        const ECG12LDataStorage::ECG12LeadData *info)
{
    qreal x1 = 50,y1 = 0,x2 = 50,y2 = 0;  // 需要连接的两点。
    bool start = true;

    if (index > _waveGainList.size() || index > _waveformDescList.size() ||
            index > MAX_WAVE_ECG12LEAD ||   info == NULL )
    {
        return;
    }
    WaveGain waveGain = _waveGainList.at(index);
    waveGain.gain = info->commonInfo.gain;
    ECG12LeadWaveformInfo waveDesc = _waveformDescList.at(index);
    if(ECG_WAVE_RATE_250 == waveDesc.waveRate)
    {
        waveDesc.waveDataNum = 10*ECG_WAVE_RATE_250;
    }
    else
    {
        waveDesc.waveDataNum = 10*ECG_WAVE_RATE_500;
    }

    _drawLeadFaultLen[index] = 0;
    _drawLeadFaultStart[waveGain.id] = 0;
    _drawWaveLabel(painter, waveDesc);

    for (int i = 0; i < waveDesc.waveDataNum; ++i)
    {
        // invalid
        if (info->wave[waveGain.id][i] & 0x40000000)
        {
            int j = i + 1;
            // 判断后续是否有更多的无效数据
            if (j < waveDesc.waveDataNum)
            {
                while (info->wave[waveGain.id][j++] & 0x40000000)
                {
                    x2 += waveDesc.offsetX;

                    if (j >= waveDesc.waveDataNum)
                    {
                        break;
                    }
                }
                i = j - 1;
            }
           _drawDottedLine(painter, x1, (qreal)waveDesc.mediumY, x2, (qreal)waveDesc.mediumY);
        }
        else
        {
            short wave = info->wave[waveGain.id][i] & 0xFFFF;
            double waveData = _mapWaveValue(waveGain, waveDesc, wave);

            // 画线。
            if (start)
            {
                y1 = waveData;
                start = false;
            }

            y2 = waveData;
            QLineF line(x1, y1, x2, y2);

            if (painter.pen().style() != Qt::SolidLine)
            {
                painter.setPen(Qt::SolidLine);
            }

            painter.drawLine(line);

            //绘制内部起搏
            if ((info->wave[waveGain.id][i] >> 16) & ECG_INTERNAL_FLAG_BIT)//ipace
            {
                QPoint start(x2, waveDesc.startY + 10);
                QPoint end(x2, waveDesc.endY - 10);
                ECGWaveWidget::drawIPaceMark(painter, start, end);
            }

            if (ecgParam.getCalcLead() == ecgParam.waveIDToLeadID(waveDesc.waveID))
            {
                // 数据有效，无效计数清0
                _drawLeadFaultStart[waveGain.id] = 0;
            }
        }
        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;
    }

    QLineF line(x1, y1, pageWidth, y1);
    if (painter.pen().style() != Qt::SolidLine)
    {
        painter.setPen(Qt::SolidLine);
    }
    //    painter.drawLine(line);
    return;
}

/**************************************************************************************************
 * 函数说明：画虚线。
 * 函数参数：
 *         painter:波形需要画入的位置。
 *         x1：需要画线的x坐标位置。
 *         x2:当前x坐标位置。
 *         y1:y1坐标的位置。
 *         y2:y2坐标的位置。
 *************************************************************************************************/
void PrintECG12LeadLayout::_drawDottedLine(QPainter &painter, qreal x1, qreal y1,
                                            qreal x2, qreal y2)
{
    QPen oldPen = painter.pen();

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setDashPattern(darsh);
    painter.setPen(pen);
    QLineF dotLine(x1, y1, x2, y2);
    painter.drawLine(dotLine);

    painter.setPen(oldPen);
}

/**************************************************************************************************
 * 映射波形值
 * value:波形实际值
 * max:波形最大值
 * min:波形最小值
 *************************************************************************************************/
double PrintECG12LeadLayout::_mapWaveValue(const WaveGain &waveGain,
        const ECG12LeadWaveformInfo &waveDesc,int waveValue)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return 0;
    }

    int max = waveDesc.waveRangeMax;
    int min = waveDesc.waveRangeMin;
    double startY = waveDesc.startY;
    double endY = waveDesc.endY;
    double dpos = 0;

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
            double scaleData = 0;
            switch (waveGain.gain)
            {
                case ECG_GAIN_X0125:
                    scaleData = 0.125 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X025:
                    scaleData = 0.25 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X05:
                    scaleData = 0.5 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X10:
                    scaleData = 1.0 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X20:
                    scaleData = 2.0 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X40:
                    scaleData = 4.0 * 10 * PIXELS_PER_MM / 2.0;
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

/**************************************************************************************************
 * 函数说明：画文本信息。
 * 函数参数：
 *         painter:绘画位置。
 *         text:需要画入的文本信息。
 *         x:x轴坐标。
 *         y:y轴坐标。
 *************************************************************************************************/
void PrintECG12LeadLayout::_drawText(QPainter &painter, QString text, qreal x, qreal y)
{
    if (text.isEmpty())
    {
        return;
    }
    QPointF p(x, y);
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawText(p, text);
}

/**************************************************************************************************
 * 获取所画10s短波形的信息。
 * note:在标准模式下，二导联的布局在短波形布局的时候已经在中间位置，所以无需修改。
 *      Cabrera格式下 长波形布局需要做微调整。
 *************************************************************************************************/
void PrintECG12LeadLayout::_getWaveFormLongWave()
{
    if(!_isCabrera)//标准格式直接退出
    {
        //打印快照绘制10s短波形时，设置波形显示范围为整个纸张的宽度
        ECG12LeadWaveformInfo waveDesc = _waveformDescList.at((int)ECG_LEAD_II);
        waveDesc.startY = WAVE_START_PIXEL_Y;
        waveDesc.endY = WAVE_PIXEL_Y;
        waveDesc.mediumY = (waveDesc.endY - waveDesc.startY) / 2 + waveDesc.startY;
        _waveformDescList.replace((int)ECG_LEAD_II,waveDesc);

        return;
    }
    WaveGain waveGain = _waveGainList.at(3);
    ECG12LeadWaveformInfo waveDesc = _waveformDescList.at(3);
    waveDesc.reset();
    waveDesc.waveID = waveGain.id;
    waveDesc.waveRate = waveformCache.getSampleRate(waveGain.id);
    if(ECG_WAVE_RATE_250 == waveDesc.waveRate)
    {
        waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_250;
    }
    else
    {
        waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_500;
    }
    waveformCache.getRange(waveGain.id, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
    waveformCache.getBaseline(waveGain.id, waveDesc.waveBaseline);
    waveformCache.getTitle(waveGain.id, waveDesc.waveTitle);

    // 打印机速度/数据速度,x轴的偏移量。
    waveDesc.offsetX = (double)PrintSymbol::convertToPixel(_printSpeed) / waveDesc.waveRate;

    // cacl position
    //打印快照绘制10s短波形时，设置波形显示范围为整个纸张的宽度
    waveDesc.startY = WAVE_START_PIXEL_Y;
    waveDesc.endY = WAVE_PIXEL_Y;
    waveDesc.mediumY = (waveDesc.endY - waveDesc.startY) / 2 + waveDesc.startY;

    // replace list
    _waveformDescList.replace(3,waveDesc);
}

/**************************************************************************************************
 * 获取所画2.5s短波形的信息。
 *************************************************************************************************/
void PrintECG12LeadLayout::_getWaveFormShortWave()
{
    _waveformDescList.clear();
    ECG12LeadWaveformInfo waveDesc;
    for (int i = 0; i < MAX_WAVE_ECG12LEAD; ++i)
    {
        struct WaveGain waveGain = _waveGainList.at(i);
        waveDesc.reset();
        waveDesc.waveID = waveGain.id;
        waveDesc.waveRate = waveformCache.getSampleRate(waveGain.id);
        if(ECG_WAVE_RATE_250 == waveDesc.waveRate)
        {
            waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_250;
        }
        else
        {
            waveDesc.waveDataNum = ECG12LEAD_WAVE_NUM_RATE_500;
        }
        waveformCache.getRange(waveGain.id, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
        waveformCache.getBaseline(waveGain.id, waveDesc.waveBaseline);
        //waveformCache.getTitle(waveGain.id, waveDesc.waveTitle);
        waveDesc.waveTitle = ECGSymbol::convert(ecgParam.waveIDToLeadID(waveGain.id),
        ecgParam.getLeadConvention(),true , _isCabrera);

        // 打印机速度/数据速度,x轴的偏移量。
        waveDesc.offsetX = (double)PrintSymbol::convertToPixel(_printSpeed) / waveDesc.waveRate;
        // cacl position
        waveDesc.startY = WAVE_START_PIXEL_Y + (i % 3)* (WAVE_PIXEL_Y - 2 * WAVE_START_PIXEL_Y) / 3;
        waveDesc.endY = waveDesc.startY + (WAVE_PIXEL_Y - 2 * WAVE_START_PIXEL_Y) / 3;
        waveDesc.mediumY = (waveDesc.endY - waveDesc.startY) / 2 + waveDesc.startY;

        // insert list
        _waveformDescList.append(waveDesc);
    }
}

/**************************************************************************************************
 * 构建选择打印页
 *
 *************************************************************************************************/
PrintPage *PrintECG12LeadLayout::_drawSelectPage(bool IsRealtime)
{
    static int id;
    if(IsRealtime)
    {
        return NULL;
    }
    if (!_dataInit(IsRealtime))
    {
        return NULL;
    }
    if (NULL == _file)
    {
        return NULL;
    }
    while(NULL == _selectdata)  
    {
        if (!ecg12LSnapShotReview.isChecked(_index))
        {
            --_index;
            if (_index < 0)
            {
                return NULL;
            }
            continue;
        }
        _dataLen = _file->getBlockDataLen(_index);
        _selectdata = new ECG12LDataStorage::ECG12LeadData();
        _file->readBlockData(_index, (char*)_selectdata, _dataLen);

        //趋势头文件的索引增加
        _rescuetartid++;

        //进入开始打印页
        return _startPage(_selectdata);
    }
    if(_step < 4)//2.5s的十二道波形
    {
        _getWaveFormShortWave();

        //需要一个宏定义 因为波形数据固定，页码宽度固定
        PrintPage *page = new PrintPage(_pagewidth);
        page->setID("ECG12Lead" + QString().sprintf("%03d", id++));
        QPainter painter(page);
        painter.setFont(printFont());
        painter.setPen(Qt::white);

        // 清空所有的数
        painter.fillRect(page->rect(), Qt::SolidPattern);
        //标志打印的时间段
        _markTimeSegments(painter,_step);
        _step++;
        _shortWaveOffest = _step - 1;



        if(_shortWaveOffest < 0)//做个保护
        {
            return NULL;
        }
        for(int i = 3 * (_step - 1);i <= 3 * (_step - 1) + 2;i++)
        {
            _drawShortWave(i, painter, _pagewidth,_shortWaveOffest,_selectdata);
            /* 打印完一次12导数据后清空“Lead Off”偏移 */
            memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
        }
        return page;
    }
    else if(_step < 5)//10s的II导联波形
    {
        _getWaveFormLongWave();
        int width;
        if(_printSpeed == PRINT_SPEED_250)
        {
            width = 25 * PIXELS_PER_MM * 10 + 50;
        }
        else // PRINT_SPEED_500
        {
            width = 50 * PIXELS_PER_MM * 10 + 50;
        }

        PrintPage *page = new PrintPage(width);
        page->setID("ECG12Lead" + QString().sprintf("%03d", id++));
        QPainter painter(page);
        painter.setFont(printFont());
        painter.setPen(Qt::white);
        painter.fillRect(page->rect(), Qt::SolidPattern);

        //标志打印的时间段
//        _markTimeSegments(painter,_step);

        //绘制10S的波形
        _drawlongWave(_leadMap(ECG_LEAD_II), painter, width, _selectdata);

        if(_index > 0)
        {
            _index--;
            _shortWaveOffest = -1;
            _longWaveOffest = -1;
            _step = 0;
            if (NULL != _selectdata)
            {
                delete _selectdata;
                _selectdata = NULL;
            }
        }else
        {
            _step++;
        }
        return page;
    }
    else if(_step == 5) //一张完整的12导打印结束
    {
        memset(_drawLeadFaultStart,0,sizeof(_drawLeadFaultStart));
        memset(_drawLeadFaultLen,0,sizeof(_drawLeadFaultLen));
        _printStartPage = false;
        _step = -1;
        _shortWaveOffest = -1;
        _longWaveOffest = -1;
        _index = -1;
        if (NULL != _selectdata)
        {
            delete _selectdata;
            _selectdata = NULL;
        }
        return NULL;
    }
    return NULL;
}

/**************************************************************************************************
 * 普通导联和cabrera的导联切换长波形的映射
 * 返回映射在波形列表中 二道实际的位置
 *************************************************************************************************/
ECGLead PrintECG12LeadLayout::_leadMap(ECGLead index)
{
    if(_isCabrera)
    {
        switch(index)
        {
        case ECG_LEAD_II:
            index = ECG_LEAD_AVR;
            break;
       default:
            break;
        }
    }
    return index;
}

/**************************************************************************************************
 * 绘制十二道波形的时间片段
 *************************************************************************************************/
void PrintECG12LeadLayout::_markTimeSegments(QPainter &painter,int index)
{
    QString time[5] = {("0 - 2.5s"),("2.5 - 5s"),("5 - 7.5s"),("7.5 - 10s"),("10s")};
    if(index < 4)
    {
        _drawText(painter,time[index],300,37);

    }
    else if(index == 4)
    {
        _drawText(painter,time[4],300,200);
    }
    else
    {
        return;
    }

}
