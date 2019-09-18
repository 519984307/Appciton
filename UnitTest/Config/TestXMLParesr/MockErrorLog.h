#pragma once
#include "ErrorLogInterface.h"
#include "gmock/gmock.h"

class MockErrorLog : public ErrorLogInterface
{
public:
    MOCK_METHOD0(count, int());
    MOCK_METHOD1(getLog, ErrorLogItemBase*(int index));
    MOCK_METHOD1(append, void(ErrorLogItemBase *item));
    MOCK_METHOD0(clear, void());
    MOCK_METHOD1(getTypeCount, int(unsigned int type));
    MOCK_METHOD1(getLatestLog, ErrorLogItemBase*(unsigned int type));
    MOCK_METHOD0(getSummary,Summary());
};
