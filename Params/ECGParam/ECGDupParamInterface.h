/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/3
 **/

#include "ECGDefine.h"
#include "ECGSymbol.h"

class ECGDupParamInterface
{
public:
    virtual ~ECGDupParamInterface() {}

    static ECGDupParamInterface *registerECGDupParam(ECGDupParamInterface *instance);

    static ECGDupParamInterface *getECGDupParam(void);

    /**
     * @brief updateHRSource update the hr source
     */
    virtual void updateHRSource() = 0;
};



