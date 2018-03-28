#include <QFile>
#include "WavFile.h"
#include "Debug.h"

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
WavFile::WavFile()
    : _size(0)
    , _index(0)
    , _data(NULL)
{
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
WavFile::~WavFile()
{
    if (_data)
    {
        delete[] _data;
        _data = NULL;
    }
}

/***************************************************************************************************
 * 说明:
 *      音频格式一致性检查
 * 参数:
 *      wav: 参照的音频文件
 * 返回值:
 *      无
 **************************************************************************************************/
bool WavFile::checkFormat(const WavFile *wav)
{
    if (!wav)
    {
        return false;
    }

    return ((_format.audioFormat == wav->_format.audioFormat)
            && (_format.numChannels == wav->_format.numChannels)
            && (_format.sampleRate == wav->_format.sampleRate)
            && (_format.byteRate == wav->_format.byteRate)
            && (_format.blockAlign == wav->_format.blockAlign)
            && (_format.bitsPerSample == wav->_format.bitsPerSample));
}

/***************************************************************************************************
 * 说明:
 *      加载音频文件
 * 参数:
 *      fileName: 文件名
 * 返回值:
 *      true 成功；false 失败。
 **************************************************************************************************/
bool WavFile::load(const char *fileName)
{
    if (!fileName)
    {
        return false;
    }

    if (!QFile::exists(fileName))
    {
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        debug("Fail to open %s", fileName);
        return false;
    }

    RiffChunk riff;
    JUNKChunk junk;
    DataSubChunk data;
    file.read((char*) &riff, sizeof(RiffChunk));
    if (memcmp(riff.chunkID, "RIFF", 4) != 0
            || memcmp(riff.format, "WAVE", 4) != 0)
    {
        debug("No a vaild wave file : %s!", fileName);
        file.close();
        return false;
    }

    int curOffset = file.pos();
    //check junk chunk, need to skip this chunk
    file.read((char*)&junk, sizeof(JUNKChunk));
    if(memcmp(junk.chunkID, "JUNK", 4) == 0)
    {
        //has junk chunk
        if(junk.chunkSize % 2)
        {
            junk.chunkSize += 1; //if is odd, add one more byte
        }
        curOffset = file.pos() + junk.chunkSize;
    }
    file.seek(curOffset);

    // format子块有可能16个字节，也有可能18个字节
    FormatSubChunk format;
    file.read((char*) &format, sizeof(FormatSubChunk) - 2);
    uint16_t blkAlign = format.numChannels * format.bitsPerSample / 8;
    if ((0 == blkAlign)
            || (format.blockAlign != blkAlign)
            || (format.audioFormat != 1)
            || (memcmp(format.subChunkID, "fmt ", 4) != 0)
            || ((format.numChannels != 1) && (format.numChannels != 2)))
    {
        debug("Bad File Format!");
        file.close();
        return false;
    }
    // 如果是18个字节，说明带有附加信息，需要在多读出两个字节出来
    if (18 == format.subChunkSize)
    {
        file.read(2);
    }

    // 读取数据子块
    file.read((char*) &data, sizeof(DataSubChunk));
    if (memcmp(data.subChunkID, "data", 4) != 0)
    {
        debug("Bad Data Chunk!:%s, %x %x %x %x", fileName, data.subChunkID[0],
                data.subChunkID[1], data.subChunkID[2], data.subChunkID[3]);
        file.close();
        return false;
    }

    _size = data.subChunkSize;
    if (file.size() - file.pos() < _size)
    {
        debug("Can't read enough data!");
        return false;
    }

    memcpy(&_format, &format, sizeof(FormatSubChunk));

    if (_data)
    {
        delete[] _data;
        _data = NULL;
    }

//    debug("filename = %s, size = %d, rate = %d", fileName, data.subChunkSize, _format.sampleRate);
    _data = new unsigned char[_size];
    if (!_data)
    {
        debug("Can not allocate data buffer!");
        return false;
    }

    if (file.read((char*) _data, _size) != _size)
    {
        debug("Cannot Support the Wave File with additional information");
        delete[] _data;
        _data = NULL;
        return false;
    }

    return true;
}

/***************************************************************************************************
 * 说明:
 *      更新文件索引
 * 参数:
 *      offset: 偏移量
 * 返回值:
 *      无
 **************************************************************************************************/
void WavFile::updateIndex(int offset)
{
    _index += offset;

    if (_index > _size)
    {
        _index = _size;
    }

    if (_index < 0)
    {
        _index = 0;
    }
}
