#pragma once
#include <stdint.h>

/***************************************************************************************************
 * Wave文件格式如下
 * ------------------------------------------------
 * |             RIFF WAVE Chunk                  |
 * |             ID  = 'RIFF'                     |
 * |             RiffType = 'WAVE'                |
 * ------------------------------------------------
 * |             JUNK Chunk(optional)             |
 * |             ID = 'JUNK'                      |
 * ------------------------------------------------
 * |             Format Chunk                     |
 * |             ID = 'fmt '                      |
 * ------------------------------------------------
 * |             Fact Chunk(optional)             |
 * |             ID = 'fact'                      |
 * ------------------------------------------------
 * |             Data Chunk                       |
 * |             ID = 'data'                      |
 * ------------------------------------------------
 **************************************************************************************************/

/***************************************************************************************************
 * "RIFF" chunk
 **************************************************************************************************/
struct RiffChunk
{

    int8_t chunkID[4];      // 'RIFF'
    uint32_t chunkSize;     // Size
    int8_t format[4];       // 'WAVE'
}__attribute__((packed));

/***************************************************************************************************
 * "JUNK" chunk
 **************************************************************************************************/
struct JUNKChunk
{
    int8_t chunkID[4];      // 'JUNK'
    uint32_t chunkSize;     // Size
}__attribute__((packed));


/***************************************************************************************************
 * "fmt" format sub-chunk
 **************************************************************************************************/
struct FormatSubChunk
{
    int8_t subChunkID[4];   //'fmt '
    uint32_t subChunkSize;  // 数值为16或18，18则最后又附加信息
    uint16_t audioFormat;   // 编码方式，一般为0x0001
    uint16_t numChannels;   // 声道数目，1--单声道；2--双声道
    uint32_t sampleRate;    // 采样频率
    uint32_t byteRate;      // 每秒所需字节数
    uint16_t blockAlign;    // 数据块对齐单位(每个采样需要的字节数)
    uint16_t bitsPerSample; // 每个采样需要的bit数
    uint16_t pack;          // 附加信息（可选，通过Size来判断有无）
}__attribute__((packed));

/***************************************************************************************************
 * "data" sub-chunk
 **************************************************************************************************/
struct DataSubChunk
{
    int8_t subChunkID[4];
    uint32_t subChunkSize;
}__attribute__((packed));


class WavFile
{
public:
    WavFile();
    ~WavFile();

    int size() const
    {
        return _size;
    }

    int index() const
    {
        return _index;
    }

    void rewind()
    {
        _index = 0;
    }

    unsigned char *data()
    {
        return _data;
    }

    unsigned int numChannels() const
    {
        return _format.numChannels;
    }

    unsigned int sampleRate() const
    {
        return _format.sampleRate;
    }

    unsigned int bitsPerSample() const
    {
        return _format.bitsPerSample;
    }

    unsigned int blockAlign() const
    {
        return _format.blockAlign;
    }

    bool checkFormat(const WavFile *wav);
    bool load(const char *fileName);
    void updateIndex(int offset);

private:
    int _size;                 // 音频数据长度, 单位字节
    int _index;                // 播放索引
    unsigned char *_data;      // 音频数据缓冲
    FormatSubChunk _format;    // 文件格式
};
