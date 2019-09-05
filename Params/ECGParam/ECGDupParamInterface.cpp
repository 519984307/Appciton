/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/3
 **/

#include "ECGDupParamInterface.h"

ECGDupParamInterface *_instance = NULL;

ECGDupParamInterface *ECGDupParamInterface::getECGDupParam() {
    return _instance;
}

ECGDupParamInterface *ECGDupParamInterface::registerECGDupParam(ECGDupParamInterface *instance) {
    ECGDupParamInterface *old = _instance;
    _instance = instance;
    return old;
}
