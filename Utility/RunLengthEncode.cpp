/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/1
 **/

#include "Utility.h"
#include <stdio.h>

#define RLE_LEAD_CHAR ((char)0xAA)
#define RLE_DUPLICATE_COUNT 2

namespace Util {

/***************************************************************************************************
 * RunLengthEncode : run length encode
 *            @src  : source data buffer to be encoded
 *          @srcLen : source data buffer length
 *          @dest : encode data store buffer
 *          @destlen : pointer to recieve the encode data length
 *
 * return : true, endoce success; false, encode failed, mainly parameter error.
 * Note: At the worst case , the encode data length will be 2n + 1 (n is the source data length).
 * eg:
 *  source data [0xaa, 0x0, 0xaa, 0x0, 0xaa], 5 bytes
 *  encode date will be [0xaa 0xaa 0x1 0x0 0xaa 0xaa 0x1 0x0 0xaa 0xaa 0x1], 11 bytes
 **************************************************************************************************/
bool RunLengthEncode(const char *src, int srclen, char *dest, int *destlen)
{
    int srcIndex = 0;
    int destIndex = 0;
    char lastChar = 0;
    int count = 0;

    if (src == NULL || srclen <= 0 || destlen == NULL || dest == NULL)
    {
        return false;
    }

    while (srcIndex < srclen)
    {
        if (srcIndex == 0)
        {
            lastChar = src[srcIndex];
            count = 1;
        }
        else if (lastChar == src[srcIndex] && count < 255)
        {
            count++;
        }
        else
        {
            if (count > RLE_DUPLICATE_COUNT || lastChar == RLE_LEAD_CHAR)
            {
                dest[destIndex++] = RLE_LEAD_CHAR;
                dest[destIndex++] = lastChar;
                dest[destIndex++] = count;
            }
            else
            {
                for (int i = 0; i < count; i++ )
                {
                    dest[destIndex++] = lastChar;
                }
            }
            lastChar = src[srcIndex];
            count = 1;
        }
        srcIndex++;
    }

    // handle last byte
    if (count > RLE_DUPLICATE_COUNT || lastChar == RLE_LEAD_CHAR)
    {
        dest[destIndex++] = RLE_LEAD_CHAR;
        dest[destIndex++] = lastChar;
        dest[destIndex++] = count;
    }
    else
    {
        for (int i = 0; i < count; i++ )
        {
            dest[destIndex++] = lastChar;
        }
    }

    *destlen = destIndex;
#if 0
    fprintf(stderr, "before: %x\n", srclen);
    for (int i = 0; i< srclen; i++)
    {
        fprintf(stderr, "%02x ", src[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "after: %x\n", *destlen);
    for (int i = 0; i< *destlen; i++)
    {
        fprintf(stderr, "%02x ", dest[i]);
    }
    fprintf(stderr, "\n");
#endif

    return true;
}

/***************************************************************************************************
 * RunLenghtDecode : run length decode
 *            @src  : source data buffer to be decoded
 *          @srcLen : source data buffer length
 *          @dest : decode data store buffer
 *          @destlen : pointer to recieve the decode data length
 *
 * return: true, decode success; false, decode failed
 **************************************************************************************************/
bool RunLengthDecode(const char *src, int srclen, char *dest, int *destlen)
{
    int srcIndex = 0;
    int destIndex = 0;

    if (src == NULL || srclen <=0 || dest == NULL || destlen == NULL)
    {
        return 0;
    }

    while (srcIndex < srclen)
    {
        if (src[srcIndex] == RLE_LEAD_CHAR)
        {
            if (srcIndex + 2 >= srclen)
            {
                // source data length not match
                return false;
            }
            char value = src[srcIndex + 1];
            char count = src[srcIndex + 2];

            for (int i = 0; i < count; i++)
            {
                dest[destIndex++] = value;
            }

            srcIndex += 3;
        }
        else
        {
           dest[destIndex++] = src[srcIndex++];
        }
    }

    *destlen = destIndex;

#if 0
    fprintf(stderr, "before: %x\n", srclen);
    for (int i = 0; i< srclen; i++)
    {
        fprintf(stderr, "%02x ", src[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "after: %x\n", *destlen);
    for (int i = 0; i< *destlen; i++)
    {
        fprintf(stderr, "%02x ", dest[i]);
    }
    fprintf(stderr, "\n");
#endif

    return true;
}
}  // namespace Util
