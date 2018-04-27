#pragma once
#include <QImage>
#include <QDateTime>

#define RECORDER_PIXEL_PER_MM (8)       // pixel per mm in the record page
#define RECORDER_PAGE_HEIGHT (48 * RECORDER_PIXEL_PER_MM)      // recorder page height in pixel

class RecordPage : public QImage
{
public:
    /**
     * @brief RecordPage constructor
     * @param width page width
     */
    RecordPage(int width)
        :QImage(width, RECORDER_PAGE_HEIGHT, QImage::Format_MonoLSB)
    {
        id = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        fill(Qt::color0);
    }

    /**
     * @brief setID get teh page id
     * @param id
     */
    void setID(const QString &id)
    {
        this->id = id;
    }

    /**
     * @brief getID get the page id
     */
    QString getID() const {return id;}

    /**
     * @brief getColumnData get a column of data from the page
     * @param x x axis position
     * @param buf buffer to store the data, the size must be sufficent
     */
    void getColumnData(int x, unsigned char *buf)
    {
        if(buf == NULL || x >= width() || x < 0 )
        {
            return;
        }
        unsigned char bit = 0;
        int index = 0;
        for(int y = height() - 1; y >= 0; y--)
        {
            bit = pixelIndex(x, y);
            buf[index / 8] |= (bit << (index % 8));
            index ++;
        }
    }

private:
    QString id;
};


