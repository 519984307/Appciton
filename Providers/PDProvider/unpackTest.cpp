// 主要验证删除多余0x01的功能。
#include "RingBuff.h"
#include <cstdio>

static RingBuff<unsigned char> ringBuff;
static const int ringBuffLen = 16;
static const int groupNR = 3;
static unsigned char testArray[groupNR][ringBuffLen] = 
{
	{0x01, 0x01, 0x01, 0x90, 0x80, 0x01, 0x01, 0x08, 0x80, 0x12, 0x23, 0x01, 0x01, 0x00, 0x89, 0x01},
	{0x01, 0x01, 0x01, 0x90, 0x80, 0x01, 0x01, 0x08, 0x80, 0x12, 0x23, 0x01, 0x01, 0x00, 0x15, 0x01},
	{0x01, 0x04, 0x01, 0x90, 0x80, 0x01, 0x01, 0x08, 0x80, 0x12, 0x23, 0x01, 0x01, 0x00, 0x89, 0x16},
};
unsigned char *buff = NULL;
int len = ringBuffLen;
void produceData(int index)
{
	buff = testArray[index];
}

void printfRingBuff(void)
{
	for (int i = 0; i < ringBuff.size(); i++)
	{
		printf("0x%x ", ringBuff.at(i));
	}
	printf("\n");
//	ringBuff.clear();
}

void strip(void)
{
    int startIndex = 0;
    bool isok;
    unsigned char v = ringBuff.head(isok);
    if (isok && len > 0)
    {
        if ((v == 0x01) && (buff[0] == 0x01))  // 0x01为数据包起始数据。
        {
            startIndex = 1;                    // 说明有连续两个0x01出现，需要丢弃一个。
        }
    }

    for (int i = startIndex; i < len; i++)
    {
        ringBuff.push(buff[i]);
        if (buff[i] != 0x01)
        {
           continue;
        }

        i++;
        if (i >= len)
        {
            break;
        }

        if (buff[i] == 0x01)                    // 剔除。
        {
            continue;
        }

        ringBuff.push(buff[i]);
    }
}

int main(void)
{
	for (int i = 0; i < groupNR; i++)
	{
		produceData(i);
		strip();
		printfRingBuff();
		printf("\n");
	}
    return 0;
}

